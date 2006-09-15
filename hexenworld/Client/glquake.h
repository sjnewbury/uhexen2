/*
	glquake.h
	common glquake header

	$Id: glquake.h,v 1.44 2006-09-15 12:03:18 sezero Exp $
*/


#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__MACOSX__)
#include <OpenGL/gl.h>
#elif defined(__MACOS__)
#include <gl.h>
#elif defined(__MORPHOS__)
#include <proto/tinygl.h>
#include <tgl/gl.h>
#else
#include <GL/gl.h>
#endif

#include "gl_opt.h"
#include "gl_func.h"
#include "gl_model.h"

#ifndef	GLX_3DFX_WINDOW_MODE_MESA
#define	GLX_3DFX_WINDOW_MODE_MESA		0x1
#endif

#ifndef	GLX_3DFX_FULLSCREEN_MODE_MESA
#define	GLX_3DFX_FULLSCREEN_MODE_MESA		0x2
#endif

#ifndef	GL_TEXTURE0_ARB
#define	GL_TEXTURE0_ARB				0x84C0
#define	GL_TEXTURE1_ARB				0x84C1
#define	GL_TEXTURE2_ARB				0x84C2
#define	GL_TEXTURE3_ARB				0x84C3
#define	GL_TEXTURE4_ARB				0x84C4
#define	GL_TEXTURE5_ARB				0x84C5

#define	GL_ACTIVE_TEXTURE_ARB			0x84E0
#define	GL_CLIENT_ACTIVE_TEXTURE_ARB		0x84E1
#define	GL_MAX_TEXTURE_UNITS_ARB		0x84E2
#endif

#ifndef	GL_MULTISAMPLE_ARB
#define	GL_MULTISAMPLE_ARB			0x809D
#endif

#ifndef	GL_SHARED_TEXTURE_PALETTE_EXT
#define	GL_SHARED_TEXTURE_PALETTE_EXT		0x81FB
#endif

#ifndef GL_COLOR_INDEX8_EXT
#define GL_COLOR_INDEX8_EXT			0x80E5
#endif

// the GL_Bind macro
#define GL_Bind(texnum) {\
	if (currenttexture != (texnum)) {\
		currenttexture = (texnum);\
		glBindTexture_fp(GL_TEXTURE_2D, currenttexture);\
	}\
}

// defs for palettized textures
#define INVERSE_PAL_R_BITS 6
#define INVERSE_PAL_G_BITS 6
#define INVERSE_PAL_B_BITS 6
#define INVERSE_PAL_TOTAL_BITS	( INVERSE_PAL_R_BITS + INVERSE_PAL_G_BITS + INVERSE_PAL_B_BITS )

// misc. common glquake defines
#define MAX_GLTEXTURES		2048
#define MAX_EXTRA_TEXTURES	156   // 255-100+1
#define	MAX_CACHED_PICS		256
#define	MAX_LIGHTMAPS		64U

#define	GL_UNUSED_TEXTURE	0xFFFFFFFFU

#define	gl_solid_format		3
#define	gl_alpha_format		4

// types for textures

typedef struct
{
	GLuint		texnum;
	float	sl, tl, sh, th;
} glpic_t;

typedef struct cachepic_s
{
	char		name[MAX_QPATH];
	qpic_t		pic;
	byte		padding[32];	// for appended glpic
} cachepic_t;

#define MAX_IDENTIFIER	64	// MAX_QPATH
typedef struct
{
	GLuint		texnum;
	char	identifier[MAX_IDENTIFIER];
	int		width, height;
	qboolean	mipmap;
//	unsigned short	crc;
	unsigned long	hash;
} gltexture_t;


extern	GLuint	texture_extension_number;
extern	int	gl_filter_min;
extern	int	gl_filter_max;
extern	float	gldepthmin, gldepthmax;
extern	int	glx, gly, glwidth, glheight;
extern	GLuint	gl_extra_textures[MAX_EXTRA_TEXTURES];   // generic textures for models

void GL_BeginRendering (int *x, int *y, int *width, int *height);
void GL_EndRendering (void);

GLuint GL_LoadTexture (char *identifier, int width, int height, byte *data, qboolean mipmap, qboolean alpha, int mode, qboolean rgba);
GLuint GL_LoadPicTexture (qpic_t *pic);
void D_ClearOpenGLTextures (int last_tex);

