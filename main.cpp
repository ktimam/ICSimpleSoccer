#ifndef LINUX
#ifndef WIN3D

#pragma warning (disable:4786)
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
//#include <time.h>

#include "SimSoccer/constants.h"
#include "Common/misc/utils.h"
#include "Common/Time/PrecisionTimer.h"
#include "Common/Game/EntityManager.h"
#include "SimSoccer/SoccerPitch.h"
#include "SimSoccer/SoccerTeam.h"
#include "SimSoccer/PlayerBase.h"
#include "SimSoccer/Goalkeeper.h"
#include "SimSoccer/FieldPlayer.h"
#include "SimSoccer/FieldGoal.h"
#include "SimSoccer/SteeringBehaviors.h"
#include "Common/misc/Snapshot.h"
#include "Common/json/json.hpp"
#include "Common/misc/Cgdi.h"
#include "SimSoccer/ParamLoader.h"
#include "Resource.h"
#include "Common/misc/WindowUtils.h"
#include "Common/Debug/DebugConsole.h"
#include "Common/misc/WinHttpWrapper.h"
#include "Common/Game/PhysicsManager.h"

using namespace WinHttpWrapper;
using namespace std;
using json = nlohmann::json;

#define PLAYER_STATE_INFO_ON

//#define SERVER_MODE
//#define CLIENT_MODE
#define LIVE_MODE
//#define REMOTE_MODE
//--------------------------------- Globals ------------------------------
//
//------------------------------------------------------------------------

const int MATCH_DURATION = 45;
const int MATCH_RATE = 6;

const int MILLI_IN_SECOND = 20;
const int MILLI_IN_MINUTE = 60 * 20;
const int SECOND_MAX_VALUE = 60;

const bool LOG_MATCH_OUTPUT = true;

const int SNAPSHOT_RATE = 5;

int mTickCount = 0;
bool mMatchFinished = false;

const char* g_szApplicationName = "Simple Soccer";
const char*	g_szWindowClassName = "MyWindowClass";

const wstring REMOTE_API_SERVER_URL = L"localhost";
int REMOTE_API_SERVER_PORT = 3010;
bool REMOTE_API_SERVER_HTTPS = false;
const wstring requestHeader = L"Content-Type: application/json";

SoccerPitch* g_SoccerPitch;
Snapshot*    g_MatchReplay;
json         g_LastSnapshot;

int g_FinalScore1 = 0;
int g_FinalScore2 = 0;

//the vertex buffer
std::vector<Vector2D>   g_vecPlayerVB;
//the buffer for the transformed vertices
std::vector<Vector2D>   g_vecPlayerVBTrans;

//create a timer
PrecisionTimer timer(Prm.FrameRate);

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
    std::ostringstream stringStream;
    stringStream << minutes << " : "  << seconds;
    std::string time = stringStream.str();
    return time;
}

//used when a user clicks on a menu item to ensure the option is 'checked'
//correctly
void CheckAllMenuItemsAppropriately(HWND hwnd)
{
   CheckMenuItemAppropriately(hwnd, IDM_SHOW_REGIONS, Prm.bRegions);
   CheckMenuItemAppropriately(hwnd, IDM_SHOW_STATES, Prm.bStates);
   CheckMenuItemAppropriately(hwnd, IDM_SHOW_IDS, Prm.bIDs);
   CheckMenuItemAppropriately(hwnd, IDM_AIDS_SUPPORTSPOTS, Prm.bSupportSpots);
   CheckMenuItemAppropriately(hwnd, ID_AIDS_SHOWTARGETS, Prm.bViewTargets);
   CheckMenuItemAppropriately(hwnd, IDM_AIDS_HIGHLITE, Prm.bHighlightIfThreatened);
}

