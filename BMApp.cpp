#include "BMApp.h"



BMApp::BMApp()
{
	active_mode = BM_MODE_EDIT;
	active_tool = BM_TOOL_RAISE_LOWER;
	main_bm_node = NULL;
	main_cam = NULL;
	cam_distance_from_surface = 50.0f;
	cam_rotation = NULL_VEC;
	GF.device = NULL;
	max_depth_scale = 100.0f;
	last_opened_file = "";
	lights.clear();
}


BMApp::~BMApp()
{
	clear();
}

bool BMApp::load_config()
{
	if (GF.device == NULL)
		return false;
	irr::io::IXMLReader *xml = GF.device->getFileSystem()->createXMLReader("config.xml");
	if (!xml)
	{
		return false;
	}
	irr::core::stringw current_block = L"";
	while (xml->read())
	{
		switch (xml->getNodeType())
		{
		case irr::io::EXN_ELEMENT:
		{
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("help_text_keys"))
				help_text_keys = xml->getAttributeValue(0);
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("initial_file"))
				last_opened_file = wchar2str(xml->getAttributeValue(0));
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("fonts"))
			{
				for (unsigned int i = 0; i < xml->getAttributeCount(); i++)
				{
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("small"))
						GF.font_small_name = xml->getAttributeValue(i);
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("large"))
						GF.font_large_name = xml->getAttributeValue(i);
				}
			}
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("step_size_change"))
				dbathymetry_change_value = xml->getAttributeValueAsFloat(0);
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("depth_scale"))
				max_depth_scale = xml->getAttributeValueAsFloat(0);
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("step_size"))
			{
				for (unsigned int i = 0; i < xml->getAttributeCount(); i++)
				{
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("size"))
						bathymetry_change_value = xml->getAttributeValueAsFloat(i);
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("max"))
						bathymetry_change_value_max = xml->getAttributeValueAsFloat(i);
				}
			}
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("step_radius"))
			{
				for (unsigned int i = 0; i < xml->getAttributeCount(); i++)
				{
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("radius"))
						bathymetry_change_range = xml->getAttributeValueAsInt(i);
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("max"))
						bathymetry_change_range_max = xml->getAttributeValueAsFloat(i);
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("min"))
						bathymetry_change_range_min = xml->getAttributeValueAsFloat(i);
				}
			}
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("Zoom"))
			{
				for (unsigned int i = 0; i < xml->getAttributeCount(); i++)
				{
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("step"))
						GF.receiver->mouse_wheel_sensitivity = xml->getAttributeValueAsFloat(i);
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("min_dist"))
						zoom_min_dist = xml->getAttributeValueAsFloat(i);
					if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("max_dist"))
						zoom_max_dist = xml->getAttributeValueAsFloat(i);
				}
			}
			if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("data"))
			{
				current_block = ((irr::core::stringw) xml->getNodeName());
			}
		}
		break;
		case irr::io::EXN_ELEMENT_END:
		{
			current_block = "";
		}
		break;
		}
	}
	xml->drop();
	return true;
}
bool BMApp::init()
{
	GF.Init();
	if (GF.device == 0)
		return false; // could not create selected driver.
	load_config();
	GF.reload_fonts();
	GF.SetMenu();
	main_bm_node = new BathyGraph(GF.smgr->getRootSceneNode(), GF.smgr, 666);// , x, y, depths);
	if (main_bm_node != NULL)
	{
		if (!main_bm_node->Bathymetry.LoadFromFile(last_opened_file, GF.device))
			main_bm_node->Bathymetry.CreateEmpty(30,30);
		main_bm_node->RefreshFromData();
	}
	else
		return false;
//	std::cout << "start layer\n";
	//main_layer = new LayerGraph(GF.smgr->getRootSceneNode(), GF.smgr, 667);
	//if (main_layer != NULL)
	//{
	//	//std::cout << "got layer\n";
	//	main_layer->RefreshFromData();
	//	//std::cout << "refreshed layer\n";

	//}
	GF.smgr->setAmbientLight(video::SColorf(0.6f, 0.6f, 0.6f, 0.6f));
	lights.push_back(GF.smgr->addLightSceneNode(0, core::vector3df(60, 60, -200.0f), video::SColorf(0.1f, 0.1f, 0.1f), 200.0f, 1));
	//main_bm_node->setRotation(core::vector3df(-90.0f, -90.0, 90.0));
	if (main_cam != NULL)
	{
		main_cam->drop();
		main_cam = NULL;
	}
//	main_cam = GF.smgr->addCameraSceneNodeMaya();
	main_cam = GF.smgr->addCameraSceneNode();
	if (main_cam)
	{
		main_cam->setPosition(main_bm_node->getPosition()+irr::core::vector3df(0.0f,0.0f,-50.0f));
		main_cam->setTarget(main_bm_node->getPosition());
	}
	lock_land_mask = false;
	GF.receiver->depth_scale_updated = true;
	cam_distance_from_surface = bm_max(main_bm_node->size_x, main_bm_node->size_y);

	return true;
}

