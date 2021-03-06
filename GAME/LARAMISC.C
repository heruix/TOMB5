#include "LARAMISC.H"

#include "SPECIFIC.H"
#include "LARA.H"
#if PSX_VERSION || PSXPC_VERSION
#include "CALCLARA.H"
#include "MATHS.H"
#include "FXTRIG.H"
#include "LARASURF.H"
#include "GETSTUFF.H"
#include "COLLIDE.H"
#include "3D_GEN.H"
#include "COLLIDE_S.H"
#endif
#include "CONTROL.H"
#include "DRAW.H"
#include "EFFECT2.H"
#include "LARASWIM.H"

#ifdef PC_VERSION
#include "GAME.H"
#else
#include "SETUP.H"
#endif

#include INPUT_H
#include "OBJECTS.H"
#include "GAMEFLOW.H"
#include "HEALTH.H"
#include "NEWINV2.H"
#include "LARAFIRE.H"
#include "SPOTCAM.H"
#if !SAT_VERSION
#include <string.h>
#endif
#include "EFFECTS.H"
#include "SOUND.H"
#include "SFX.H"
#include "CD.H"

#if PSX_VERSION && !PSXPC_TEST
#include <STDIO.H>
#endif

#if DEBUG_VERSION
char* states[131] =
{
	"WALK", "RUN", "STOP", "F JUMP", "POSE", "FASTBACK", "TURN R", "TURN L", "DEATH", 
	"FASTFALL", "HANG", "REACH", "SPLAT", "TREAD", "LAND", "COMPRESS", "BACK", "SWIM", 
	"GLIDE", "NULL", "FAST TURN", "STEP RIGHT", "STEP LEFT", "ROLL2", "SLIDE", "BJUMP", 
	"RJUMP", "LJUMP", "UP JUMP", "FALL BACK", "HANG LEFT", "HANG RIGHT", "SLIDE BACK", 
	"SURF TREAD", "SURF SWIM", "DIVE", "PUSHBLOCK", "PULLBLOCK", "PPREADY", "PICKUP", 
	"SWITCHON", "SWITCHOFF", "USE-KEY", "USE-PUZZLE", "UW_DEATH", "ROLL", "SPECIAL", 
	"SURFBACK", "SURFLEFT", "SURFRIGHT", "USE MIDAS", "DIE MIDAS", "SWANDIVE", "FASTDIVE", 
	"GYMNAST", "WATER OUT", "CLIMB STANCE", "CLIMBING", "CLIMB LEFT", "CLIMB END", 
	"CLIMB RIGHT", "CLIMB DOWN", "LARATEST1", "LARATEST2", "LARATEST3", "WADE", 
	"WATER-ROLL", "PICKUP FLARE", "TWIST", "KICK", "DEATHSLIDE", "DUCK", "DUCKROLL", 
	"DASH", "DASHDIVE", "HANG2", "MONKEYSWING", "MONKEYL", "MONKEYR", "MONKEY180", "ALL4S", 
	"CRAWL", "HANGTURNL", "HANGTURNR", "ALL4TURNL", "ALL4TURNR", "CRAWLBACK", "HANG2DUCK", 
	"CRAWL2HANG", "CONTROLLED", "CROWBAR", "?", "BLOCKSWITCH", "LIFT TRAPDOOR", 
	"PULL TRAPDOOR", "TURN SWITCH", "COG SWITCH", "RAIL SWITCH", "HIDDEN PICKUP", "ON POLE", 
	"POLE UP", "POLE DOWN", "POLE LEFT", "POLE RIGHT", "PULLEY", "DUCKROTL", "DUCKROTR", 
	"CORNEREXTL", "CORNEREXTR", "CORNERINTL", "CORNERINTR", "ON ROPE", "UP ROPE", "DOWN ROPE", 
	"SWING FWD", "SWING BACK", "116", "OPEN DOUBLE DOORS", "SWIMCHEAT", "TROPEPOSE", 
	"TROPETURN", "TROPEWALK", "TROPEFALL_L", "TROPEFALL_R", "TROPEGETON", "TROPEGETOFF", 
	"CROWDOVE", "TROPEUNDOFALL", "PBARS_SPIN", "PBARS_LEAPOFF", "LAST"
};