bool RenderSoccerPitch()
{
    //draw the grass
    gdi->DarkGreenPen();
    gdi->DarkGreenBrush();
    gdi->Rect(0, 0, g_SoccerPitch->cxClient(), g_SoccerPitch->cyClient());

    //render regions
    if (Prm.bRegions)
    {
        for (unsigned int r = 0; r < g_SoccerPitch->Regions().size(); ++r)
        {
            //g_SoccerPitch->Regions()[r]->Render(true);

            gdi->HollowBrush();
            gdi->GreenPen();
            gdi->Rect(g_SoccerPitch->Regions()[r]->Left(), g_SoccerPitch->Regions()[r]->Top(), g_SoccerPitch->Regions()[r]->Right(), g_SoccerPitch->Regions()[r]->Bottom());
            
            gdi->TextColor(Cgdi::green);
            gdi->TextAtPos(Vector2D(g_SoccerPitch->Regions()[r]->Center().GetX(), g_SoccerPitch->Regions()[r]->Center().GetZ()), ttos(g_SoccerPitch->Regions()[r]->ID()));
        }
    }

    //render the goals
    gdi->HollowBrush();
    gdi->RedPen();
    gdi->Rect(g_SoccerPitch->PlayingArea()->Left(), (g_SoccerPitch->cyClient() - Prm.GoalWidth) / 2, g_SoccerPitch->PlayingArea()->Left() + 40, g_SoccerPitch->cyClient() - (g_SoccerPitch->cyClient() - Prm.GoalWidth) / 2);

    gdi->BluePen();
    gdi->Rect(g_SoccerPitch->PlayingArea()->Right(), (g_SoccerPitch->cyClient() - Prm.GoalWidth) / 2, g_SoccerPitch->PlayingArea()->Right() - 40, g_SoccerPitch->cyClient() - (g_SoccerPitch->cyClient() - Prm.GoalWidth) / 2);

    //render the pitch markings
    gdi->WhitePen();
    gdi->Circle(Vector2D(g_SoccerPitch->PlayingArea()->Center().GetX(), g_SoccerPitch->PlayingArea()->Center().GetZ()), g_SoccerPitch->PlayingArea()->Width() * 0.125);
    gdi->Line(g_SoccerPitch->PlayingArea()->Center().GetX(), g_SoccerPitch->PlayingArea()->Top(), g_SoccerPitch->PlayingArea()->Center().GetX(), g_SoccerPitch->PlayingArea()->Bottom());
    gdi->WhiteBrush();
    gdi->Circle(Vector2D(g_SoccerPitch->PlayingArea()->Center().GetX(), g_SoccerPitch->PlayingArea()->Center().GetZ()), 2.0);


    //the ball
    gdi->WhitePen();
    gdi->WhiteBrush();
    //m_pBall->Render();

    bool empty = g_LastSnapshot.empty();
    if ( !empty)
    {
        gdi->BlackBrush();
        json::iterator it = g_LastSnapshot["scrn"].begin();
        json::iterator entity_position_json = it.value()["p"].begin();
        json entity_position_x = entity_position_json.value();
        json entity_position_y = (++entity_position_json).value();
        Vector2D entity_position = Vector2D(entity_position_x, entity_position_y);
        gdi->Circle(entity_position, Prm.BallSize);

        //Render the teams
        /*m_pRedTeam->Render();
        m_pBlueTeam->Render(); */
        // iterate the array
        for (++it; it != g_LastSnapshot["scrn"].end(); ++it) {

            int id = it.value()["id"];// .begin().value();
            PlayerBase* player = (PlayerBase*)EntityMgr->GetEntityFromID(id);
            entity_position_json = it.value()["p"].begin();
            entity_position_x = entity_position_json.value();
            entity_position_y = (++entity_position_json).value();
            Vector2D entity_position = Vector2D(entity_position_x, entity_position_y);

            json::iterator entity_heading_json = it.value()["h"].begin();
            json entity_heading_x = entity_heading_json.value();
            json entity_heading_y = (++entity_heading_json).value();
            Vector2D entity_heading = Vector2D(entity_heading_x, entity_heading_y);

            gdi->TransparentText();
            gdi->TextColor(Cgdi::grey);

            //set appropriate team color
            if (player->Team()->Color() == SoccerTeam::blue) { gdi->BluePen(); }
            else { gdi->RedPen(); }



            //render the player's body
            g_vecPlayerVBTrans = WorldTransform(g_vecPlayerVB,
                entity_position,
                entity_heading,
                entity_heading.Perp(),
                Vector2D(Prm.PlayerScale, Prm.PlayerScale));
            gdi->ClosedShape(g_vecPlayerVBTrans);

            //and 'is 'ead
            gdi->BrownBrush();
            gdi->Circle(entity_position, 6);


            //and 'is 'ead
            gdi->BrownBrush();
#ifdef LIVE_MODE
            if (Prm.bHighlightIfThreatened && (player->Team()->ControllingPlayer() == player) && player->isThreatened()) gdi->YellowBrush();
#endif
            gdi->Circle(entity_position, 6);

#ifdef LIVE_MODE
            //render the state
            if (Prm.bStates)
            {
                gdi->TextColor(0, 170, 0);
                gdi->TextAtPos(entity_position.x, entity_position.y - 20, player->GetCurrentStateName());
            }

            //show IDs
            if (Prm.bIDs)
            {
                gdi->TextColor(0, 170, 0);
                gdi->TextAtPos(entity_position.x - 20, entity_position.y - 20, ttos(player->ID()));
            }


            if (Prm.bViewTargets)
            {
                gdi->RedBrush();
                gdi->Circle(Vector2D(player->Steering()->Target().GetX(), player->Steering()->Target().GetZ()), 3);
                gdi->TextAtPos(Vector2D(player->Steering()->Target().GetX(), player->Steering()->Target().GetZ()), ttos(player->ID()));
            }
            //gdi->WhitePen();
            //gdi->Line(entity_position.x, entity_position.y, player->Steering()->Target().GetX(), player->Steering()->Target().GetZ());


            gdi->ThickBluePen();
            double forwardforce = player->Steering()->ForwardComponent()*10;
            Vec3 force = player->Heading() * forwardforce;
            force = force + player->Pos();
            gdi->Line(entity_position.x, entity_position.y, force.GetX(), force.GetZ());
#endif
        }
    }

    //render the walls
    gdi->WhitePen();
    bool RenderNormals = false;
    for (unsigned int w = 0; w < g_SoccerPitch->Walls().size(); ++w)
    {
        gdi->Line(g_SoccerPitch->Walls()[w].From(), g_SoccerPitch->Walls()[w].To());

        //render the normals if rqd
        if (RenderNormals)
        {
            int MidX = (int)((g_SoccerPitch->Walls()[w].From().x+ g_SoccerPitch->Walls()[w].To().x)/2);
            int MidY = (int)((g_SoccerPitch->Walls()[w].From().y+ g_SoccerPitch->Walls()[w].To().y)/2);

            gdi->Line(MidX, MidY, (int)(MidX+(g_SoccerPitch->Walls()[w].Normal().x * 5)), (int)(MidY+(g_SoccerPitch->Walls()[w].Normal().y * 5)));
        }
    }
    gdi->TextColor(Cgdi::black);
    gdi->TextAtPos((g_SoccerPitch->cxClient() / 2) - 10, 2, GetCurrentTimeString());

#ifdef LIVE_MODE
    //render the sweet spots
    if (Prm.bSupportSpots)
    {
        SoccerTeam* controllingteam = g_SoccerPitch->HomeTeam();
        if (g_SoccerPitch->AwayTeam()->InControl())
        {
            controllingteam = g_SoccerPitch->AwayTeam();
        }

        gdi->HollowBrush();
        gdi->GreyPen();

        SupportSpotCalculator* ssc = controllingteam->GetSupportSpotCalculator();
        const std::vector<SupportSpotCalculator::SupportSpot>& SupportSpots = ssc->SupportSpots();
        for (unsigned int spt = 0; spt < SupportSpots.size(); ++spt)
        {
            gdi->Circle(Vector2D(SupportSpots[spt].m_vPos.GetX(), SupportSpots[spt].m_vPos.GetZ()), SupportSpots[spt].m_dScore);
        }

        if (ssc->BestSupportingSpot())
        {
            gdi->GreenPen();
            gdi->Circle(Vector2D(ssc->BestSupportingSpot()->m_vPos.GetX(), ssc->BestSupportingSpot()->m_vPos.GetZ()), ssc->BestSupportingSpot()->m_dScore);
        }
    }
#endif
// 
    //show the score
    gdi->TextColor(Cgdi::red);
    gdi->TextAtPos((g_SoccerPitch->cxClient() /2)-50, g_SoccerPitch->cyClient() -18, "Red: " + ttos(g_SoccerPitch->HomeTeam()->OpponentsGoal()->NumGoalsScored()));

    gdi->TextColor(Cgdi::blue);
    gdi->TextAtPos((g_SoccerPitch->cxClient() /2)+10, g_SoccerPitch->cyClient() -18, "Blue: " + ttos(g_SoccerPitch->AwayTeam()->OpponentsGoal()->NumGoalsScored()));

    return true;
}

