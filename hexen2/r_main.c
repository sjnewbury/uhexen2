/*
	r_main.c

	$Header: /home/ozzie/Download/0000/uhexen2/hexen2/r_main.c,v 1.34 2009-10-19 09:32:53 sezero Exp $
*/

#include "quakedef.h"
#include "r_local.h"
#include "d_local.h"

//#define	PASSAGES

static vec3_t	viewlightvec;
static alight_t	r_viewlighting = {128, 192, viewlightvec};

#if id386
void		*colormap;
#endif

float		r_time1;
float		r_lasttime1 = 0;

int		r_numallocatededges;

qboolean	r_drawpolys;
qboolean	r_drawculledpolys;
qboolean	r_worldpolysbacktofront;
qboolean	r_recursiveaffinetriangles = true;

int		r_pixbytes = 1;
float		r_aliasuvscale = 1.0;
int		r_outofsurfaces;
int		r_outofedges;

byte		*mainTransTable;
byte		*transTable;	/* the particle table */
byte		*playerTranslation;
const int	color_offsets[MAX_PLAYER_CLASS] =
{
		2 * 14 * 256,
		0,
		1 * 14 * 256,
		2 * 14 * 256,
		2 * 14 * 256
#if defined(H2W)
		,
		2 * 14 * 256
#endif
};

qboolean	r_dowarp, r_dowarpold, r_viewchanged;

int		numbtofpolys;
btofpoly_t	*pbtofpolys;
mvertex_t	*r_pcurrentvertbase;

int		c_surf;
int		r_maxsurfsseen, r_maxedgesseen, r_cnumsurfs;
qboolean	r_surfsonstack;
int		r_clipflags;

byte		*r_warpbuffer;

static byte	*r_stack_start;

qboolean	r_fov_greater_than_90;

entity_t	r_worldentity;

//
// view origin
//
vec3_t		vup, base_vup;
vec3_t		vpn, base_vpn;
vec3_t		vright, base_vright;
vec3_t		r_origin;

//
// screen size info
//
refdef_t	r_refdef;
float		xcenter, ycenter;
float		xscale, yscale;
float		xscaleinv, yscaleinv;
float		xscaleshrink, yscaleshrink;
float		aliasxscale, aliasyscale, aliasxcenter, aliasycenter;

int		screenwidth;

float		pixelAspect;
float		screenAspect;
float		verticalFieldOfView;
float		xOrigin, yOrigin;

mplane_t	screenedge[4];

//
// refresh flags
//
int		r_framecount = 1;	// so frame counts initialized to 0 don't match
int		r_visframecount;
int		d_spanpixcount;
int		r_polycount;
int		r_drawnpolycount;
int		r_wholepolycount;

int		*pfrustum_indexes[4];
int		r_frustum_indexes[4*6];

mleaf_t		*r_viewleaf, *r_oldviewleaf;

texture_t	*r_notexture_mip;

float		r_aliastransition, r_resfudge;

int		d_lightstylevalue[256];	// 8.8 fraction of base light value

float		dp_time1, dp_time2, db_time1, db_time2, rw_time1, rw_time2;
float		se_time1, se_time2, de_time1, de_time2, dv_time1, dv_time2;

static edge_t	ledges[NUMSTACKEDGES + ((CACHE_SIZE - 1) / sizeof(edge_t)) + 1];
static edge_t	*SaveEdges;
static surf_t	lsurfs[NUMSTACKSURFACES + ((CACHE_SIZE - 1) / sizeof(surf_t)) + 1];
static surf_t	*SaveSurfaces;
static int	SaveEdgesCount, SaveSurfacesCount, SaveEdgesSize, SaveSurfacesSize;
static qboolean	AllowTranslucency;

extern	cvar_t		scr_fov;

cvar_t	r_draworder = {"r_draworder", "0", CVAR_NONE};
cvar_t	r_speeds = {"r_speeds", "0", CVAR_NONE};
cvar_t	r_timegraph = {"r_timegraph", "0", CVAR_NONE};
cvar_t	r_graphheight = {"r_graphheight", "10", CVAR_NONE};
cvar_t	r_clearcolor = {"r_clearcolor", "0", CVAR_NONE};
cvar_t	r_waterwarp = {"r_waterwarp", "1", CVAR_ARCHIVE};
cvar_t	r_fullbright = {"r_fullbright", "0", CVAR_NONE};
cvar_t	r_drawentities = {"r_drawentities", "1", CVAR_NONE};
cvar_t	r_drawviewmodel = {"r_drawviewmodel", "1", CVAR_NONE};
cvar_t	r_aliasstats = {"r_polymodelstats", "0", CVAR_NONE};
cvar_t	r_dspeeds = {"r_dspeeds", "0", CVAR_NONE};
cvar_t	r_drawflat = {"r_drawflat", "0", CVAR_NONE};
cvar_t	r_ambient = {"r_ambient", "0", CVAR_NONE};
cvar_t	r_reportsurfout = {"r_reportsurfout", "0", CVAR_NONE};
cvar_t	r_maxsurfs = {"r_maxsurfs", "0", CVAR_NONE};
cvar_t	r_numsurfs = {"r_numsurfs", "0", CVAR_NONE};
cvar_t	r_reportedgeout = {"r_reportedgeout", "0", CVAR_NONE};
cvar_t	r_maxedges = {"r_maxedges", "0", CVAR_NONE};
cvar_t	r_numedges = {"r_numedges", "0", CVAR_NONE};
cvar_t	r_aliastransbase = {"r_aliastransbase", "200", CVAR_NONE};
cvar_t	r_aliastransadj = {"r_aliastransadj", "100", CVAR_NONE};
cvar_t	r_aliasmip = {"r_aliasmip", "80", CVAR_NONE};
cvar_t	r_wholeframe = {"r_wholeframe", "1", CVAR_ARCHIVE};
cvar_t	r_transwater = {"r_transwater", "1", CVAR_ARCHIVE};
cvar_t	r_texture_external = {"r_texture_external", "0", CVAR_ARCHIVE};

