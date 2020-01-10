#pragma once
#include <irrlicht.h>
#include <array>
#define bm_max(a,b) (a)>(b)?(a):(b)
#define bm_min(a,b) (a)<(b)?(a):(b)


//ACTIVE MODES FOR PROGRAM
#define BM_MODE_EDIT	0
#define BM_MODE_MENUS	1

#define BM_TOOL_NONE				0
#define BM_TOOL_RAISE_LOWER			1
#define BM_TOOL_SMOOTH				2
#define BM_TOOL_TOWARDS_DEPTH		3
static const char *  tool_names[] = {"None","Raise/Lower","Even","Towards Depth"};
//Extra orders received from ui
enum
{
	BM_UI_ORDER_NONE=0,
	BM_UI_ORDER_NEW_FILE,
	BM_UI_ORDER_LOAD_FILE,
	BM_UI_ORDER_SAVE_AS_FILE,
	BM_UI_ORDER_RESET_VIEW,
	BM_UI_ORDER_RAISE_POINT,
	BM_UI_ORDER_LOWER_POINT,
	BM_UI_ORDER_INCREASE_RANGE,
	BM_UI_ORDER_DECREASE_RANGE,
	BM_UI_ORDER_INCREASE_CHANGE,
	BM_UI_ORDER_DECREASE_CHANGE,
	BM_UI_ORDER_TOGGLE_LANDMASK,
	BM_UI_ORDER_SELECT_TOOL_RL,
	BM_UI_ORDER_SELECT_TOOL_EVEN,
	BM_UI_ORDER_OPEN_HELP_KEYS,
	BM_UI_ORDER_UNDO,
	BM_UI_ORDER_REDO,
	BM_UI_ORDER_SCALE_ALL_DEPTHS
};


// Define some values that we'll use to identify individual GUI controls.
#define GUI_ID_NONE			0
#define GUI_ID_QUIT_BUTTON  101
#define GUI_ID_NEW_WINDOW_BUTTON  102
#define GUI_ID_FILE_OPEN_BUTTON  103
#define GUI_ID_TRANSPARENCY_SCROLL_BAR  104
#define GUI_ID_BATY_WIRE  105
#define GUI_ID_BATY_SMOOTH  106
#define GUI_ID_SCREENSHOT  107
#define GUI_ID_DEPTH_SCROLL_BAR  108
#define GUI_ID_MAINMENU  109
#define GUI_ID_FILEMENU  110
#define GUI_ID_VIEWMENU  111
#define GUI_ID_TOOLMENU  112
#define GUI_ID_EDITMENU  113
#define GUI_ID_HELPMENU  114

#define GUI_ID_LOAD_BATHY  131
#define GUI_ID_SAVE_AS_BATHY  132
#define GUI_ID_NEW_BATHY  133
#define GUI_ID_TOOL_RL		134
#define GUI_ID_TOOL_TDEPTH		135
#define GUI_ID_TOOL_EVEN		136

#define GUI_ID_EDIT_LANDMASK   150
#define GUI_ID_EDIT_TDEPTH	   151
#define GUI_ID_EDIT_RADIUS	   152
#define GUI_ID_EDIT_STEPSIZE   153
#define GUI_ID_EDIT_UNDO	   154
#define GUI_ID_EDIT_REDO	   155


#define GUI_ID_HELP_KEYS		201
#define GUI_ID_HELP_ABOUT		210

#define bm_default_slider_steps 10000

//#define bm_min_depth_scale 0.1f
//#define bm_max_depth_scale 3000.0f

#define NULL_VEC irr::core::vector3df(0.0f,0.0f,0.0f)

struct bm_intvec
{
	int X;
	int Y;
	bm_intvec()
	{
		clear();
	}
	bm_intvec(int x, int y)
	{
		clear();
		X = x;
		Y = y;
	}
	void clear()
	{
		X = 0;
		Y = 0;
	}
};

struct bm_vec
{
	float X;
	float Y;
	bm_vec()
	{
		clear();
	}
	bm_vec(float x, float y)
	{
		clear();
		X = x;
		Y = y;
	}
	void clear()
	{
		X = 0.0f;
		Y = 0.0f;
	}
};