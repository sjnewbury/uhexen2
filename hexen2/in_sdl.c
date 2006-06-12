/*
	in_sdl.c
	SDL game input code

	$Header: /home/ozzie/Download/0000/uhexen2/hexen2/in_sdl.c,v 1.34 2006-06-12 08:48:49 sezero Exp $
*/

#include "sdl_inc.h"
#include "quakedef.h"


extern qboolean	draw_reinit;

// mouse variables
static cvar_t	m_filter = {"m_filter", "0", CVAR_NONE};

static int	mouse_buttons;
static int	mouse_oldbuttonstate;
static int	mouse_x, mouse_y, old_mouse_x, old_mouse_y, mx_accum, my_accum;
extern cvar_t	_enable_mouse;

extern modestate_t	modestate;
extern qboolean	in_mode_set;
static qboolean	mouseactive = false;
static qboolean	mouseinitialized = false;
static qboolean	mouseactivatetoggle = false;
static qboolean	mouseshowtoggle = 1;

// joystick defines and variables
// where should defines be moved?
#define JOY_ABSOLUTE_AXIS	0x00000000		// control like a joystick
#define JOY_RELATIVE_AXIS	0x00000010		// control like a mouse, spinner, trackball
#define	JOY_MAX_AXES		6				// X, Y, Z, R, U, V
#define JOY_AXIS_X			0
#define JOY_AXIS_Y			1
#define JOY_AXIS_Z			2
#define JOY_AXIS_R			3
#define JOY_AXIS_U			4
#define JOY_AXIS_V			5
#if 0
enum _ControlList
{
	AxisNada = 0, AxisForward, AxisLook, AxisSide, AxisTurn
};
static DWORD	dwAxisFlags[JOY_MAX_AXES] =
{
	JOY_RETURNX, JOY_RETURNY, JOY_RETURNZ, JOY_RETURNR, JOY_RETURNU, JOY_RETURNV
};
static DWORD	dwAxisMap[JOY_MAX_AXES];
static DWORD	dwControlMap[JOY_MAX_AXES];
static PDWORD	pdwRawValue[JOY_MAX_AXES];
#endif

// none of these cvars are saved over a session
// this means that advanced controller configuration needs to be executed
// each time.  this avoids any problems with getting back to a default usage
// or when changing from one controller to another.  this way at least something
// works.
static	cvar_t	in_joystick = {"joystick", "1", CVAR_NONE};
static	cvar_t	joy_name = {"joyname", "joystick", CVAR_NONE};
static	cvar_t	joy_advanced = {"joyadvanced", "0", CVAR_NONE};
static	cvar_t	joy_advaxisx = {"joyadvaxisx", "0", CVAR_NONE};
static	cvar_t	joy_advaxisy = {"joyadvaxisy", "0", CVAR_NONE};
static	cvar_t	joy_advaxisz = {"joyadvaxisz", "0", CVAR_NONE};
static	cvar_t	joy_advaxisr = {"joyadvaxisr", "0", CVAR_NONE};
static	cvar_t	joy_advaxisu = {"joyadvaxisu", "0", CVAR_NONE};
static	cvar_t	joy_advaxisv = {"joyadvaxisv", "0", CVAR_NONE};
static	cvar_t	joy_forwardthreshold = {"joyforwardthreshold", "0.15", CVAR_NONE};
static	cvar_t	joy_sidethreshold = {"joysidethreshold", "0.15", CVAR_NONE};
static	cvar_t	joy_pitchthreshold = {"joypitchthreshold", "0.15", CVAR_NONE};
static	cvar_t	joy_yawthreshold = {"joyyawthreshold", "0.15", CVAR_NONE};
static	cvar_t	joy_forwardsensitivity = {"joyforwardsensitivity", "-1.0", CVAR_NONE};
static	cvar_t	joy_sidesensitivity = {"joysidesensitivity", "-1.0", CVAR_NONE};
static	cvar_t	joy_pitchsensitivity = {"joypitchsensitivity", "1.0", CVAR_NONE};
static	cvar_t	joy_yawsensitivity = {"joyyawsensitivity", "-1.0", CVAR_NONE};
static	cvar_t	joy_wwhack1 = {"joywwhack1", "0.0", CVAR_NONE};
static	cvar_t	joy_wwhack2 = {"joywwhack2", "0.0", CVAR_NONE};

#if 0
static qboolean	joy_avail, joy_advancedinit, joy_haspov;
static DWORD	joy_oldbuttonstate, joy_oldpovstate;

static int		joy_id;
static DWORD	joy_flags;
static DWORD	joy_numbuttons;

static JOYINFOEX	ji;
#endif

// forward-referenced functions
static void IN_StartupJoystick (void);
static void Joy_AdvancedUpdate_f (void);
//static void IN_JoyMove (usercmd_t *cmd);

/*
===========
Force_CenterView_f
===========
*/
static void Force_CenterView_f (void)
{
	cl.viewangles[PITCH] = 0;
}


/*
===========
IN_ShowMouse
===========
*/
void IN_ShowMouse (void)
{
	// no need to check mouseinitialized here
	if (!mouseshowtoggle)
	{
		SDL_ShowCursor(1);
		mouseshowtoggle = 1;
	}
}


/*
===========
IN_HideMouse
===========
*/
void IN_HideMouse (void)
{
	// no need to check mouseinitialized here
	if (mouseshowtoggle)
	{
		SDL_ShowCursor (0);
		mouseshowtoggle = 0;
	}
}


