/*
 * $Header: /home/ozzie/Download/0000/uhexen2/gamecode/hc/siege/vorpal.hc,v 1.1 2005-01-26 17:26:13 sezero Exp $
 */

/*
==============================================================================

Q:\art\models\weapons\vorpal\final\vorpal.hc

==============================================================================
*/

// For building the model
$cd Q:\art\models\weapons\vorpal\final
$origin 0 0 0
$base BASE skin
$skin skin
$flags 0

$frame SwdRoot      
//

// FRAME 2 - 23
$frame 2ndSwd1      2ndSwd2      2ndSwd3      2ndSwd4      2ndSwd5      
$frame 2ndSwd9      2ndSwd10     
$frame 2ndSwd11     2ndSwd13     2ndSwd14     2ndSwd15     
$frame 2ndSwd16     2ndSwd18     2ndSwd19     2ndSwd21     
$frame 2ndSwd22     2ndSwd23     2ndSwd24     2ndSwd25     2ndSwd26     
$frame 2ndSwd27     2ndSwd28
     
//
// FRAME 24 - 36
$frame 3rdSwd1      3rdSwd2            
$frame 3rdSwd9         
$frame 3rdSwd11     3rdSwd12     3rdSwd13     3rdSwd14     3rdSwd15     
$frame 3rdSwd16     3rdSwd17     
$frame 3rdSwd22     3rdSwd23     3rdSwd24     

// FRAME 113 - 131
$frame 6thSwd13     6thSwd14     6thSwd15     
$frame 6thSwd16     6thSwd17


float VORP_BASE_DAMAGE			= 15;
float VORP_ADD_DAMAGE			= 10;
float VORP_PWR_BASE_DAMAGE		= 50;
float VORP_PWR_ADD_DAMAGE		= 30;
float VORP_RADIUS				= 150;

float VORP_PUSH					= 5;
/*
============
Smite that which is directly in front of you
============
*/
void vorpal_melee (void)
{
	vector	source;
	vector	org,dir;
	float damg;
	float inertia;

	makevectors (self.v_angle);
	source = self.origin+self.proj_ofs;
	traceline (source, source + v_forward*64, FALSE, self);  // Straight in front

	self.enemy = world;

	if (trace_fraction == 1.0)	// Anything right in front ?
	{
		traceline (source, source + v_forward*88 - (v_up * 30), FALSE, self);  // 30 down
	
		if (trace_fraction == 1.0)  
		{
			traceline (source, source + v_forward*88 + v_up * 30, FALSE, self);  // 30 up
		
			if (trace_fraction == 1.0)  
				return;
		}
	}
	
	org = trace_endpos + (v_forward * 2);

	if (trace_ent.takedamage) 	// It can be hurt
	{
		SpawnPuff (org, '0 0 0', 20,trace_ent);
		self.enemy = trace_ent;

		if(!trace_ent.mass)
			inertia=1;
		else if(trace_ent.mass<=5)
			inertia=trace_ent.mass;
		else
			inertia=trace_ent.mass/10;

		damg = (18 + random(7))*melee_dmg_mod_for_strength(self.strength);

		if(self.flags2&FL2_EXCALIBUR)
			damg*=random(2) + 3;//owch!

		if(trace_ent.thingtype!=THINGTYPE_FLESH)
			damg/=2;

		org = trace_endpos + (v_forward * -1);

		if (!MetalHitSound(trace_ent.thingtype))
			sound (self, CHAN_WEAPON, "weapons/vorpht1.wav", 1, ATTN_NORM);

		dir =  trace_ent.origin - self.origin;
		if(trace_ent.solid!=SOLID_BSP&&trace_ent.movetype!=MOVETYPE_PUSH)
		{
			trace_ent.velocity = dir * VORP_PUSH*(1/inertia);
			if(trace_ent.movetype==MOVETYPE_FLY)
			{
				if(trace_ent.flags&FL_ONGROUND)
					trace_ent.velocity_z=80/inertia;
			}
			else
				trace_ent.velocity_z = 80/inertia;
			trace_ent.flags (-) FL_ONGROUND;
		}
		T_Damage (trace_ent, self, self, damg);
	}
	else	// hit wall
	{
		sound (self, CHAN_WEAPON, "weapons/vorpht2.wav", 1, ATTN_NORM);
		org = trace_endpos + (v_forward * -1);
		org += '0 0 10';
		CreateWhiteSmoke(org,'0 0 2',HX_FRAME_TIME);
	}
}

