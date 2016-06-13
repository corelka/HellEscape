#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <irrlicht.h>
#include <iostream>
#include <windows.h>
#include <time.h>
#include "library.h"
#include <driverChoice.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

IrrlichtDevice *device;
IVideoDriver *driver;
ISceneManager *smgr;
IGUIEnvironment *guienv;


wchar_t* about = L" Hell Escape v1.0 \n Created for xpn2015\n\n\n Inspired by #helenarebel"; // text for about window
bool state = false; // define viewing of menu or game process;
bool IsNewGame = true; 
stringw result(L"Your score is: ");
long float score = 0;
f32 randy = -10;
f32 randx = 0;
f32 g = -0.1; //gravitation
int diff = 0; //difficulty

IGUIButton* b1; // New Game button

block blocks[35]; // total number of platforms

class KeyReceiver : public IEventReceiver
{
private:
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
	IAnimatedMeshSceneNode* hero;
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
			hero->setRotation(vector3df(hero->getRotation().X, 0, hero->getRotation().Z));
			if (event.KeyInput.Key == KEY_ESCAPE && IsNewGame == false)
				state = !state;
		}
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType)
			{
			case EGET_BUTTON_CLICKED:
				if (id == 101)
				{
					event.GUIEvent.Caller->setText(L"Continue");
					IsNewGame = false;
					state = true;
				}
				if (id == 103)
				{
					if (IsNewGame)
						device->closeDevice();
					else
					{
						state = false;
						b1->setText(L"New Game");
						score = 0;
						g = -0.1;
						randy = -10;
						randx = 0;
						IsNewGame = true;
						diff = 0;
						hero->setPosition(vector3df(0, 0, 0.5));
						for (int i = 0; i < 30; i++)
						{
							
							randx += rand() % 30;
							if (randx > 15) randx -= 30;
							
							int tmpy = rand() % 4;
							if (i == 1)
							{
								while (tmpy < 1)
								{
									tmpy = rand() % 4;
								}
							}
							if (i == 0)randx = 0;
							randy += tmpy;
							blocks[i].node->setPosition(vector3df(randx, randy, 0));
						}
					}
				}
				if (id == 102)
				{
					guienv->addMessageBox(L"About", about, true, 1, 0, 108);
				}
				break;
			}
		}

		return false;
	}
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	KeyReceiver(IAnimatedMeshSceneNode* hero) :hero(hero)
	{
		for (u32 i = 0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}
};

void CreateGUI() //creation in-game menu with buttons "New game", "About", "Exit";
{
	guienv->getSkin()->setFont(guienv->getFont("../lib/irrlicht-1.8.3/media/fonthaettenschweiler.bmp"), EGDF_DEFAULT);
	b1 = guienv->addButton(rect<s32>(driver->getScreenSize().Width / 2 - 80, driver->getScreenSize().Height / 2 - 200, driver->getScreenSize().Width / 2 + 80, driver->getScreenSize().Height / 2 - 150), 0, 101, L"New Game");
	IGUIButton* b2 = guienv->addButton(rect<s32>(driver->getScreenSize().Width / 2 - 80, driver->getScreenSize().Height / 2 - 100, driver->getScreenSize().Width / 2 + 80, driver->getScreenSize().Height / 2 - 50), 0, 102, L"About");
	IGUIButton* b3 = guienv->addButton(rect<s32>(driver->getScreenSize().Width / 2 - 80, driver->getScreenSize().Height / 2, driver->getScreenSize().Width / 2 + 80, driver->getScreenSize().Height / 2 + 50), 0, 103, L"Exit");
}

void RenderAll(ITexture* back) // rendering of environment
{
	driver->beginScene(true, true, SColor(255, 155, 155, 155));
	driver->draw2DImage(back, position2d < s32 >(0, 0), rect < s32 >(0, 0, driver->getScreenSize().Width, driver->getScreenSize().Height), 0, SColor(255, 255, 255, 255), true);
	if (state == true)smgr->drawAll();
	if (state == false)guienv->drawAll();
	driver->endScene();
}