//void CreatePassages (void);
//void SetVisibilityByPassages (void);

//=============================================================================


/*
==================
R_InitTextures
==================
*/
void	R_InitTextures (void)
{
	int		x, y, m;
	byte	*dest;

// create a simple checkerboard texture for the default
	r_notexture_mip = (texture_t *) Hunk_AllocName (sizeof(texture_t) + 16*16+8*8+4*4+2*2, "notexture");

	r_notexture_mip->width = r_notexture_mip->height = 16;
	r_notexture_mip->offsets[0] = sizeof(texture_t);
	r_notexture_mip->offsets[1] = r_notexture_mip->offsets[0] + 16*16;
	r_notexture_mip->offsets[2] = r_notexture_mip->offsets[1] + 8*8;
	r_notexture_mip->offsets[3] = r_notexture_mip->offsets[2] + 4*4;

	for (m = 0; m < 4; m++)
	{
		dest = (byte *)r_notexture_mip + r_notexture_mip->offsets[m];
		for (y = 0; y < (16>>m); y++)
		{
			for (x = 0; x < (16>>m); x++)
			{
				if ( (y < (8>>m)) ^ (x < (8>>m)) )
					*dest++ = 0;
				else
					*dest++ = 0xff;
			}
		}
	}
}


/*
================
R_InitTurb
================
*/
static void R_InitTurb (void)
{
	int		i;

	for (i = 0 ; i < (SIN_BUFFER_SIZE) ; i++)
	{
		sintable[i]	= AMP  + sin(i * 3.14159 * 2 / CYCLE) * AMP;
		intsintable[i]	= AMP2 + sin(i * 3.14159 * 2 / CYCLE) * AMP2;
							// AMP2, not 20
	}
}

/*
===============
R_Init
===============
*/
void R_Init (void)
{
	int		dummy;

// get stack position so we can guess if we are going to overflow
	r_stack_start = (byte *)&dummy;

	R_InitTurb ();

	Cmd_AddCommand ("timerefresh", R_TimeRefresh_f);
	Cmd_AddCommand ("pointfile", R_ReadPointFile_f);

	Cvar_RegisterVariable (&r_draworder);
	Cvar_RegisterVariable (&r_speeds);
	Cvar_RegisterVariable (&r_timegraph);
	Cvar_RegisterVariable (&r_graphheight);
	Cvar_RegisterVariable (&r_drawflat);
	Cvar_RegisterVariable (&r_ambient);
	Cvar_RegisterVariable (&r_clearcolor);
	Cvar_RegisterVariable (&r_waterwarp);
	Cvar_RegisterVariable (&r_fullbright);
	Cvar_RegisterVariable (&r_drawentities);
	Cvar_RegisterVariable (&r_drawviewmodel);
	Cvar_RegisterVariable (&r_aliasstats);
	Cvar_RegisterVariable (&r_dspeeds);
	Cvar_RegisterVariable (&r_reportsurfout);
	Cvar_RegisterVariable (&r_maxsurfs);
	Cvar_RegisterVariable (&r_numsurfs);
	Cvar_RegisterVariable (&r_reportedgeout);
	Cvar_RegisterVariable (&r_maxedges);
	Cvar_RegisterVariable (&r_numedges);
	Cvar_RegisterVariable (&r_aliastransbase);
	Cvar_RegisterVariable (&r_aliastransadj);
	Cvar_RegisterVariable (&r_aliasmip);
	Cvar_RegisterVariable (&r_wholeframe);
	Cvar_RegisterVariable (&r_transwater);
	Cvar_RegisterVariable (&r_texture_external);

	Cvar_SetValue ("r_maxedges", (float)NUMSTACKEDGES);
	Cvar_SetValue ("r_maxsurfs", (float)NUMSTACKSURFACES);

	view_clipplanes[0].leftedge = true;
	view_clipplanes[1].rightedge = true;
	view_clipplanes[1].leftedge = view_clipplanes[2].leftedge = view_clipplanes[3].leftedge = false;
	view_clipplanes[0].rightedge = view_clipplanes[2].rightedge = view_clipplanes[3].rightedge = false;

	r_refdef.xOrigin = XCENTERING;
	r_refdef.yOrigin = YCENTERING;

	transTable = (byte *)FS_LoadHunkFile ("gfx/tinttab.lmp");
	if (!transTable)
		Sys_Error ("Couldn't load gfx/tinttab.lmp");
	if (fs_filesize != 65536)
		Sys_Error ("Unexpected file size (%lu) for %s\n", (unsigned long)fs_filesize, "gfx/tinttab.lmp");

	mainTransTable = (byte *)FS_LoadHunkFile ("gfx/tinttab2.lmp");
	if (!mainTransTable)
		Sys_Error ("Couldn't load gfx/tinttab2.lmp");
	if (fs_filesize != 65536)
		Sys_Error ("Unexpected file size (%lu) for %s\n", (unsigned long)fs_filesize, "gfx/tinttab2.lmp");

	playerTranslation = (byte *)FS_LoadHunkFile ("gfx/player.lmp");
	if (!playerTranslation)
		Sys_Error ("Couldn't load gfx/player.lmp");

	R_InitParticles ();

	D_Init ();

#if id386
	Sys_MakeCodeWriteable ((long)R_EdgeCodeStart, (long)R_EdgeCodeEnd - (long)R_EdgeCodeStart);
	Sys_MakeCodeWriteable ((long)R_EdgeCodeStartT, (long)R_EdgeCodeEndT - (long)R_EdgeCodeStartT);

	D_Patch();
	R_TranPatch1();
	R_TranPatch2();
	R_TranPatch6();

	Sys_MakeCodeWriteable ((int)D_Draw16StartT, (int)D_Draw16EndT - (int)D_Draw16StartT);
	R_TranPatch3();

	Sys_MakeCodeWriteable ((int)D_SpriteSpansStartT, (int)D_SpriteSpansEndT - (int)D_SpriteSpansStartT);
	R_TranPatch4();

	Sys_MakeCodeWriteable ((int)D_SpriteSpansStartT2, (int)D_SpriteSpansEndT2 - (int)D_SpriteSpansStartT2);
	R_TranPatch5();

	Sys_MakeCodeWriteable ((int)D_DrawTurbulent8TSpan, (int)D_DrawTurbulent8TSpanEnd - (int)D_DrawTurbulent8TSpan);
	R_TranPatch7();
#endif
}


