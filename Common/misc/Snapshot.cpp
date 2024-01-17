#include "Snapshot.h"

//JSON Format
// {
//  "Entities": {
//    {"Model": "ball", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "green", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "black", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]}
//  },
//  "Entities": {
//    {"Model": "ball", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "green", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "red", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "black", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]},
//    {"Model": "blue", "Position": ["10", "50", "0"], "Heading": ["10", "50", "0"]}
//  }
// }

json Snapshot::AddSnapshot(SoccerPitch* soccer_pitch)
{
    json entities;
    json ball;

    ball["id"] = "0";
    ball["p"] = { soccer_pitch->Ball()->Pos().GetX(), soccer_pitch->Ball()->Pos().GetZ(), 0};
    ball["h"] = { soccer_pitch->Ball()->Heading().GetX(), soccer_pitch->Ball()->Heading().GetZ(), 0 };
    entities.push_back(ball);

    std::vector<PlayerBase*>::const_iterator it = soccer_pitch->HomeTeam()->Members().begin();
    for (it; it != soccer_pitch->HomeTeam()->Members().end(); ++it)
    {
        json player;

        player["id"] = (*it)->ID();
        player["p"] = { (*it)->Pos().GetX(), (*it)->Pos().GetZ(), 0 };
        player["h"] = { (*it)->Heading().GetX(), (*it)->Heading().GetZ(), 0 };
        entities.push_back(player);
        
    }

    it = soccer_pitch->AwayTeam()->Members().begin();
    for (it; it != soccer_pitch->AwayTeam()->Members().end(); ++it)
    {
        json player;

        player["id"] = (*it)->ID();
        player["p"] = { (*it)->Pos().GetX(), (*it)->Pos().GetZ(), 0 };
        player["h"] = { (*it)->Heading().GetX(), (*it)->Heading().GetZ(), 0 };
        entities.push_back(player);

    }

    json screensnap;
    screensnap["scrn"] = entities;

    m_Snapshots["scrnsnaps"].push_back(screensnap);
    return screensnap;
}
