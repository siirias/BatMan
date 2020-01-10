#include "Graphics.h"



Graphics::Graphics()
{
	device = NULL;
	driver = NULL;
	env = NULL;
	skin = NULL;
	font_large_name = "";
	font_small_name = "";
	font_small = NULL;
	font_large = NULL;
	receiver = NULL;
	smgr = NULL;
	driverType = EDT_OPENGL;
	gui_default = "bathymetry_gui_default.xml";
}


Graphics::~Graphics()
{
}

int Graphics::Init()
{
	device = createDevice(driverType, core::dimension2d<u32>(800, 600));
	if (device == NULL)
		return false;
	device->setWindowCaption(L"Bathymetry Editor");
	device->setResizable(true);
	driver = device->getVideoDriver();
	env = device->getGUIEnvironment();
	skin = env->getSkin();
	reload_fonts();
	// Then create the event receiver, giving it that context structure.
	if (receiver != NULL)
	{
		delete receiver;
	}
	receiver = new Receiver(context);
	// And tell the device to use our custom event receiver.
	device->setEventReceiver(receiver);
	receiver->wire = false;
	receiver->setSkinTransparency(255, env->getSkin());

	return true;
}

int Graphics::SetMenu()
{

	if (env == NULL)
		return false;
	env->loadGUI(gui_default.c_str());
	reload_fonts();
	irr::gui::IGUIContextMenu *main_menu=env->addMenu(env->getRootGUIElement(), GUI_ID_MAINMENU);
	if (main_menu != NULL)
	{
		irr::gui::IGUIContextMenu *tmp_menu;
		int tmp_index=main_menu->addItem(L"File",GUI_ID_FILEMENU,true,true,false,false);
		tmp_menu=main_menu->getSubMenu(tmp_index);
		if (tmp_menu != NULL)
		{
			tmp_menu->addItem(L"New bathymetry", GUI_ID_NEW_BATHY);
			tmp_menu->addItem(L"Save bathymetry as...", GUI_ID_SAVE_AS_BATHY);
			tmp_menu->addItem(L"Load bathymetry", GUI_ID_LOAD_BATHY);
			tmp_menu->addItem(0);
			tmp_menu->addItem(L"Quit", GUI_ID_QUIT_BUTTON);
		}
		tmp_index = main_menu->addItem(L"View", GUI_ID_VIEWMENU, true, true, false, false);
		tmp_menu = main_menu->getSubMenu(tmp_index);
		if (tmp_menu != NULL)
		{
			tmp_menu->addItem(L"Toggle grid (g)", GUI_ID_BATY_WIRE);
			tmp_menu->addItem(L"Toggle smooth (s)", GUI_ID_BATY_SMOOTH);
		}
		tmp_index = main_menu->addItem(L"Tools", GUI_ID_TOOLMENU, true, true, false, false);
		tmp_menu = main_menu->getSubMenu(tmp_index);
		if (tmp_menu != NULL)
		{
			tmp_menu->addItem(L"Raise/lower (R)", GUI_ID_TOOL_RL);
			tmp_menu->addItem(L"Near Target depth (D)", GUI_ID_TOOL_TDEPTH,false);
			tmp_menu->addItem(L"Even (E)", GUI_ID_TOOL_EVEN);
		}
		tmp_index = main_menu->addItem(L"Edit", GUI_ID_EDITMENU, true, true, false, false);
		tmp_menu = main_menu->getSubMenu(tmp_index);
		if (tmp_menu != NULL)
		{
			tmp_menu->addItem(L"(un)lock land-mask (l)", GUI_ID_EDIT_LANDMASK);
			tmp_menu->addItem(0);
			tmp_menu->addItem(L"Undo (cntr+z)", GUI_ID_EDIT_UNDO);
			tmp_menu->addItem(L"Redo (cntr+y)", GUI_ID_EDIT_REDO);
			tmp_menu->addItem(0);
			tmp_menu->addItem(L"Set target depth...(alt+d)", GUI_ID_EDIT_TDEPTH, false);
			tmp_menu->addItem(L"Set change step...(alt+s)", GUI_ID_EDIT_STEPSIZE, false);
			tmp_menu->addItem(L"Set change radius...(alt+r)", GUI_ID_EDIT_RADIUS, false);
		}
		tmp_index = main_menu->addItem(L"Help", GUI_ID_HELPMENU, true, true, false, false);
		tmp_menu = main_menu->getSubMenu(tmp_index);
		if (tmp_menu != NULL)
		{
			tmp_menu->addItem(L"show keybindings...", GUI_ID_HELP_KEYS);
			tmp_menu->addItem(0);
			tmp_menu->addItem(L"About...", GUI_ID_HELP_ABOUT);
		}
	}
//	env->saveGUI(L"kokeilu.xml");
	//env->addButton(rect<s32>(10, 240, 110, 240 + 32), 0, GUI_ID_QUIT_BUTTON,
	//	L"Quit", L"Exits Program");
	////env->addButton(rect<s32>(10,280,110,280 + 32), 0, GUI_ID_NEW_WINDOW_BUTTON,
	////		L"New Window", L"Launches a new Window");
	//env->addButton(rect<s32>(10, 320, 110, 320 + 32), 0, GUI_ID_BATY_SMOOTH,
	//	L"Toggle smooth/block", L"Toggle render mode");

	//env->addButton(rect<s32>(10, 360, 110, 360 + 32), 0, GUI_ID_BATY_WIRE,
	//	L"Toggle Wires", L"Toggle wireframe");
	//env->addButton(rect<s32>(10, 400, 110, 400 + 64), 0, GUI_ID_SCREENSHOT,
	//	L"SCEENSHOT", L"take a screenshot");

	///*
	//Now, we add a static text and a scrollbar, which modifies the
	//transparency of all gui elements. We set the maximum value of
	//the scrollbar to 255, because that's the maximal value for
	//a color value.
	//Then we create an other static text and a list box.
	//*/

	//env->addStaticText(L"Transparent Control:", rect<s32>(150, 20, 350, 40), true);
	//scrollbar = env->addScrollBar(true,
	//	rect<s32>(150, 45, 350, 60), 0, GUI_ID_TRANSPARENCY_SCROLL_BAR);
	//scrollbar->setMax(255);
	//scrollbar->setPos(255);

	//// set scrollbar position to alpha value of an arbitrary element
	//scrollbar->setPos(env->getSkin()->getColor(EGDC_WINDOW).getAlpha());

	//env->addStaticText(L"Logging ListBox:", rect<s32>(50, 110, 250, 130), true);
	//listbox = env->addListBox(rect<s32>(50, 140, 250, 210));
	//env->addEditBox(L"Editable Text", rect<s32>(350, 80, 550, 100));

	// Store the appropriate data in a context structure.
	context.device = device;
	context.counter = 0;
	context.listbox = listbox;

	receiver->setSkinTransparency(255, env->getSkin());
	smgr = device->getSceneManager();
	return true;
}

int Graphics::reload_fonts()
{
	int fonts_loaded = 0;
	font_small = env->getFont(font_small_name);
	if (font_small)
		fonts_loaded++;
	font_large = env->getFont(font_large_name);
	if (font_large)
		fonts_loaded++;
	if (font_small)
		skin->setFont(font_small);
	skin->setFont(font_small, EGDF_TOOLTIP);
	skin->setFont(font_small, EGDF_WINDOW);
	skin->setFont(font_large, EGDF_MENU);
	skin->setFont(font_small, EGDF_BUTTON);

	return 0;
}
