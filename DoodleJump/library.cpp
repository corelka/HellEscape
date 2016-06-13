#include "library.h"
#include <iostream>



video::ITexture* ResizeTexture(ITexture* txt, IVideoDriver* drv, int newX, int newZ)//geting new size of texture
{
	IImage* back_img = drv->createImageFromData(txt->getColorFormat(), txt->getSize(), txt->lock(), false);
	IImage* back_img_trg = drv->createImage(ECF_A1R5G5B5, dimension2du(newX, newZ));
	back_img->copyToScaling(back_img_trg);
	ITexture* texture = drv->addTexture("text", back_img_trg);
	texture->grab();
	return texture;
}


void set_breakable(block block, ISceneManager* smg, IVideoDriver* drv)//platform become breakable
{
	block.node->setMesh(smg->getMesh("../models_obj/platform01.x"));
	block.node->setMaterialTexture(0, drv->getTexture("../models_obj/platform01.tga"));
	block.node->setLoopMode(false);
	block.IsBreakabel = true;
}

void set_usual(block block, ISceneManager* smg, IVideoDriver* drv)//platform become usual
{
	block.node->setMesh(smg->getMesh("../models_obj/platform00.x"));
	block.node->setMaterialTexture(0, drv->getTexture("../models_obj/platform00.tga"));
	block.node->setLoopMode(false);
	block.IsBreakabel = false;
}

block platform(block unit, ISceneManager* smg, IVideoDriver* drv, f32 x, f32 y)//creating of platform
{
	unit.node = smg->addAnimatedMeshSceneNode(smg->getMesh("../models_obj/platform00.x"));
	if (unit.node)
	{
		unit.node->setMaterialFlag(EMF_LIGHTING, false);
		unit.node->setMaterialTexture(0, drv->getTexture("../models_obj/platform00.tga"));
		unit.node->setPosition(vector3df(x, y, 0));
		unit.node->setRotation(vector3df(0, 0, 0));
		unit.node->setLoopMode(false);
	}
	unit.IsBreakabel = false;
	return unit;
}