/* ============================================================
   NOTES on enabling-disabling the mouse:
   - In windowed mode, mouse is temporarily disabled in main
     menu, so the un-grabbed pointer can be used for desktop
     This state is stored in mousestate_sa as true
   - In fullscreen mode, we don't disable the mouse in menus,
     if we toggle windowed/fullscreen, the above state variable
     is used to correct this in VID_ToggleFullscreen()
   - In the console mode and in the options menu-group, mouse
     is not disabled, and mousestate_sa is set to false
   - Starting a or connecting to a server activates the mouse
     and sets mousestate_sa to false
   - Pausing the game disables (so un-grabs) the mouse, unpausing
     activates it. We don't play with mousestate_sa in such cases
*/

/*
===========
IN_ActivateMouse
===========
*/
void IN_ActivateMouse (void)
{
	if (!mouseinitialized)
		return;

	if (!mouseactivatetoggle)
#if 0	// change to 1 if dont want to disable mouse in fullscreen
		if ((modestate!=MS_WINDOWED) || _enable_mouse.value)
#else
		if (_enable_mouse.value)
#endif
		{
			mouseactivatetoggle = true;
			mouseactive = true;
			SDL_WM_GrabInput (SDL_GRAB_ON);
		}
}

/*
===========
IN_DeactivateMouse
===========
*/
void IN_DeactivateMouse (void)
{
	if (!mouseinitialized)
		return;
	if (mouseactivatetoggle)
	{
		mouseactivatetoggle = false;
		mouseactive = false;
		SDL_WM_GrabInput (SDL_GRAB_OFF);
	}
}

/*
===========
IN_StartupMouse
===========
*/
static void IN_StartupMouse (void)
{
	//IN_HideMouse ();
	if ( COM_CheckParm ("-nomouse") )
	{
		SDL_WM_GrabInput (SDL_GRAB_OFF);
		return;
	}

	mouseinitialized = true;
	mouse_buttons = 3;

	//if (mouseactivatetoggle)
#if 0	// change to 1 if dont want to disable mouse in fullscreen
	if ((modestate!=MS_WINDOWED) || _enable_mouse.value)
#else
	if (_enable_mouse.value)
#endif
		IN_ActivateMouse ();
}


/*
===========
IN_Init
===========
*/
void IN_Init (void)
{
	if (!draw_reinit)
	{
		// mouse variables
		Cvar_RegisterVariable (&m_filter);

		// joystick variables
		Cvar_RegisterVariable (&in_joystick);
		Cvar_RegisterVariable (&joy_name);
		Cvar_RegisterVariable (&joy_advanced);
		Cvar_RegisterVariable (&joy_advaxisx);
		Cvar_RegisterVariable (&joy_advaxisy);
		Cvar_RegisterVariable (&joy_advaxisz);
		Cvar_RegisterVariable (&joy_advaxisr);
		Cvar_RegisterVariable (&joy_advaxisu);
		Cvar_RegisterVariable (&joy_advaxisv);
		Cvar_RegisterVariable (&joy_forwardthreshold);
		Cvar_RegisterVariable (&joy_sidethreshold);
		Cvar_RegisterVariable (&joy_pitchthreshold);
		Cvar_RegisterVariable (&joy_yawthreshold);
		Cvar_RegisterVariable (&joy_forwardsensitivity);
		Cvar_RegisterVariable (&joy_sidesensitivity);
		Cvar_RegisterVariable (&joy_pitchsensitivity);
		Cvar_RegisterVariable (&joy_yawsensitivity);
		Cvar_RegisterVariable (&joy_wwhack1);
		Cvar_RegisterVariable (&joy_wwhack2);

		Cmd_AddCommand ("force_centerview", Force_CenterView_f);
		Cmd_AddCommand ("joyadvancedupdate", Joy_AdvancedUpdate_f);
	}

	IN_StartupMouse ();
	IN_StartupJoystick ();

	SDL_EnableUNICODE(1); /* needed for input in console */
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL*2);
}

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown (void)
{

	IN_DeactivateMouse ();
	IN_ShowMouse ();
}


/*
===========
IN_MouseEvent
===========
*/
static void IN_MouseEvent (int mstate)
{
	int		i;

	if (mouseactive)
	{
	// perform button actions
		for (i=0 ; i<=mouse_buttons ; i++)
		{
			if ( (mstate & SDL_BUTTON(i+1)) &&
				!(mouse_oldbuttonstate & SDL_BUTTON(i+1)) )
			{
				Key_Event (K_MOUSE1 + i, true);
			}

			if ( !(mstate & SDL_BUTTON(i+1)) &&
				(mouse_oldbuttonstate & SDL_BUTTON(i+1)) )
			{
					Key_Event (K_MOUSE1 + i, false);
			}
		}

		mouse_oldbuttonstate = mstate;
	}
}


