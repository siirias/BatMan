#pragma once
#include "BathyData.h"
#include <irrlicht.h>
#include "constants.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class BathyGraph : public scene::ISceneNode
{
	//Bathymetric container holding all relevant data for visualisation of the bathymetry, and BathyData class, which has the actual data.
public:
	BathyGraph(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id);
	~BathyGraph();
	bool ClearGraphicalData();
	bool RefreshFromData();
	SColor color_from_height(float height);
	vector3df world_coord_from_index(bm_intvec coord);
	bm_intvec index_from_world_coord(vector3df coord);
	virtual void OnRegisterSceneNode();
	virtual void render();
	virtual const core::aabbox3d<f32>& getBoundingBox() const;
	virtual u32 getMaterialCount() const;
	virtual video::SMaterial& getMaterial(u32 i);



	BathyData Bathymetry;
	core::aabbox3d<f32> Box;
	video::S3DVertex *Vertices;
	u32 *PrimIndices;
	video::S3DVertex *smooth_Vertices;
	u32 *smooth_PrimIndices;
	video::SMaterial Material;
	video::SMaterial WireMaterial;
	int cells_x;
	int cells_y;
	float size_x;
	float size_y;
	float max_z;
	float depth_multip;
	int cells;
	int prims_cell;
	int selected_cell;
	bool wire;
	bool smooth;
};

