#include "psx/libpad.h"
#include "psx/libetc.h"

#include "../PsyX_main.h"
#include "PsyX_pad.h"
#include "PsyX/PsyX_public.h"

#include <string.h>

extern "C"
{
extern int g_padCommEnable;
}

typedef struct
{
	Sint32				deviceId;	// linked device Id
	SDL_GameController* gc;

	u_char*				padData;
	bool				switchingAnalog;
} PsyXController;

int						g_cfg_controllerToSlotMapping[MAX_CONTROLLERS] = { -1, -1 };

PsyXController			g_controllers[MAX_CONTROLLERS];
const u_char*			g_sdlKeyboardState = NULL;

u_short PsyX_Pad_UpdateKeyboardInput();
void	PsyX_Pad_UpdateGameControllerInput(SDL_GameController* cont, LPPADRAW pad);

// Initializes SDL controllers
int PsyX_Pad_InitSystem()
{
	// do not init second time!
	if (g_sdlKeyboardState != NULL)
		return 1;

	memset(g_controllers, 0, sizeof(g_controllers));

	// init keyboard state
	g_sdlKeyboardState = SDL_GetKeyboardState(NULL);

	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0)
	{
		eprinterr("Failed to initialise SDL GameController subsystem!\n");
		return 0;
	}

	// Add more controllers from custom file
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

	return 1;
}

// Prints controller list into console
void PsyX_Pad_Debug_ListControllers()
{
	int numJoysticks = SDL_NumJoysticks();
	int numHaptics = SDL_NumHaptics();

	if (numJoysticks)
	{
		eprintf("SDL GameController list:\n");

		for (int i = 0; i < numJoysticks; i++)
		{
			if (SDL_IsGameController(i))
			{
				eprintinfo("  %d '%s'\n", i, SDL_GameControllerNameForIndex(i));
			}
		}
	}
	else
		eprintwarn("No SDL GameControllers found!\n");

	if (numHaptics)
	{
		eprintf("SDL haptic list:\n");

		for (int i = 0; i < numHaptics; i++)
		{
			eprintinfo("  %d '%s'\n", i, SDL_HapticName(i));
		}
	}
	else
		eprintwarn("No SDL haptics found!\n");
}

// Opens specific system controller and assigns to specified slot
void PsyX_Pad_OpenController(Sint32 deviceId, int slot)
{
	PsyXController* controller = &g_controllers[slot];

	if (controller->gc)
	{
		return;
	}

	controller->gc = SDL_GameControllerOpen(deviceId);
	controller->switchingAnalog = false;

	if (controller->gc)
	{
		// assign device id automatically
		if (controller->deviceId == -1)
			controller->deviceId = deviceId;
	}
}

// Closes controller in specific slot
void PsyX_Pad_CloseController(int slot)
{
	PsyXController* controller = &g_controllers[slot];
	SDL_GameControllerClose(controller->gc);

	controller->gc = NULL;
}

// Called from LIBPAD
void PsyX_Pad_InitPad(int slot, u_char* padData)
{
	PsyXController* controller = &g_controllers[slot];

	controller->padData = padData;
	controller->deviceId = g_cfg_controllerToSlotMapping[slot];

	if (padData)
	{
		LPPADRAW pad = (LPPADRAW)padData;
		
		bool wasConnected = (pad->id == 0x41 || pad->id == 0x73);

		if(!wasConnected)
			pad->id = slot == 0 ? 0x41 : 0xFF;	// since keyboard is a main controller - it's always on

		// only reset buttons
		pad->buttons[0] = 0xFF;
		pad->buttons[1] = 0xFF;
		pad->analog[0] = 128;
		pad->analog[1] = 128;
		pad->analog[2] = 128;
		pad->analog[3] = 128;
	}
}

// called from Psy-X SDL events
void PsyX_Pad_Event_ControllerAdded(Sint32 deviceId)
{
	int i;
	PsyXController* controller;

	// reinitialize haptics (why we still here?)
	SDL_QuitSubSystem(SDL_INIT_HAPTIC);			// FIXME: this will crash if you already have haptics
	SDL_InitSubSystem(SDL_INIT_HAPTIC);

	PsyX_Pad_Debug_ListControllers();

	// find mapping and open
	for (i = 0; i < MAX_CONTROLLERS; i++)
	{
		controller = &g_controllers[i];

		if (controller->deviceId == -1 || controller->deviceId == deviceId)
		{
			PsyX_Pad_OpenController(deviceId, i);
			break;
		}
	}
}

