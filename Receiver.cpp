#include "Receiver.h"

Receiver::Receiver(SAppContext & context) : Context(context)
{ 
	wire = true; 
	smooth = false;
	take_screenshot=false;
	quitting=false;
	depth_scale = 0.0f;
	depth_scale_updated = false;
	mouse_x_old=mouse_x = -1;
	mouse_y_old = mouse_y = -1;
	mouse_wheel_sensitivity=3.0f;
	cam_pos_move = NULL_VEC;
	cam_rot_move = NULL_VEC;
	cam_dist_move = 0.0f;
	gui_transparency = 255;
	gui_activated = false;
	selected_filename = "";
	selected_directory = "";
	change_mode_to = -1;
	ui_order = BM_UI_ORDER_NONE;
	last_menu_selection = GUI_ID_NONE;
	pressed_shift = false;
}


Receiver::~Receiver()
{
}

void Receiver::setSkinTransparency(s32 alpha, irr::gui::IGUISkin * skin)
{
	for (s32 i = 0; i<irr::gui::EGDC_COUNT; ++i)
	{
		video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(alpha);
		skin->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
}

bool Receiver::OnEvent(const SEvent& event)
{
	bool activate_toggle_grid = false;
	bool activate_toggle_smooth = false;
	bool scale_all_depths_to_current_depth = false;
	if (event.EventType == EET_KEY_INPUT_EVENT )
	{
		if (event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case KEY_KEY_E:
				if (event.KeyInput.Shift)
					ui_order = BM_UI_ORDER_SELECT_TOOL_EVEN;
				break;
			case KEY_F10:
				take_screenshot = true;
				break;
			case KEY_KEY_G:
				activate_toggle_grid = true;
				break;
			case KEY_KEY_L:
				ui_order = BM_UI_ORDER_TOGGLE_LANDMASK;
				break;
			case KEY_KEY_S:
				if (event.KeyInput.Shift && event.KeyInput.Control)
					scale_all_depths_to_current_depth=true;
				else
					activate_toggle_smooth = true;
					break;
			case KEY_KEY_R:
				if (event.KeyInput.Shift)
					ui_order = BM_UI_ORDER_SELECT_TOOL_RL;
				else
					ui_order = BM_UI_ORDER_RESET_VIEW;
				break;
			case KEY_KEY_Z:
				if (event.KeyInput.Control)
					ui_order = BM_UI_ORDER_UNDO;
				break;
			case KEY_KEY_Y:
				if (event.KeyInput.Control)
					ui_order = BM_UI_ORDER_REDO;
				break;

			case KEY_PLUS:
				if(event.KeyInput.Shift)
					ui_order = BM_UI_ORDER_INCREASE_CHANGE;
				else
					ui_order = BM_UI_ORDER_INCREASE_RANGE;
				break;
			case KEY_MINUS:
				if (event.KeyInput.Shift)
					ui_order = BM_UI_ORDER_DECREASE_CHANGE;
				else
					ui_order = BM_UI_ORDER_DECREASE_RANGE;
				break;
			case KEY_LSHIFT:
				pressed_shift = true;
			case KEY_RSHIFT:
				pressed_shift = true;

			}
		}
		else //keys are released
		{
			switch (event.KeyInput.Key)
			{
				case KEY_LSHIFT:
					pressed_shift = false;
				case KEY_RSHIFT:
					pressed_shift = false;
			}
		}
	}
	if (event.EventType == EET_GUI_EVENT)
	{
		gui_activated = true;
		if (Context.device == NULL)
			return false;
		s32 id = event.GUIEvent.Caller->getID();
		IGUIEnvironment* env = Context.device->getGUIEnvironment();
		switch (event.GUIEvent.EventType)
		{

		case EGET_SCROLL_BAR_CHANGED:
			if (id == GUI_ID_TRANSPARENCY_SCROLL_BAR)
			{
				gui_transparency= ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				setSkinTransparency(gui_transparency, env->getSkin());
			}
			if (id == GUI_ID_DEPTH_SCROLL_BAR)
			{
				s32 pos = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				//let's say 1.0 is in the middle:
				float position = ((float)pos / bm_default_slider_steps);
				//if(position>0.5f)
				//	depth_scale = (2.0f*(position-0.5f))*(bm_max_depth_scale-1.0f) + 1.0f;
				//else
				//	depth_scale = (2.0f*(position))*(1.0f-bm_min_depth_scale) + bm_min_depth_scale;
				depth_scale = position;

				depth_scale_updated = true;
			}
			break;

		case EGET_BUTTON_CLICKED:
			switch (id)
			{
			case GUI_ID_QUIT_BUTTON:
				Context.device->closeDevice();
				quitting = true;
				break;
			case GUI_ID_BATY_WIRE:
				activate_toggle_grid = true;
				break;
			case GUI_ID_BATY_SMOOTH:
				activate_toggle_smooth = true;
				break;
			case GUI_ID_SCREENSHOT:
				take_screenshot = true;
				break;

			default:
				break;
			}
			break;

		case EGET_MENU_ITEM_SELECTED:
		{
			irr::gui::IGUIContextMenu *active_menu = (IGUIContextMenu*)event.GUIEvent.Caller;
			if (active_menu == NULL)
				break;
			switch (id)
			{
				case GUI_ID_FILEMENU:
				{
					if (active_menu->getItemCommandId(active_menu->getSelectedItem())==GUI_ID_QUIT_BUTTON)
					{
						Context.device->closeDevice();
						quitting = true;
					}
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_LOAD_BATHY)
					{
						IGUIFileOpenDialog *fod = env->addFileOpenDialog(L"Open a bathymetry file",true);
						if (fod != NULL)
						{
							change_mode_to = BM_MODE_MENUS;
							last_menu_selection = GUI_ID_LOAD_BATHY;
						}
					}
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_SAVE_AS_BATHY)
					{
						IGUIFileOpenDialog *fod = env->addFileOpenDialog(L"Save bathymetry file as",true);
						if (fod != NULL)
						{
							change_mode_to = BM_MODE_MENUS;
							last_menu_selection = GUI_ID_SAVE_AS_BATHY;
						}
					}
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_NEW_BATHY)
					{
						ui_order = BM_UI_ORDER_NEW_FILE;
					}
					break;
				}
				case GUI_ID_VIEWMENU:
				{
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_BATY_WIRE)
						activate_toggle_grid=true;
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_BATY_SMOOTH)
						activate_toggle_smooth=true;
					break;
				}
				case GUI_ID_TOOLMENU:
				{
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_TOOL_RL)
						ui_order = BM_UI_ORDER_SELECT_TOOL_RL;
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_TOOL_EVEN)
						ui_order = BM_UI_ORDER_SELECT_TOOL_EVEN;
					break;

				}
				case GUI_ID_EDITMENU:
				{
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_EDIT_LANDMASK)
						ui_order=BM_UI_ORDER_TOGGLE_LANDMASK;
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_EDIT_UNDO)
						ui_order = BM_UI_ORDER_UNDO;
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_EDIT_REDO)
						ui_order = BM_UI_ORDER_REDO;
					break;
				}
				case GUI_ID_HELPMENU:
				{
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_HELP_KEYS)
					{
						ui_order = BM_UI_ORDER_OPEN_HELP_KEYS;
					}
					if (active_menu->getItemCommandId(active_menu->getSelectedItem()) == GUI_ID_HELP_ABOUT)
					{
						IGUIWindow* window=env->addWindow(core::rect<s32>(10, 10, 400, 200), false, L"Bathymetry Editor");
						if (window)
						{
							env->addStaticText(L"Bathymetry Editor\n2015\nFinnish Meteorological Institute\nsimo.siiria@fmi.fi",
								core::rect<s32>(10,50,400,180),false,true,window);
						}
					}
					break;
				}
			}
			break;
		}
		case EGET_FILE_SELECTED:
		{
			// show the model filename, selected in the file dialog
			IGUIFileOpenDialog* dialog =(IGUIFileOpenDialog*)event.GUIEvent.Caller;
			selected_filename=wchar2str((wchar_t*)dialog->getFileName());
			change_mode_to = BM_MODE_EDIT;
			if(last_menu_selection==GUI_ID_LOAD_BATHY)
				ui_order = BM_UI_ORDER_LOAD_FILE;
			if (last_menu_selection == GUI_ID_SAVE_AS_BATHY)
				ui_order = BM_UI_ORDER_SAVE_AS_FILE;
		}
		break;
		case EGET_FILE_CHOOSE_DIALOG_CANCELLED:
		{
			change_mode_to = BM_MODE_EDIT;
		}

		default:
			break;
		}
	}

	if (event.EventType == EET_MOUSE_INPUT_EVENT)
	{
		switch (event.MouseInput.Event)
		{
			case EMIE_MOUSE_MOVED:
			{
				mouse_x_old = mouse_x;
				mouse_y_old = mouse_y;
				mouse_x = event.MouseInput.X;
				mouse_y = event.MouseInput.Y;
				int diff_x = mouse_x_old - event.MouseInput.X;
				int diff_y = mouse_y_old - event.MouseInput.Y;
				if (event.MouseInput.isRightPressed() )
				{
					if (!pressed_shift)
					{
						if (mouse_x_old > 0 && mouse_y_old > 0) //Move only if these are initilaized
						{
							cam_pos_move = irr::core::vector3df((float)diff_x, -1.0f*(float)diff_y, 0.0f);
						}
					}
				}
				if (event.MouseInput.isLeftPressed() )
				{
					if (!pressed_shift)
					{
						if (mouse_x_old > 0 && mouse_y_old > 0) //Move only if these are initilaized
						{
							//cam_rot_move.Y = (float)diff_x;
							cam_rot_move.X = -1.0f*(float)diff_y;
						}
					}
				}
				if (event.MouseInput.isMiddlePressed())
				{
					if (mouse_x_old > 0 && mouse_y_old > 0) //Move only if these are initilaized
					{
						cam_rot_move.Z = (float)diff_x;
					}
				}
				break;
			}
			case EMIE_MOUSE_WHEEL:
			{
				if (mouse_x_old > 0 && mouse_y_old > 0) //Move only if these are initilaized
				{
					cam_dist_move = -mouse_wheel_sensitivity*event.MouseInput.Wheel;
				}
				break;
			}
			case EMIE_LMOUSE_PRESSED_DOWN:
				if (pressed_shift && !gui_activated)
					ui_order = BM_UI_ORDER_LOWER_POINT;
				break;
			case EMIE_RMOUSE_PRESSED_DOWN:
				if (pressed_shift && !gui_activated)
					ui_order = BM_UI_ORDER_RAISE_POINT;
				break;

		}
	}
	if (activate_toggle_grid)
		wire = !wire;
	if (activate_toggle_smooth)
		smooth ? smooth = false : smooth = true;
	if (scale_all_depths_to_current_depth)
	{
		ui_order = BM_UI_ORDER_SCALE_ALL_DEPTHS;
	}
	return false;
}