/*
============
Deflect missiles
============
*/
/*
void vorpal_downmissile (void)
{
	vector  source,dir;
	entity  victim;
	float chance;
	entity hold;

	if (!self.artifact_active & ART_TOMEOFPOWER)
		return;

	victim = findradius( self.origin, 150);
	while(victim)
	{
		if ((victim.movetype == MOVETYPE_FLYMISSILE) && (victim.owner != self))
		{
			victim.owner = self;
			chance = random();
			dir = victim.origin + (v_forward * -1);
			CreateLittleWhiteFlash(dir);
			sound (self, CHAN_WEAPON, "weapons/vorpturn.wav", 1, ATTN_NORM);
			if (chance < 0.9)  // Deflect it
			{
				victim.v_angle = self.v_angle + randomv('-180 -180 -180', '180 180 180'); 

				makevectors (victim.v_angle);
				victim.velocity = v_forward * 1000;
			}
			else  // reflect missile
				victim.velocity = '0 0 0' - victim.velocity;
		}
		victim = victim.chain;
	}
}
*/

/*
============
Fire Vorpal sword in normal mode
============
*/
void vorpal_normal_fire (void)
{
	entity  victim;
	float damg,damage_flg;

	vorpal_melee ();

	return;

	damage_flg = 0;
	victim = findradius( self.origin, 100);
	while(victim)
	{	// Enemy would be the one that took direct melee damage so don't hurt him twice
		if ((victim.health) && (victim!=self) && infront(victim))
		{
			traceline (self.origin + self.proj_ofs, (victim.absmin+victim.absmax)*0.5, FALSE, self);  // 30 up

			if (trace_ent == victim)
			{
				damage_flg = 1;

				//sound (self, CHAN_WEAPON, "weapons/vorpblst.wav", 1, ATTN_NORM);
				weapon_sound(self, "weapons/vorpblst.wav");

				damg = (15 + random(10))*melee_dmg_mod_for_strength(self.strength);
				if(victim.thingtype!=THINGTYPE_FLESH)
					damg/=2;
				T_Damage (victim, self, self, damg);
			}
		}

		victim = victim.chain;
	}
}

/*
============
Decide if vorpal sword is in Normal or Powerup mode
============
*/
void vorpal_fire (void)
{
	vorpal_normal_fire();
}

/*
============
vorpal_ready - vorpal sword is in ready position
============
*/
void vorpal_ready (void) 
{
	self.weaponframe = $SwdRoot;
	self.th_weapon=vorpal_ready;
}

/*
============
vorpal_twitch - vorpal sword twitches 
============
*/
void vorpal_twitch (void)
{
	self.wfs = advanceweaponframe($2ndSwd1,$2ndSwd28);
	self.th_weapon = vorpal_twitch;

	if (self.weaponframe == $2ndSwd11)	// Frame 48
		//sound (self, CHAN_WEAPON, "weapons/vorpswng.wav", 1, ATTN_NORM);
		weapon_sound(self, "weapons/vorpswng.wav");
	else if (self.weaponframe == $2ndSwd18)	// Frame 55
		//sound (self, CHAN_WEAPON, "weapons/vorpswng.wav", 1, ATTN_NORM);
		weapon_sound(self, "weapons/vorpswng.wav");

	if (self.wfs == WF_LAST_FRAME)
		vorpal_ready();
}

// Rotate model around player
void moveswipe(void) [++0 .. 6]
{ 
	vector org;

	makevectors (self.owner.v_angle);
	org = self.owner.origin + self.owner.view_ofs - '0 0 25';
	setorigin (self, org);
	self.angles = self.owner.v_angle;
	self.angles_y += 180;

	self.think = moveswipe;
	thinktime self : HX_FRAME_TIME;

	if (self.frame>5)		
		self.think = SUB_Remove;
}


void SpawnSwipe(void)
{
	entity swipe;
	vector org;

	swipe = spawn ();

	CreateEntityNew(swipe,ENT_SWIPE,"models/vorpswip.mdl",SUB_Null);

	makevectors (self.v_angle);
	swipe.angles = self.v_angle;
	swipe.angles_y += 180;

	org = self.origin + self.view_ofs - '0 0 25';
	setorigin (swipe, org);
	
	swipe.counter =0;
	swipe.owner = self;

	swipe.velocity = '0 0 0';
	swipe.touch = SUB_Null;
	swipe.drawflags(+)DRF_TRANSLUCENT;

	swipe.think = moveswipe;
	thinktime swipe : HX_FRAME_TIME;

}