/*
===========
IN_MouseMove
===========
*/
static void IN_MouseMove (usercmd_t *cmd)
{
	int		mx, my;

	SDL_GetRelativeMouseState(&mx,&my);

	mx_accum = 0;
	my_accum = 0;

	if (m_filter.value)
	{
		mouse_x = (mx + old_mouse_x) * 0.5;
		mouse_y = (my + old_mouse_y) * 0.5;
	}
	else
	{
		mouse_x = mx;
		mouse_y = my;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;

// add mouse X/Y movement to cmd
	if ( (in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;

	if (in_mlook.state & 1)
	{
		if (mx || my)
			V_StopPitchDrift ();
	}

	if ( (in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	else
	{
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}

	if (cl.idealroll == 0) // Did keyboard set it already??
	{
		if (cl.v.movetype == MOVETYPE_FLY)
		{
			if (mouse_x < 0)
				cl.idealroll = -10;
			else if (mouse_x > 0)
				cl.idealroll = 10;
		}
	}
}


/*
===========
IN_Move
===========
*/
void IN_Move (usercmd_t *cmd)
{
	if (cl.v.cameramode)	// Stuck in a different camera so don't move
	{
		memset (cmd, 0, sizeof(*cmd));
		return;
	}

	if (mouseactive)
	{
		IN_MouseMove (cmd);
	}

	// FIXME - Stub - DDOI
	//if (ActiveApp)
	//	IN_JoyMove (cmd);
}


/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates (void)
{

	if (mouseactive)
	{
		mx_accum = 0;
		my_accum = 0;
		mouse_oldbuttonstate = 0;
	}
}


/*
===============
IN_StartupJoystick
===============
*/
static void IN_StartupJoystick (void)
{
// FIXME - Stubbed for now - DDOI
#if 0
	int			i, numdevs;
	JOYCAPS		jc;
	MMRESULT	mmr;

	// assume no joystick
	joy_avail = false;

	// abort startup if user requests no joystick
	if ( COM_CheckParm ("-nojoy") )
		return;

	// verify joystick driver is present
	if ((numdevs = joyGetNumDevs ()) == 0)
	{
		Con_Printf ("\njoystick not found -- driver not present\n\n");
		return;
	}

	// cycle through the joystick ids for the first valid one
	for (joy_id=0 ; joy_id<numdevs ; joy_id++)
	{
		memset (&ji, 0, sizeof(ji));
		ji.dwSize = sizeof(ji);
		ji.dwFlags = JOY_RETURNCENTERED;

		if ((mmr = joyGetPosEx (joy_id, &ji)) == JOYERR_NOERROR)
			break;
	}

	// abort startup if we didn't find a valid joystick
	if (mmr != JOYERR_NOERROR)
	{
		Con_Printf ("\njoystick not found -- no valid joysticks (%x)\n\n", mmr);
		return;
	}

	// get the capabilities of the selected joystick
	// abort startup if command fails
	memset (&jc, 0, sizeof(jc));
	if ((mmr = joyGetDevCaps (joy_id, &jc, sizeof(jc))) != JOYERR_NOERROR)
	{
		Con_Printf ("\njoystick not found -- invalid joystick capabilities (%x)\n\n", mmr);
		return;
	}

	// save the joystick's number of buttons and POV status
	joy_numbuttons = jc.wNumButtons;
	joy_haspov = jc.wCaps & JOYCAPS_HASPOV;

	// old button and POV states default to no buttons pressed
	joy_oldbuttonstate = joy_oldpovstate = 0;

	// mark the joystick as available and advanced initialization not completed
	// this is needed as cvars are not available during initialization
	Con_Printf ("\njoystick detected\n\n");
	joy_avail = true;
	joy_advancedinit = false;
#endif
}


/*
===========
RawValuePointer
===========
*/
#if 0
static PDWORD RawValuePointer (int axis)
{
	switch (axis)
	{
	case JOY_AXIS_X:
		return &ji.dwXpos;
	case JOY_AXIS_Y:
		return &ji.dwYpos;
	case JOY_AXIS_Z:
		return &ji.dwZpos;
	case JOY_AXIS_R:
		return &ji.dwRpos;
	case JOY_AXIS_U:
		return &ji.dwUpos;
	case JOY_AXIS_V:
		return &ji.dwVpos;
	}
	return NULL;
}
#endif


/*
===========
Joy_AdvancedUpdate_f
===========
*/
static void Joy_AdvancedUpdate_f (void)
{
#if 0
	// called once by IN_ReadJoystick and by user whenever an update is needed
	// cvars are now available
	int	i;
	DWORD dwTemp;

	// initialize all the maps
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		dwAxisMap[i] = AxisNada;
		dwControlMap[i] = JOY_ABSOLUTE_AXIS;
		pdwRawValue[i] = RawValuePointer(i);
	}

	if( joy_advanced.value == 0.0)
	{
		// default joystick initialization
		// 2 axes only with joystick control
		dwAxisMap[JOY_AXIS_X] = AxisTurn;
		// dwControlMap[JOY_AXIS_X] = JOY_ABSOLUTE_AXIS;
		dwAxisMap[JOY_AXIS_Y] = AxisForward;
		// dwControlMap[JOY_AXIS_Y] = JOY_ABSOLUTE_AXIS;
	}
	else
	{
		if (strcmp (joy_name.string, "joystick") != 0)
		{
			// notify user of advanced controller
			Con_Printf ("\n%s configured\n\n", joy_name.string);
		}

		// advanced initialization here
		// data supplied by user via joy_axisn cvars
		dwTemp = (DWORD) joy_advaxisx.value;
		dwAxisMap[JOY_AXIS_X] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_X] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD) joy_advaxisy.value;
		dwAxisMap[JOY_AXIS_Y] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Y] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD) joy_advaxisz.value;
		dwAxisMap[JOY_AXIS_Z] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Z] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD) joy_advaxisr.value;
		dwAxisMap[JOY_AXIS_R] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_R] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD) joy_advaxisu.value;
		dwAxisMap[JOY_AXIS_U] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_U] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD) joy_advaxisv.value;
		dwAxisMap[JOY_AXIS_V] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_V] = dwTemp & JOY_RELATIVE_AXIS;
	}

	// compute the axes to collect from DirectInput
	joy_flags = JOY_RETURNCENTERED | JOY_RETURNBUTTONS | JOY_RETURNPOV;
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		if (dwAxisMap[i] != AxisNada)
		{
			joy_flags |= dwAxisFlags[i];
		}
	}
