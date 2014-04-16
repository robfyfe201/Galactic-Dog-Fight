/*
=================
main.cpp
Main entry point for the Card application
=================
*/

#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cEnemy.h"
#include "cSprite.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"

using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

D3DXVECTOR2 enemyScale = D3DXVECTOR2 (0.4f, 0.4f);
D3DXVECTOR2 playerScale = D3DXVECTOR2 (0.4f,0.4f);

D3DXVECTOR2 enemyTrans = D3DXVECTOR2(0.0f, 0.0f);
D3DXVECTOR2 playerTrans = D3DXVECTOR2(300.0f, 300.0f);

vector<cEnemy*> aEnemy;
vector<cEnemy*>::iterator iter;
vector<cEnemy*>::iterator iterA;
vector<cEnemy*>::iterator iterB;
vector<cEnemy*>::iterator iterC;
vector<cEnemy*>::iterator iterD;

vector<cSprite*> playerBullet;
vector<cSprite*>::iterator bulletiter;
vector<cSprite*>::iterator Bulletiter;



float enemy = 50;

short Bullets = 0;
short shootingDelay = 0; 
int planetShields = 20; 

RECT clientBounds;

TCHAR szTempOutput[30];

//player controls
bool leftArrow, rightArrow, upArrow, downArrow; 

bool gHit = false;
int gScore = 0;
char gScoreStr[50];
char gScorestr[50];

D3DXVECTOR3 expPos;
list<cExplosion*> gExplode;

cXAudio gExplodeSound;
cXAudio gBackingMusic;

cD3DXTexture* gameTextures[5];
char* gameTxtres[] = { "Images\\enemy1.png", "Images\\enemy2.png", "Images\\enemy3.png", "Images\\bullet.png", "Images\\explosion.png"};

//Scene management
short currentScene = 0;