// called from Psy-X SDL events
void PsyX_Pad_Event_ControllerRemoved(Sint32 deviceId)
{
	int i;
	PsyXController* controller;

	PsyX_Pad_Debug_ListControllers();

	// find mapping and close
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		controller = &g_controllers[i];

		if (controller->deviceId == deviceId)
		{
			PsyX_Pad_CloseController(i);
		}
	}
}

void PsyX_Pad_InternalPadUpdates()
{
	PsyXController* controller;
	LPPADRAW pad;
	u_short kbInputs;

	if (g_padCommEnable == 0)
		return;

	kbInputs = PsyX_Pad_UpdateKeyboardInput();

	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		controller = &g_controllers[i];

		if (controller->padData)
		{
			pad = (LPPADRAW)controller->padData;

			PsyX_Pad_UpdateGameControllerInput(controller->gc, pad);

			ushort test = *(u_short*)pad->buttons;

			// In order to switch From/To analog user has to use left gamepad stick

			// Select + Start pressed
			if ((test & 0x1) == 0 && (test & 0x8) == 0)
			{
				*(u_short*)pad->buttons = 0xffff;

				if (!controller->switchingAnalog)
				{
					// switch to analog state
					if (pad->id == 0x41)
					{
						eprintf("Port %d ANALOG: ON\n", i + 1);
						pad->id = 0x73;
					}
					else
					{
						eprintf("Port %d ANALOG: OFF\n", i + 1);
						pad->id = 0x41;
					}
				}
				controller->switchingAnalog = true;
			}
			else
			{
				controller->switchingAnalog = false;
			}

			// Update keyboard for PAD
			if ((g_activeKeyboardControllers & (1 << i)) && kbInputs != 0xffff)
			{
				pad->status = 0;	// PadStateStable?

				if (pad->id != 0x41)
				{
					if(pad->id != 0x73)
						eprintf("Port %d ANALOG: OFF\n", i + 1);

					pad->id = 0x41; // force disable analog
				}

				*(u_short*)pad->buttons &= kbInputs;
			}
		}
	}

#if defined(__ANDROID__)
	///@TODO SDL_NumJoysticks always reports > 0 for some reason on Android.
#endif
}


int GetControllerButtonState(SDL_GameController* cont, int buttonOrAxis)
{
	if (buttonOrAxis & CONTROLLER_MAP_FLAG_AXIS)
	{
		int value = SDL_GameControllerGetAxis(cont, (SDL_GameControllerAxis)(buttonOrAxis & ~(CONTROLLER_MAP_FLAG_AXIS | CONTROLLER_MAP_FLAG_INVERSE)));

		if (abs(value) > 500 && (buttonOrAxis & CONTROLLER_MAP_FLAG_INVERSE))
			value *= -1;

		return value;
	}

	return SDL_GameControllerGetButton(cont, (SDL_GameControllerButton)buttonOrAxis) * 32767;
}

void PsyX_Pad_UpdateGameControllerInput(SDL_GameController* cont, LPPADRAW pad)
{
	short leftX, leftY, rightX, rightY;
	u_short ret = 0xFFFF;

	if (!cont)
	{
		pad->analog[0] = 127;
		pad->analog[1] = 127;
		pad->analog[2] = 127;
		pad->analog[3] = 127;

		*(u_short*)pad->buttons = ret;
		return;
	}

	// TODOL separate mapping for Pad 2
	const PsyXControllerMapping& mapping = g_cfg_controllerMapping;

	if (GetControllerButtonState(cont, mapping.gc_square) > 16384)//Square
		ret &= ~0x8000;

	if (GetControllerButtonState(cont, mapping.gc_circle) > 16384)//Circle
		ret &= ~0x2000;

	if (GetControllerButtonState(cont, mapping.gc_triangle) > 16384)//Triangle
		ret &= ~0x1000;

	if (GetControllerButtonState(cont, mapping.gc_cross) > 16384)//Cross
		ret &= ~0x4000;

	if (GetControllerButtonState(cont, mapping.gc_l1) > 16384)//L1
		ret &= ~0x400;

	if (GetControllerButtonState(cont, mapping.gc_r1) > 16384)//R1
		ret &= ~0x800;

	if (GetControllerButtonState(cont, mapping.gc_l2) > 16384)//L2
		ret &= ~0x100;

	if (GetControllerButtonState(cont, mapping.gc_r2) > 16384)//R2
		ret &= ~0x200;

	if (GetControllerButtonState(cont, mapping.gc_dpad_up) > 16384)//UP
		ret &= ~0x10;

	if (GetControllerButtonState(cont, mapping.gc_dpad_down) > 16384)//DOWN
		ret &= ~0x40;

	if (GetControllerButtonState(cont, mapping.gc_dpad_left) > 16384)//LEFT
		ret &= ~0x80;

	if (GetControllerButtonState(cont, mapping.gc_dpad_right) > 16384)//RIGHT
		ret &= ~0x20;

	if (GetControllerButtonState(cont, mapping.gc_l3) > 16384)//L3
		ret &= ~0x2;

	if (GetControllerButtonState(cont, mapping.gc_r3) > 16384)//R3
		ret &= ~0x4;

	if (GetControllerButtonState(cont, mapping.gc_select) > 16384)//SELECT
		ret &= ~0x1;

	if (GetControllerButtonState(cont, mapping.gc_start) > 16384)//START
		ret &= ~0x8;

	leftX = GetControllerButtonState(cont, mapping.gc_axis_left_x);
	leftY = GetControllerButtonState(cont, mapping.gc_axis_left_y);

	rightX = GetControllerButtonState(cont, mapping.gc_axis_right_x);
	rightY = GetControllerButtonState(cont, mapping.gc_axis_right_y);

	*(u_short*)pad->buttons = ret;

	// map to range
	pad->analog[0] = (rightX / 256) + 128;
	pad->analog[1] = (rightY / 256) + 128;
	pad->analog[2] = (leftX / 256) + 128;
	pad->analog[3] = (leftY / 256) + 128;
}

