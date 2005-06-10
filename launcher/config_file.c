#include "com_sys.h"
#include "launcher_defs.h"

#define LAUNCHER_CONFIG_FILE "launcher_options"

// Default values for the options
int destiny		= DEST_H2;
int mp_support		= 0;
int opengl_support	= 1;
int fullscreen		= 1;
int resolution		= RES_640;
int fxgamma		= 0;
int is8bit		= 0;
int use_fsaa		= 0;
int aasamples		= 2;
int vsync		= 0;
int mtex		= 1;
int gl_nonstd		= 0;
char gllibrary[256]	= "libGL.so.1";
int sound		= 1;
int sndrate		= 0;
int sndbits		= 1;
int midi		= 1;
int cdaudio		= 1;
int lan			= 1;
int mouse		= 1;
int debug		= 0;
int use_heap		= 0;
int use_zone		= 0;
int heapsize		= 32768;
int zonesize		= 48;
#ifndef DEMOBUILD
int h2game		= 0;
int hwgame		= 0;
#endif

extern char userdir[1024];

FILE * open_config_file(char *flags) {

#define MAX_PATH 1024

	FILE *thefile;
	char *config_file_name =0;
	config_file_name = (char *)calloc(MAX_PATH, sizeof(char));

	strcat (config_file_name,userdir);
	strcat (config_file_name,"/");
	strcat (config_file_name,LAUNCHER_CONFIG_FILE);
	thefile = fopen(config_file_name, flags);
	free (config_file_name);
	// NULL check has to be done later
	return thefile;
}


int write_config_file() {

	FILE *cfg_file;
	cfg_file = open_config_file("w");
	if (cfg_file == NULL) {
		fprintf(stderr, " Error: couldn't open config file for writing\n");
		return 1;

	} else {

		fprintf(cfg_file, "# Linux Hexen2 Launcher Options file\n\n");
		fprintf(cfg_file, "# This file has been automatically generated\n\n");

		fprintf(cfg_file, "destiny=%d\n",destiny);
#ifndef DEMOBUILD
		fprintf(cfg_file, "hwgame=%d\n",hwgame);
		fprintf(cfg_file, "mp_support=%d\n",mp_support);
#endif
		fprintf(cfg_file, "opengl_support=%d\n",opengl_support);
		fprintf(cfg_file, "fullscreen=%d\n",fullscreen);
		fprintf(cfg_file, "resolution=%d\n",resolution);
		fprintf(cfg_file, "fxgamma=%d\n",fxgamma);
		fprintf(cfg_file, "is8bit=%d\n",is8bit);
		fprintf(cfg_file, "use_fsaa=%d\n",use_fsaa);
		fprintf(cfg_file, "aasamples=%d\n",aasamples);
		fprintf(cfg_file, "vsync=%d\n",vsync);
		fprintf(cfg_file, "mtex=%d\n",mtex);
		fprintf(cfg_file, "gl_nonstd=%d\n",gl_nonstd);
		fprintf(cfg_file, "gllibrary=\"%s\"\n",gllibrary);
		fprintf(cfg_file, "sound=%d\n",sound);
		fprintf(cfg_file, "sndrate=%d\n",sndrate);
		fprintf(cfg_file, "sndbits=%d\n",sndbits);
		fprintf(cfg_file, "midi=%d\n",midi);
		fprintf(cfg_file, "cdaudio=%d\n",cdaudio);
		fprintf(cfg_file, "lan=%d\n",lan);
		fprintf(cfg_file, "mouse=%d\n",mouse);
		fprintf(cfg_file, "debug=%d\n",debug);
		fprintf(cfg_file, "use_heap=%d\n",use_heap);
		fprintf(cfg_file, "use_zone=%d\n",use_zone);
		fprintf(cfg_file, "heapsize=%d\n",heapsize);
		fprintf(cfg_file, "zonesize=%d\n",zonesize);
	}
	fclose (cfg_file); 
	printf("Options saved successfully.\n");
	return 0;
}

