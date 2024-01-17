#include "SoccerPitch.h"
#include "SoccerBall.h"
#include "FieldGoal.h"
#include "../Common/Game/Region.h"
#include "../Common/2D/Transformations.h"
#include "../Common/2D/Geometry.h"
#include "SoccerTeam.h"
//#include "Debug/DebugConsole.h"
#include "../Common/Game/EntityManager.h"
#include "ParamLoader.h"
#include "PlayerBase.h"
#include "TeamStates.h"
#include "../Common/misc/FrameCounter.h"

const int NumRegionsHorizontal = 6; 
const int NumRegionsVertical   = 3;

//------------------------------- ctor -----------------------------------
//------------------------------------------------------------------------
SoccerPitch::SoccerPitch(int cx, int cy):m_cxClient(cx),
                                         m_cyClient(cy),
                                         m_bPaused(false),
                                         m_bGoalKeeperHasBall(false),
                                         m_Regions(NumRegionsHorizontal*NumRegionsVertical),
                                         m_bGameOn(false)
{
    const float out_area = 20;

  //define the playing area
  m_pPlayingArea = new Region(out_area, out_area, cx- out_area, cy- out_area);

  //create the regions  
  CreateRegions(PlayingArea()->Width() / (double)NumRegionsHorizontal,
                PlayingArea()->Height() / (double)NumRegionsVertical);

  //create the goals
   m_pRedGoal  = new FieldGoal(Vec3( m_pPlayingArea->Left(), 0, (cy-Prm.GoalWidth)/2),
       Vec3(m_pPlayingArea->Left(), 0, cy - (cy-Prm.GoalWidth)/2),
       Vec3(1,0,0));
   


  m_pBlueGoal = new FieldGoal(Vec3( m_pPlayingArea->Right(), 0, (cy-Prm.GoalWidth)/2),
      Vec3(m_pPlayingArea->Right(), 0, cy - (cy-Prm.GoalWidth)/2),
      Vec3(-1,0,0));

  //Create the Physics
  m_PhysicsManager = PhysicsManager::Instance();

  //create the soccer ball
  RefConst<Shape> sphere_shape = new SphereShape(0.5f);
  BodyCreationSettings bcs(sphere_shape, Vec3((double)m_cxClient / 2.0, 2.0_r, (double)m_cyClient / 2.0), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
  bcs.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
  bcs.mMassPropertiesOverride.mMass = Prm.BallMass;
  //bcs.mFriction = 0.5;
  bcs.mLinearDamping = 0.15;
  BodyID ball_id = m_PhysicsManager->GetBodyInterface().CreateAndAddBody(bcs, EActivation::Activate);
  m_pBall = new SoccerBall(m_PhysicsManager->GetBodyInterface(), ball_id,
      m_vecWalls);

  
  //create the teams 
  m_pRedTeam  = new SoccerTeam(m_pRedGoal, m_pBlueGoal, this, SoccerTeam::red);
  m_pBlueTeam = new SoccerTeam(m_pBlueGoal, m_pRedGoal, this, SoccerTeam::blue);

  //make sure each team knows who their opponents are
  m_pRedTeam->SetOpponents(m_pBlueTeam);
  m_pBlueTeam->SetOpponents(m_pRedTeam); 

  //create the field outside lines
  Vector2D TopLeft(m_pPlayingArea->Left(), m_pPlayingArea->Top());                                        
  Vector2D TopRight(m_pPlayingArea->Right(), m_pPlayingArea->Top());
  Vector2D BottomRight(m_pPlayingArea->Right(), m_pPlayingArea->Bottom());
  Vector2D BottomLeft(m_pPlayingArea->Left(), m_pPlayingArea->Bottom());
                                      
  m_vecWalls.push_back(Wall2D(BottomLeft, Vector2D(m_pRedGoal->RightPost().GetX(), m_pRedGoal->RightPost().GetZ())));
  m_vecWalls.push_back(Wall2D(Vector2D(m_pRedGoal->LeftPost().GetX(), m_pRedGoal->LeftPost().GetZ()), TopLeft));
  m_vecWalls.push_back(Wall2D(TopLeft, TopRight));
  m_vecWalls.push_back(Wall2D(TopRight, Vector2D(m_pBlueGoal->LeftPost().GetX(), m_pBlueGoal->LeftPost().GetZ())));
  m_vecWalls.push_back(Wall2D(Vector2D(m_pBlueGoal->RightPost().GetX(), m_pBlueGoal->RightPost().GetZ()), BottomRight));
  m_vecWalls.push_back(Wall2D(BottomRight, BottomLeft));

  const float wall_width = 10;
  const float wall_height = 50;
  const float GoalWidth = Prm.GoalWidth;
  const float goal_post_width = 5.0f;
  const float goal_wall_width = (m_cyClient / 2 - GoalWidth / 2) / 2 + goal_post_width;
  const float out_wall_width = m_cxClient / 2 + goal_post_width;
  const float Top = m_pPlayingArea->Top();
  const float Left = m_pPlayingArea->Left();
  const float Right = m_pPlayingArea->Right();
  const float Bottom = m_pPlayingArea->Bottom();
  const float m_pRedGoalRightPost = m_pRedGoal->RightPost().GetZ();
  const float m_pBlueGoalRightPost = m_pBlueGoal->RightPost().GetZ();


  BodyInterface& body_interface = m_PhysicsManager->GetBodyInterface();

  // Create Floor
  BoxShapeSettings floor_shape_settings(Vec3(m_cxClient / 2, 1.0f, m_cyClient / 2));
  ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
  ShapeRefC floor_shape = floor_shape_result.Get();
  BodyCreationSettings floor_settings(floor_shape, RVec3(m_cxClient / 2, -1.0_r, m_cyClient / 2), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
  //floor_settings.mFriction = 0;
  Body* floor = body_interface.CreateBody(floor_settings);
  body_interface.AddBody(floor->GetID(), EActivation::DontActivate);


  {
	  BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	  ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	  ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	  BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Left - wall_width, wall_height - 10, Top + goal_wall_width - goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	  Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	  body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
  }

  {
	  BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	  ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	  ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	  BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Left - wall_width, wall_height - 10, m_pRedGoalRightPost + goal_wall_width + goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	  Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	  body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
  }

  {
	  BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	  ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	  ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	  BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Right + wall_width, wall_height - 10, Top + goal_wall_width - goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	  Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	  body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
  }

  {
	  BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	  ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	  ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	  BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Right + wall_width, wall_height - 10, m_pBlueGoalRightPost + goal_wall_width + goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	  Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	  body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
  }

  {
	  BoxShapeSettings line_wall_shape_settings(Vec3(out_wall_width, wall_height, wall_width));
	  ShapeSettings::ShapeResult line_wall_shape_result = line_wall_shape_settings.Create();
	  ShapeRefC line_wall_shape = line_wall_shape_result.Get();
	  BodyCreationSettings line_wall_settings(line_wall_shape, RVec3(out_wall_width, wall_height - 10, Top - wall_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	  Body* line_wall = body_interface.CreateBody(line_wall_settings);
	  body_interface.AddBody(line_wall->GetID(), EActivation::DontActivate);
  }

  {
	  BoxShapeSettings line_wall_shape_settings(Vec3(out_wall_width, wall_height, wall_width));
	  ShapeSettings::ShapeResult line_wall_shape_result = line_wall_shape_settings.Create();
	  ShapeRefC line_wall_shape = line_wall_shape_result.Get();
	  BodyCreationSettings line_wall_settings(line_wall_shape, RVec3(out_wall_width, wall_height - 10, Bottom + wall_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	  Body* line_wall = body_interface.CreateBody(line_wall_settings);
	  body_interface.AddBody(line_wall->GetID(), EActivation::DontActivate);
  }

  ParamLoader* p = ParamLoader::Instance();
}

//-------------------------------- dtor ----------------------------------
//------------------------------------------------------------------------
SoccerPitch::~SoccerPitch()
{
  delete m_pBall;

  delete m_pRedTeam;
  delete m_pBlueTeam;

  delete m_pRedGoal;
  delete m_pBlueGoal;

  delete m_pPlayingArea;

  for (unsigned int i=0; i<m_Regions.size(); ++i)
  {
    delete m_Regions[i];
  }
}

//----------------------------- Update -----------------------------------
//
//  this demo works on a fixed frame rate (60 by default) so we don't need
//  to pass a time_elapsed as a parameter to the game entities
//------------------------------------------------------------------------
void SoccerPitch::Update()
{
  if (m_bPaused) return;

  static int tick = 0;

  //update the balls
  m_pBall->Update();

  //update the teams
  m_pRedTeam->Update();
  m_pBlueTeam->Update();

  PhysicsManager::Instance()->Update();

  //if a goal has been detected reset the pitch ready for kickoff
  if (m_pBlueGoal->Scored(m_pBall) || m_pRedGoal->Scored(m_pBall))
  {
    m_bGameOn = false;
    
    //reset the ball                                                      
    m_pBall->PlaceAtPosition(Vec3((double)m_cxClient/2.0, 0.51, (double)m_cyClient/2.0));

    //get the teams ready for kickoff
    m_pRedTeam->GetFSM()->ChangeState(PrepareForKickOff::Instance());
    m_pBlueTeam->GetFSM()->ChangeState(PrepareForKickOff::Instance());
  }
}

//------------------------- CreateRegions --------------------------------
void SoccerPitch::CreateRegions(double width, double height)
{  
  //index into the vector
  int idx = m_Regions.size()-1;
    
  for (int col=0; col<NumRegionsHorizontal; ++col)
  {
    for (int row=0; row<NumRegionsVertical; ++row)
    {
      m_Regions[idx--] = new Region(PlayingArea()->Left()+col*width,
                                   PlayingArea()->Top()+row*height,
                                   PlayingArea()->Left()+(col+1)*width,
                                   PlayingArea()->Top()+(row+1)*height,
                                   idx);
    }
  }
}


//------------------------------ Render ----------------------------------
//------------------------------------------------------------------------
//bool SoccerPitch::Render()
//{
//  //draw the grass
//  gdi->DarkGreenPen();
//  gdi->DarkGreenBrush();
//  gdi->Rect(0,0,m_cxClient, m_cyClient);
//
//  //render regions
//  if (Prm.bRegions)
//  {   
//    for (unsigned int r=0; r<m_Regions.size(); ++r)
//    {
//      m_Regions[r]->Render(true);
//    }
//  }
//  
//  //render the goals
//  gdi->HollowBrush();
//  gdi->RedPen();
//  gdi->Rect(m_pPlayingArea->Left(), (m_cyClient-Prm.GoalWidth)/2, m_pPlayingArea->Left()+40, m_cyClient - (m_cyClient-Prm.GoalWidth)/2);
//
//  gdi->BluePen();
//  gdi->Rect(m_pPlayingArea->Right(), (m_cyClient-Prm.GoalWidth)/2, m_pPlayingArea->Right()-40, m_cyClient - (m_cyClient-Prm.GoalWidth)/2);
//  
//  //render the pitch markings
//  gdi->WhitePen();
//  gdi->Circle(m_pPlayingArea->Center(), m_pPlayingArea->Width() * 0.125);
//  gdi->Line(m_pPlayingArea->Center().x, m_pPlayingArea->Top(), m_pPlayingArea->Center().x, m_pPlayingArea->Bottom());
//  gdi->WhiteBrush();
//  gdi->Circle(m_pPlayingArea->Center(), 2.0);
//
//
//  //the ball
//  gdi->WhitePen();
//  gdi->WhiteBrush();
//  m_pBall->Render();
//  
//  //Render the teams
//  m_pRedTeam->Render();
//  m_pBlueTeam->Render(); 
//
//  //render the walls
//  gdi->WhitePen();
//  for (unsigned int w=0; w<m_vecWalls.size(); ++w)
//  {
//    m_vecWalls[w].Render();
//  }
//
//  //show the score
//  gdi->TextColor(Cgdi::red);
//  gdi->TextAtPos((m_cxClient/2)-50, m_cyClient-18, "Red: " + ttos(m_pBlueGoal->NumGoalsScored()));
//
//  gdi->TextColor(Cgdi::blue);
//  gdi->TextAtPos((m_cxClient/2)+10, m_cyClient-18, "Blue: " + ttos(m_pRedGoal->NumGoalsScored()));
//
//  return true;  
//}