int M_DrawBigCharacter (int x, int y, int num, int numNext);
void GL_BuildLightmaps (void);
void GL_SetupLightmapFmt (qboolean check_cmdline);
void GL_MakeAliasModelDisplayLists (model_t *m, aliashdr_t *hdr);
void GL_Set2D (void);
void GL_SubdivideSurface (msurface_t *fa);
#ifdef QUAKE2
void R_LoadSkys (void);
void R_DrawSkyBox (void);
void R_ClearSkyBox (void);
#endif
//void EmitSkyPolys (msurface_t *fa);
void EmitWaterPolys (msurface_t *fa);
void EmitBothSkyLayers (msurface_t *fa);
qboolean R_CullBox (vec3_t mins, vec3_t maxs);
void R_DrawBrushModel (entity_t *e, qboolean Translucent);
void R_DrawSkyChain (msurface_t *s);
void R_DrawWorld (void);
void R_DrawWaterSurfaces (void);
void R_RenderBrushPoly (msurface_t *fa, qboolean override);
void R_RenderDlights (void);
void R_RotateForEntity (entity_t *e);
void R_MarkLights (dlight_t *light, int bit, mnode_t *node);
void R_AnimateLight(void);
int R_LightPoint (vec3_t p);
int *R_LightPointColour (vec3_t p);
void R_StoreEfrags (efrag_t **ppefrag);
void R_InitParticles (void);
void R_InitParticleTexture (void);
void R_ClearParticles (void);
void R_DrawParticles (void);
void R_NetGraph (void);


// r_local.h -- private refresh defs

#define ALIAS_BASE_SIZE_RATIO		(1.0 / 11.0)
					// normalizing factor so player model works
					// out to about 1 pixel per triangle
#define	MAX_SKIN_HEIGHT		480

#define TILE_SIZE		128	// size of textures generated by R_GenTiledSurf

#define SKYSHIFT		7
#define	SKYSIZE			(1 << SKYSHIFT)
#define SKYMASK			(SKYSIZE - 1)

#define BACKFACE_EPSILON	0.01


typedef struct surfcache_s
{
	struct surfcache_s	*next;
	struct surfcache_s 	**owner;	// NULL is an empty chunk of memory
	int			lightadj[MAXLIGHTMAPS]; // checked for strobe flush
	int			dlight;
	int			size;		// including header
	unsigned		width;
	unsigned		height;		// DEBUG only needed for debug
	float			mipscale;
	struct texture_s	*texture;	// checked for animating textures
	byte			data[4];	// width*height elements
} surfcache_t;


typedef struct
{
	pixel_t		*surfdat;	// destination for generated surface
	int		rowbytes;	// destination logical width in bytes
	msurface_t	*surf;		// description for surface to generate
	fixed8_t	lightadj[MAXLIGHTMAPS]; // adjust for lightmap levels for dynamic lighting
	texture_t	*texture;	// corrected for animating textures
	int		surfmip;	// mipmapped ratio of surface texels / world pixels
	int		surfwidth;	// in mipmapped texels
	int		surfheight;	// in mipmapped texels
} drawsurf_t;


// Changes to ptype_t must also be made in d_iface.h
typedef enum {
	pt_static,
	pt_grav,
	pt_fastgrav,
	pt_slowgrav,
	pt_fire,
	pt_explode,
	pt_explode2,
	pt_blob,
	pt_blob2,
	pt_rain,
	pt_c_explode,
	pt_c_explode2,
	pt_spit,
	pt_fireball,
	pt_ice,
	pt_spell,
	pt_test,
	pt_quake,
	pt_rd,			// rider's death
	pt_vorpal,
	pt_setstaff,
	pt_magicmissile,
	pt_boneshard,
	pt_scarab,
	pt_darken,
	pt_grensmoke,
	pt_redfire,
	pt_acidball,
	pt_bluestep
} ptype_t;

// Changes to rtype_t must also be made in glquake.h
typedef enum
{
	rt_rocket_trail = 0,
	rt_smoke,
	rt_blood,
	rt_tracer,
	rt_slight_blood,
	rt_tracer2,
	rt_voor_trail,
	rt_fireball,
	rt_ice,
	rt_spit,
	rt_spell,
	rt_vorpal,
	rt_setstaff,
	rt_magicmissile,
	rt_boneshard,
	rt_scarab,
	rt_grensmoke,
	rt_purify,
	rt_darken,
	rt_acidball,
	rt_bloodshot
} rt_type_t;