/*
==================================================================
* LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
* LPARAM lParam)
* The window procedure
==================================================================
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{
		case WM_KEYDOWN:
			{
				//Controls when the player starts the game
				if (wParam == 'A')
				{
					if (currentScene == 1)
					{
						currentScene = 0;
					}
					 else if (currentScene == 0)
					{
						currentScene = 1;
					}
					return 0;
				}
				//Controls when the player can restart the game
				if (wParam == 'R')
				{
					if (currentScene == 2)
					{
						currentScene = 1;
					}
					return 0;
				}
				//Controls when the bullet is fired from the players ship
				if (wParam == VK_SPACE)
				{
					if ((Bullets <= 100 && shootingDelay == 0))
					{
						shootingDelay++;
						cSprite* theplayerBullet = new cSprite();
						theplayerBullet->setTexture(gameTextures[3]);
						theplayerBullet->setSpritePos(D3DXVECTOR3(playerTrans.x, playerTrans.y, 0.0f));
						theplayerBullet->setTranslation(D3DXVECTOR2(0, -2000.0f));
						theplayerBullet->setisActive(true);
						playerBullet.push_back(theplayerBullet);
						Bullets++;
					}
					return 0;			
					}
				//Player movement controls
						if (wParam == VK_LEFT)
						{
							leftArrow = true;
							return 0;
						}
						if (wParam == VK_RIGHT)
						{
							rightArrow = true;
							return 0;
						}
						if (wParam == VK_UP)
						{
							upArrow = true;
							return 0;
						}
						if (wParam == VK_DOWN)
						{
							downArrow = true;
							return 0;
						}
							return 0;
				}
				case WM_KEYUP:
				{
						 if (wParam == VK_LEFT)
						 {
							 leftArrow = false;
							 return 0;
						 }
						 if (wParam == VK_RIGHT)
						 {
							 rightArrow = false;
							 return 0;
						 }
						 if (wParam == VK_DOWN)
						 {
							 downArrow = false;
							 return 0;
						 }
						 if (wParam == VK_UP)
						 {
							 upArrow = false;
							 return 0;
						 }
						 return 0;
		}

		case WM_CLOSE:
			{
			// Exit the Game
				PostQuitMessage(0);
				 return 0;
			}

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
	}
	// Always return the message to the default window
	// procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);

}

/*
==================================================================
* bool initWindow( HINSTANCE hInstance )
* initWindow registers the window class for the application, creates the window
==================================================================
*/
bool initWindow( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcex.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GUNSIGHT));// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "Galactic Dog Fight"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance,"planet.png"); // the handle to the small icon

	RegisterClassEx(&wcex);
	// Create the window
	wndHandle = CreateWindow("Galactic Dog Fight",			// the window class to use
							 "Galactic Dog Fight",			// the title bar text
							WS_OVERLAPPEDWINDOW,	// the window style
							CW_USEDEFAULT, // the starting x coordinate
							CW_USEDEFAULT, // the starting y coordinate
							800, // the pixel width of the window
							600, // the pixel height of the window
							NULL, // the parent window; NULL for desktop
							NULL, // the menu for the application; NULL for none
							hInstance, // the handle to the application instance
							NULL); // no values passed to the window
	// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;

	float numFrames   = 0.0f;				// Used to hold the number of frames
	float timeElapsed = 0.0f;				// cumulative elapsed time

	GetClientRect(wndHandle,&clientBounds);


	float fpsRate = 1.0f/35.0f;


	//Play the awesome backing track
	gBackingMusic.playSound(L"Sounds\\BackingMusic.wav", true);


	/* initialize random seed: */
	D3DXMATRIX enemyTransform;
	D3DXVECTOR3 enemyPos; 


	//====================================================================
	// Load all textures for the game
	//====================================================================
	for (int txture = 0; txture < 5; txture++)
	{
		gameTextures[txture] = new cD3DXTexture(d3dMgr->getTheD3DDevice(), gameTxtres[txture]);
	}

	int noOfEnemies = 0; // Number of alien ships on the screen
	float delay = 0;   //delay

	LPDIRECT3DSURFACE9 menuSurface;				// the Direct3D menu surface
	LPDIRECT3DSURFACE9 gameSurface;				// the Direct3D game surface
	LPDIRECT3DSURFACE9 gameoverSurface;				// the Direct3D game surface

	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer

	//Set starting position and instantiate the player
	cD3DXTexture* gameTxtr = new cD3DXTexture(d3dMgr->getTheD3DDevice(), "Images\\Spaceship.png");
	cSprite thePlayer;
	thePlayer.setTranslation(playerTrans);
	thePlayer.setTexture(gameTxtr);


	//Transformation Matrix for the player
	D3DXMATRIX playerTransform;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	// Create the background surfaces for the different screens
	gameSurface = d3dMgr->getD3DSurfaceFromFile("Images\\starscape.jpg");
	menuSurface = d3dMgr->getD3DSurfaceFromFile("Images\\MenuBackground.png");
	gameoverSurface = d3dMgr->getD3DSurfaceFromFile("Images\\GameOverBackground.png");

	// load custom font
	cD3DXFont* balloonFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "JI Balloon Caps");

	//Set size of box for score text
	RECT textPos;
	SetRect(&textPos, 10, 10, 550, 100);
	//Set the size of the box for the players ships power
	RECT textPos2;
	SetRect(&textPos2,10, 50, 450, 640);

	//Enemy variables
	int i = 0;
	short enemiesonScreen = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);


	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// Game code goes here
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			float dt = (currentTime - previousTime)*sPC;

			// Accumulate how much time has passed.
			timeElapsed += dt;

			//menu screen
			if (currentScene == 0)
			{				
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(menuSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dxSRMgr->beginDraw();
				d3dxSRMgr->endDraw();
				d3dMgr->endRender();
				timeElapsed = 0.0f;
			}
			//gameover screen
			if (currentScene == 1 && planetShields <=0)
			{
				planetShields = 20;
				gScore = 0;
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(gameoverSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dxSRMgr->beginDraw();
				d3dxSRMgr->endDraw();
				d3dMgr->endRender();
				timeElapsed = 0.0f;
				currentScene = 2;
			}

			//game screen			
			if (currentScene == 1)
			{
				int random = 0;
				random += rand() % 3;
				//Create the crazy aliens!
				if (gScore >= 0)
				{
					//Enemy Creation Logic
					if (noOfEnemies == 0 && enemiesonScreen < 1000)
					{
						noOfEnemies++;
						enemiesonScreen++;
						//Create a random spawning position
						if (enemyPos.x < 600)
						{
							if (random == 1)
								enemy += 150;
							else if (random == 0)
								enemy += 220;
							else if (random == 2 && enemyPos.x >= 100)
								enemy -= 150;
							else if (random == 2 && enemyPos.x < 100)
								enemy += enemy;
							else if (random == 3)
								enemy -= 100;
						}

						//Adding everything needed for the enemy ships
						else
						enemy = 50;
						enemyPos = D3DXVECTOR3(enemy, -130, 0.0f);
						aEnemy.push_back(new cEnemy());
						aEnemy[i]->setSpritePos(enemyPos);
						aEnemy[i]->setTranslation(enemyTrans);
						aEnemy[i]->setTexture(gameTextures[rand() % 3]);
						i++;
					}
				}
				

				//Transformation matrix
				D3DXMatrixTransformation2D(&playerTransform, NULL, 0.0f, &playerScale, &(thePlayer.getSpriteCentre()), NULL, &playerTrans);

				//Movement speed of the player
				if (leftArrow)
					playerTrans.x -= 1.0f;
				if (rightArrow)
					playerTrans.x += 1.0f;
				if (downArrow)
					playerTrans.y += 0.6f;
				if (upArrow)
					playerTrans.y -= 0.6f;

				// Keeps the player ship inside the screen
				if (playerTrans.x < 0)
					playerTrans.x = 0;
				if (playerTrans.x >= 730)
					playerTrans.x = 730;
				if (playerTrans.y < 0)
					playerTrans.y = 0;
				if (playerTrans.y >= 500)
					playerTrans.y = 500;

				//Enemy movement and difficulty increase when reaching certain scores
				for (iterA = aEnemy.begin(); iterA != aEnemy.end(); ++iterA)
				{
					if (gScore <10)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0, 0.2f, 0));
					}
					if (gScore >= 10 && gScore < 20)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0, 0.3f, 0));
					}
					if (gScore >= 20 && gScore < 30)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0, 0.4f, 0));
					}
					if (gScore >= 30 && gScore < 50)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0, 0.6f, 0));
					}
					if (gScore >= 50 && gScore < 1000)
					{
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0, 1.0f, 0));
					}
					if ((*iterA)->getSpritePos().x >= 0)
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0, 0.2f, 0));
					if ((*iterA)->getSpritePos().y < -130)
						(*iterA)->setSpritePos((*iterA)->getSpritePos() + D3DXVECTOR3(0, 0.2f, 0));
				}

				//Destroy enemies that go past the screem edge
				//Minus 1 from the planets shield for being bad and missing an enemy ship!
				iterB = aEnemy.begin();
				while (iterB != aEnemy.end())
				{
					if ((*iterB)->getSpritePos().y > 810.0f)
					{
						planetShields --;
						enemiesonScreen--;
						i--;
						if (noOfEnemies >= 1)
						{
							noOfEnemies = 0;
						}
						iterB = aEnemy.erase(iterB);
					}
					else
					{
						++iterB;
					}
				}
				
				//Begins the bullet iteration
				bulletiter = playerBullet.begin();
				while (bulletiter != playerBullet.end())
				{
					if ((*bulletiter)->getSpritePos2D().y < -20.0f)
					{
						bulletiter = playerBullet.erase(bulletiter);
						Bullets--;
					}
					else
					{
						++bulletiter;
					}
				}
				//Bullet Delay
				for (bulletiter = playerBullet.begin(); bulletiter != playerBullet.end(); ++bulletiter)
				{
					if ((*bulletiter)->getSpritePos().y >= 250.0f)
					{
						shootingDelay = 0;
					}
				}

				//Collision detection
				Bulletiter = playerBullet.begin();
				iterD = aEnemy.begin();
				while (Bulletiter != playerBullet.end())
				{
					while (iterD != aEnemy.end())
					{
						if ((*Bulletiter)->collidedWith((*Bulletiter)->getBoundingRect(), (*iterD)->getBoundingRect()))
						{
							OutputDebugString("Hit!\n");
							OutputDebugString("Collision!!");
							gExplode.push_back(new cExplosion(expPos, gameTextures[3]));
							gExplodeSound.playSound(L"Sounds\\explosion.wav", false);
							noOfEnemies--;
							enemiesonScreen--;
							Bullets--;
							i--;
							shootingDelay = 0;
							gScore++;
							//Delete collisions
							Bulletiter = playerBullet.erase(Bulletiter);
							iterD = aEnemy.erase(iterD);
						}
			
						else
						{
							++iterD;
						}
					}
					if (Bulletiter != playerBullet.end())
					{
						++Bulletiter;
					}
				}

				//Create both the text for the Score board and the ships power
				sprintf_s(gScorestr, 50, "Score : %d", gScore);
				sprintf_s(gScoreStr, 50, "Planets Shield : %d", planetShields);

				//Begins drawing the surfaces
				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(gameSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);
				d3dxSRMgr->beginDraw();

				//Draw the player
				d3dxSRMgr->setTheTransform(playerTransform);
				d3dxSRMgr->drawSprite(thePlayer.getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);

				//Draw enemies
				for (iter = aEnemy.begin(); iter != aEnemy.end(); ++iter)
				{
					(*iter)->update(timeElapsed);
					d3dxSRMgr->setTheTransform((*iter)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*iter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}


				//Bullet Update
				//Bullet Draw
				for (bulletiter = playerBullet.begin(); bulletiter != playerBullet.end(); ++bulletiter)
				{
					RECT tempRect;
					D3DXVECTOR2 tempV2;
					D3DXVECTOR3 tempV3;
					tempV2 = (*bulletiter)->getSpritePos2D() + ((*bulletiter)->getTranslation())*timeElapsed;
					tempV3 = D3DXVECTOR3(tempV2.x, tempV2.y, 0.0f);					
					(*bulletiter)->setSpritePos(tempV3);
					(*bulletiter)->update();
					d3dxSRMgr->setTheTransform((*bulletiter)->getSpriteTransformMatrix());
					d3dxSRMgr->drawSprite((*bulletiter)->getTexture(), NULL, NULL, NULL, 0xFFFFFFFF);
				}
				d3dxSRMgr->endDraw();
				balloonFont->printText(gScorestr, textPos);
				balloonFont->printText(gScoreStr, textPos2);
				d3dMgr->endRender();
				timeElapsed = 0.0f;
			}
		} 

		previousTime = currentTime;
	}
	d3dxSRMgr->cleanUp();
	d3dMgr->clean();
	return (int)msg.wParam;
}

	