char* quadrant_names[4] =
{
	"NORTH", "EAST", "SOUTH", "WEST"
};

char* gunstates[6] =
{
	"ARMLESS", "HANDSBUSY", "DRAW", "UNDRAW", "READY", "SPECIAL"
};

char* waterstates[5] =
{
	"ABOVEWATER", "UNDERWATER", "SURFACE", "CHEAT", "WADE"
};

char* guntypes[9] =
{
	"UNARMED", "PISTOLS", "MAGNUMS", "UZIS", "SHOTGUN", "M16", "ROCKET", "HARPOON", "FLARE"
};

char* coll_typess[8] =
{
	"NONE", "FRONT", "LEFT", "RIGHT", "TOP", "TOPBOTTOM", "TOPFRONT", "CLAMP"
};

char* htypes[4] =
{
	"WALL", "SMALL_SLOPE", "BIG_SLOPE", "DIAGONAL"
};
#endif

static struct COLL_INFO *coll = &mycoll; // offset 0xA0B8C
short SubsuitAir = 0; // offset 0xA122E
struct COLL_INFO mycoll;
short cheat_hit_points; // offset 0xA3828

void GetLaraDeadlyBounds()//4B408(<), 4B86C (F)
{
#if PSX_VERSION || PSXPC_VERSION///@TODO PC subs not there yet.
	short* bounds;
	short tbounds[6];

	bounds = GetBoundsAccurate(lara_item);
	mPushUnitMatrix();
	mRotYXZ(lara_item->pos.y_rot, lara_item->pos.x_rot, lara_item->pos.z_rot);
	mSetTrans(0, 0, 0);
	mRotBoundingBoxNoPersp(bounds, &tbounds[0]);
	mPopMatrix();

	DeadlyBounds[0] = lara_item->pos.x_pos + tbounds[0];
	DeadlyBounds[1] = lara_item->pos.x_pos + tbounds[1];
	DeadlyBounds[2] = lara_item->pos.y_pos + tbounds[2];
	DeadlyBounds[3] = lara_item->pos.y_pos + tbounds[3];
	DeadlyBounds[4] = lara_item->pos.z_pos + tbounds[4];
	DeadlyBounds[5] = lara_item->pos.z_pos + tbounds[5];
#else
	UNIMPLEMENTED();
#endif
}

void DelAlignLaraToRope(struct ITEM_INFO* item)//4B3D8, 4B83C
{
	UNIMPLEMENTED();
}

void InitialiseLaraAnims(struct ITEM_INFO* item)//4B340(<), 4B7A4 (F)
{
	if (room[item->room_number].flags & RF_FILL_WATER)
	{
		lara.water_status = LW_UNDERWATER;
		item->goal_anim_state = STATE_LARA_UNDERWATER_STOP;
		item->current_anim_state = STATE_LARA_UNDERWATER_STOP;
		item->fallspeed = 0;
		item->anim_number = ANIMATION_LARA_UNDERWATER_IDLE;
		item->frame_number = anims[ANIMATION_LARA_UNDERWATER_IDLE].frame_base;
	}
	else
	{
		lara.water_status = LW_ABOVE_WATER;
		item->goal_anim_state = STATE_LARA_STOP;
		item->current_anim_state = STATE_LARA_STOP;
		item->anim_number = ANIMATION_LARA_STAY_SOLID;
		item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
	}
}

void InitialiseLaraLoad(short item_num)//4B308, 4B76C (F)
{
	lara.item_number = item_num;
	lara_item = &items[item_num];
}

