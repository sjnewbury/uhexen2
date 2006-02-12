/*  Copyright (C) 1996-1997  Id Software, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

    See file, 'COPYING', for details.
*/


#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __alpha
#ifdef _WIN32
extern void* my_mutex;
#define	LOCK	WaitForSingleObject (my_mutex, INFINITE)
#define	UNLOCK	ReleaseMutex (my_mutex)
#else //_win32
#include <pthread.h>
extern  pthread_mutex_t *my_mutex;
#define LOCK	pthread_mutex_lock (my_mutex)
#define UNLOCK	pthread_mutex_unlock (my_mutex)
#endif //_win32
#else // __alpha
#define LOCK
#define UNLOCK
#endif // __alpha

extern	int		numthreads;

typedef void (threadfunc_t) (void *);

void InitThreads (void);
void RunThreadsOn ( threadfunc_t func );