int main(int argc, char** argv)
{
	srand((unsigned)time(NULL));

	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if (driverType == video::EDT_COUNT)  return 1;
	
	device = createDevice(driverType, core::dimension2d< u32 >(1024, 768), 16, false);
	if (device == 0) return 1;

	device->setWindowCaption(L"Hell Escape");
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	CreateGUI();
	
	/*-----------------------------HERO_CREATION-----------------------------------------------------------------------------*/
	IAnimatedMeshSceneNode* hero = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../models_obj/knight.x"));
	if (hero)
	{
		hero->setMaterialFlag(EMF_LIGHTING, false);
		hero->setMaterialTexture(0, driver->getTexture("../models_obj/knight.tga"));
		hero->setPosition(vector3df(0, 0, 0.5));
		hero->setRotation(vector3df(90, 0, 180));
		hero->setLoopMode(false);
		hero->setAnimationSpeed(13);
	}

	/*-----------------------------------------------------------------------------------------------------------------------*/

	/*------------------------------CREATION_OF_BLOCKS-----------------------------------------------------------------------*/
		
	int y = -5;
	int x = 0;
	
	for (int i = 0; i < 30; i++)
	{
		randx += rand() % 30;
		if (randx > 15) randx -= 30;
		int tmp = rand() % 4;
		if (i == 1)
		{
			while (tmp < 1)
			{
				tmp = rand() % 4;
			}
		}
		if(i == 0)randx = 0;
		randy += tmp;
		blocks[i] = platform(blocks[i], smgr, driver, randx, randy);
	}
	for (int i = 0; i < 5; i++)
	{
		blocks[30 + i] = platform(blocks[30 + i], smgr, driver, -50, 0);
		set_breakable(blocks[30 + i], smgr, driver);
		blocks[30 + i].node->setFrameLoop(0, 0);
		blocks[30 + i].node->setAnimationSpeed(20);
		blocks[30 + i].node->setVisible(false);
	}

	IAnimatedMeshSceneNode* spring = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../models_obj/spring.x"));
	if (spring)
	{
		spring->setMaterialFlag(EMF_LIGHTING, false);
		spring->setMaterialTexture(0, driver->getTexture("../models_obj/platform01.tga"));
		spring->setFrameLoop(0, 0);
		spring->setLoopMode(false);
		spring->setCurrentFrame(0);
		blocks[0].node->addChild(spring);
		spring->setPosition(vector3df(0, 0.3, 0));
	}

	/*------------------------------------------------------------------------------------------------------------------------*/
	KeyReceiver NewRec(hero);
	device->setEventReceiver(&NewRec);

	ICameraSceneNode *camera = smgr->addCameraSceneNode(0, vector3df(0, 0, 20), vector3df(0, 0, 0));
	
	ITexture *back_tex = driver->getTexture("../models_obj/background.png");
	ITexture *calc_back = ResizeTexture(back_tex, driver, driver->getScreenSize().Width, driver->getScreenSize().Height);
	
	int last_pos = 0;
	
	int ToInvis = 0; // defines index of platform that should be invisible
	int time = 0; //counter.

	while (device->run())
		if (device->isWindowActive())
		{
			RenderAll(calc_back);

			if (state == true)
			{
				hero->setPosition(hero->getPosition() + vector3df(0, g, 0));
				g -= 0.001;
				if (fabs(g) < 0.0000009)last_pos = hero->getPosition().Y + 1;
				if (g > 0 && hero->getPosition().Y > last_pos)score += 0.1;
				//---------------MOVEMENT_(UP_DOWN)----------------------------------------------------------
				if (hero->getPosition().Y > 45)
				{
					hero->setPosition(vector3df(hero->getPosition().X, hero->getPosition().Y - 30, hero->getPosition().Z));
					for (int i = 0; i < 35; i++)
					{
						blocks[i].node->setPosition(vector3df(blocks[i].node->getPosition().X, blocks[i].node->getPosition().Y - 30, blocks[i].node->getPosition().Z));
					}
					randy = randy - 30;
				}
				if (hero->getPosition().Y < -15)//player loose
				{
					state = false;
					IsNewGame = true;
					hero->setPosition(vector3df(0, 30, 0.5));
					g = -0.1;
					b1->setText(L"New Game");

					randx = 0;
					randy = -10;
					for (int i = 0; i < 30; i++)
					{
						randx += rand() % 30;
						if (randx > 15) randx -= 30;
						randy += rand() % 4;
						if (i == 0)randx = 0;
						blocks[i].node->setPosition(vector3df(randx, randy, 0));
					}
					core::stringw tmp(L"Your Score is: ");
					tmp += (int)(score * 10);
					guienv->addMessageBox(L"Finish!", tmp.c_str(), true, 1, 0, 108, 0);
					score = 0;
				}
				//-------------------------------------------------------------------------------------------------------

				//---------------MOVEMENT_(LEFT_RIGHT to borders)--------------------------------------------------------
				if (hero->getPosition().X < -20)
					hero->setPosition(hero->getPosition() + vector3df(39, 0, 0));
				if (hero->getPosition().X > 20)
					hero->setPosition(hero->getPosition() + vector3df(-39, 0, 0));
				//-------------------------------------------------------------------------------------------------------

				for (int i = 0; i < 35; i++)//collision detection
				{

					if (hero->getTransformedBoundingBox().intersectsWithBox(blocks[i].node->getTransformedBoundingBox()))
						if (hero->getTransformedBoundingBox().getCenter().X >= blocks[i].node->getPosition().X - 0.5 && hero->getTransformedBoundingBox().getCenter().X <= blocks[i].node->getPosition().X + 2.5 && hero->getTransformedBoundingBox().getCenter().Y - 1.5 >= blocks[i].node->getPosition().Y && g <= 0)
						{
							if (i < 30)
							{
								hero->setFrameLoop(0, 50);
								if (!blocks[i].node->getChildren().empty())
								{
									if (spring->getTransformedBoundingBox().intersectsWithBox(hero->getTransformedBoundingBox()))
									{
										spring->setFrameLoop(0, 20);
										g = 0.2;
									}
									else
									{
										g = 0.1;
									}
								}
								else
								{
									g = 0.1;
								}
							}
							else
							{
								blocks[i].node->setFrameLoop(0, 15);
								ToInvis = i;
							}
						}


					if (blocks[i].node->getPosition().Y < (hero->getPosition().Y - 15))//if platform is out of visibility its position will be recalculated in order to highest platform
					{
						f32 x = rand() % 30;
						if (x > 15)x -= 30;
						int tmpy = rand() % 4;
						if (i == 1)
						{
							tmpy = rand() % 10;
							while (tmpy < 1)
							{
								tmpy = rand() % 4;
							}
						}

						blocks[i].node->setPosition(vector3df(x, randy + tmpy, 0));
						if (i < 30)randy += tmpy;
						if (i == 0) spring->setFrameLoop(0, 0);
					}
				}
				if (score > 100 && score < 200) // different difficulties
				{
					if (diff != 1)diff = 1;
				}
				else if (score > 200 && score < 300)
				{
					if (diff != 2)diff = 2;
				}
				else if (score > 300 && score < 400)
				{
					if (diff != 3)diff = 3;
				}
				else if (score > 400 && score < 500)
				{
					if (diff != 4)diff = 4;
				}
				else if (score > 500 && score < 600)
				{
					if (diff != 5)diff = 5;
				}
				if (!blocks[30+diff].node->isVisible() && 30+diff!=ToInvis){ blocks[30+diff].node->setVisible(true); std::cout << "lalka!"; }

				if (NewRec.IsKeyDown(KEY_LEFT))
				{
					hero->setPosition(hero->getPosition() + vector3df(0.15+diff*0.05, 0, 0));
					hero->setRotation(vector3df(hero->getRotation().X, -20, hero->getRotation().Z));
				}
				if (NewRec.IsKeyDown(KEY_RIGHT))
				{
					hero->setPosition(hero->getPosition() + vector3df(-0.15 - diff*0.05, 0, 0));
					hero->setRotation(vector3df(hero->getRotation().X, 20, hero->getRotation().Z));
				}
				
				if (ToInvis != 0)//breakable platform just become invisible
				{
					time++;
					if (time == 60)
					{
						time = 0; 
						blocks[ToInvis].node->setVisible(false);
						blocks[ToInvis].node->setFrameLoop(0, 0);
						blocks[ToInvis].node->setPosition(vector3df(0, -20, 0));
						ToInvis = 0;
					}
				}

				camera->setPosition(vector3df(0, hero->getPosition().Y, 20));
				camera->setTarget(vector3df(0, hero->getPosition().Y, 0));
				int m = hero->getPosition().Y;
			}
		}
	device->drop();
	
	return 0;
}