//---------------------------- WindowProc ---------------------------------
//	
//	This is the callback function which handles all the windows messages
//-------------------------------------------------------------------------

LRESULT CALLBACK WindowProc (HWND   hwnd,
                             UINT   msg,
                             WPARAM wParam,
                             LPARAM lParam)
{
 
   //these hold the dimensions of the client window area
	 static int cxClient, cyClient; 

	 //used to create the back buffer
   static HDC		hdcBackBuffer;
   static HBITMAP	hBitmap;
   static HBITMAP	hOldBitmap;

    switch (msg)
    {
	
		//A WM_CREATE msg is sent when your application window is first
		//created
    case WM_CREATE:
      {
         //to get get the size of the client window first we need  to create
         //a RECT and then ask Windows to fill in our RECT structure with
         //the client window size. Then we assign to cxClient and cyClient 
         //accordingly
			   RECT rect;

			   GetClientRect(hwnd, &rect);

			   cxClient = rect.right;
			   cyClient = rect.bottom;

#ifdef REMOTE_MODE
               HttpRequest req(REMOTE_API_SERVER_URL, REMOTE_API_SERVER_PORT, REMOTE_API_SERVER_HTTPS);
               HttpResponse response;

               cout << "Action: REMOTE api server - input" << endl;
               req.Post(L"/api/input",
                   requestHeader,
                   R"({"payload":"hello"})",
                   response);
               json response_json = json::parse(response.text);
               int epoch = response_json["epoch_index"];
               int input = response_json["input_index"];
               json query_json;
               query_json["epoch"] = epoch;
               query_json["input"] = input;
               string query_text = query_json.dump();
               cout << "Returned Status:" << response.statusCode << endl;
               cout << "Content Length:" << response.contentLength << endl << endl;
               //PrintDictionary(response.GetHeaderDictionary());
               //wcout << endl << response.header << endl;
               response.Reset();

               cout << "Action: REMOTE api server - notice" << endl;
               req.Post(L"/api/notice_list",
                   requestHeader,
                   query_text,
                   response);
               while (response.text == "[]")
               {
                   //Match result not retrieved yet, try again
                   cout << "Match result not retrieved yet, trying again..." << endl;
                   response.Reset(); 
                   Sleep(100);
                   req.Post(L"/api/notice_list",
                       requestHeader,
                       query_text,
                       response);
               }
               json match_json = json::parse(response.text);
               json::iterator it = match_json.begin();
               string payload = it.value()["payload"];
               json payloadjson = json::parse(payload);
               unsigned seed = payloadjson["seed"];
               g_FinalScore1 = payloadjson["score1"];
               g_FinalScore2 = payloadjson["score2"];
               cout << "Returned Status:" << response.statusCode << endl;
               cout << "Content Length:" << response.contentLength << endl << endl;
               //PrintDictionary(response.GetHeaderDictionary());
               //wcout << endl << response.header << endl;
               response.Reset();

               std::cout << "Using Seed : " << seed << std::endl;
               std::cout << "FinalScore1 : " << g_FinalScore1 << std::endl;
               std::cout << "FinalScore2 : " << g_FinalScore2 << std::endl;
               srand(1674374940);// seed);
#else
         //seed random number generator
         srand((unsigned)time(0));
#endif

         
         //---------------create a surface to render to(backbuffer)

         //create a memory device context
         hdcBackBuffer = CreateCompatibleDC(NULL);

         //get the DC for the front buffer
         HDC hdc = GetDC(hwnd);

         hBitmap = CreateCompatibleBitmap(hdc,
                                          cxClient,
                                          cyClient);

			  
         //select the bitmap into the memory device context
			   hOldBitmap = (HBITMAP)SelectObject(hdcBackBuffer, hBitmap);

         //don't forget to release the DC
         ReleaseDC(hwnd, hdc); 
         
         PhysicsManager::Instance()->init();

         g_SoccerPitch = new SoccerPitch(WindowWidth, WindowHeight);
         g_MatchReplay = new Snapshot();
         //setup the vertex buffers and calculate the bounding radius
         const int NumPlayerVerts = 4;
         const Vector2D player[NumPlayerVerts] = { Vector2D(-3, 8),
                                                   Vector2D(3,10),
                                                   Vector2D(3,-10),
                                                   Vector2D(-3,-8) };

         for (int vtx = 0; vtx < NumPlayerVerts; ++vtx)
         {
             g_vecPlayerVB.push_back(player[vtx]);
         }

         CheckAllMenuItemsAppropriately(hwnd);

      }

      break;

    case WM_COMMAND:
      {
        switch(wParam)
        {
          case ID_AIDS_NOAIDS:

            Prm.bStates        = 0;
            Prm.bRegions       = 0;
            Prm.bIDs           = 0;
            Prm.bSupportSpots  = 0;
            Prm.bViewTargets   = 0;

            CheckAllMenuItemsAppropriately(hwnd);

            break;
            
          case IDM_SHOW_REGIONS:

            Prm.bRegions = !Prm.bRegions;

            CheckAllMenuItemsAppropriately(hwnd);

            break;

          case IDM_SHOW_STATES:

            Prm.bStates = !Prm.bStates;

            CheckAllMenuItemsAppropriately(hwnd);

            break;

          case IDM_SHOW_IDS:

            Prm.bIDs = !Prm.bIDs;

            CheckAllMenuItemsAppropriately(hwnd);

            break;


          case IDM_AIDS_SUPPORTSPOTS:

            Prm.bSupportSpots = !Prm.bSupportSpots;

            CheckAllMenuItemsAppropriately(hwnd);

             break;

           case ID_AIDS_SHOWTARGETS:

            Prm.bViewTargets = !Prm.bViewTargets;

            CheckAllMenuItemsAppropriately(hwnd);

             break;
              
           case IDM_AIDS_HIGHLITE:

            Prm.bHighlightIfThreatened = !Prm.bHighlightIfThreatened; 

            CheckAllMenuItemsAppropriately(hwnd);

            break;
            
        }//end switch
      }

      break;


    case WM_KEYUP:
      {
        switch(wParam)
        {
           case VK_ESCAPE:
            {             
              SendMessage(hwnd, WM_DESTROY, NULL, NULL);
            }
          
            break;

          case 'R':
            {
               delete g_SoccerPitch;
           
               g_SoccerPitch = new SoccerPitch(cxClient, cyClient);
            }

            break;

          case 'P':
            {
              g_SoccerPitch->TogglePause();
            }

            break;

        }//end switch
        
      }//end WM_KEYUP

      break;

    
    case WM_PAINT:
      {
 		       
         PAINTSTRUCT ps;
          
         BeginPaint (hwnd, &ps);
         
         gdi->StartDrawing(hdcBackBuffer);
         
         //g_SoccerPitch->Render();
         RenderSoccerPitch();

         gdi->StopDrawing(hdcBackBuffer);

        

         //now blit backbuffer to front
			   BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdcBackBuffer, 0, 0, SRCCOPY); 
          
         EndPaint (hwnd, &ps);

      }

      break;

    //has the user resized the client area?
		case WM_SIZE:
		  {
        //if so we need to update our variables so that any drawing
        //we do using cxClient and cyClient is scaled accordingly
			  cxClient = LOWORD(lParam);
			  cyClient = HIWORD(lParam);

      //now to resize the backbuffer accordingly. First select
      //the old bitmap back into the DC
			SelectObject(hdcBackBuffer, hOldBitmap);

      //don't forget to do this or you will get resource leaks
      DeleteObject(hBitmap); 

			//get the DC for the application
      HDC hdc = GetDC(hwnd);

			//create another bitmap of the same size and mode
      //as the application
      hBitmap = CreateCompatibleBitmap(hdc,
											cxClient,
											cyClient);

			ReleaseDC(hwnd, hdc);
			
			//select the new bitmap into the DC
      SelectObject(hdcBackBuffer, hBitmap);

      }

      break;
          
		 case WM_DESTROY:
			 {

         //clean up our backbuffer objects
         SelectObject(hdcBackBuffer, hOldBitmap);

         DeleteDC(hdcBackBuffer);
         DeleteObject(hBitmap); 
         
         // kill the application, this sends a WM_QUIT message  
				 PostQuitMessage (0);
			 }

       break;

     }//end switch

     //this is where all the messages not specifically handled by our 
		 //winproc are sent to be processed
		 return DefWindowProc (hwnd, msg, wParam, lParam);
}