#endif
}


/*
===============
IN_ReadJoystick
===============
*/
#if 0
static qboolean IN_ReadJoystick (void)
{
	memset (&ji, 0, sizeof(ji));
	ji.dwSize = sizeof(ji);
	ji.dwFlags = joy_flags;

	if (joyGetPosEx (joy_id, &ji) == JOYERR_NOERROR)
	{
		// this is a hack -- there is a bug in the Logitech WingMan Warrior DirectInput Driver
		// rather than having 32768 be the zero point, they have the zero point at 32668
		// go figure -- anyway, now we get the full resolution out of the device
		if (joy_wwhack1.value != 0.0)
		{
			ji.dwUpos += 100;
		}
		return true;
	}
	else
	{
		// read error occurred
		// turning off the joystick seems too harsh for 1 read error,
		// but what should be done?
		// Con_Printf ("IN_ReadJoystick: no response\n");
		// joy_avail = false;
		return false;
	}
}
#endif


/*
===========
IN_Commands
===========
*/
void IN_Commands (void)
{
#if 0
	int		i, key_index;
	DWORD	buttonstate, povstate;

	if (!joy_avail)
	{
		return;
	}

	if (cls.state != ca_connected || cls.signon != SIGNONS)
	{
		if (joy_advancedinit != true)
		{
			Joy_AdvancedUpdate_f();
			joy_advancedinit = true;
		}

		IN_ReadJoystick ();
	}

	// loop through the joystick buttons
	// key a joystick event or auxillary event for higher number buttons for each state change
	buttonstate = ji.dwButtons;
	for (i=0 ; i < joy_numbuttons ; i++)
	{
		if ( (buttonstate & (1<<i)) && !(joy_oldbuttonstate & (1<<i)) )
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event (key_index + i, true);
		}

		if ( !(buttonstate & (1<<i)) && (joy_oldbuttonstate & (1<<i)) )
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event (key_index + i, false);
		}
	}
	joy_oldbuttonstate = buttonstate;

	if (joy_haspov)
	{
		// convert POV information into 4 bits of state information
		// this avoids any potential problems related to moving from one
		// direction to another without going through the center position
		povstate = 0;
		if (ji.dwPOV != JOY_POVCENTERED)
		{
			if (ji.dwPOV == JOY_POVFORWARD)
				povstate |= 0x01;
			if (ji.dwPOV == JOY_POVRIGHT)
				povstate |= 0x02;
			if (ji.dwPOV == JOY_POVBACKWARD)
				povstate |= 0x04;
			if (ji.dwPOV == JOY_POVLEFT)
				povstate |= 0x08;
		}
		// determine which bits have changed and key an auxillary event for each change
		for (i=0 ; i < 4 ; i++)
		{
			if ( (povstate & (1<<i)) && !(joy_oldpovstate & (1<<i)) )
			{
				Key_Event (K_AUX29 + i, true);
			}

			if ( !(povstate & (1<<i)) && (joy_oldpovstate & (1<<i)) )
			{
				Key_Event (K_AUX29 + i, false);
			}
		}
		joy_oldpovstate = povstate;
	}
#endif
}


