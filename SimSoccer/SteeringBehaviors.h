#ifndef SteeringBehaviorsS_H
#define SteeringBehaviorsS_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name:   SteeringBehaviorss.h
//
//  Desc:   class to encapsulate steering behaviors for a soccer player
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
//#include <windows.h>
#include <string>

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

class PlayerBase;
class SoccerPitch;
class SoccerBall;
class CWall;
class CObstacle;


//---------------------------- class details -----------------------------

class SteeringBehaviors
{
private:
  
  PlayerBase*   m_pPlayer;                                                  

  SoccerBall*   m_pBall;

  //the steering force created by the combined effect of all
  //the selected behaviors
  Vec3     m_vSteeringForce;

  //the current target (usually the ball or predicted ball position)
  Vec3     m_vTarget;

  //the distance the player tries to interpose from the target
  double        m_dInterposeDist;

  //multipliers. 
  double        m_dMultSeparation;

  //how far it can 'see'
  double        m_dViewDistance;


  //binary flags to indicate whether or not a behavior should be active
  int           m_iFlags;

  enum behavior_type
  {
    none               = 0x0000,
    seek               = 0x0001,
    arrive             = 0x0002,
    separation         = 0x0004,
    pursuit            = 0x0008,
    interpose          = 0x0010
  };

  //used by group behaviors to tag neighbours
  bool         m_bTagged;
  
  //Arrive makes use of these to determine how quickly a vehicle
  //should decelerate to its target
  enum Deceleration{slow = 3, normal = 2, fast = 1};


  //this behavior moves the agent towards a target position
  Vec3 Seek(Vec3 target);

  //this behavior is similar to seek but it attempts to arrive 
  //at the target with a zero velocity
  Vec3 Arrive(Vec3 target, Deceleration decel);

  //This behavior predicts where its prey will be and seeks
  //to that location
  Vec3 Pursuit(const SoccerBall* ball);
 
  Vec3 Separation();

  //this attempts to steer the agent to a position between the opponent
  //and the object
  Vec3 Interpose(const SoccerBall* ball,
      Vec3 pos,
                     double    DistFromTarget);


  //finds any neighbours within the view radius
  void      FindNeighbours();


  //this function tests if a specific bit of m_iFlags is set
  bool      On(behavior_type bt){return (m_iFlags & bt) == bt;}

  bool      AccumulateForce(Vec3&sf, Vec3 ForceToAdd);

  Vec3  SumForces();

  //a vertex buffer to contain the feelers rqd for dribbling
  std::vector<Vec3> m_Antenna;

  
public:

  SteeringBehaviors(PlayerBase*       agent,
                    SoccerPitch*  world,
                    SoccerBall*   ball);

  virtual ~SteeringBehaviors(){}

 
  Vec3 Calculate();

  //calculates the component of the steering force that is parallel
  //with the vehicle heading
  double    ForwardComponent();

  //calculates the component of the steering force that is perpendicuar
  //with the vehicle heading
  double    SideComponent();

  Vec3 Force()const{return m_vSteeringForce;}

  //renders visual aids and info for seeing how each behavior is
  //calculated
  //void      RenderInfo();
  //void      RenderAids();

  Vec3  Target()const{return m_vTarget;}
  void      SetTarget(const Vec3 t){m_vTarget = t;}

  double     InterposeDistance()const{return m_dInterposeDist;}
  void      SetInterposeDistance(double d){m_dInterposeDist = d;}

  bool      Tagged()const{return m_bTagged;}
  void      Tag(){m_bTagged = true;}
  void      UnTag(){m_bTagged = false;}
  

  void SeekOn(){m_iFlags |= seek;}
  void ArriveOn(){m_iFlags |= arrive;}
  void PursuitOn(){m_iFlags |= pursuit;}
  void SeparationOn(){m_iFlags |= separation;}
  void InterposeOn(double d){m_iFlags |= interpose; m_dInterposeDist = d;}

  
  void SeekOff()  {if(On(seek))   m_iFlags ^=seek;}
  void ArriveOff(){if(On(arrive)) m_iFlags ^=arrive;}
  void PursuitOff(){if(On(pursuit)) m_iFlags ^=pursuit;}
  void SeparationOff(){if(On(separation)) m_iFlags ^=separation;}
  void InterposeOff(){if(On(interpose)) m_iFlags ^=interpose;}


  bool SeekIsOn(){return On(seek);}
  bool ArriveIsOn(){return On(arrive);}
  bool PursuitIsOn(){return On(pursuit);}
  bool SeparationIsOn(){return On(separation);}
  bool InterposeIsOn(){return On(interpose);}

};




#endif