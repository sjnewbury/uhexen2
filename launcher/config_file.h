/*
	config_file.h
	hexen2 launcher config file handling

	$Id: config_file.h,v 1.12 2007-03-15 18:18:15 sezero Exp $

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		51 Franklin St, Fifth Floor,
		Boston, MA  02110-1301, USA
*/

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#define LAUNCHER_CONFIG_FILE "launcher_options"

extern int destiny;
extern int mp_support;
extern int opengl_support;
extern int fullscreen;
extern int resolution;
extern int conwidth;
extern int use_con;
extern int fxgamma;
extern int is8bit;
extern int use_fsaa;
extern int aasamples;
extern int vsync;
extern int use_lm1;
extern int gl_nonstd;
extern char gllibrary[MAX_OSPATH];
extern int midi;
extern int cdaudio;
extern int sound;
extern int sndrate;
extern int sndbits;
extern int lan;
extern int mouse;
extern int debug;
extern int debug2;
extern int use_heap;
extern int use_zone;
extern int heapsize;
extern int zonesize;
#ifndef DEMOBUILD
extern int h2game;
extern int hwgame;
#endif

int write_config_file (void);
int read_config_file (void);

#endif	// CONFIG_FILE_H

