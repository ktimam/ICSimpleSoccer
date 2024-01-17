#include "GoalKeeper.h"
//#include "Common/misc/Cgdi.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "SoccerPitch.h"
#include "../Common/2D/transformations.h"
#include "GoalKeeperStates.h"
#include "FieldGoal.h"
#include "../Common/Game/EntityFunctionTemplates.h"
#include "ParamLoader.h"



//----------------------------- ctor ------------------------------------
//-----------------------------------------------------------------------
GoalKeeper::GoalKeeper(SoccerTeam*        home_team,
                       int                home_region,
                       State<GoalKeeper>* start_state,
                       BodyInterface& bodyInterface, BodyID body_id,
                       double              max_force,
                       double              max_speed,
                       double              max_turn_rate): PlayerBase(home_team,
                                                             home_region,
                                                             bodyInterface, body_id,
                                                             max_force,
                                                             max_speed,
                                                             max_turn_rate,
                                                             PlayerBase::goal_keeper)
                                         
                                        
{   
   //set up the state machine
  m_pStateMachine = new StateMachine<GoalKeeper>(this);

  m_pStateMachine->SetCurrentState(start_state);
  m_pStateMachine->SetPreviousState(start_state);
  m_pStateMachine->SetGlobalState(GlobalKeeperState::Instance());

  m_pStateMachine->CurrentState()->Enter(this);        
}



//-------------------------- Update --------------------------------------

void GoalKeeper::Update()
{ 
  //run the logic for the current state
  m_pStateMachine->Update();

  //calculate the combined force from each steering behavior 
  Vec3 SteeringForce = m_pSteering->Calculate();

  //if no steering force is produced decelerate the player by applying a
  //braking force
  if (m_pSteering->Force().IsNearZero())
  {
      const double BrakingRate = 0.2;// 0.8;

      //m_vVelocity = m_vVelocity * BrakingRate;      
      m_BodyInterface.SetFriction(m_EntityPhysicsID, BrakingRate);
  }
  else
  {
      m_BodyInterface.SetFriction(m_EntityPhysicsID, 0);
  }

  //the steering force's side component is a force that rotates the 
  //player about its axis. We must limit the rotation so that a player
  //can only turn by PlayerMaxTurnRate rads per update.
  double TurningForce = m_pSteering->SideComponent();

  Clamp(TurningForce, -Prm.PlayerMaxTurnRate, Prm.PlayerMaxTurnRate);

  //rotate the heading vector
  //Vec2DRotateAroundOrigin(m_vHeading, TurningForce);
  //m_BodyInterface.SetAngularVelocity(m_EntityPhysicsID, Vec3(TurningForce, 0, TurningForce));
  Rotate(TurningForce);


  //Acceleration = Force/Mass
  Vec3 Acceleration = SteeringForce / Mass();

  //update velocity
  //m_vVelocity += Acceleration;
  m_BodyInterface.AddLinearVelocity(m_EntityPhysicsID, Acceleration);

  //make sure player does not exceed maximum velocity
  //m_vVelocity.Truncate(m_dMaxSpeed);
  SetVelocity(Vec3::sClamp(Velocity(), Vec3(-m_dMaxSpeed, -m_dMaxSpeed, -m_dMaxSpeed), Vec3(m_dMaxSpeed, m_dMaxSpeed, m_dMaxSpeed)));

  //update the position
  //m_vPosition += m_vVelocity;


  //enforce a non-penetration constraint if desired
  /*if(Prm.bNonPenetrationConstraint)
  {
    EnforceNonPenetrationContraint(this, AutoList<PlayerBase>::GetAllMembers());
  }*/

  //update the heading if the player has a non zero velocity
  /*if ( !m_vVelocity.isZero())
  {    
    m_vHeading = Vec2DNormalize(m_vVelocity);

    m_vSide = m_vHeading.Perp();
  }*/

  /*m_vHeading = Vec2DNormalize(m_vLookAt);
  m_vSide = m_vHeading.Perp();*/

  //look-at vector always points toward the ball
  if (!Pitch()->GoalKeeperHasBall())
  {
   m_vLookAt = (Ball()->Pos() - Pos()).Normalized();
  }
}


bool GoalKeeper::BallWithinRangeForIntercept()const
{
  return ((Team()->HomeGoal()->Center() - Ball()->Pos()).LengthSq() <=
          Prm.GoalKeeperInterceptRangeSq);
}

bool GoalKeeper::TooFarFromGoalMouth()const
{
  return ((Pos() - GetRearInterposeTarget()).LengthSq() >
          Prm.GoalKeeperInterceptRangeSq);
}

Vec3 GoalKeeper::GetRearInterposeTarget()const
{
  double xPosTarget = Team()->HomeGoal()->Center().GetX();

  double yPosTarget = Pitch()->PlayingArea()->Center().GetZ() -
                     Prm.GoalWidth*0.5 + (Ball()->Pos().GetZ()*Prm.GoalWidth) /
                     Pitch()->PlayingArea()->Height();

  return Vec3(xPosTarget, 0, yPosTarget); 
}

//-------------------- HandleMessage -------------------------------------
//
//  routes any messages appropriately
//------------------------------------------------------------------------
bool GoalKeeper::HandleMessage(const Telegram& msg)
{
  return m_pStateMachine->HandleMessage(msg);
}

//--------------------------- Render -------------------------------------
//
//------------------------------------------------------------------------
//void GoalKeeper::Render()                                         
//{
//  if (Team()->Color() == SoccerTeam::blue) 
//    gdi->BluePen();
//  else 
//    gdi->RedPen();
//  
//  m_vecPlayerVBTrans = WorldTransform(m_vecPlayerVB,
//                                       Pos(),
//                                       m_vLookAt,
//                                       m_vLookAt.Perp(),
//                                       Scale());
//
//  gdi->ClosedShape(m_vecPlayerVBTrans);
//  
//  //draw the head
//  gdi->BrownBrush();
//  gdi->Circle(Pos(), 6);
//
//  //draw the ID
//  if (Prm.bIDs)
//  {
//    gdi->TextColor(0, 170, 0);;
//    gdi->TextAtPos(Pos().x-20, Pos().y-20, ttos(ID()));
//  }
//
//  //draw the state
//  if (Prm.bStates)
//  { 
//    gdi->TextColor(0, 170, 0); 
//    gdi->TransparentText(); 
//    gdi->TextAtPos(m_vPosition.x, m_vPosition.y -20, std::string(m_pStateMachine->GetNameOfCurrentState()));
//  }
//}