int read_config_file() {

	FILE *cfg_file;
	char buff[1024];

	cfg_file = open_config_file("r");
	if (cfg_file == NULL) {
		printf("Creating default configuration file.....\n");
		write_config_file();
		return 0;
	} else {
		printf("Reading configuration file.... ");
		do {
			fgets(buff, sizeof(buff), cfg_file);
			if (!feof(cfg_file)) {

				buff[strlen(buff)-1] = '\0';
				if (strstr(buff, "destiny=") == buff) {
					destiny = atoi(buff + 8);
					if (destiny != DEST_H2 && destiny != DEST_HW)
							destiny = DEST_H2;
				}
#ifndef DEMOBUILD
				else if (strstr(buff, "hwgame=") == buff) {
					hwgame = atoi(buff + 7);
					if (hwgame < 0 || hwgame >= MAX_HWGAMES)
							hwgame = 0;
					}
				else if (strstr(buff, "mp_support=") == buff) {
					mp_support = atoi(buff + 11);
					if (mp_support != 0 && mp_support != 1)
							mp_support = 0;
					}
#endif
				else if (strstr(buff, "opengl_support=") == buff) {
					opengl_support = atoi(buff + 15);
					if (opengl_support != 0 && opengl_support != 1)
							opengl_support = 1;
				}
				else if (strstr(buff, "fullscreen=") == buff) {
					fullscreen = atoi(buff + 11);
					if (fullscreen != 0 && fullscreen != 1)
							fullscreen = 1;
				}
				else if (strstr(buff, "resolution=") == buff) {
					resolution = atoi(buff + 11);
					if (resolution < 0 || resolution > RES_MAX)
							resolution = RES_640;
				}
				else if (strstr(buff, "fxgamma=") == buff) {
					fxgamma = atoi(buff + 8);
					if (fxgamma != 0 && fxgamma != 1)
							fxgamma = 0;
				}
				else if (strstr(buff, "is8bit=") == buff) {
					is8bit = atoi(buff + 7);
					if (is8bit != 0 && is8bit != 1)
							is8bit = 0;
				}
				else if (strstr(buff, "use_fsaa=") == buff) {
					use_fsaa = atoi(buff + 9);
					if (use_fsaa != 0 && use_fsaa != 1)
							use_fsaa = 0;
				}
				else if (strstr(buff, "aasamples=") == buff) {
					aasamples = atoi(buff + 10);
					if (aasamples < 0 || aasamples > 4 || ((int)(aasamples/2))*2 != aasamples)
							aasamples = 0;
				}
				else if (strstr(buff, "vsync=") == buff) {
					vsync = atoi(buff + 6);
					if (vsync != 0 && vsync != 1)
							vsync = 0;
				}
				else if (strstr(buff, "mtex=") == buff) {
					mtex = atoi(buff + 5);
					if (mtex != 0 && mtex != 1)
							mtex = 1;
				}
				else if (strstr(buff, "gl_nonstd=") == buff) {
					gl_nonstd = atoi(buff + 10);
					if (gl_nonstd != 0 && gl_nonstd != 1)
							gl_nonstd = 0;
				}
				else if (strstr(buff, "gllibrary=") == buff) {
					memset (gllibrary, 0, sizeof(gllibrary));
					// first and last chars are quotes, strip them
					memcpy (gllibrary, buff+10+1, strlen(buff+10+2));
				}
				else if (strstr(buff, "sound=") == buff) {
					sound = atoi(buff + 6);
					if (sound < 0 || sound >= MAX_SOUND)
							sound = 1;
				}
				else if (strstr(buff, "sndrate=") == buff) {
					sndrate = atoi(buff + 8);
					if (sndrate < 0 || sndrate >= MAX_RATES)
							sndrate = 0;
				}
				else if (strstr(buff, "sndbits=") == buff) {
					sndbits = atoi(buff + 8);
					if (sndbits != 0 && sndbits != 1)
							sndbits = 1;
				}
				else if (strstr(buff, "midi=") == buff) {
					midi = atoi(buff + 5);
					if (midi != 0 && midi != 1)
							midi = 1;
				}
				else if (strstr(buff, "cdaudio=") == buff) {
					cdaudio = atoi(buff + 8);
					if (cdaudio != 0 && cdaudio != 1)
							cdaudio = 1;
				}
				else if (strstr(buff, "lan=") == buff) {
					lan = atoi(buff + 4);
					if (lan != 0 && lan != 1)
							lan = 1;
				}
				else if (strstr(buff, "mouse=") == buff) {
					mouse = atoi(buff + 6);
					if (mouse != 0 && mouse != 1)
							mouse = 1;
				}
				else if (strstr(buff, "debug=") == buff) {
					debug = atoi(buff + 6);
					if (debug != 0 && debug != 1)
							debug = 0;
				}
				else if (strstr(buff, "use_heap=") == buff) {
					use_heap = atoi(buff + 9);
					if (use_heap != 0 && use_heap != 1)
							use_heap = 0;
				}
				else if (strstr(buff, "use_zone=") == buff) {
					use_zone = atoi(buff + 9);
					if (use_zone != 0 && use_zone != 1)
							use_zone = 0;
				}
				else if (strstr(buff, "heapsize=") == buff) {
					heapsize = atoi(buff + 9);
					if (heapsize <  8192 || heapsize > 98304)
							heapsize = 32768;
				}
				else if (strstr(buff, "zonesize=") == buff) {
					zonesize = atoi(buff + 9);
					if (zonesize < 48 || zonesize > 1024)
							zonesize = 48;
				}
			}

		} while(!feof(cfg_file));

		printf ("OK.\n");
		fclose (cfg_file);
	}

	return 0;
}