void vorpal_a (void)
{
	self.wfs = advanceweaponframe($3rdSwd1,$3rdSwd24);
	self.th_weapon = vorpal_a;

	if (self.weaponframe == $3rdSwd2)	// Frame 80
	{
		if (self.artifact_active & ART_TOMEOFPOWER)
			//sound (self, CHAN_WEAPON, "weapons/vorppwr.wav", 1, ATTN_NORM);
			weapon_sound(self, "weapons/vorppwr.wav");
		else
			//sound (self, CHAN_WEAPON, "weapons/vorpswng.wav", 1, ATTN_NORM);
			weapon_sound(self, "weapons/vorpswng.wav");
		
	}
	else if (self.weaponframe == $3rdSwd9)	// Frame 84
		vorpal_fire ();

	if ((self.weaponframe == $3rdSwd9) && (self.bluemana>=2))
		SpawnSwipe();

	if (self.wfs == WF_LAST_FRAME)
		self.th_weapon=vorpal_ready;
}


/*
============
vorpal_select - vorpal sword was just chosen
============
*/
void vorpal_select (void)
{
	self.wfs = advanceweaponframe($2ndSwd5,$2ndSwd1);

	if(self.flags2&FL2_EXCALIBUR)
		self.weaponmodel = "models/v_excal.mdl";
	else
		self.weaponmodel = "models/vorpal.mdl";
	self.th_weapon=vorpal_select;
	self.last_attack=time;

	if (self.wfs == WF_LAST_FRAME)
	{
		self.attack_finished = time - 1;
		vorpal_twitch();
	}
}

/*
============
vorpal_deselect - vorpal sword was just un-chosen
============
*/
void vorpal_deselect (void)
{
	self.wfs = advanceweaponframe($6thSwd13,$6thSwd17);
	self.th_weapon=vorpal_deselect;

	if (self.wfs == WF_LAST_FRAME)
		W_SetCurrentAmmo();

}

void pal_vorpal_fire(void)
{
float num_weap;
	vorpal_a ();

	num_weap=num_for_weap (self.weapon);
	if(self.flags2&FL2_EXCALIBUR)
		self.attack_finished = time+0.1;
	else
		self.attack_finished = time+rate_and_acc_for_weap[(self.playerclass - 1)*6+(num_weap - 1)*2];
}

