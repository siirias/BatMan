#pragma once
#include "constants.h"
#include "helper_functions.h"
#include <irrlicht.h>
#include <string>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Declare a structure to hold some context for the event receiver so that it
// has it available inside its OnEvent() method.
struct SAppContext
{
	IrrlichtDevice *device;
	s32				counter;
	IGUIListBox*	listbox;
};




/*
The Event Receiver is not only capable of getting keyboard and
mouse input events, but also events of the graphical user interface
(gui). There are events for almost everything: Button click,
Listbox selection change, events that say that a element was hovered
and so on. To be able to react to some of these events, we create
an event receiver.
We only react to gui events, and if it's such an event, we get the
id of the caller (the gui element which caused the event) and get
the pointer to the gui environment.
*/
class Receiver : public IEventReceiver
{
public:
	Receiver(SAppContext & context);
	~Receiver();
	void setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin);
	virtual bool OnEvent(const SEvent& event);

	bool wire;
	bool smooth;
	bool separate_landmask;
	bool scale_all_depths_to_current_depth;
	int last_menu_selection; //For example, whether loading or saving things
	bool take_screenshot;
	bool quitting;
	bool depth_scale_updated;
	bool gui_activated; //used to not move camera when this is true
	float depth_scale;
	int gui_transparency; /* 0-255*/
	int change_mode_to; //when active suggest a mdoe change for program, -1 means no change
	int ui_order;
	int mouse_x;
	int mouse_y;
	int mouse_x_old;
	int mouse_y_old;
	irr::core::vector3df cam_pos_move;
	irr::core::vector3df cam_rot_move;
	float mouse_wheel_sensitivity;
	float cam_dist_move;
	std::string selected_filename;
	std::string selected_directory;
	bool pressed_shift;

private:
	SAppContext & Context;
};
