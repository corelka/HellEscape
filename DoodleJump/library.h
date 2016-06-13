#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


struct block
{
	IAnimatedMeshSceneNode* node;
	bool IsBreakabel;
};

video::ITexture* ResizeTexture(ITexture* txt, IVideoDriver* drv, int newX, int newZ);

void set_breakable(block block, ISceneManager* smg, IVideoDriver* drv);

void set_usual(block block, ISceneManager* smg, IVideoDriver* drv);

block platform(block unit, ISceneManager* smg, IVideoDriver* drv, f32 x, f32 y);