#pragma once
#include <irrlicht.h>
#include <string>
#include "constants.h"
#include "Receiver.h"
#include<fstream>
#include <sstream>
#include <iostream>   // std::cout

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

struct SAppContext;

class Graphics
{
public:
	Graphics();
	~Graphics();
	int Init();
	int SetMenu();
	int reload_fonts();

	stringw font_small_name;
	stringw font_large_name;
	std::string gui_default;
	video::E_DRIVER_TYPE driverType;
	IrrlichtDevice * device;
	video::IVideoDriver* driver;
	IGUIEnvironment* env;
	IGUISkin* skin;
	IGUIFont* font_small;
	IGUIFont* font_large;
	Receiver* receiver;
	ISceneManager* smgr;
	SAppContext context;


	IGUIScrollBar* scrollbar;
	IGUIListBox * listbox;
};

