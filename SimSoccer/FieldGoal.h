#ifndef FIELDGOAL_H
#define FIELDGOAL_H
//------------------------------------------------------------------------
//
//Name:   FieldGoal.h
//
//Desc:   class to define a goal for a soccer pitch. The goal is defined
//        by two 2D vectors representing the left and right posts.
//
//        Each time-step the method Scored should be called to determine
//        if a goal has been scored.
//
//Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include "SoccerBall.h"
#include "../Common/2D/Vector2D.h"
#include "../Common/2D/Geometry.h"



class FieldGoal 
{

private:

  Vec3   m_vLeftPost;
  Vec3   m_vRightPost;

  //a vector representing the facing direction of the goal
  Vec3   m_vFacing;

  //the position of the center of the goal line
  Vec3   m_vCenter;

  //each time Scored() detects a goal this is incremented
  int        m_iNumGoalsScored;

public:

  FieldGoal(Vec3 left, Vec3 right, Vec3 facing):m_vLeftPost(left),
                                                       m_vRightPost(right),
                                                       m_vCenter((left+right)/2.0),
                                                       m_iNumGoalsScored(0),
                                                       m_vFacing(facing)
  {  }

  //Given the current ball position and the previous ball position,
  //this method returns true if the ball has crossed the goal line 
  //and increments m_iNumGoalsScored
  inline bool Scored(const SoccerBall*const ball);

  //-----------------------------------------------------accessor methods
  Vec3 Center()const{return m_vCenter;}
  Vec3 Facing()const{return m_vFacing;}
  Vec3 LeftPost()const{return m_vLeftPost;}
  Vec3 RightPost()const{return m_vRightPost;}

  int      NumGoalsScored()const{return m_iNumGoalsScored;}
  void     ResetGoalsScored(){m_iNumGoalsScored = 0;}
};


/////////////////////////////////////////////////////////////////////////

bool FieldGoal::Scored(const SoccerBall*const ball)
{
  if (LineIntersection2D(Vector2D(ball->Pos().GetX(), ball->Pos().GetZ()), Vector2D(ball->OldPos().GetX(), ball->OldPos().GetZ()),
      Vector2D(m_vLeftPost.GetX(), m_vLeftPost.GetZ()), Vector2D(m_vRightPost.GetX(), m_vRightPost.GetZ())))
  {
    ++m_iNumGoalsScored;

    return true;
  }

  return false;
}


#endif