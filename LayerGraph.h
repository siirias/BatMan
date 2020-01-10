#pragma once
#include "ISceneNode.h"
#include <irrlicht.h>
#include <iostream>
#include "constants.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class LayerGraph :	public scene::ISceneNode
{
public:
	LayerGraph(scene::ISceneNode * parent, scene::ISceneManager * mgr, s32 id);
	~LayerGraph();
	bool RefreshFromData();
	bool ClearGraphicalData();
	virtual void OnRegisterSceneNode();
	virtual void render();
	virtual const core::aabbox3d<f32>& getBoundingBox() const;
	virtual u32 getMaterialCount() const;
	virtual video::SMaterial& getMaterial(u32 i);


	core::aabbox3d<f32> Box;
	video::SMaterial Material;
	video::S3DVertex *Vertices;
	u32 *PrimIndices;
	bm_vec ul_corner;
	bm_vec lr_corner;
	float set_height;

};