bool BMApp::refresh_app()
{
	if (GF.device == NULL || GF.receiver==NULL)
		return false;
	GF.device->run();
	if (GF.device->isWindowActive() && main_cam != NULL && GF.receiver!=NULL)
	{
		if (GF.receiver->gui_activated)
		{
			GF.receiver->cam_dist_move = 0.0f;
			GF.receiver->cam_pos_move = NULL_VEC;
			GF.receiver->cam_rot_move = NULL_VEC;
		}
		handle_inputs();
		update_cam();
		GF.receiver->gui_activated = false;
		if (lights.size()>0)
			lights[0]->setPosition(main_cam->getTarget()+irr::core::vector3df(0.0f,0.0f,-50.0f));
		GF.driver->beginScene(true, true, SColor(0, 128, 128, 128));

		GF.smgr->drawAll();
		GF.env->drawAll();
		GF.driver->setTransform(ETS_WORLD,IdentityMatrix);
//		bm_intvec selected_cell=main_bm_node->index_from_world_coord(main_cam->getTarget());
//		vector3df selected_spot= main_bm_node->world_coord_from_index(selected_cell);
		bm_intvec selected_cell = main_bm_node->index_from_world_coord(cursor_location);
		vector3df cursor_choice = main_bm_node->world_coord_from_index(selected_cell);

		//GF.driver->draw3DLine(vector3df(selected_spot.X, selected_spot.Y, selected_spot.Z + 100.0f),
		//	vector3df(selected_spot.X, selected_spot.Y, selected_spot.Z - 10.0f),
		//	SColor(255, 255, 255, 255));
		GF.driver->draw3DLine(vector3df(cursor_choice.X, cursor_choice.Y, 100.0f),
			vector3df(cursor_choice.X, cursor_choice.Y, - 1.0f),
			SColor(255, 255, 0, 0));
		main_bm_node->selected_cell = selected_cell.Y + selected_cell.X*main_bm_node->cells_y;
		IGUIFont *font = GF.font_large;
		if (main_bm_node->selected_cell >= 0 && main_bm_node->selected_cell < main_bm_node->cells && font!=NULL)
		{
			core::dimension2d<u32> screen_dim = GF.driver->getScreenSize();
			stringw tmp_txt;
			char tmp_ch[200];
			irr::core::dimension2d<u32> dim = font->getDimension(L"PARAM: --some value--spca");
			int x_loc = screen_dim.Width - dim.Width;//*font->getKerningHeight();
			int line_h = dim.Height;
			int height = dim.Height*1.5;
			SColor TextColor = SColor(GF.receiver->gui_transparency,255,255,255);
			GF.driver->draw2DRectangle(SColor(GF.receiver->gui_transparency/2, 0, 0, 0), rect<s32>(x_loc-10, height-5, screen_dim.Width, height + 6*dim.Height+10),NULL);
			sprintf(tmp_ch, "Tool: %s", tool_names[active_tool]);
			tmp_txt = tmp_ch;
			font->draw(tmp_txt, rect<s32>(x_loc, height, screen_dim.Width, height + line_h), TextColor);
			height += line_h;

			sprintf(tmp_ch, "Depth: %3.1f m", main_bm_node->Bathymetry.depths[main_bm_node->selected_cell]);
			tmp_txt = tmp_ch;
			font->draw(tmp_txt, rect<s32>(x_loc, height, screen_dim.Width, height+line_h), TextColor);
			height += line_h;
			coordinate tmp_coord = main_bm_node->Bathymetry.cell2coordinate(main_bm_node->selected_cell);
			sprintf(tmp_ch, "Coord: %5.3f, %5.3f", tmp_coord.lat,tmp_coord.lon);
			tmp_txt = tmp_ch;
			font->draw(tmp_txt, rect<s32>(x_loc, height, screen_dim.Width, height + line_h), TextColor);
			height += line_h;

			sprintf(tmp_ch, "Cell: %d, %d", selected_cell.X, selected_cell.Y);
			tmp_txt = tmp_ch;
			font->draw(tmp_txt, rect<s32>(x_loc, height, screen_dim.Width, height + line_h), TextColor);
			height += line_h;

			sprintf(tmp_ch, "dD: %.1f, Rad: %d", bathymetry_change_value, bathymetry_change_range);
			tmp_txt = tmp_ch;
			font->draw(tmp_txt, rect<s32>(x_loc, height, screen_dim.Width, height + line_h), TextColor);
			height += line_h;

			lock_land_mask ? sprintf(tmp_ch, "landmask: LOCKED") : sprintf(tmp_ch, "landmask: unlocked");
			tmp_txt = tmp_ch;
			font->draw(tmp_txt, rect<s32>(x_loc, height, screen_dim.Width, height + line_h), TextColor);
			height += line_h;


		}
		GF.driver->endScene();
		if (GF.receiver->take_screenshot)
		{
			save_screenshot();
		}
	}
	return false;
}

