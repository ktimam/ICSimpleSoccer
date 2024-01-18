#ifndef GOALY_H
#define GOALY_H
//------------------------------------------------------------------------
//
//  Name:   GoalKeeper.h
//
//  Desc:   class to implement a goalkeeper agent
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include "../Common/2D/Vector2D.h"
#include "PlayerBase.h"
#include "../Common/FSM/StateMachine.h"

class PlayerBase;




class GoalKeeper : public PlayerBase
{
private:
  
   //an instance of the state machine class
  StateMachine<GoalKeeper>*  m_pStateMachine;
  
  //this vector is updated to point towards the ball and is used when
  //rendering the goalkeeper (instead of the underlaying vehicle's heading)
  //to ensure he always appears to be watching the ball
  Vec3   m_vLookAt;

public:
  
   GoalKeeper(SoccerTeam*        home_team,
              int                home_region,
              State<GoalKeeper>* start_state,
              BodyInterface& bodyInterface, BodyID body_id,
              double              max_force,
              double              max_speed,
              double              max_turn_rate);

   ~GoalKeeper(){delete m_pStateMachine;}

   //these must be implemented
   void        Update();
   //void        Render();
   bool        HandleMessage(const Telegram& msg);


   //returns true if the ball comes close enough for the keeper to 
   //consider intercepting
   bool        BallWithinRangeForIntercept()const;

   //returns true if the keeper has ventured too far away from the goalmouth
   bool        TooFarFromGoalMouth()const;

   //this method is called by the Intercept state to determine the spot
   //along the goalmouth which will act as one of the interpose targets
   //(the other is the ball).
   //the specific point at the goal line that the keeper is trying to cover
   //is flexible and can move depending on where the ball is on the field.
   //To achieve this we just scale the ball's y value by the ratio of the
   //goal width to playingfield width
   Vec3    GetRearInterposeTarget()const;

   StateMachine<GoalKeeper>* GetFSM()const{return m_pStateMachine;}
   std::string GetCurrentStateName()const { return std::string(m_pStateMachine->GetNameOfCurrentState()); }

   
   Vec3    LookAt()const{return m_vLookAt;}
   void        SetLookAt(Vec3 v){m_vLookAt=v;}
};



#endif