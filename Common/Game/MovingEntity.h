#ifndef MOVING_ENTITY
#define MOVING_ENTITY
//------------------------------------------------------------------------
//
//  Name:   MovingEntity.h
//
//  Desc:   A base class defining an entity that moves. The entity has 
//          a local coordinate system and members for defining its
//          mass and velocity.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------

#include <cassert>

#include "../2D/Vector2D.h"
#include "BaseGameEntity.h"
//#include "../Game/PhysicsManager.h"
#include <Common/Game/PhysicsManager.h>


class MovingEntity : public BaseGameEntity
{
protected:
  
  //the maximum speed this entity may travel at.
  double      m_dMaxSpeed;

  //the maximum force this entity can produce to power itself 
  //(think rockets and thrust)
  double      m_dMaxForce;
  
  //the maximum rate (radians per second)this vehicle can rotate         
  double      m_dMaxTurnRate;

public:


  MovingEntity(BodyInterface& bodyInterface, BodyID body_id,
                double   max_speed,
               double   turn_rate,
               double   max_force):BaseGameEntity(BaseGameEntity::GetNextValidID(), bodyInterface, body_id),
                                  m_dMaxSpeed(max_speed),
                                  m_dMaxTurnRate(turn_rate),
                                  m_dMaxForce(max_force)
  {
  }


  virtual ~MovingEntity(){}

  //accessors
  Vec3  Velocity()const{return m_BodyInterface.GetLinearVelocity(m_EntityPhysicsID);}
  void      SetVelocity(const Vec3& NewVel){ m_BodyInterface.SetLinearVelocity(m_EntityPhysicsID, NewVel);}
  
  double    Mass()const{

      float mass = 0;
      BodyLockRead lock(PhysicsManager::Instance()->GetPhysicsSystem().GetBodyLockInterface(), m_EntityPhysicsID);
      if (lock.Succeeded())
      {
          mass = 1.0f / lock.GetBody().GetMotionProperties()->GetInverseMass();
      }
      return mass;
  }
  
  Vec3  Side()const{return Heading().Cross(Vec3(0, 1, 0)); }

  double    MaxSpeed()const{return m_dMaxSpeed;}                       
  void      SetMaxSpeed(double new_speed){m_dMaxSpeed = new_speed;}
  
  double    MaxForce()const{return m_dMaxForce;}
  void      SetMaxForce(double mf){m_dMaxForce = mf;}

  bool      IsSpeedMaxedOut()const{return m_dMaxSpeed*m_dMaxSpeed >= Velocity().LengthSq();}
  double    Speed()const{return Velocity().Length();}
  double    SpeedSq()const{return Velocity().LengthSq();}
  
  Vec3  Heading()const{
      return m_BodyInterface.GetRotation(m_EntityPhysicsID) * Vec3(0, 0, 1);
  }
  void Rotate(float degrees) {
      //float degrees_rad = DegreesToRadians(degrees);
      float heading_degrees = m_BodyInterface.GetRotation(m_EntityPhysicsID).GetRotationAngle(Vec3(0, -1, 0));
      m_BodyInterface.SetRotation(m_EntityPhysicsID, Quat::sRotation(Vec3(0, -1, 0), degrees + heading_degrees),EActivation::Activate);
  }
  //void      SetHeading(Vec3 new_heading);
  bool      RotateHeadingToFacePosition(Vec3 target);

  double    MaxTurnRate()const{return m_dMaxTurnRate;}
  void      SetMaxTurnRate(double val){m_dMaxTurnRate = val;}

};


//--------------------------- RotateHeadingToFacePosition ---------------------
//
//  given a target position, this method rotates the entity's heading and
//  side vectors by an amount not greater than m_dMaxTurnRate until it
//  directly faces the target.
//
//  returns true when the heading is facing in the desired direction
//-----------------------------------------------------------------------------
inline bool MovingEntity::RotateHeadingToFacePosition(Vec3 target)
{
  Vec3 position = m_BodyInterface.GetCenterOfMassPosition(m_EntityPhysicsID);
  Vec3 toTarget = (target - position);
  toTarget = toTarget.Normalized();

  Vec3 heading = Heading();
  heading = heading.Normalized();
  double dot = heading.Dot(toTarget);

  //some compilers lose acurracy so the value is clamped to ensure it
  //remains valid for the acos
  Clamp(dot, -1, 1);

  //first determine the angle between the heading vector and the target
  double angle = acos(dot);

  //clamp the amount to turn to the max turn rate
  if (angle > m_dMaxTurnRate) angle = m_dMaxTurnRate;
  
  //return true if the player is facing the target
  if (angle < 0.05) return true;

  m_BodyInterface.SetAngularVelocity(m_EntityPhysicsID, Vec3(0,0,0));
  Rotate(angle);

  return false;
}


//------------------------- SetHeading ----------------------------------------
//
//  first checks that the given heading is not a vector of zero length. If the
//  new heading is valid this fumction sets the entity's heading and side 
//  vectors accordingly
//-----------------------------------------------------------------------------
//inline void MovingEntity::SetHeading(Vec3 new_heading)
//{
//  assert( (new_heading.LengthSq() - 1.0) < 0.00001);
//  
//  m_vHeading = new_heading;
//
//  //the side vector must always be perpendicular to the heading
//  m_vSide = m_vHeading.Perp();
//}




#endif