int BMApp::handle_inputs()
{
	float change_point = 0.0f; //Store the amount of change ordered for bathymetry.
	if (GF.receiver == NULL)
		return false;
	main_bm_node->wire = GF.receiver->wire;
	main_bm_node->smooth = GF.receiver->smooth;
	if (main_bm_node->separate_land != GF.receiver->separate_landmask)
	{
		main_bm_node->RefreshFromData();
		main_bm_node->separate_land = GF.receiver->separate_landmask;
	}
	if (GF.receiver->change_mode_to != -1)
	{
		active_mode = GF.receiver->change_mode_to;
		GF.receiver->change_mode_to = -1;
	}
	if (active_mode == BM_MODE_EDIT)
	{
		//JUST A TEST FOR MOUSE DETECTION. DO NICER VERSION!
		ISceneCollisionManager *colmgr = GF.smgr->getSceneCollisionManager();
		line3d<f32> raytrace = colmgr->getRayFromScreenCoordinates(GF.device->getCursorControl()->getPosition(), main_cam);
		if (fabs(raytrace.end.Z - raytrace.start.Z) != 0.0f)
		{
			float ratio = fabs(raytrace.start.Z) / fabs(raytrace.end.Z - raytrace.start.Z);
			cursor_location.X = (raytrace.start.X + (raytrace.end.X - raytrace.start.X)*ratio);
			cursor_location.Y = (raytrace.start.Y + (raytrace.end.Y - raytrace.start.Y)*ratio);
			cursor_location.Z = (raytrace.start.Z + (raytrace.end.Z - raytrace.start.Z)*ratio);
		}
		if (GF.receiver->depth_scale_updated)
		{
			if (main_bm_node != NULL)
				main_bm_node->setScale(vector3df(1.0f, 1.0f, GF.receiver->depth_scale*max_depth_scale));
			GF.receiver->depth_scale_updated = false;
		}
		if (GF.receiver->cam_pos_move != NULL_VEC && main_cam != NULL)
		{
			//main_cam->setPosition(main_cam->getPosition()+GF.receiver->cam_pos_move);
			GF.receiver->cam_pos_move.rotateXYBy(cam_rotation.Z);
			GF.receiver->cam_pos_move = GF.receiver->cam_pos_move*(cam_distance_from_surface/400.0f);
			main_cam->setTarget(main_cam->getTarget() + GF.receiver->cam_pos_move);
			GF.receiver->cam_pos_move = NULL_VEC;
		}
		if (GF.receiver->cam_rot_move != NULL_VEC && main_cam != NULL)
		{
			cam_rotation += GF.receiver->cam_rot_move;
			GF.receiver->cam_rot_move = NULL_VEC;
		}
		if (GF.receiver->cam_dist_move != 0.0f && main_cam != NULL)
		{
			cam_distance_from_surface += GF.receiver->cam_dist_move;
			cam_distance_from_surface = bm_max(zoom_min_dist, cam_distance_from_surface);
			cam_distance_from_surface = bm_min(zoom_max_dist, cam_distance_from_surface);
			GF.receiver->cam_dist_move = 0.0f;
		}

		//NEW ORDERS:
		if (GF.receiver->ui_order != BM_UI_ORDER_NONE)
		{
			switch (GF.receiver->ui_order)
			{
			case BM_UI_ORDER_OPEN_HELP_KEYS:
				if (GF.font_small != NULL && GF.env != NULL)
				{
					stringw topic = L"Bathymetry Editor Hotkeys";
					core::dimension2d<u32> dim= GF.font_small->getDimension(help_text_keys.c_str());
					dim.Width = bm_max(dim.Width, GF.font_small->getDimension(topic.c_str()).Width);
					IGUIWindow* window = GF.env->addWindow(core::rect<s32>(10, 10, 10+dim.Width+30, 10+dim.Height+60), false, topic.c_str());
					if (window)
					{
						GF.env->addStaticText(help_text_keys.c_str(),
							core::rect<s32>(20, 20, 30 + dim.Width, 30 + dim.Height), false, true, window);
					}
				}
				break;
			case BM_UI_ORDER_NEW_FILE:
				main_bm_node->Bathymetry.CreateEmpty(10,10);
				main_bm_node->ClearGraphicalData();
				main_bm_node->RefreshFromData();
				if (main_bm_node != NULL)
					main_bm_node->setScale(vector3df(1.0f, 1.0f, GF.receiver->depth_scale));
				break;
			case BM_UI_ORDER_LOAD_FILE:
				main_bm_node->Bathymetry.LoadFromFile(GF.receiver->selected_filename,GF.device);
				main_bm_node->ClearGraphicalData();
				main_bm_node->RefreshFromData();
				cam_distance_from_surface = bm_max(main_bm_node->size_x, main_bm_node->size_y);
				if (main_bm_node != NULL)
					main_bm_node->setScale(vector3df(1.0f, 1.0f, GF.receiver->depth_scale));
				break;
			case BM_UI_ORDER_SAVE_AS_FILE:
				main_bm_node->Bathymetry.SaveToXMLFile(GF.receiver->selected_filename, GF.device);
				break;
			case BM_UI_ORDER_RESET_VIEW:
				cam_rotation = NULL_VEC;
				break;
			case BM_UI_ORDER_TOGGLE_LANDMASK:
				lock_land_mask=!lock_land_mask;
				break;
			case BM_UI_ORDER_SELECT_TOOL_RL:
				active_tool = BM_TOOL_RAISE_LOWER;
				break;
			case BM_UI_ORDER_SELECT_TOOL_EVEN:
				active_tool = BM_TOOL_SMOOTH;
				break;
			case BM_UI_ORDER_INCREASE_RANGE:
				bathymetry_change_range++;
				bathymetry_change_range = bm_min(bathymetry_change_range_max, bathymetry_change_range);
				break;
			case BM_UI_ORDER_DECREASE_RANGE:
				bathymetry_change_range--;
				bathymetry_change_range = bm_max(bathymetry_change_range_min, bathymetry_change_range);
				break;
			case BM_UI_ORDER_INCREASE_CHANGE:
				bathymetry_change_value+=dbathymetry_change_value;
				bathymetry_change_value = bm_min(bathymetry_change_value_max, bathymetry_change_value);
				break;
			case BM_UI_ORDER_DECREASE_CHANGE:
				bathymetry_change_value-= dbathymetry_change_value;
				bathymetry_change_value = bm_max(dbathymetry_change_value, bathymetry_change_value);
				break;
			case BM_UI_ORDER_RAISE_POINT:
				change_point = bathymetry_change_value;
				break;
			case BM_UI_ORDER_LOWER_POINT:
				change_point = -bathymetry_change_value;
				break;
			case BM_UI_ORDER_UNDO:
				main_bm_node->Bathymetry.make_undo();
				main_bm_node->RefreshFromData();
				break;
			case BM_UI_ORDER_REDO:
				main_bm_node->Bathymetry.make_redo();
				main_bm_node->RefreshFromData();
				break;
			case BM_UI_ORDER_SCALE_ALL_DEPTHS:
				main_bm_node->Bathymetry.ScaleDepths(main_bm_node->selected_cell,lock_land_mask);
				main_bm_node->Bathymetry.close_undo_step();
				main_bm_node->RefreshFromData();
				break;
			}
			GF.receiver->ui_order = BM_UI_ORDER_NONE;
		}
		if (change_point!=0.0f && main_bm_node->selected_cell >= 0 && main_bm_node->selected_cell < main_bm_node->cells && !GF.receiver->gui_activated)
		{ //modify the point.
			switch (active_tool)
			{
			case BM_TOOL_RAISE_LOWER:
				main_bm_node->Bathymetry.ChangeDepth(main_bm_node->selected_cell, bathymetry_change_range, change_point,lock_land_mask);
				break;
			case BM_TOOL_SMOOTH:
				main_bm_node->Bathymetry.SmoothDepth(main_bm_node->selected_cell, bathymetry_change_range, change_point, lock_land_mask);
				break;
			}
			main_bm_node->Bathymetry.close_undo_step();
			main_bm_node->RefreshFromData();
		}
	}
	return true;
}