/*
===========
IN_JoyMove
===========
*/
#if 0
static void IN_JoyMove (usercmd_t *cmd)
{
	float	speed, aspeed;
	float	fAxisValue, fTemp;
	int		i;

	// complete initialization if first time in
	// this is needed as cvars are not available at initialization time
	if (joy_advancedinit != true)
	{
		Joy_AdvancedUpdate_f();
		joy_advancedinit = true;
	}

	// verify joystick is available and that the user wants to use it
	if (!joy_avail || !in_joystick.value)
	{
		return;
	}

	// collect the joystick data, if possible
	if (IN_ReadJoystick () != true)
	{
		return;
	}

	if (in_speed.state & 1)
		speed = cl_movespeedkey.value;
	else
		speed = 1;
	aspeed = speed * host_frametime;

	// loop through the axes
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		// get the floating point zero-centered, potentially-inverted data for the current axis
		fAxisValue = (float) *pdwRawValue[i];
		// move centerpoint to zero
		fAxisValue -= 32768.0;

		if (joy_wwhack2.value != 0.0)
		{
			if (dwAxisMap[i] == AxisTurn)
			{
				// this is a special formula for the Logitech WingMan Warrior
				// y=ax^b; where a = 300 and b = 1.3
				// also x values are in increments of 800 (so this is factored out)
				// then bounds check result to level out excessively high spin rates
				fTemp = 300.0 * pow(abs(fAxisValue) / 800.0, 1.3);
				if (fTemp > 14000.0)
					fTemp = 14000.0;
				// restore direction information
				fAxisValue = (fAxisValue > 0.0) ? fTemp : -fTemp;
			}
		}

		// convert range from -32768..32767 to -1..1
		fAxisValue /= 32768.0;

		switch (dwAxisMap[i])
		{
		case AxisForward:
			if ((joy_advanced.value == 0.0) && (in_mlook.state & 1))
			{
				// user wants forward control to become look control
				if (fabs(fAxisValue) > joy_pitchthreshold.value)
				{
					// if mouse invert is on, invert the joystick pitch value
					// only absolute control support here (joy_advanced is false)
					if (m_pitch.value < 0.0)
					{
						cl.viewangles[PITCH] -= (fAxisValue * joy_pitchsensitivity.value) * aspeed * cl_pitchspeed.value;
					}
					else
					{
						cl.viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity.value) * aspeed * cl_pitchspeed.value;
					}
					V_StopPitchDrift();
				}
			}
			else
			{
				// user wants forward control to be forward control
				if (fabs(fAxisValue) > joy_forwardthreshold.value)
				{
//					cmd->forwardmove += (fAxisValue * joy_forwardsensitivity.value) * speed * cl_forwardspeed.value;
					cmd->forwardmove += (fAxisValue * joy_forwardsensitivity.value) * speed * 200;
				}
			}
			break;

		case AxisSide:
			if (fabs(fAxisValue) > joy_sidethreshold.value)
			{
//				cmd->sidemove += (fAxisValue * joy_sidesensitivity.value) * speed * cl_sidespeed.value;
				cmd->sidemove += (fAxisValue * joy_sidesensitivity.value) * speed * 225;
			}
			break;

		case AxisTurn:
			if ((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
			{
				// user wants turn control to become side control
				if (fabs(fAxisValue) > joy_sidethreshold.value)
				{
//					cmd->sidemove -= (fAxisValue * joy_sidesensitivity.value) * speed * cl_sidespeed.value;
					cmd->sidemove -= (fAxisValue * joy_sidesensitivity.value) * speed * 225;
				}
			}
			else
			{
				// user wants turn control to be turn control
				if (fabs(fAxisValue) > joy_yawthreshold.value)
				{
					if (dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						cl.viewangles[YAW] += (fAxisValue * joy_yawsensitivity.value) * aspeed * cl_yawspeed.value;
					}
					else
					{
						cl.viewangles[YAW] += (fAxisValue * joy_yawsensitivity.value) * speed * 180.0;
					}
				}
			}
			break;

		case AxisLook:
			if (in_mlook.state & 1)
			{
				if (fabs(fAxisValue) > joy_pitchthreshold.value)
				{
					// pitch movement detected and pitch movement desired by user
					if (dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						cl.viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity.value) * aspeed * cl_pitchspeed.value;
					}
					else
					{
						cl.viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity.value) * speed * 180.0;
					}
					V_StopPitchDrift();
				}
			}
			break;

		default:
			break;
		}
	}

	// bounds check pitch
	if (cl.viewangles[PITCH] > 80.0)
		cl.viewangles[PITCH] = 80.0;
	if (cl.viewangles[PITCH] < -70.0)
		cl.viewangles[PITCH] = -70.0;
}
#endif