/*
===============
R_NewMap
===============
*/
void R_NewMap (void)
{
	int		i;

	memset (&r_worldentity, 0, sizeof(r_worldentity));
	r_worldentity.model = cl.worldmodel;

// clear out efrags in case the level hasn't been reloaded
// FIXME: is this one short?
	for (i = 0; i < cl.worldmodel->numleafs; i++)
		cl.worldmodel->leafs[i].efrags = NULL;

	r_viewleaf = NULL;
	R_ClearParticles ();

	r_cnumsurfs = r_maxsurfs.integer;

	if (r_cnumsurfs <= MINSURFACES)
		r_cnumsurfs = MINSURFACES;

	if (r_cnumsurfs > NUMSTACKSURFACES)
	{
		surfaces = (surf_t *) Hunk_AllocName (r_cnumsurfs * sizeof(surf_t), "surfaces");
		surface_p = surfaces;
		surf_max = &surfaces[r_cnumsurfs];
		r_surfsonstack = false;
	// surface 0 doesn't really exist; it's just a dummy because index 0
	// is used to indicate no edge attached to surface
		surfaces--;
#if id386
		R_SurfacePatch ();
#endif
	}
	else
	{
		r_surfsonstack = true;
	}

	SaveSurfacesSize = r_cnumsurfs * sizeof(surf_t);
	SaveSurfaces = (surf_t *) Hunk_AllocName (SaveSurfacesSize, "surfback");

	r_maxedgesseen = 0;
	r_maxsurfsseen = 0;

	r_numallocatededges = r_maxedges.integer;

	if (r_numallocatededges < MINEDGES)
		r_numallocatededges = MINEDGES;

	if (r_numallocatededges <= NUMSTACKEDGES)
	{
		auxedges = NULL;
	}
	else
	{
		auxedges = (edge_t *) Hunk_AllocName (r_numallocatededges * sizeof(edge_t), "edges");
	}

	SaveEdgesSize = r_numallocatededges * sizeof(edge_t);
	SaveEdges = (edge_t *) Hunk_AllocName (SaveEdgesSize, "edgeback");

	r_dowarpold = false;
	r_viewchanged = false;

#ifdef PASSAGES
	CreatePassages ();
#endif
}


/*
===============
R_SetVrect
===============
*/
void R_SetVrect (vrect_t *pvrectin, vrect_t *pvrect, int lineadj)
{
	float		size;
	int		h;

	size = scr_viewsize.integer > 100 ? 100.0 : scr_viewsize.integer;
	if (cl.intermission)
	{
		size = 100.0;	// intermission is always full screen
		lineadj = 0;
	}
	size /= 100.0;

	h = pvrectin->height - lineadj;
	pvrect->width = pvrectin->width * size;
	if (pvrect->width < 96)
	{
		size = 96.0 / pvrectin->width;
		pvrect->width = 96;	// min for icons
	}
	pvrect->width &= ~7;
	pvrect->height = pvrectin->height * size;
	if (pvrect->height > pvrectin->height - lineadj)
		pvrect->height = pvrectin->height - lineadj;

	pvrect->height &= ~1;

	pvrect->x = (pvrectin->width - pvrect->width)/2;
	pvrect->y = (h - pvrect->height)/2;
}


