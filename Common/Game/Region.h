#ifndef REGION_H
#define REGION_H
//------------------------------------------------------------------------
//
//  Name:   Region.h
//
//  Desc:   Defines a rectangular region. A region has an identifying
//          number, and four corners.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------

#include <math.h>

//#include "Common/misc/Cgdi.h"
#include "../misc/utils.h"
#include "../misc/Stream_Utility_Functions.h"

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


class Region
{
public:

  enum region_modifier{halfsize, normal};
  
protected:

  double        m_dTop;
  double        m_dLeft;
  double        m_dRight;
  double        m_dBottom;

  double        m_dWidth;
  double        m_dHeight;

  Vec3     m_vCenter;
  
  int          m_iID;

public:

  Region():m_dTop(0),m_dBottom(0),m_dLeft(0),m_dRight(0)
  {}


  Region(double left,
         double top,
         double right,
         double bottom,
         int id = -1):m_dTop(top),
                        m_dRight(right),
                        m_dLeft(left),
                        m_dBottom(bottom),
                        m_iID(id)
  {
    //calculate center of region
    m_vCenter = Vec3( (left+right)*0.5, 0, (top+bottom)*0.5 );

    m_dWidth  = fabs(right-left);
    m_dHeight = fabs(bottom-top);
  }

  virtual ~Region(){}

  //virtual inline void     Render(bool ShowID)const;

  //returns true if the given position lays inside the region. The
  //region modifier can be used to contract the region bounderies
  inline bool     Inside(Vec3 pos, region_modifier r)const;

  //returns a vector representing a random location
  //within the region
  inline Vec3 GetRandomPosition()const;

  //-------------------------------
  double     Top()const{return m_dTop;}
  double     Bottom()const{return m_dBottom;}
  double     Left()const{return m_dLeft;}
  double     Right()const{return m_dRight;}
  double     Width()const{return fabs(m_dRight - m_dLeft);}
  double     Height()const{return fabs(m_dTop - m_dBottom);}
  double     Length()const{return fmax(Width(), Height());}
  double     Breadth()const{return fmin(Width(), Height());}

  Vec3  Center()const{return m_vCenter;}
  int       ID()const{return m_iID;}

};



inline Vec3 Region::GetRandomPosition()const
{
  return Vec3(RandInRange(m_dLeft, m_dRight), 0,
                   RandInRange(m_dTop, m_dBottom));
}

inline bool Region::Inside(Vec3 pos, region_modifier r=normal)const
{
  if (r == normal)
  {
    return ((pos.GetX() > m_dLeft) && (pos.GetX() < m_dRight) &&
         (pos.GetZ() > m_dTop) && (pos.GetZ() < m_dBottom));
  }
  else
  {
    const double marginX = m_dWidth * 0.25;
    const double marginY = m_dHeight * 0.25;

    return ((pos.GetX() > (m_dLeft+marginX)) && (pos.GetX() < (m_dRight-marginX)) &&
         (pos.GetZ() > (m_dTop+marginY)) && (pos.GetZ() < (m_dBottom-marginY)));
  }

}

//inline void Region::Render(bool ShowID = 0)const
//{
//  gdi->HollowBrush();
//  gdi->GreenPen();
//  gdi->Rect(m_dLeft, m_dTop, m_dRight, m_dBottom);
//
//  if (ShowID)
//  { 
//    gdi->TextColor(Cgdi::green);
//    gdi->TextAtPos(Center(), ttos(ID()));
//  }
//}


#endif