void IN_SendKeyEvents (void)
{
	SDL_Event event;
	int sym, state;
	int modstate;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				if ((event.key.keysym.sym == SDLK_RETURN) &&
				    (event.key.keysym.mod & KMOD_ALT))
				{
				//	SDL_WM_ToggleFullScreen (SDL_GetVideoSurface());
					VID_ToggleFullscreen();
					break;
				}
				else if ((event.key.keysym.sym == SDLK_g) &&
					 (event.key.keysym.mod & KMOD_CTRL))
				{
					SDL_WM_GrabInput( (SDL_WM_GrabInput (SDL_GRAB_QUERY) == SDL_GRAB_ON) ? SDL_GRAB_OFF : SDL_GRAB_ON );
					break;
				}

			case SDL_KEYUP:
				sym = event.key.keysym.sym;
				state = event.key.state;
				modstate = SDL_GetModState();

				switch (key_dest)
				{
					case key_game:
						if ((event.key.keysym.unicode != 0) || (modstate & KMOD_SHIFT))
						{	/* only use unicode for ~ and ` in game mode */
							if ((event.key.keysym.unicode & 0xFF80) == 0)
							{
								if (((event.key.keysym.unicode & 0x7F) == '`') ||
								    ((event.key.keysym.unicode & 0x7F) == '~') )
									sym=event.key.keysym.unicode & 0x7F;
							}
						}
						break;
					case key_message:
					case key_console:
						if ((event.key.keysym.unicode != 0) || (modstate & KMOD_SHIFT))
						{
							if ((sym == SDLK_BACKSPACE) || (sym == SDLK_RETURN))
								break;	// S.A: fixes QNX weirdness
							if ((event.key.keysym.unicode & 0xFF80) == 0)
								sym=event.key.keysym.unicode & 0x7F;
							/* else: it's an international character */
						}
						//printf("You pressed %s (%d) (%c)\n",SDL_GetKeyName(sym),sym,sym);
						break;
					default:
						break;
				}

				switch (sym)
				{
					case SDLK_DELETE:
						sym = K_DEL;
						break;
					case SDLK_BACKSPACE:
						sym = K_BACKSPACE;
						break;
					case SDLK_F1:
						sym = K_F1;
						break;
					case SDLK_F2:
						sym = K_F2;
						break;
					case SDLK_F3:
						sym = K_F3;
						break;
					case SDLK_F4:
						sym = K_F4;
						break;
					case SDLK_F5:
						sym = K_F5;
						break;
					case SDLK_F6:
						sym = K_F6;
						break;
					case SDLK_F7:
						sym = K_F7;
						break;
					case SDLK_F8:
						sym = K_F8;
						break;
					case SDLK_F9:
						sym = K_F9;
						break;
					case SDLK_F10:
						sym = K_F10;
						break;
					case SDLK_F11:
						sym = K_F11;
						break;
					case SDLK_F12:
						sym = K_F12;
						break;
					case SDLK_BREAK:
					case SDLK_PAUSE:
						sym = K_PAUSE;
						break;
					case SDLK_UP:
						sym = K_UPARROW;
						break;
					case SDLK_DOWN:
						sym = K_DOWNARROW;
						break;
					case SDLK_RIGHT:
						sym = K_RIGHTARROW;
						break;
					case SDLK_LEFT:
						sym = K_LEFTARROW;
						break;
					case SDLK_INSERT:
						sym = K_INS;
						break;
					case SDLK_HOME:
						sym = K_HOME;
						break;
					case SDLK_END:
						sym = K_END;
						break;
					case SDLK_PAGEUP:
						sym = K_PGUP;
						break;
					case SDLK_PAGEDOWN:
						sym = K_PGDN;
						break;
					case SDLK_RSHIFT:
					case SDLK_LSHIFT:
						sym = K_SHIFT;
						break;
					case SDLK_RCTRL:
					case SDLK_LCTRL:
						sym = K_CTRL;
						break;
					case SDLK_RALT:
					case SDLK_LALT:
						sym = K_ALT;
						break;
					case SDLK_KP0:
						if (modstate & KMOD_NUM)
							sym = K_INS;
						else
							sym = SDLK_0;
						break;
					case SDLK_KP1:
						if (modstate & KMOD_NUM)
							sym = K_END;
						else
							sym = SDLK_1;
						break;
					case SDLK_KP2:
						if (modstate & KMOD_NUM)
							sym = K_DOWNARROW;
						else
							sym = SDLK_2;
						break;
					case SDLK_KP3:
						if (modstate & KMOD_NUM)
							sym = K_PGDN;
						else
							sym = SDLK_3;
						break;
					case SDLK_KP4:
						if (modstate & KMOD_NUM)
							sym = K_LEFTARROW;
						else
							sym = SDLK_4;
						break;
					case SDLK_KP5:
						sym = SDLK_5;
						break;
					case SDLK_KP6:
						if (modstate & KMOD_NUM)
							sym = K_RIGHTARROW;
						else
							sym = SDLK_6;
						break;
					case SDLK_KP7:
						if (modstate & KMOD_NUM)
							sym = K_HOME;
						else
							sym = SDLK_7;
						break;
					case SDLK_KP8:
						if (modstate & KMOD_NUM)
							sym = K_UPARROW;
						else
							sym = SDLK_8;
						break;
					case SDLK_KP9:
						if (modstate & KMOD_NUM)
							sym = K_PGUP;
						else
							sym = SDLK_9;
						break;
					case SDLK_KP_PERIOD:
						if (modstate & KMOD_NUM)
							sym = K_DEL;
						else
							sym = SDLK_PERIOD;
						break;
					case SDLK_KP_DIVIDE:
						sym = SDLK_SLASH;
						break;
					case SDLK_KP_MULTIPLY:
						sym = SDLK_ASTERISK;
						break;
					case SDLK_KP_MINUS:
						sym = SDLK_MINUS;
						break;
					case SDLK_KP_PLUS:
						sym = SDLK_PLUS;
						break;
					case SDLK_KP_ENTER:
						sym = SDLK_RETURN;
						break;
					case SDLK_KP_EQUALS:
						sym = SDLK_EQUALS;
						break;
					case 178: /* the '�' key */
						//sym = 178;
						sym = '~';
				}
				// If we're not directly handled and still above
				// 255 just force it to 0
				if (sym > 255)
					sym = 0;
				Key_Event (sym, state);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (!mouseactive || in_mode_set)
					break;
				switch(event.button.button)
				{
				case 1:
					Key_Event(K_MOUSE1, true);
					break;
				case 2:
					Key_Event(K_MOUSE2, true);
					break;
				case 3:
					Key_Event(K_MOUSE3, true);
					break;
				case 4:
					Key_Event(K_MWHEELUP, true);
					break;
				case 5:
					Key_Event(K_MWHEELDOWN, true);
					break;
				case 6:
					Key_Event(K_MOUSE4, true);
					break;
				case 7:
					Key_Event(K_MOUSE5, true);
					break;
				default:
					Con_Printf("Mouse event for button %d received, 1-7 expected\n", event.button.button);
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (!mouseactive || in_mode_set)
					break;
				switch(event.button.button)
				{
				case 1:
					Key_Event(K_MOUSE1, false);
					break;
				case 2:
					Key_Event(K_MOUSE2, false);
					break;
				case 3:
					Key_Event(K_MOUSE3, false);
					break;
				case 4:
					Key_Event(K_MWHEELUP, false);
					break;
				case 5:
					Key_Event(K_MWHEELDOWN, false);
					break;
				case 6:
					Key_Event(K_MOUSE4, false);
					break;
				case 7:
					Key_Event(K_MOUSE5, false);
					break;
				default:
					Con_Printf("Mouse event for button %d received, 1-7 expected\n", event.button.button);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				if (!mouseactive || in_mode_set)
					break;
				IN_MouseEvent (SDL_GetMouseState(NULL,NULL));
				break;

			case SDL_QUIT:
				if (draw_reinit)
					break;
				CL_Disconnect ();
				Sys_Quit ();
				break;
			default:
				break;
		}
	}
}

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.33  2006/05/31 16:49:02  sezero
 * cl_input.c, in_sdl.c, in_win.c, hexenworld/Client/view.c: made
 * things a little bit more readable.
 *
 * Revision 1.32  2006/03/24 15:05:39  sezero
 * killed the archive, server and info members of the cvar structure.
 * the new flags member is now employed for all those purposes. also
 * made all non-globally used cvars static.
 *
 * Revision 1.31  2006/02/18 08:51:10  sezero
 * continue making static functions and vars static. whitespace and coding style
 * cleanup. also renamed the variables name and dest to savename and savedest in
 * host_cmd.c to prevent any confusion and pollution.
 *
 * Revision 1.30  2006/01/14 08:39:24  sezero
 * fixed the incorrect (mislead) usage of modestate values, although the result
 * doesn't change.
 *
 * Revision 1.29  2006/01/12 12:43:49  sezero
 * Created an sdl_inc.h with all sdl version requirements and replaced all
 * SDL.h and SDL_mixer.h includes with it. Made the source to compile against
 * SDL versions older than 1.2.6 without disabling multisampling. Multisampling
 * (fsaa) option is now decided at runtime. Minimum required SDL and SDL_mixer
 * versions are now 1.2.4. If compiled without midi, minimum SDL required is
 * 1.2.0. Added SDL_mixer version checking to sdl-midi with measures to prevent
 * relocation errors.
 *
 * Revision 1.28  2006/01/12 12:34:38  sezero
 * added video modes enumeration via SDL. added on-the-fly video mode changing
 * partially based on the Pa3PyX hexen2 tree. TODO: make the game remember its
 * video settings, clean it up, fix it up...
 *
 * Revision 1.27  2005/10/21 18:13:20  sezero
 * made key repeats work
 *
 * Revision 1.26  2005/09/28 06:07:32  sezero
 * renamed ToggleFullScreenSA to VID_ToggleFullscreen which
 * actually is of VID_ class and now is easier to locate
 *
 * Revision 1.25  2005/07/16 23:23:52  sezero
 * killed fastfabs, not worthy of keeping anymore
 *
 * Revision 1.24  2005/06/06 13:10:20  sezero
 * mlook and lookspring fixes by J.Krige
 * ( http://www.quakesrc.org/tutorials/old/1 )
 *
 * Revision 1.23  2005/06/03 13:25:29  sezero
 * Latest mouse fixes and clean-ups
 *
 * Revision 1.22  2005/05/21 17:10:58  sezero
 * re-enabled complete disabling/enabling of mousa in fullscreen
 * mode. (only replaced a bunch of if 1's to if 0's)
 *
 * Revision 1.21  2005/05/21 17:04:16  sezero
 * - revived -nomouse that "disables mouse no matter what"
 * - renamed _windowed_mouse to _enable_mouse which is our intention,
 *   that is, dynamically disabling/enabling mouse while in game
 * - old code had many oversights/leftovers that prevented mouse being
 *   really disabled in fullscreen mode. fixed and cleaned-up here
 * - even in windowed mode, when mouse was disabled, mouse buttons and
 *   the wheel got processed. fixed it here.
 * - mouse cursor is never shown while the game is alive, regardless
 *   of mouse being enabled/disabled (I never liked an ugly pointer
 *   around while playing) Its only intention would be to be able to
 *   use the desktop, and for that see, the grab notes below
 * - if mouse is disabled, it is un-grabbed in windowed mode. Note: One
 *   can always use the keyboard shortcut CTRL-G for grabbing-ungrabbing
 *   the mouse regardless of mouse being enabled/disabled.
 * - ToggleFullScreenSA() used to update vid_mode but always forgot
 *   modestate. It now updates modestate as well.
 * - Now that IN_ActivateMouse() and IN_DeactivateMouse() are fixed,
 *   IN_ActivateMouseSA() and IN_DeactivateMouseSA() are redundant and
 *   are removed. BTW, I added a new qboolean mousestate_sa (hi Steve)
 *   which keeps track of whether we intentionally disabled the mouse.
 * - mouse disabling in fullscreen mode (in the absence of -nomouse
 *   arg) is not allowed in this patch, but this is done by a if 1/if 0
 *   conditional compilation. Next patch will change all those if 1 to
 *   if 0, and voila!, we can fully disable/enable mouse in fullscreen.
 * - moved modestate enums/defines to vid.h so that they can be used
 *   by other code properly.
 *
 * Revision 1.20  2005/05/21 16:53:15  sezero
 * killed another unused mouse function
 *
 * Revision 1.19  2005/05/17 06:47:50  sezero
 * initial cosmetic cleanup in in_sdl.c
 *
 * Revision 1.18  2005/04/30 09:59:17  sezero
 * Many things in gl_vidsdl.c, and *especially in vid_sdl.c, are there
 * for the dynamic video mode swithching which we removed a long time
 * ago (and we don't seem to put it back in any foreseeable future.)
 * Some stuff were there only to provide human readable descriptions in
 * the menu and I removed them in 1.2.3 or in 1.2.4. In this patch:
 * 1. Tried cleaning-up the remaining mess: There still were some
 *    windoze left-overs, unused variables/cvars, functions using those
 *    vars/cvars serving no purpose (especially those window_rect and
 *    window_center stuff, and more). I removed them as best as I could.
 *    There still are things in vid_sdl.c that I didn't fully understand,
 *    they are there, for now.
 * 2. The -window and -w cmdline args are now now removed: They actually
 *    did nothing, unless the user did some silly thing like using both
 *    -w and -f on the same cmdline.
 * 3. The two mode-setting functions (windowed and f/s) are made into one
 *    as VID_SDL_SetMode
 * 4. The -height arg now is functional *only* if used together -height.
 *    Since we only do the normal modes, I removed the width switch and
 *    calculated:  height = 3*width/4
 *    Issue: We need some sanity check in case of both -width and -height
 *    args are specified
 * 5. -bpp wasn't written into modenum[x].bpp, I did it here. As a side
 *    note, bpp doesn't affect anything, or my eyes are in more need of a
 *    doctor than I know: -bpp 8 / 16 / 32 give the same picture.
 * 6. The code calls VID_SetPalette very multiple times in gl_vidsdl.c.
 *    Why the hell is that?.. Something windoze spesific?  I unified them
 *    here in VID_Init: After VID_SetMode, VID_SetPalette is called first,
 *    and then 8-bit palette is activated if -paltex is specified.
 *    Note: I didn't touch vid_sdl.c in this manner, but DDOI (one of the
 *    guys during Dan's porting, perpahs) has a comment on a VID_SetPalette
 *    call being "Useless".
 * 7. Many whitespace clean-up as a bonus material ;)
 *
 * Revision 1.17  2005/02/11 08:31:15  sezero
 * remove unused-and-emptied IN_Accumulate
 *
 * Revision 1.16  2005/02/11 08:30:29  sezero
 * remove already commented-out win32 code from IN_StartupMouse
 *
 * Revision 1.15  2005/02/08 21:10:22  sezero
 * tiny whitespace cleanup
 *
 * Revision 1.14  2005/01/23 15:17:13  sezero
 * support for mouse buttons 4 and 5 (patch from Julien Langer)
 * Let's see if this behaves for everybody.
 *
 * Revision 1.13  2005/01/13 10:46:45  sezero
 * - Fixed mouse behavior which was always broken in hexen2-linux.
 *   Middle-button is MOUSE2, right-button is MOUSE3, not vice versa
 *   (see SDL_mouse.h). The wheelmouse patch fixed this a little, but
 *   introduced a mouse2/mouse3 fight in in_sdl.c. It is fixed here.
 * - Fixed the debug message about undefined mouse buttons.
 * - Did some syncing between H2 and HW versions of input files.
 *
 * Revision 1.12  2005/01/05 14:20:00  sezero
 * bind Alt+Enter combination to Steven's stuff for fullscreen-windowed toggling
 *
 * Revision 1.11  2004/12/29 19:49:40  sezero
 * From Steven (2004-12-29):
 * - Fullscreen/Windowed mode is now switchable. Seems to work good.
 * - Mouse can now be disabled for fullscreen modes.
 *
 * Revision 1.10  2004/12/18 14:08:07  sezero
 * Clean-up and kill warnings 9:
 * Kill many unused vars.
 *
 * Revision 1.9  2004/12/18 13:52:54  sezero
 * Clean-up and kill warnings 5:
 * Misc irritants..
 *
 * Revision 1.8  2004/12/18 13:17:10  sezero
 * steven fixes the mouse once again ;
 *
 * Revision 1.7  2004/12/12 14:38:18  sezero
 * steven fixed the mouse again ;)
 *
 * Revision 1.6  2004/12/12 14:14:42  sezero
 * style changes to our liking
 *
 * Revision 1.5  2004/12/05 11:00:37  sezero
 * added wheelmouse support.
 *
 * Revision 1.4  2004/12/05 10:52:18  sezero
 * Sync with Steven, 2004-12-04 :
 *  Fix the "Old Mission" menu PoP
 *  Also release the windowed mouse on pause
 *  Heapsize is now 32768 default
 *  The final splash screens now centre the messages properly
 *  Add more mods to the video mods table
 *  Add the docs folder and update it
 *
 * Revision 1.3  2004/12/04 18:47:47  sezero
 * Kill some compiler warnings. Add sys_ia32.s to the "make clean" lists.
 *
 * Revision 1.2  2004/11/28 00:58:08  sezero
 *
 * Commit Steven's changes as of 2004.11.24:
 *
 * * Rewritten Help/Version message(s)
 * * Proper fullscreen mode(s) for OpenGL.
 * * Screen sizes are selectable with "-width" and "-height" options.
 * * Mouse grab in window modes , which is released when menus appear.
 * * Interactive video modes in software game disabled.
 * * Replaced Video Mode menu with a helpful message.
 * * New menu items for GL Glow, Chase mode, Draw Shadows.
 * * Changes to initial cvar_t variables:
 *      r_shadows, gl_other_glows, _windowed_mouse,
 *
 * Revision 1.1.1.1  2004/11/28 00:04:39  sezero
 * Initial import of AoT 1.2.0 code
 *
 * Revision 1.5  2002/01/01 12:28:13  phneutre
 * better handling of ~, ` and � keys
 *
 * Revision 1.4  2001/12/14 17:58:49  phneutre
 * unicode support in console mode
 *
 * Revision 1.3  2001/11/12 23:57:21  theoddone33
 * Alt-enter and ^G support
 *
 * Revision 1.2  2001/11/12 23:31:58  theoddone33
 * Some Loki-ish parameters and general cleanup/bugfixes.
 *
 * 3     3/01/98 8:20p Jmonroe
 * removed the slow "quake" version of common functions
 * 
 * 2     2/04/98 12:53a Jmonroe
 * added fastfabs
 * 
 * 10    9/01/97 4:29a Rjohnson
 * Joystick fix
 * 
 * 9     8/27/97 12:11p Rjohnson
 * Hardcoded speeds
 * 
 * 8     8/26/97 11:38a Rlove
 * 
 * 7     7/15/97 1:59p Rjohnson
 * Fix for mouse and client
 * 
 * 6     3/25/97 11:28a Rlove
 * New camera entity
 * 
 * 5     3/07/97 1:46p Rjohnson
 * Id Updates
 */
