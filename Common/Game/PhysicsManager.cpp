#include "PhysicsManager.h"

//--------------------------- Instance ----------------------------------------
//
//   this class is a singleton
//-----------------------------------------------------------------------------
PhysicsManager* PhysicsManager::Instance()
{
	static PhysicsManager instance;

	return &instance;
}
