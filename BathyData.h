#pragma once
#include<vector>
#include<string>
#include<fstream>
#include <sstream>
#include <iostream>   // std::cout
#include <irrlicht.h> //This one is needed to get the xml reader. This class has nothing to do with graphics
#include "constants.h"
#include "helper_functions.h"
struct bt_size
{
public:
	bt_size()
	{
		bt_size(0, 0);
	}
	bt_size(int x, int y)
	{
		X = x;
		Y = y;
		Total = X*Y;
	}
	unsigned int getX()
	{
		return X;
	}
	unsigned int getY()
	{
		return Y;
	}
	unsigned int getTotal()
	{
		return Total;
	}
	unsigned int setX(unsigned int x)
	{
		X = x;
		refresh();
		return X;
	}
	unsigned int setY(unsigned int y)
	{
		Y = y;
		refresh();
		return Y;
	}
	void clear()
	{
		X = 0;
		Y = 0;
		refresh();
	}
private:
	unsigned int refresh()
	{
		Total = X*Y;
		return Total;
	}
	unsigned int X;
	unsigned int Y;
	unsigned int Total;
};
struct coordinate 
{
	float lat;
	float lon;
	int x;
	int y;
	coordinate()
	{
		lat = 0.0f;
		lon = 0.0f;
		x = 0;
		y = 0;
	}

};
struct bm_undo
{
	unsigned int cell_no;
	float old_depth;
	float new_depth;
	bm_undo()
	{
		cell_no = 0;
		old_depth = 0.0f;
		new_depth = 0.0f;
	}
};

class BathyData
{
	//Class that saves nubmerical data of given bathymetry. Free from any graphical interfaces
public:
	BathyData();
	~BathyData();
	bool LoadFromFile(std::string FileName, irr::IrrlichtDevice *device); //Load bathymetric data from given file. returns true, if succesfull.
	bool LoadFromXMLFile(std::string FileName, irr::IrrlichtDevice *device); //Load bathymetric data and metadata from given XML file. returns true, if succesfull.
	bool SaveToXMLFile(std::string FileName, irr::IrrlichtDevice *device); //save bathymetric data and metadata from given XML file. returns true, if succesfull.
	bool CreateEmpty(int X_cells, int Y_cells);
	void reset_metadata();
	void AddRandomNoise(float delta); //adds noise to each depth.
	int SetDepth(int cell_no, float new_depth);
	int ChangeDepth(int cell_no, int range, float amount, bool land_mask_lock);
	int ChangeDepth(int x, int y, int range, float amount, bool land_mask_lock);
	int SmoothDepth(int cell_no, int range, float amount, bool land_mask_lock);
	int SmoothDepth(int x, int y, int range, float amount, bool land_mask_lock);
	int ScaleDepths(int cell_no, bool land_mask_lock);
	bool add_to_current_undo_step(bm_undo new_entry);
	bool open_undo_step(); //create new empty undo-step, if one doesn't allready exist.
	bool close_undo_step();
	bool make_undo();
	bool make_redo();
	coordinate cell2coordinate(int x, int y);
	coordinate cell2coordinate(int cell);

	bt_size cell_sizes;
	std::vector<float> depths; //listed depths
	float lat_min;
	float lat_max;
	float lon_min;
	float lon_max;
	float x_in_m; //how many meters one cell is in x-axis (if constant)
	float y_in_m; //how many meters one cell is in y-axis (if constant)
	float depth_in_m; //how many meters depth unit is in meters (if constant)
	bool depth_negative;
	std::vector<std::vector<bm_undo>> undo_steps;
	int current_undo_step;
};

