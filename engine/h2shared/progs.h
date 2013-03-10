/*
 * progs.h -- PROGS structures, public functions and vars
 * $Id$
 *
 * Copyright (C) 1996-1997  Id Software, Inc.
 * Copyright (C) 1997-1998  Raven Software Corp.
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

#ifndef __HX2_PROGS_H
#define __HX2_PROGS_H

#include "pr_comp.h"	/* defs shared with qcc */
#include "progdefs.h"	/* generated by program cdefs */

typedef union eval_s
{
	string_t	string;
	float		_float;
	float		vector[3];
	func_t		function;
	int		_int;
	int		edict;
} eval_t;

#define	MAX_ENT_LEAFS	16
typedef struct edict_s
{
	qboolean	free;
	link_t		area;			/* linked to a division node or leaf */

	int		num_leafs;
	short		leafnums[MAX_ENT_LEAFS];

	entity_state_t	baseline;

	float		freetime;		/* sv.time when the object was freed */
	float		alloctime;		/* sv.time when the object was allocated */
	entvars_t	v;			/* C exported fields from progs */

	/* other fields from progs come immediately after */
} edict_t;

#define	EDICT_FROM_AREA(l)	STRUCT_FROM_LINK(l,edict_t,area)

//============================================================================

extern	dprograms_t	*progs;
extern	dfunction_t	*pr_functions;
extern	dstatement_t	*pr_statements;
extern	sv_globals_t	sv_globals;
extern	float		*pr_globals;	/* same as sv_globals */

extern	int		pr_edict_size;	/* in bytes */

extern	qboolean	is_progs_v6;


/* If USE_MULTIPLE_PROGS is defined as 1, the hexen2 binary will look for
 * a file named "maplist.txt" in its searchpath and using the info in it,
 * it will load a map-specific prog file. Without this, the rider bosses
 * shall not appear in the original Hexen2. The mission pack & HexenWorld
 * uses a single prog file and don't need this.
 * When progs.dat exists (it usually does), uHexen2 accepts maplist.txt
 * only if it is from the same game directory as progs.dat itself or if
 * it is from a searchpath with a higher priority. */
#define	USE_MULTIPLE_PROGS	1

/* USE_MULTIPLE_PROGS is only for original hexen2. */
#if defined(H2W)
#undef	USE_MULTIPLE_PROGS
#define	USE_MULTIPLE_PROGS	0
#endif


void PR_Init (void);

void PR_ExecuteProgram (func_t fnum);
void PR_LoadProgs (void);

const char *PR_GetString (int num);
int PR_SetEngineString (const char *s);
int PR_AllocString (int bufferlength, char **ptr);

void PR_Profile_f (void);

edict_t *ED_Alloc (void);
edict_t *ED_Alloc_Temp (void);
void ED_Free (edict_t *ed);
void ED_ClearEdict (edict_t *e);

void ED_Print (edict_t *ed);
void ED_Write (FILE *f, edict_t *ed);
const char *ED_ParseEdict (const char *data, edict_t *ent);

void ED_WriteGlobals (FILE *f);
void ED_ParseGlobals (const char *data);

void ED_LoadFromFile (const char *data);

/*
#define EDICT_NUM(n)		((edict_t *)(sv.edicts+ (n)*pr_edict_size))
#define NUM_FOR_EDICT(e)	(((byte *)(e) - sv.edicts) / pr_edict_size)
*/
edict_t *EDICT_NUM(int n);
int NUM_FOR_EDICT(edict_t *e);

#define	NEXT_EDICT(e)		((edict_t *)( (byte *)e + pr_edict_size))

#define	EDICT_TO_PROG(e)	((byte *)e - (byte *)sv.edicts)
#define PROG_TO_EDICT(e)	((edict_t *)((byte *)sv.edicts + e))

#define	G_FLOAT(o)		(pr_globals[o])
#define	G_INT(o)		(*(int *)&pr_globals[o])
#define	G_EDICT(o)		((edict_t *)((byte *)sv.edicts+ *(int *)&pr_globals[o]))
#define G_EDICTNUM(o)		NUM_FOR_EDICT(G_EDICT(o))
#define	G_VECTOR(o)		(&pr_globals[o])
#define	G_STRING(o)		(PR_GetString(*(string_t *)&pr_globals[o]))
#define	G_FUNCTION(o)		(*(func_t *)&pr_globals[o])

#define	E_FLOAT(e,o)		(((float*)&e->v)[o])
#define	E_INT(e,o)		(*(int *)&((float*)&e->v)[o])
#define	E_VECTOR(e,o)		(&((float*)&e->v)[o])
#define	E_STRING(e,o)		(PR_GetString(*(string_t *)&((float*)&e->v)[o]))

typedef void (*builtin_t) (void);
extern	builtin_t	*pr_builtins;
extern	int		pr_numbuiltins;

extern	int		pr_argc;

extern	qboolean	pr_trace;
extern	dfunction_t	*pr_xfunction;
extern	int		pr_xstatement;

extern	unsigned short	pr_crc;

#if defined(H2W)
extern	func_t		SpectatorConnect;
extern	func_t		SpectatorThink;
extern	func_t		SpectatorDisconnect;
#endif	/* H2W */

void PR_RunError (const char *error, ...) __attribute__((__format__(__printf__,1,2), __noreturn__));

void ED_PrintEdicts (void);
void ED_PrintNum (int ent);

eval_t *GetEdictFieldValue(edict_t *ed, const char *field);

extern	cvar_t		max_temp_edicts;

extern	qboolean	ignore_precache;

#endif	/* __HX2_PROGS_H */