//-------------------------------- WinMain -------------------------------
//
//	The entry point of the windows program
//------------------------------------------------------------------------
int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR     szCmdLine, 
                    int       iCmdShow)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

  //handle to our window
  HWND						hWnd;
    
  //our window class structure
  WNDCLASSEX     winclass;
		 
  // first fill in the window class stucture
  winclass.cbSize        = sizeof(WNDCLASSEX);
  winclass.style         = CS_HREDRAW | CS_VREDRAW;
  winclass.lpfnWndProc   = WindowProc;
  winclass.cbClsExtra    = 0;
  winclass.cbWndExtra    = 0;
  winclass.hInstance     = hInstance;
  winclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  winclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  winclass.hbrBackground = NULL;
  winclass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
  winclass.lpszClassName = g_szWindowClassName;
  winclass.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

  //register the window class
  if (!RegisterClassEx(&winclass))
  {
    MessageBox(NULL, "Registration Failed!", "Error", 0);

    //exit the application
    return 0;
  }

  //create the window and assign its ID to hwnd    
  hWnd = CreateWindowEx (NULL,                 // extended style
                         g_szWindowClassName,  // window class name
                         g_szApplicationName,  // window caption
                         WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                         GetSystemMetrics(SM_CXSCREEN)/2 - WindowWidth/2,
                         GetSystemMetrics(SM_CYSCREEN)/2 - WindowHeight/2,                    
                         WindowWidth + 10,     // initial x size
                         WindowHeight + 70,    // initial y size
                         NULL,                 // parent window handle
                         NULL,                 // window menu handle
                         hInstance,            // program instance handle
                         NULL);                // creation parameters

  //make sure the window creation has gone OK
  if(!hWnd)
  {
    MessageBox(NULL, "CreateWindowEx Failed!", "Error!", 0);
  }
  
  //start the timer
  timer.Start();

  MSG msg;
  int updates_count = 0;