u_short PsyX_Pad_UpdateKeyboardInput()
{
	u_short ret = 0xFFFF;

	//Not initialised yet
	if (g_sdlKeyboardState == NULL)
		return ret;

	SDL_PumpEvents();

	const PsyXKeyboardMapping& mapping = g_cfg_keyboardMapping;

	if (g_sdlKeyboardState[mapping.kc_square])//Square
		ret &= ~0x8000;

	if (g_sdlKeyboardState[mapping.kc_circle])//Circle
		ret &= ~0x2000;

	if (g_sdlKeyboardState[mapping.kc_triangle])//Triangle
		ret &= ~0x1000;

	if (g_sdlKeyboardState[mapping.kc_cross])//Cross
		ret &= ~0x4000;

	if (g_sdlKeyboardState[mapping.kc_l1])//L1
		ret &= ~0x400;

	if (g_sdlKeyboardState[mapping.kc_l2])//L2
		ret &= ~0x100;

	if (g_sdlKeyboardState[mapping.kc_l3])//L3
		ret &= ~0x2;

	if (g_sdlKeyboardState[mapping.kc_r1])//R1
		ret &= ~0x800;

	if (g_sdlKeyboardState[mapping.kc_r2])//R2
		ret &= ~0x200;

	if (g_sdlKeyboardState[mapping.kc_r3])//R3
		ret &= ~0x4;

	if (g_sdlKeyboardState[mapping.kc_dpad_up])//UP
		ret &= ~0x10;

	if (g_sdlKeyboardState[mapping.kc_dpad_down])//DOWN
		ret &= ~0x40;

	if (g_sdlKeyboardState[mapping.kc_dpad_left])//LEFT
		ret &= ~0x80;

	if (g_sdlKeyboardState[mapping.kc_dpad_right])//RIGHT
		ret &= ~0x20;

	if (g_sdlKeyboardState[mapping.kc_select])//SELECT
		ret &= ~0x1;

	if (g_sdlKeyboardState[mapping.kc_start])//START
		ret &= ~0x8;

	return ret;
}

int PsyX_Pad_GetStatus(int mtap, int slot)
{
	PsyXController* controller;

	if (slot == 0)
		return 1;	// keyboard always here

	controller = &g_controllers[slot];

	if (controller->gc && SDL_GameControllerGetAttached(controller->gc))
		return 1;

	return 0;
}

void PsyX_Pad_Vibrate(int mtap, int slot, unsigned char* table, int len)
{
	PsyXController* controller = &g_controllers[slot];

	if (len == 0)
		return;

	Uint16 freq_high	= table[0] * 255;
	Uint16 freq_low		= len > 1 ? table[1] * 255 : 0;

	// apply minimal shake
	if(freq_low != 0 && freq_low < 4096)
		freq_low = 4096;

	if (freq_high != 0 && freq_high < 4096)
		freq_high = 4096;

	SDL_GameControllerRumble(controller->gc, freq_low, freq_high, 200);
}
