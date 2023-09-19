// SimSoccerServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef WINDOWS
#include "SimSoccerServer.h"

#include "ic_api.h"

#include <stdio.h>
#include <iostream>

#include "SimSoccer/constants.h"
#include "Common/misc/utils.h"
//#include "Common/Time/PrecisionTimer.h"
#include "Common/Game/EntityManager.h"
#include "SimSoccer/SoccerPitch.h"
#include "SimSoccer/SoccerTeam.h"
#include "SimSoccer/Goalkeeper.h"
#include "SimSoccer/FieldPlayer.h"
#include "SimSoccer/FieldGoal.h"
#include "SimSoccer/SteeringBehaviors.h"
#include "Common/json/json.hpp"
#include "SimSoccer/ParamLoader.h"
//#include "Resource.h"

#include "Common/misc/Snapshot.h"

//#include "3rdparty/cpp-httplib/httplib.h"
//#include "3rdparty/picojson/picojson.h"

using json = nlohmann::json;

const int MATCH_DURATION = 10;
const int MATCH_RATE = 6;

const int MILLI_IN_SECOND = 20;
const int MILLI_IN_MINUTE = 60 * 20;
const int SECOND_MAX_VALUE = 60;

const bool LOG_MATCH_OUTPUT = true;
const int SNAPSHOT_RATE = 50;

int mTickCount = 0;
bool mMatchFinished = false;

SoccerPitch* g_SoccerPitch;
Snapshot* g_MatchReplay;
json         g_LastSnapshot;

void IncrementTime(int rate)
{
   mTickCount += MATCH_RATE * rate;

   int minutes = mTickCount / MILLI_IN_MINUTE;

   if (minutes >= MATCH_DURATION)
   {
       mMatchFinished = true;
   }
}
std::string GetCurrentTimeString()
{
   int seconds = (mTickCount / MILLI_IN_SECOND) % SECOND_MAX_VALUE;
   int minutes = mTickCount / MILLI_IN_MINUTE;
  //  std::ostringstream stringStream;
  //  stringStream << minutes << " : " << seconds;
   std::string time = "";//stringStream.str();
   return time;
}
/* ---------------------------------------------------------
  Extract a 'std::string" from an incoming CandidTypeText
  Respond with an 'std::string' wrapped in a CandidTypeText
*/
void play_match() {
  IC_API ic_api(CanisterQuery{std::string(__func__)}, false);

  // Get the principal of the caller, as cryptographically verified by the IC
  CandidTypePrincipal caller = ic_api.get_caller();

  // Get the name, passed as a Candid parameter to this method
  uint64_t seed{0};
  ic_api.from_wire(CandidTypeNat64{&seed});

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //std::cout << "Seed Generated : " << seed << std::endl;
    //seed random number generator
    srand(seed);

    //std::cout << "Creating Pitch..." << std::endl;

    g_SoccerPitch = new SoccerPitch(WindowWidth, WindowHeight);

    if (LOG_MATCH_OUTPUT)
    {
       g_MatchReplay = new Snapshot();
    }

    //std::cout << "Starting Match..." << std::endl;
    mMatchFinished = false;
    int updates_count = 0;
    while (!mMatchFinished)
    {
       IncrementTime(1);
       //update game states
       g_SoccerPitch->Update();

       if (LOG_MATCH_OUTPUT)
       {
           updates_count++;
           //Don't take snapshot for every move
           if (updates_count % SNAPSHOT_RATE == 1 || updates_count == 1)
           {
               g_LastSnapshot = g_MatchReplay->AddSnapshot(g_SoccerPitch);
           }
       }
    }

    json result;
    if (LOG_MATCH_OUTPUT)
    {
       json raw_data = g_MatchReplay->Snapshots();
       result["snapshot"] = raw_data.dump();
       //std::ofstream o("match_server.json");
       //o << std::setw(4) << raw_data.dump() << std::endl;
    }

    auto score1 = g_SoccerPitch->HomeTeam()->OpponentsGoal()->NumGoalsScored();
    auto score2 = g_SoccerPitch->AwayTeam()->OpponentsGoal()->NumGoalsScored();

    if (LOG_MATCH_OUTPUT)
    {
       delete g_MatchReplay;
    }

    delete g_SoccerPitch;

    //result["seed"]   = seed;
    result["score1"] = score1;
    result["score2"] = score2;

    //std::cout << "score1 : " << score1 << std::endl;
    //std::cout << "score2 : " << score2 << std::endl;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Create a msg, to be passed back as Candid over the wire
  std::string msg;
  msg.append(result.dump());
  //msg.append("Your principal is: " + caller.get_text());

  // Send the response back
  ic_api.to_wire(CandidTypeText{msg});
}

#endif