/*
===============
R_ViewChanged

Called every time the vid structure or r_refdef changes.
Guaranteed to be called before the first refresh
===============
*/
void R_ViewChanged (vrect_t *pvrect, int lineadj, float aspect)
{
	int		i;
	float	res_scale;

	r_viewchanged = true;

	R_SetVrect (pvrect, &r_refdef.vrect, lineadj);

	r_refdef.horizontalFieldOfView = 2.0 * tan (scr_fov.value/360*M_PI);
	r_refdef.fvrectx = (float)r_refdef.vrect.x;
	r_refdef.fvrectx_adj = (float)r_refdef.vrect.x - 0.5;
	r_refdef.vrect_x_adj_shift20 = (r_refdef.vrect.x<<20) + (1<<19) - 1;
	r_refdef.fvrecty = (float)r_refdef.vrect.y;
	r_refdef.fvrecty_adj = (float)r_refdef.vrect.y - 0.5;
	r_refdef.vrectright = r_refdef.vrect.x + r_refdef.vrect.width;
	r_refdef.vrectright_adj_shift20 = (r_refdef.vrectright<<20) + (1<<19) - 1;
	r_refdef.fvrectright = (float)r_refdef.vrectright;
	r_refdef.fvrectright_adj = (float)r_refdef.vrectright - 0.5;
	r_refdef.vrectrightedge = (float)r_refdef.vrectright - 0.99;
	r_refdef.vrectbottom = r_refdef.vrect.y + r_refdef.vrect.height;
	r_refdef.fvrectbottom = (float)r_refdef.vrectbottom;
	r_refdef.fvrectbottom_adj = (float)r_refdef.vrectbottom - 0.5;

	r_refdef.aliasvrect.x = (int)(r_refdef.vrect.x * r_aliasuvscale);
	r_refdef.aliasvrect.y = (int)(r_refdef.vrect.y * r_aliasuvscale);
	r_refdef.aliasvrect.width = (int)(r_refdef.vrect.width * r_aliasuvscale);
	r_refdef.aliasvrect.height = (int)(r_refdef.vrect.height * r_aliasuvscale);
	r_refdef.aliasvrectright = r_refdef.aliasvrect.x + r_refdef.aliasvrect.width;
	r_refdef.aliasvrectbottom = r_refdef.aliasvrect.y + r_refdef.aliasvrect.height;

	pixelAspect = aspect;
	xOrigin = r_refdef.xOrigin;
	yOrigin = r_refdef.yOrigin;

	screenAspect = r_refdef.vrect.width*pixelAspect / r_refdef.vrect.height;

	// 320*200 1.0 pixelAspect = 1.6 screenAspect
	// 320*240 1.0 pixelAspect = 1.3333 screenAspect
	// proper 320*200 pixelAspect = 0.8333333

	verticalFieldOfView = r_refdef.horizontalFieldOfView / screenAspect;

// values for perspective projection
// if math were exact, the values would range from 0.5 to to range+0.5
// hopefully they wll be in the 0.000001 to range+.999999 and truncate
// the polygon rasterization will never render in the first row or column
// but will definately render in the [range] row and column, so adjust the
// buffer origin to get an exact edge to edge fill
	xcenter = ((float)r_refdef.vrect.width * XCENTERING) + r_refdef.vrect.x - 0.5;
	aliasxcenter = xcenter * r_aliasuvscale;
	ycenter = ((float)r_refdef.vrect.height * YCENTERING) + r_refdef.vrect.y - 0.5;
	aliasycenter = ycenter * r_aliasuvscale;

	xscale = r_refdef.vrect.width / r_refdef.horizontalFieldOfView;
	aliasxscale = xscale * r_aliasuvscale;
	xscaleinv = 1.0 / xscale;
	yscale = xscale * pixelAspect;
	aliasyscale = yscale * r_aliasuvscale;
	yscaleinv = 1.0 / yscale;
	xscaleshrink = (r_refdef.vrect.width-6)/r_refdef.horizontalFieldOfView;
	yscaleshrink = xscaleshrink*pixelAspect;

// left side clip
	screenedge[0].normal[0] = -1.0 / (xOrigin*r_refdef.horizontalFieldOfView);
	screenedge[0].normal[1] = 0;
	screenedge[0].normal[2] = 1;
	screenedge[0].type = PLANE_ANYZ;

// right side clip
	screenedge[1].normal[0] = 1.0 / ((1.0-xOrigin)*r_refdef.horizontalFieldOfView);
	screenedge[1].normal[1] = 0;
	screenedge[1].normal[2] = 1;
	screenedge[1].type = PLANE_ANYZ;

// top side clip
	screenedge[2].normal[0] = 0;
	screenedge[2].normal[1] = -1.0 / (yOrigin*verticalFieldOfView);
	screenedge[2].normal[2] = 1;
	screenedge[2].type = PLANE_ANYZ;

// bottom side clip
	screenedge[3].normal[0] = 0;
	screenedge[3].normal[1] = 1.0 / ((1.0-yOrigin)*verticalFieldOfView);
	screenedge[3].normal[2] = 1;
	screenedge[3].type = PLANE_ANYZ;

	for (i = 0; i < 4; i++)
		VectorNormalize (screenedge[i].normal);

	res_scale = sqrt((double)(r_refdef.vrect.width * r_refdef.vrect.height) / (320.0 * 152.0))
				* (2.0 / r_refdef.horizontalFieldOfView);

	r_aliastransition = r_aliastransbase.value * res_scale;
	r_resfudge = r_aliastransadj.value * res_scale;

	if (scr_fov.integer <= 90.0)
		r_fov_greater_than_90 = false;
	else
		r_fov_greater_than_90 = true;

// TODO: collect 386-specific code in one place
#if	id386
	if (r_pixbytes == 1)
	{
		Sys_MakeCodeWriteable ((long)R_Surf8Start, (long)R_Surf8End - (long)R_Surf8Start);
		colormap = vid.colormap;
		R_Surf8Patch ();
	}
	else
	{
		Sys_MakeCodeWriteable ((long)R_Surf16Start, (long)R_Surf16End - (long)R_Surf16Start);
		colormap = vid.colormap16;
		R_Surf16Patch ();
	}
#endif	/* id386 */

	D_ViewChanged ();
}


