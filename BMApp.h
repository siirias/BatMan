#pragma once
#include <string>     // std::string, std::stod
#include <list>     // std::string, std::stod
#include <vector>     // std::string, std::stod
#if defined(_WIN32) || defined(_WIN64)
  #include <direct.h>
#endif
#include "constants.h"
#include "Receiver.h"
#include "BathyData.h"
#include "BathyGraph.h"
#include "LayerGraph.h"
#include "Graphics.h"
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
struct SAppContext;

class BMApp
{
public:
	BMApp();
	~BMApp();
	bool init();
	bool refresh_app();
	int handle_inputs();
	bool update_cam();
	bool save_screenshot();
	bool clear();
	bool load_config();
	
	int active_mode;
	std::string last_opened_file;
	Graphics GF;
	BathyGraph *main_bm_node;
	BathyGraph *secondary_bm_node;
	//LayerGraph *main_layer;
	ICameraSceneNode* main_cam;
	vector3df cursor_location;
	float cam_distance_from_surface;
	irr::core::vector3df cam_rotation;
	std::vector<ILightSceneNode*> lights;
	int active_tool;
	float dbathymetry_change_value;

	float bathymetry_change_value;
	float bathymetry_change_value_max;
	float max_depth_scale;
	int bathymetry_change_range;
	int bathymetry_change_range_max;
	int bathymetry_change_range_min;
	bool lock_land_mask;
	float zoom_min_dist;
	float zoom_max_dist;
	stringw help_text_keys;
};