// if this is changed, it must be changed in d_iface.h too !!!
typedef struct particle_s
{
// driver-usable fields
	vec3_t		org;
	float		color;
// drivers never touch the following fields
	struct particle_s	*next;
	vec3_t		vel;
	float		ramp;
	float		die;
	ptype_t		type;
} particle_t;


void R_ReadPointFile_f (void);
void R_TranslatePlayerSkin (int playernum);


//====================================================


extern	entity_t	r_worldentity;
extern	qboolean	r_cache_thrash;		// compatability
extern	vec3_t		modelorg, r_entorigin;
extern	entity_t	*currententity;
extern	int		r_visframecount;	// ??? what difs?
extern	int		r_framecount;
extern	mplane_t	frustum[4];
extern	int		c_brush_polys, c_alias_polys;


//
// view origin
//
extern	vec3_t	vup;
extern	vec3_t	vpn;
extern	vec3_t	vright;
extern	vec3_t	r_origin;

//
// screen size info
//
extern	refdef_t	r_refdef;
extern	mleaf_t		*r_viewleaf, *r_oldviewleaf;
extern	texture_t	*r_notexture_mip;
extern	int		d_lightstylevalue[256];	// 8.8 fraction of base light value

extern	qboolean	envmap;
extern	GLuint	currenttexture;
extern	GLuint	particletexture;
extern	int	skytexturenum;		// index in cl.loadmodel, not gl texture object
extern	GLuint	netgraphtexture;	// netgraph texture
extern	GLuint	playertextures[16];
extern	byte	*playerTranslation;
extern	int	gl_texlevel;
extern	int	numgltextures;
#ifdef H2W
// we can't detect mapname change early enough in hw,
// so flush_textures is only for hexen2
#define	flush_textures	1
#else
extern	qboolean	flush_textures;
#endif

extern	cvar_t	r_norefresh;
extern	cvar_t	r_drawentities;
extern	cvar_t	r_drawworld;
extern	cvar_t	r_drawviewmodel;
extern	cvar_t	r_speeds;
extern	cvar_t	r_waterwarp;
extern	cvar_t	r_fullbright;
extern	cvar_t	r_lightmap;
extern	cvar_t	r_shadows;
extern	cvar_t	r_mirroralpha;
extern	cvar_t	r_wateralpha;
extern	cvar_t	r_skyalpha;
extern	cvar_t	r_dynamic;
extern	cvar_t	r_novis;
extern	cvar_t	r_wholeframe;

extern	cvar_t	r_netgraph;
extern	cvar_t	r_entdistance;
extern	cvar_t	r_teamcolor;

extern	cvar_t	gl_clear;
extern	cvar_t	gl_cull;
extern	cvar_t	gl_poly;
extern	cvar_t	gl_ztrick;
extern	cvar_t	gl_multitexture;
extern	cvar_t	gl_purge_maptex;
extern	cvar_t	gl_smoothmodels;
extern	cvar_t	gl_affinemodels;
extern	cvar_t	gl_polyblend;
extern	cvar_t	gl_keeptjunctions;
extern	cvar_t	gl_reporttjunctions;
extern	cvar_t	gl_flashblend;
extern	cvar_t	gl_nocolors;
extern	cvar_t	gl_waterripple;
extern	cvar_t	gl_waterwarp;
extern	cvar_t	gl_stencilshadow;
extern	cvar_t	gl_glows;
extern	cvar_t	gl_other_glows;
extern	cvar_t	gl_missile_glows;

extern	cvar_t	gl_coloredlight;
extern	cvar_t	gl_colored_dynamic_lights;
extern	cvar_t	gl_extra_dynamic_lights;
extern	int	gl_coloredstatic;

extern	cvar_t	gl_lightmapfmt;
extern	int	gl_lightmap_format;
extern	qboolean lightmap_modified[MAX_LIGHTMAPS];

extern	qboolean gl_mtexable;
extern	qboolean have_stencil;

extern	GLint	gl_max_size;
extern	cvar_t	gl_playermip;

extern	int		mirrortexturenum;	// quake texturenum, not gltexturenum
extern	qboolean	mirror;
extern	mplane_t	*mirror_plane;

extern	float	r_world_matrix[16];

extern	const char *gl_vendor;
extern	const char *gl_renderer;
extern	const char *gl_version;
extern	const char *gl_extensions;