void LaraControl(short item_number)//4A838, 4AC9C
{
	long oldx; // $s7
	long oldy; // $fp
	long oldz; // stack offset -44
	struct ITEM_INFO* item; // $s0
	int wh; // $s3
	int wd; // $s4
	int hfw; // $s2
	int room_water_state; // $s5
	short room_number; // stack offset -48

	//a1 = &lara
	//a0 = lara.bitfield
	//v1 = lara.IsMoving
	//v0 = lara.MoveCount & 0xFF
	//v1 = lara.MoveCount + 1;

	item = lara_item;

	if (lara.IsMoving)
	{
		if (++lara.MoveCount > 90)
		{
			lara.IsMoving = 0;
			lara.gun_status = 0;
		}//loc_4A8B0
	}//loc_4A8B0

	//a1 = &lara
	if (!bDisableLaraControl)
	{
		//v1 = &lara
		lara.locationPad = -128;
	}//loc_4A8D0

	oldx = lara_item->pos.x_pos;//s7
	oldy = lara_item->pos.y_pos;//fp
	oldz = lara_item->pos.z_pos;//a0

	//a0 = lara_item
	if (lara.gun_status == 1 && lara_item->current_anim_state == 2 &&
		lara_item->goal_anim_state == 2 && lara_item->anim_number == 0x67 &&
		lara_item->gravity_status == 0)
	{
		lara.gun_status = 0;
	}//loc_4A944

	//a0 = &lara
	if (item->current_anim_state != 0x49 && DashTimer < 120)
	{
		DashTimer++;
	}//loc_4A978

	lara.IsDucked = 0;
	//v0 = &room[item->room_number]
	room_water_state = room[item->room_number].flags & RF_FILL_WATER;
	wd = GetWaterDepth(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
	wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	hfw = -32512;
	if (wh != -32512)
	{
		hfw = item->pos.y_pos - wh;
	}//loc_4A9F0
	lara.water_surface_dist = -hfw;
	WadeSplash(item, wh, wd);
	//s1 = &lara
	//a2 = lara.water_status

	switch (lara.water_status)
	{
	case 0:///@DONE
	{
		//loc_4AA4C
		if (hfw < 256)
		{
			//loc_4AF90
			break;
		}

		if (wd >= 475)
		{
			if (hfw < 257 && room_water_state != 0)
			{
				lara.air = 1800;
				lara.water_status = 1;
				item->status = 0;

				UpdateLaraRoom(lara_item, 0);
				StopSoundEffect(SFX_LARA_FALL);

				if (item->current_anim_state == 0x34)
				{
					item->goal_anim_state = 0x23;
					item->pos.x_rot = -0x1FFE;
					AnimateLara(item);
					item->fallspeed <<= 1;
				}//loc_4AABC
				else if (item->current_anim_state == 0x35)
				{
					item->goal_anim_state = 0x23;
					item->pos.x_rot = -0x36CE;
					AnimateLara(item);
					item->fallspeed <<= 1;
				}
				else
				{
					item->pos.x_rot = -0x1FFE;
					item->current_anim_state = 0x23;
					item->anim_number = 0x11;
					item->goal_anim_state = 0x11;
					item->fallspeed = (((item->fallspeed << 1) + item->fallspeed) + (((item->fallspeed << 1) + item->fallspeed) >> 31)) >> 1;
					item->frame_number = anims[112].frame_base;
				}
				//loc_4AB38
				//a0 = lara_item
				//v0 = lara
				lara.torso_y_rot = 0;
				lara.torso_x_rot = 0;
				lara.head_y_rot = 0;
				lara.head_x_rot = 0;

				///Splash();
				//v0 = -0xFA4

			}//loc_4AB90
		}
		else
		{
			//loc_4AB60
			if (hfw >= 257)
			{
				//v0 = -0xFA4
				//v1 = 4
				lara.water_status = 4;

				//v0 = -0xFA4
				if (!item->gravity_status)
				{
					item->goal_anim_state = 2;
				}//loc_4AB94
			}//loc_4AB94

			camera.target_elevation = -0xFA4;

			if (hfw < 256)
			{
				//v0 = &lara
				lara.water_status = 0;

				if (lara.climb_status == 0x41)
				{
					item->goal_anim_state = 1;
				}
			}//loc_4ABC4

		}
		break;
	}
	case 1:///@TODO
	{
		//loc_4AD88
		room_number = item->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
		//v1 = room_number
		//a0 = room
		//v1 = room[room_number].flags
		//v0 = -32512

		//a0 = item
		if (wd != -32512 && hfw < 0
			&& ABS(hfw) < 0x100
			&& !(room[room_number].flags & RF_FILL_WATER)
			&& item->anim_number != 0x72
			&& item->anim_number != 0x77)
		{
			lara.water_status = 2;
			//v0 = s3 + 1
			//v1 = anims
			//a1 = 0;
			item->pos.y_pos = wh + 1;
			//a2 = anims[114].frame_base;
			//v1 = lara_item
			item->goal_anim_state = 0x21;
			item->current_anim_state = 0x21;
			//v0 = 0xB
			item->anim_number = 0x72;
			item->fallspeed = 0;
			item->frame_number = anims[114].frame_base;
			lara.dive_count = 0xB;
			lara_item->pos.z_rot = 0;
			item->pos.x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			//j       loc_4AEFC
		}
		else
		{
			//loc_4AE70
			//v0 = -32512
			if (room_water_state == 0)
			{
				//a2 = &lara
				if (wd != -32512)
				{
					if (hfw < 0)
					{
						//a0 = item
						if (ABS(hfw) < 256)
						{
							//a1 = -381
							//a2 = &lara
							//v1 = anims
							//v0 = 2
							lara.water_status = 2;
							item->pos.y_pos = wh;
							//a3 = anims[114].frame_base
							//v1 = 0x21
							item->goal_anim_state = 0x21;
							item->current_anim_state = 0x21;
							//v1 = lara_item
							//v0 = 0x72
							item->anim_number = 0x72;
							//v0 = 0xb
							item->fallspeed = 0;
							item->frame_number = anims[114].frame_base;
							lara.dive_count = 0xB;
							lara_item->pos.z_rot = 0;
							item->pos.x_rot = 0;
							lara.torso_y_rot = 0;
							lara.torso_x_rot = 0;
							lara.head_y_rot = 0;
							lara.head_x_rot = 0;
						}//loc_4AF24
					}//loc_4AE8C ***********************************************
				}//loc_4AF24
			}//loc_4AF90
		}
		break;
	}
	case 2:///@TODO
		//loc_4ACCC
		break;
	case 3:///@TODO
		break;
	case 4:///@TODO
		//loc_4AB94
		break;
	}

	//loc_4AF90
	S_SetReverbType(room[item->room_number].ReverbType);

	if (item->hit_points <= 0)
	{
		item->hit_points = -1;

		if (lara.death_count == 0)
		{
			S_CDStop();
		}

		lara.death_count++;

		if ((lara_item->flags & 0x100))
		{
			lara.death_count++;
			return;
		}
	}

	//loc_4B020
	switch (lara.water_status)
	{
	case 0:
		//a0 = lara
		//v0 = lara
		if (lara.Gassed)
		{
			if (item->hit_points >= 0)
			{
				lara.air--;

				if (lara.air < 0)
				{
					lara.air = -1;
					item->hit_points -= 5;
				}
			}
		}
		else
		{
			//loc_4B0B0
			if (lara.air < 1800 && item->hit_points >= 0)
			{
				lara.air += 10;
			}

			if (lara.air > 1800)
			{
				lara.air = 1800;
			}
		}
		//loc_4B0F0
		LaraAboveWater(item, coll);
		break;
	case 4:
		if (lara.Busy)
		{
			if (item->hit_points >= 0)
			{
				lara.air--;
				if (lara.air < 0)
				{
					lara.air = -1;
					item->hit_points -= 5;
				}//loc_4B0F0
			}
			//loc_4B0F0
		}
		else
		{
			//loc_4B0B0
			if (lara.air < 0x708)
			{
				if (item->frame_number >= 0)
				{
					lara.air += 10;
					if (lara.air >= 0x709)
					{
						lara.air = 0x708;
					}
				}
				//loc_4B0F0
			}//loc_4B0F0
		}
		LaraAboveWater(item, coll);
		break;
	}

#if DEBUG_CAM

	if ((RawPad & 0x10))
	{
		camera.pos.x += 0x20;
		lara_item->pos.x_pos += 0x20;
	}
	if ((RawPad & 0x40))
	{
		camera.pos.x -= 0x20;
		lara_item->pos.x_pos -= 0x20;
	}

	if ((RawPad & 0x80))
	{
		camera.pos.z -= 0x20;
		lara_item->pos.z_pos -= 0x20;
	}

	if ((RawPad & 0x20))
	{
		camera.pos.z += 0x20;
		lara_item->pos.z_pos += 0x20;
	}

	if ((RawPad & 0x100))
	{
		camera.pos.y -= 0x20;
		lara_item->pos.y_pos -= 0x20;
	}

	if ((RawPad & 0x400))
	{
		camera.pos.y += 0x20;
		lara_item->pos.y_pos += 0x20;
	}

	if ((RawPad & IN_L2))
	{
		camera.pos.x += SIN(90) * 32;
	}
	/*struct FLOOR_INFO* floor;
	short rn = camera.pos.room_number;
	floor = GetFloor(camera.pos.x, camera.pos.y, camera.pos.z, &rn);
	int height = GetHeight(floor, camera.pos.x, camera.pos.y, camera.pos.z);
	UpdateLaraRoom(lara_item, height);
	if (floor != NULL)
	{
		camera.pos.room_number = rn;
	}*/

	//item->pos.x_pos += (SIN(lara.move_angle) * item->speed) >> W2V_SHIFT;
	//item->pos.z_pos += (COS(lara.move_angle) * item->speed) >> W2V_SHIFT;
#endif
}

void LaraCheat(struct ITEM_INFO* item, struct COLL_INFO* coll)//4A790(<), 4ABF4(<) (F)
{
	lara_item->hit_points = 1000;
	LaraUnderWater(item, coll);
	if (input & IN_WALK)
	{
		if (!(input & IN_LOOK))
		{
			lara.water_status = LW_ABOVE_WATER;
			item->frame_number = anims[ANIMATION_LARA_STAY_SOLID].frame_base;
			item->anim_number = ANIMATION_LARA_STAY_SOLID;
			item->pos.z_rot = 0;
			item->pos.x_rot = 0;
			lara.torso_y_rot = 0;
			lara.torso_x_rot = 0;
			lara.head_y_rot = 0;
			lara.head_x_rot = 0;
			lara.gun_status = LG_NO_ARMS;
			LaraInitialiseMeshes();
			lara.mesh_effects = 0;
			lara_item->hit_points = cheat_hit_points;
		}
	}
}

void LaraInitialiseMeshes()//4A684, 4AAE8 (F)
{
	int i;
	for(i = 0; i < 15; i++)
	{
		lara.mesh_ptrs[i] = meshes[objects[LARA].mesh_index + 2 * i] = meshes[objects[LARA_SKIN].mesh_index + 2 * i];
	}

	if (gfCurrentLevel >= LVL5_GALLOWS_TREE && gfCurrentLevel <= LVL5_OLD_MILL)
	{
		lara.mesh_ptrs[LM_TORSO] = meshes[objects[ANIMATING6_MIP].mesh_index + 2 * LM_TORSO];
	}

	if (lara.gun_type == WEAPON_HK)
	{
		lara.back_gun = WEAPON_HK;
	}
	else if (!lara.shotgun_type_carried)
	{
		if (lara.hk_type_carried)
		{
			lara.back_gun = WEAPON_HK;
		}
	}
	else
	{
		lara.back_gun = WEAPON_UZI;
	}

	lara.gun_status = LG_NO_ARMS;
	lara.left_arm.frame_number = 0;
	lara.right_arm.frame_number = 0;
	lara.target = 0;
	lara.right_arm.lock = 0;
	lara.left_arm.lock = 0;
}

void InitialiseLara(int restore)
{
	int i;
	short item;
	short gun;

	if (lara.item_number == -1)
		return;

	item = lara.item_number;

	lara_item->data = &lara;
	lara_item->collidable = FALSE;

	if (restore)
	{
		struct lara_info backup;
		memcpy(&backup, &lara, sizeof(lara));
		memset(&lara, 0, sizeof(lara));
		memcpy(&lara.pistols_type_carried, &backup.pistols_type_carried, 59);
	}
	else
	{
		memset(&lara, 0, sizeof(lara));
	}

	lara.look = TRUE;
	lara.item_number = item;
	lara.hit_direction = -1;
	lara.air = 1800;
	lara.weapon_item = -1;
	PoisonFlag = 0;
	lara.dpoisoned = 0;
	lara.poisoned = 0;
	lara.water_surface_dist = 100;
	lara.holster = 14;
	lara.location = -1;
	lara.highest_location = -1;
	lara.RopePtr = -1;
	lara_item->hit_points = 1000;

	for(i = 0; i < gfNumPickups; i++)
	{
		DEL_picked_up_object(convert_invobj_to_obj(gfPickups[i]));
	}

	gfNumPickups = 0;

	lara.gun_status = LG_NO_ARMS;

	gun = WEAPON_NONE;

	if (!(gfLevelFlags & GF_LVOP_YOUNG_LARA) && objects[PISTOLS_ITEM].loaded)
		gun = WEAPON_PISTOLS;

	if ((gfLevelFlags & GF_LVOP_TRAIN) && objects[HK_ITEM].loaded && (lara.hk_type_carried & WTYPE_PRESENT))
		gun = WEAPON_HK;

	lara.last_gun_type = lara.gun_type = lara.request_gun_type = gun;

	LaraInitialiseMeshes();

	lara.skelebob = 0;

	if (objects[PISTOLS_ITEM].loaded)
		lara.pistols_type_carried = WTYPE_PRESENT | WTYPE_AMMO_1;

	lara.binoculars = WTYPE_PRESENT;

	if (!restore)
	{
		if (objects[FLARE_INV_ITEM].loaded)
			lara.num_flares = 3;

		lara.num_small_medipack = 3;
		lara.num_large_medipack = 1;
	}

	lara.num_pistols_ammo = -1;

	InitialiseLaraAnims(lara_item);

	DashTimer = 120;

	for (i = 0; i < gfNumTakeaways; i++)
	{
		NailInvItem(convert_invobj_to_obj(gfTakeaways[i]));
	}

	gfNumTakeaways = 0;

	weapons[WEAPON_REVOLVER].damage = gfCurrentLevel >= LVL5_BASE ? 15 : 6;

	switch(gfCurrentLevel)
	{
	case 6u:
		lara.pickupitems &= 0xFFF7u;

		lara.puzzleitems[0] = 10;
		return;
	case 5u:
		lara.pickupitems = 0;
		lara.pickupitemscombo = 0;
		lara.keyitems = 0;
		lara.keyitemscombo = 0;
		lara.puzzleitemscombo = 0;

		memset(lara.puzzleitems, 0, 12);
		return;
	case 7u:	
		lara.pickupitems = 0;

		lara.puzzleitems[0] = 0;
		return;
	case 0xCu:
		lara.pickupitems &= 0xFFFEu;

		lara.puzzleitems[2] = 0;
		lara.puzzleitems[3] = 0;
		break;
	case 0xEu:
		lara.pickupitems &= 0xFFFDu;
		break;
	default:
		if (gfCurrentLevel < LVL5_THIRTEENTH_FLOOR || gfCurrentLevel > LVL5_RED_ALERT)
			lara.pickupitems &= 0xFFF7u;
		return;
	}

	lara.bottle = 0;
	lara.wetcloth = CLOTH_MISSING;
}