/*
 * $Log: not supported by cvs2svn $
 * 
 * 17    5/25/98 1:39p Mgummelt
 * 
 * 16    5/25/98 1:31p Mgummelt
 * 
 * 15    5/07/98 11:19p Mgummelt
 * Almost ready for release- fix some dwarf-related compatibility problems
 * 
 * 14    4/24/98 1:31a Mgummelt
 * Siege version 0.02 4/24/98 1:31 AM
 * 
 * 13    4/23/98 5:19p Mgummelt
 * Siege version 0.01 4/23/98
 * 
 * 12    4/16/98 4:35p Mgummelt
 * 
 * 10    4/10/98 10:58a Nalbury
 * moved some stuff from the phs to the pvs...
 * 
 * 9     4/07/98 9:07p Nalbury
 * No more reliable junk...
 * 
 * 8     3/30/98 7:07a Nalbury
 * Cleaned up the movement for several projectiles...
 * 
 * 7     3/28/98 3:39a Nalbury
 * started work on the powered up fire orb
 * 
 * 6     3/27/98 1:34p Mgummelt
 * Adding PHS_OVERRIDE_R channel flag and ATTN_LOOP to all door and plat
 * sounds, replacing all "null.wav" sounds with stopSound.
 * 
 * 5     3/18/98 11:49a Nalbury
 * Powered up now explodes.
 * 
 * 4     3/12/98 12:21p Nalbury
 * Using weapon sound instead of regular sound.
 * 
 * 3     3/04/98 3:00p Nalbury
 * Added some axe stuff...
 * 
 * 2     3/04/98 5:54a Nalbury
 * Massive net optimizations.  Removed unneeded crap.
 * 
 * 1     2/04/98 1:59p Rjohnson
 * 
 * 69    9/11/97 4:34p Mgummelt
 * 
 * 68    8/30/97 5:01p Mgummelt
 * 
 * 67    8/27/97 9:38p Jweier
 * 
 * 66    8/26/97 7:38a Mgummelt
 * 
 * 65    8/22/97 2:49p Jweier
 * 
 * 64    8/20/97 11:30a Rlove
 * 
 * 63    8/19/97 7:44p Rlove
 * 
 * 61    8/18/97 1:42p Rlove
 * removed dprints
 * 
 * 60    8/12/97 6:10p Mgummelt
 * 
 * 59    8/08/97 6:22p Mgummelt
 * 
 * 58    7/31/97 1:41p Mgummelt
 * 
 * 57    7/25/97 11:24a Mgummelt
 * 
 * 56    7/24/97 4:06p Rlove
 * 
 * 55    7/24/97 3:53p Rlove
 * 
 * 54    7/21/97 3:03p Rlove
 * 
 * 53    7/17/97 1:53p Rlove
 * 
 * 52    7/12/97 1:31p Rlove
 * 
 * 51    6/20/97 8:25a Rlove
 * Tightened up the code a bit.
 * 
 * 47    6/19/97 4:08p Rjohnson
 * removed crandom()
 * 
 * 46    6/18/97 7:48p Mgummelt
 * 
 * 45    6/18/97 7:46p Mgummelt
 * 
 * 44    6/18/97 2:41p Mgummelt
 * 
 * 43    6/17/97 10:20a Rlove
 * 
 * 42    6/17/97 8:37a Rlove
 * 
 * 41    6/16/97 4:42p Rlove
 * 
 * 40    6/16/97 2:47p Rlove
 * Shortened its reach in normal mode
 * 
 * 39    6/16/97 11:42a Rlove
 * aim has to be given the origin of where the firing takes place at.
 * 
 * 38    6/16/97 8:04a Rlove
 * Fixed null.wav error, also paladin weapons deplete mana now
 * 
 * 37    6/13/97 6:08p Rlove
 * 
 * 36    6/13/97 10:11a Rlove
 * Moved all message.hc to strings.hc
 * 
 * 33    6/09/97 11:20a Rlove
 * 
 * 32    6/09/97 10:13a Rlove
 * 
 * 31    6/05/97 9:30a Rlove
 * Weapons now have deselect animations
 * 
 * 30    6/02/97 9:55a Rlove
 * Changed where firing is done
 * 
 * 29    5/31/97 9:22a Rlove
 * Newer, faster, better
 * 
 * 28    5/30/97 3:47p Rlove
 * Increased power up swipe distance and it won't hit things on the other
 * sides of walls
 * 
 * 27    5/28/97 8:13p Mgummelt
 * 
 * 26    5/22/97 6:39p Mgummelt
 * 
 * 25    5/22/97 6:37p Mgummelt
 * 
 * 24    5/16/97 9:03a Rlove
 * New swipe, damage in melee calc
 * 
 * 23    5/15/97 1:33p Rlove
 * 
 * 22    5/12/97 11:06a Rlove
 * 
 * 21    5/12/97 10:37a Rlove
 * 
 * 20    5/06/97 1:29p Mgummelt
 * 
 * 19    5/05/97 2:49p Rlove
 * 
 * 17    5/05/97 7:38a Rlove
 * 
 * 16    4/25/97 8:32p Mgummelt
 * 
 * 15    4/22/97 9:59a Rlove
 * Toned down vorpal kickback
 * 
 * 14    4/21/97 9:16a Rlove
 * Tried new axe animations
 * 
 * 13    4/18/97 2:24p Rlove
 * Changed vorpal sword over to new weapon code
 * 
 * 12    4/18/97 11:44a Rlove
 * changed advanceweaponframe to return frame state
 * 
 * 11    4/17/97 1:28p Rlove
 * added new built advanceweaponframe
 * 
 * 10    4/16/96 11:52p Mgummelt
 * 
 * 9     4/16/97 7:59a Rlove
 * Removed references to ammo_  fields
 * 
 * 8     4/14/97 5:04p Rlove
 * 
 * 5     4/10/97 2:14p Rlove
 * Some tweaking of gauntlets and vorpal sword.
 * 
 * 4     4/09/97 2:41p Rlove
 * New Raven weapon sounds
 * 
 * 3     4/04/97 5:40p Rlove
 * 
 * 2     4/01/97 2:44p Rlove
 * New weapons, vorpal sword and purifier
 * 
 */
