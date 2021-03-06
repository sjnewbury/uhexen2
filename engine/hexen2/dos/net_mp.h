/*
 * net_mpath.h --
 * MPATH TCP driver: the (Winsock) UDP driver for dosquake
 * for use when run from within win95.
 * from quake1 source with minor adaptations for uhexen2.
 * $Id: net_mp.h,v 1.4 2009-01-08 12:01:51 sezero Exp $
 *
 * Copyright (C) 1996-1997  Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __net_mpath_h
#define __net_mpath_h

int  MPATH_Init (void);
void MPATH_Shutdown (void);
void MPATH_Listen (qboolean state);
int  MPATH_OpenSocket (int port);
int  MPATH_CloseSocket (int socketid);
int  MPATH_Connect (int socketid, struct qsockaddr *addr);
int  MPATH_CheckNewConnections (void);
int  MPATH_Read (int socketid, byte *buf, int len, struct qsockaddr *addr);
int  MPATH_Write (int socketid, byte *buf, int len, struct qsockaddr *addr);
int  MPATH_Broadcast (int socketid, byte *buf, int len);
const char *MPATH_AddrToString (struct qsockaddr *addr);
int  MPATH_StringToAddr (const char *string, struct qsockaddr *addr);
int  MPATH_GetSocketAddr (int socketid, struct qsockaddr *addr);
int  MPATH_GetNameFromAddr (struct qsockaddr *addr, char *name);
int  MPATH_GetAddrFromName (const char *name, struct qsockaddr *addr);
int  MPATH_AddrCompare (struct qsockaddr *addr1, struct qsockaddr *addr2);
int  MPATH_GetSocketPort (struct qsockaddr *addr);
int  MPATH_SetSocketPort (struct qsockaddr *addr, int port);

#endif	/* __net_mpath_h */