bool BMApp::update_cam()
{
	irr::core::vector3df cam_modif = NULL_VEC;
	cam_modif += irr::core::vector3df(0.0f,0.0f,-1.0f*cam_distance_from_surface);
	cam_rotation.X = bm_min(bm_max(-85.0f, cam_rotation.X), -1.0f);
	cam_rotation.Y = bm_min(bm_max(-80, cam_rotation.Y), 80.0f);
	cam_modif.rotateYZBy(cam_rotation.X);
	cam_modif.rotateXZBy(cam_rotation.Y);
	cam_modif.rotateXYBy(cam_rotation.Z);
	main_cam->setPosition(main_cam->getTarget()+cam_modif);
	main_cam->setUpVector(vector3df(0.0f, 0.0f,-1.0f));
	return false;
}

bool BMApp::save_screenshot()
{
	irr::video::IImage* const image = GF.driver->createScreenShot();
	if (image) //should always be true, but you never know. ;)
	{
		//construct a filename, consisting of local time and file extension
		irr::c8 filename[64];
		snprintf(filename, 64, "screenshot_%u.png", GF.device->getTimer()->getRealTime());

		//write screenshot to file
		if (!GF.driver->writeImageToFile(image, filename))
			GF.device->getLogger()->log(L"Failed to take screenshot.", irr::ELL_WARNING);

		//Don't forget to drop image since we don't need it anymore.
		image->drop();
	}
	GF.receiver->take_screenshot = false;
	return true;
}

bool BMApp::clear()
{
	if (GF.device != NULL)
	{
		GF.device->drop();
		GF.device = NULL;
	}
	std::vector<ILightSceneNode*>::iterator i;
	for (i = lights.begin(); i != lights.end(); i++)
	{
		(*i) = NULL;
	}
	lights.clear();
	return true;
}