/*
===============
R_MarkLeaves
===============
*/
static void R_MarkLeaves (void)
{
	byte	*vis;
	mnode_t	*node;
	int		i;

	if (r_oldviewleaf == r_viewleaf)
		return;

	r_visframecount++;
	r_oldviewleaf = r_viewleaf;

	vis = Mod_LeafPVS (r_viewleaf, cl.worldmodel);

	for (i = 0; i < cl.worldmodel->numleafs; i++)
	{
		if (vis[i>>3] & (1<<(i&7)))
		{
			node = (mnode_t *)&cl.worldmodel->leafs[i+1];
			do
			{
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}


static void R_PrepareAlias (void)
{
	int			j;
	int			lnum;
	alight_t	lighting;
// FIXME: remove and do real lighting
	float		lightvec[3] = {-1, 0, 0};
	vec3_t		dist;
	float		add;
	vec3_t		adjust_origin;

	VectorCopy (currententity->origin, r_entorigin);
	VectorSubtract (r_origin, r_entorigin, modelorg);

	// see if the bounding box lets us trivially reject, also sets
	// trivial accept status
	if (R_AliasCheckBBox ())
	{
		VectorCopy(currententity->origin, adjust_origin);
		adjust_origin[2] += (currententity->model->mins[2] + currententity->model->maxs[2]) / 2;
		j = R_LightPoint (adjust_origin);

		lighting.ambientlight = j;
		lighting.shadelight = j;

		lighting.plightvec = lightvec;

		for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
		{
			if (cl_dlights[lnum].die >= cl.time)
			{
				VectorSubtract (currententity->origin, cl_dlights[lnum].origin, dist);

				if (cl_dlights[lnum].radius> 0)
				{
					add = cl_dlights[lnum].radius - VectorLength(dist);

					if (add > 0)
						lighting.ambientlight += add;
				}
				else
				{
					add = VectorLength(dist) + cl_dlights[lnum].radius;

					if (add < 0)
						lighting.ambientlight += add;
				}
			}
		}

	// clamp lighting so it doesn't overbright as much
		if (lighting.ambientlight > 128)
			lighting.ambientlight = 128;
		if (lighting.ambientlight < 0)
			lighting.ambientlight = 0;
		if (lighting.ambientlight + lighting.shadelight > 192)
			lighting.shadelight = 192 - lighting.ambientlight;

		R_AliasDrawModel (&lighting);
	}
}


/*
=============
R_DrawEntitiesOnList
=============
*/
static void R_DrawEntitiesOnList (void)
{
	int			i;

	if (!r_drawentities.integer)
		return;

	for (i = 0; i < cl_numvisedicts; i++)
	{
		currententity = cl_visedicts[i];

		if (currententity == &cl_entities[cl.viewentity])
		{
			if (chase_active.integer == 0)
				continue;	// don't draw the player
			else
			// chase-cam pitch adj. by FrikaC
				currententity->angles[0] *= 0.3;
		}

		switch (currententity->model->type)
		{
		case mod_sprite:
			if ((currententity->model->flags & EF_TRANSPARENT) ||
				(currententity->drawflags & DRF_TRANSLUCENT))
			{
				break;
			}
			VectorCopy (currententity->origin, r_entorigin);
			VectorSubtract (r_origin, r_entorigin, modelorg);
			R_DrawSprite ();
			break;

		case mod_alias:
			if ((currententity->model->flags & (EF_TRANSPARENT|EF_HOLEY|EF_SPECIAL_TRANS)) ||
				(currententity->drawflags & DRF_TRANSLUCENT))
			{
				break;
			}
			R_PrepareAlias ();
			break;

		default:
			break;
		}
	}

	// Draw the transparent stuff
	for (i = 0; i < cl_numvisedicts; i++)
	{
		currententity = cl_visedicts[i];

		if (currententity == &cl_entities[cl.viewentity] && chase_active.integer == 0)
			continue;	// don't draw the player

		switch (currententity->model->type)
		{
		case mod_sprite:
			if ((currententity->model->flags & EF_TRANSPARENT) ||
				(currententity->drawflags & DRF_TRANSLUCENT))
			{
				VectorCopy (currententity->origin, r_entorigin);
				VectorSubtract (r_origin, r_entorigin, modelorg);
				R_DrawSprite ();
			}
			break;

		case mod_alias:
			if ((currententity->model->flags & (EF_TRANSPARENT|EF_HOLEY|EF_SPECIAL_TRANS)) ||
				(currententity->drawflags & DRF_TRANSLUCENT))
			{
				R_PrepareAlias ();
			}
			break;

		default:
			break;
		}
	}
}


/*
=============
R_DrawViewModel
=============
*/
static void R_DrawViewModel (void)
{
// FIXME: remove and do real lighting
	float		lightvec[3] = {-1, 0, 0};
	int			j;
	int			lnum;
	vec3_t		dist;
	float		add;
	dlight_t	*dl;

	currententity = &cl.viewent;

	if (!currententity->model)
		return;

	VectorCopy (currententity->origin, r_entorigin);
	VectorSubtract (r_origin, r_entorigin, modelorg);

	VectorCopy (vup, viewlightvec);
	VectorInverse (viewlightvec);

	j = R_LightPoint (currententity->origin);

	if (j < 24)
		j = 24;		// always give some light on gun

	r_viewlighting.ambientlight = j;
	r_viewlighting.shadelight = j;

// add dynamic lights
	for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
	{
		dl = &cl_dlights[lnum];
		if (!dl->radius)
			continue;
		if (dl->die < cl.time)
			continue;

		VectorSubtract (currententity->origin, dl->origin, dist);

		if (dl->radius > 0)
		{
			add = dl->radius - VectorLength(dist);

			if (add > 0)
				r_viewlighting.ambientlight += add;
		}
		else
		{
			add = VectorLength(dist) + dl->radius;

			if (add < 0)
				r_viewlighting.ambientlight += add;
		}
	}

// clamp lighting so it doesn't overbright as much
	if (r_viewlighting.ambientlight > 128)
		r_viewlighting.ambientlight = 128;
	if (r_viewlighting.ambientlight < 0)
		r_viewlighting.ambientlight = 0;
	if (r_viewlighting.ambientlight + r_viewlighting.shadelight > 192)
		r_viewlighting.shadelight = 192 - r_viewlighting.ambientlight;

	r_viewlighting.plightvec = lightvec;

	cl.light_level = r_viewlighting.ambientlight;

	if ((cl.v.health <= 0)		 ||
	    (chase_active.integer)	 ||
	    (cl.items & IT_INVISIBILITY) ||
	    (!r_drawviewmodel.integer)	 ||
	    (r_fov_greater_than_90))
	{
		return;
	}

	R_AliasDrawModel (&r_viewlighting);
}


/*
=============
R_BmodelCheckBBox
=============
*/
static int R_BmodelCheckBBox (qmodel_t *clmodel, float *minmaxs)
{
	int		i, *pindex, clipflags;
	vec3_t		acceptpt, rejectpt;
	double		d;

	clipflags = 0;

	if (currententity->angles[0] || currententity->angles[1]
		|| currententity->angles[2])
	{
		for (i = 0; i < 4; i++)
		{
			d = DotProduct (currententity->origin, view_clipplanes[i].normal);
			d -= view_clipplanes[i].dist;

			if (d <= -clmodel->radius)
				return BMODEL_FULLY_CLIPPED;

			if (d <= clmodel->radius)
				clipflags |= (1<<i);
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
		// generate accept and reject points
		// FIXME: do with fast look-ups or integer tests based on the sign bit
		// of the floating point values

			pindex = pfrustum_indexes[i];

			rejectpt[0] = minmaxs[pindex[0]];
			rejectpt[1] = minmaxs[pindex[1]];
			rejectpt[2] = minmaxs[pindex[2]];

			d = DotProduct (rejectpt, view_clipplanes[i].normal);
			d -= view_clipplanes[i].dist;

			if (d <= 0)
				return BMODEL_FULLY_CLIPPED;

			acceptpt[0] = minmaxs[pindex[3+0]];
			acceptpt[1] = minmaxs[pindex[3+1]];
			acceptpt[2] = minmaxs[pindex[3+2]];

			d = DotProduct (acceptpt, view_clipplanes[i].normal);
			d -= view_clipplanes[i].dist;

			if (d <= 0)
				clipflags |= (1<<i);
		}
	}

	return clipflags;
}


/*
=============
RotatedBBox

Returns an axially aligned box that contains
the input box at the given rotation
=============
*/
static void RotatedBBox (vec3_t mins, vec3_t maxs, vec3_t angles, vec3_t tmins, vec3_t tmaxs)
{
	vec3_t	tmp, v;
	int		i, j;
	vec3_t	forward, right, up;

	if (!angles[0] && !angles[1] && !angles[2])
	{
		VectorCopy (mins, tmins);
		VectorCopy (maxs, tmaxs);
		return;
	}

	for (i = 0; i < 3; i++)
	{
		tmins[i] = 99999;
		tmaxs[i] = -99999;
	}

	AngleVectors (angles, forward, right, up);

	for (i = 0; i < 8; i++)
	{
		if ( i & 1 )
			tmp[0] = mins[0];
		else
			tmp[0] = maxs[0];

		if ( i & 2 )
			tmp[1] = mins[1];
		else
			tmp[1] = maxs[1];

		if ( i & 4 )
			tmp[2] = mins[2];
		else
			tmp[2] = maxs[2];

		VectorScale (forward, tmp[0], v);
		VectorMA (v, -tmp[1], right, v);
		VectorMA (v, tmp[2], up, v);

		for (j = 0; j < 3; j++)
		{
			if (v[j] < tmins[j])
				tmins[j] = v[j];
			if (v[j] > tmaxs[j])
				tmaxs[j] = v[j];
		}
	}
}


/*
=============
R_DrawBEntitiesOnList
=============
*/
static void R_DrawBEntitiesOnList (void)
{
	int		i, j, k, clipflags;
	vec3_t		oldorigin;
	qmodel_t	*clmodel;
	float		minmaxs[6];
	vec3_t		mins, maxs;

	if (!r_drawentities.integer)
		return;

	VectorCopy (modelorg, oldorigin);
	insubmodel = true;
	r_dlightframecount = r_framecount;

	for (i = 0; i < cl_numvisedicts; i++)
	{
		currententity = cl_visedicts[i];

		switch (currententity->model->type)
		{
		case mod_brush:
			clmodel = currententity->model;

			// see if the bounding box lets us trivially reject,
			// also sets trivial accept status
			RotatedBBox (clmodel->mins, clmodel->maxs,
					currententity->angles, mins, maxs);
			VectorAdd (mins, currententity->origin, minmaxs);
			VectorAdd (maxs, currententity->origin, (minmaxs+3));

			clipflags = R_BmodelCheckBBox (clmodel, minmaxs);
			if (clipflags != BMODEL_FULLY_CLIPPED)
			{
				VectorCopy (currententity->origin, r_entorigin);
				VectorSubtract (r_origin, r_entorigin, modelorg);

				r_pcurrentvertbase = clmodel->vertexes;

				// FIXME: stop transforming twice
				R_RotateBmodel ();

				// calculate dynamic lighting for bmodel
				// if it's not an instanced model
				if (clmodel->firstmodelsurface != 0)
				{
					for (k = 0; k < MAX_DLIGHTS; k++)
					{
						if ((cl_dlights[k].die < cl.time) || (!cl_dlights[k].radius))
						{
							continue;
						}

						R_MarkLights (&cl_dlights[k], 1<<k, clmodel->nodes + clmodel->hulls[0].firstclipnode);
					}
				}

				// if the driver wants polygons, deliver those. Z-buffering is on
				// at this point, so no clipping to the world tree is needed, just
				// frustum clipping
				if (r_drawpolys | r_drawculledpolys)
				{
					R_ZDrawSubmodelPolys (clmodel);
				}
				else
				{
					r_pefragtopnode = NULL;

					for (j = 0; j < 3; j++)
					{
						r_emins[j] = minmaxs[j];
						r_emaxs[j] = minmaxs[3+j];
					}

					R_SplitEntityOnNode2 (cl.worldmodel->nodes);
					if (r_pefragtopnode)
					{
						currententity->topnode = r_pefragtopnode;

						if (r_pefragtopnode->contents >= 0)
						{
							// not a leaf; has to be clipped to the world BSP
							r_clipflags = clipflags;
							R_DrawSolidClippedSubmodelPolygons (clmodel);
						}
						else
						{
							// falls entirely in one leaf, so we just put all the
							// edges in the edge list and let 1/z sorting handle
							// drawing order

							R_DrawSubmodelPolygons (clmodel, clipflags);
						}
						currententity->topnode = NULL;
					}
				}

				// put back world rotation and frustum clipping
				// FIXME: R_RotateBmodel should just work off base_vxx
				VectorCopy (base_vpn, vpn);
				VectorCopy (base_vup, vup);
				VectorCopy (base_vright, vright);
				VectorCopy (base_modelorg, modelorg);
				VectorCopy (oldorigin, modelorg);
				R_TransformFrustum ();
			}
			break;

		default:
			break;
		}
	}

	insubmodel = false;
}


/*
================
R_EdgeDrawing
================
*/
static void R_EdgeDrawing (qboolean Translucent)
{
	int	EdgesSize, SurfacesSize;

	if (!Translucent)
	{
		if (auxedges)
		{
			r_edges = auxedges;
		}
		else
		{
			r_edges =  (edge_t *) (((intptr_t)&ledges[0] + CACHE_SIZE - 1) & ~(CACHE_SIZE - 1));
		}

		if (r_surfsonstack)
		{
			surfaces = (surf_t *) (((intptr_t)&lsurfs[0] + CACHE_SIZE - 1) & ~(CACHE_SIZE - 1));
			surf_max = &surfaces[r_cnumsurfs];
		// surface 0 doesn't really exist; it's just a dummy because
		// index 0 is used to indicate no edge attached to surface
			surfaces--;
#if id386
			R_SurfacePatch ();
#endif
		}

		R_BeginEdgeFrame ();

		if (r_dspeeds.integer)
		{
			rw_time1 = Sys_DoubleTime ();
		}

		R_RenderWorld ();
	}
	else
	{
		memcpy(r_edges,SaveEdges,SaveEdgesCount * sizeof(edge_t));
		memcpy(surfaces,SaveSurfaces,SaveSurfacesCount * sizeof(surf_t));
	}

	if (r_drawculledpolys)
		R_ScanEdges (Translucent);

// only the world can be drawn back to front with no z reads or compares, just
// z writes, so have the driver turn z compares on now
	if (!Translucent)
	{
		D_TurnZOn ();

		if (r_dspeeds.integer)
		{
			rw_time2 = Sys_DoubleTime ();
			db_time1 = rw_time2;
		}

		R_DrawBEntitiesOnList ();

		SaveSurfacesCount = surface_p - surfaces;
		SurfacesSize = SaveSurfacesCount * sizeof(surf_t);
		SaveEdgesCount = edge_p - r_edges;
		EdgesSize = SaveEdgesCount * sizeof(edge_t);

		if (SurfacesSize > SaveSurfacesSize || EdgesSize > SaveEdgesSize)
		{
				Con_Printf("WARNING: Translucency disabled:  %d/%d %d/%d\n",SurfacesSize,SaveSurfacesSize,EdgesSize,SaveEdgesSize);
			AllowTranslucency = false;
		}
		else
		{
			AllowTranslucency = true;
			memcpy(SaveEdges,r_edges,EdgesSize);
			memcpy(SaveSurfaces,surfaces,SurfacesSize);
		}

		if (r_dspeeds.integer)
		{
			db_time2 = Sys_DoubleTime ();
			se_time1 = db_time2;
		}

		if (!r_dspeeds.integer)
		{
			VID_UnlockBuffer ();
			S_ExtraUpdate ();	// don't let sound get messed up if going slow
			VID_LockBuffer ();
		}
	}

	if (!(r_drawpolys | r_drawculledpolys))
		R_ScanEdges (Translucent);
}


/*
================
R_RenderView

r_refdef must be set before the first call
================
*/
static void R_RenderView_ (void)
{
	byte	warpbuffer[WARP_WIDTH * WARP_HEIGHT];

	r_warpbuffer = warpbuffer;

	if (r_timegraph.integer || r_speeds.integer || r_dspeeds.integer)
	{
		if (r_wholeframe.integer)
			r_time1 = r_lasttime1;
		else
			r_time1 = Sys_DoubleTime ();
	}

	R_SetupFrame ();

#ifdef PASSAGES
	SetVisibilityByPassages ();
#else
	R_MarkLeaves ();	// done here so we know if we're in water
#endif

// make FDIV fast. This reduces timing precision after we've been running for a
// while, so we don't do it globally.  This also sets chop mode, and we do it
// here so that setup stuff like the refresh area calculations match what's
// done in screen.c
	Sys_LowFPPrecision ();

	if (!r_worldentity.model || !cl.worldmodel)
		Sys_Error ("%s: NULL worldmodel", __thisfunc__);

	if (!r_dspeeds.integer)
	{
		VID_UnlockBuffer ();
		S_ExtraUpdate ();	// don't let sound get messed up if going slow
		VID_LockBuffer ();
	}

	R_EdgeDrawing (false);

	if (!r_dspeeds.integer)
	{
		VID_UnlockBuffer ();
		S_ExtraUpdate ();	// don't let sound get messed up if going slow
		VID_LockBuffer ();
	}

	if (r_dspeeds.integer)
	{
		se_time2 = Sys_DoubleTime ();
		de_time1 = se_time2;
	}

	R_DrawEntitiesOnList ();
	if (TransCount && AllowTranslucency)
		R_EdgeDrawing (true);

	if (r_dspeeds.integer)
	{
		de_time2 = Sys_DoubleTime ();
		dv_time1 = de_time2;
	}

	R_DrawViewModel ();

	if (r_dspeeds.integer)
	{
		dv_time2 = Sys_DoubleTime ();
		dp_time1 = Sys_DoubleTime ();
	}

	R_DrawParticles ();

	if (r_dspeeds.integer)
		dp_time2 = Sys_DoubleTime ();

	if (r_dowarp)
		D_WarpScreen ();

	V_SetContentsColor (r_viewleaf->contents);

	if (r_timegraph.integer)
		R_TimeGraph ();

	if (r_aliasstats.integer)
		R_PrintAliasStats ();

	if (r_speeds.integer)
		R_PrintTimes ();

	if (r_dspeeds.integer)
		R_PrintDSpeeds ();

	if (r_reportsurfout.integer && r_outofsurfaces)
		Con_Printf ("Short %d surfaces\n", r_outofsurfaces);

	if (r_reportedgeout.integer && r_outofedges)
		Con_Printf ("Short roughly %d edges\n", r_outofedges * 2 / 3);

// back to high floating-point precision
	Sys_HighFPPrecision ();
}

void R_RenderView (void)
{
/*	int		dummy;
	int		delta;

	delta = (byte *)&dummy - r_stack_start;
	if (delta < -10000 || delta > 10000)
		Sys_Error ("%s: called without enough stack", __thisfunc__);
*/
	if ( Hunk_LowMark() & 3 )
		Sys_Error ("Hunk is missaligned");

/*	if ( (intptr_t)(&dummy) & 3 )
		Sys_Error ("Stack is missaligned");
*/
	if ( (intptr_t)(&r_warpbuffer) & 3 )
		Sys_Error ("Globals are missaligned");

	R_RenderView_ ();
}