#ifdef SERVER_MODE
  while(!mMatchFinished)
  {
      /*while (!timer.ReadyForNextFrame())
      {
          Sleep(2);
      }*/

      IncrementTime(1);

      //update game states
      g_SoccerPitch->Update();
      PhysicsManager::Instance()->Update();
      g_SoccerPitch->CheckGoal();
      updates_count++;
      //Don't take snapshot for every move
      if (updates_count % SNAPSHOT_RATE == 1 || updates_count == 1) 
      {
        g_LastSnapshot = g_MatchReplay->AddSnapshot(g_SoccerPitch);
      }   					
  }//end while
  // write prettified JSON to another file// , std::ios::out | std::ios::binary | std::ios::ate);
  json raw_data = g_MatchReplay->Snapshots();
  // create a JSON value
  //std::vector<std::uint8_t> v_bson = json::to_msgpack(raw_data);
  //std::ofstream obson("match.msgpack.json", std::ios::out | std::ios::binary | std::ios::ate);
  //// print the vector content
  //for (auto& byte : v_bson)
  //{
  //    obson << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
  //}
  //obson << std::endl;

  std::ofstream o("match.json");
  o << std::setw(4) << raw_data << std::endl;
#endif

  //enter the message loop
  bool bDone = false; 

  mTickCount = 0;

#ifdef CLIENT_MODE
  std::ifstream ifs("match.json");// , std::ios::in | std::ios::binary | std::ios::ate);
  std::string content;// ((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  ifs >> content;
  json data = json::parse(content);
  //std::vector<json>::const_iterator it = g_MatchReplay->Snapshots().begin();
  json::const_iterator it = data["scrnsnaps"].begin();
  for (it; it != data["scrnsnaps"].end(); ++it)
#else
  while(!bDone)
#endif // LIVE_MODE
  {
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
      {
          if (msg.message == WM_QUIT)
          {
              // Stop loop if it's a quit message
              bDone = true;
          }

          else
          {
              TranslateMessage(&msg);
              DispatchMessage(&msg);
          }
      }

      while (!timer.ReadyForNextFrame() && msg.message != WM_QUIT)
      {
          Sleep(2);
      }
      //update game states
#ifdef CLIENT_MODE
      IncrementTime(SNAPSHOT_RATE);
      g_LastSnapshot = (*it);
#else
      if (!mMatchFinished)
      {
          if (g_SoccerPitch->GameOn())
          {
            IncrementTime(1);
          }
          g_SoccerPitch->Update();
          PhysicsManager::Instance()->Update();
          g_SoccerPitch->CheckGoal();
         
          if (LOG_MATCH_OUTPUT)
          {
            updates_count++;
            //Don't take snapshot for every move
            if (updates_count % SNAPSHOT_RATE == 1 || updates_count == 1)
            {
                g_LastSnapshot = g_MatchReplay->AddSnapshot(g_SoccerPitch);
            }
            if (mMatchFinished)
            {
              json raw_data = g_MatchReplay->Snapshots();
              std::ofstream o("match_client.json");
              o << std::setw(4) << raw_data.dump() << std::endl;
            }
          }
      }
#endif // LIVE_MODE

      //render 
      RedrawWindow(hWnd, true);

  }//end while

  delete g_SoccerPitch;

  UnregisterClass( g_szWindowClassName, winclass.hInstance );

  return msg.wParam;
}

#endif
#endif
