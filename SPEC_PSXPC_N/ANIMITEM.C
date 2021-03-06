#include "ANIMITEM.H"

#include "3D_GEN.H"
#include "CONTROL.H"
#include "LOAD_LEV.H"
#include "DRAW.H"
#include "CAMERA.H"
#include "SETUP.H"
#include "DRAWOBJ.H"
#include "MATHS.H"
#include "MISC.H"
#include "LIGHT.H"

#include "GTEREG.H"
#include <assert.h>

void SetRotation2(int* fp, int t0, int t1, int t2, int t3, int t4)
{
	int a0 = fp[20];

	R11 = t0 & 0xFFFF;
	R12 = (t0 >> 16) & 0xFFFF;
	R13 = t1 & 0xFFFF;
	R21 = (t1 >> 16) & 0xFFFF;
	R22 = t2 & 0xFFFF;
	R23 = (t2 >> 16) & 0xFFFF;
	R31 = t3 & 0xFFFF;
	R32 = (t3 >> 16) & 0xFFFF;
	R33 = t4 & 0xFFFF;

	((int*)a0)[0] = t0;
	((int*)a0)[1] = t1;
	((int*)a0)[2] = t2;
	((int*)a0)[3] = t3;
	((int*)a0)[4] = t4;
}

void mRotY2(int ry, int* fp)
{
	ry = (ry >> 2) & 0x3FFC;
	if (ry == 0)
	{
		return;
	}

	//t0 = 0x9A8C8 rcossin_tbl
	//loc_81870
	int t5 = ((int*)&rcossin_tbl[ry >> 1])[0];
	int t7 = 0xFFFF0000;
	int t6 = (t5 >> 16) & 0xFFFF;
	t5 &= 0xFFFF;
	int t2 = -t5;
	VX0 = t6;
	VY0 = (t6 >> 16) & 0xFFFF;
	VZ0 = t2;

	int t0 = (R11 & 0xFFFF) | ((R12 & 0xFFFF) << 16);
	    t2 = (R22 & 0xFFFF) | ((R23 & 0xFFFF) << 16);
	int t3 = (R31 & 0xFFFF) | ((R32 & 0xFFFF) << 16);

	docop2(0x486012);

	VX1 = t5 & 0xFFFF;
	VY1 = (t5 >> 16) & 0xFFFF;
	VZ1 = t6 & 0xFFFF;

	t0 &= t7;
	t2 &= 0xFFFF;
	t3 &= t7;
	int t4 = MAC1;
	int t1 = MAC2;
	t5 = MAC3;

	docop2(0x48E012);

	t4 &= 0xFFFF;
	t0 |= t4;
	t1 <<= 16;
	t5 &= 0xFFFF;
	t3 |= t5;

	t5 = MAC1;
	t6 = MAC2;
	t4 = MAC3;

	t5 &= 0xFFFF;
	t1 |= t5;
	t6 <<= 16;
	t2 |= t6;

	SetRotation2(fp, t0, t1, t2, t3, t4);
}

void mTranslateXYZ2(int tx, int ty, int tz, int* fp)//81AB0
{
	int t4 = ty >> 15;
	if (ty < 0)
	{
		ty = -ty;
		t4 = ty >> 15;
		ty &= 0x7FFF;
		t4 = -t4;
		ty = -ty;

	}
	else
	{
		//loc_81AD0
		ty &= 0x7FFF;
	}

	//loc_81AD4
	int t5 = tz >> 15;
	if (tz < 0)
	{
		tz = -tz;
		t5 = tz >> 15;
		tz &= 0x7FFF;
		t5 = -t5;
		tz = -tz;
	}
	else
	{
		//loc_81AF4
		tz &= 0x7FFF;
	}

	//loc_81AF8
	int t3 = tx >> 15;
	if (tx < 0)
	{
		tx = -tx;
		t3 = tx >> 15;
		tx &= 0x7FFF;
		t3 = -t3;
		tx = -tx;
	}
	else
	{
		//loc_81B18
		tx &= 0x7FFF;
	}

	//loc_81B1C
	IR1 = t3;
	IR2 = t4;
	IR3 = t5;

	int v0 = fp[20];

	docop2(0x41E012);

	t3 = MAC1;
	t4 = MAC2;
	t5 = MAC3;

	IR1 = tx;
	IR2 = ty;
	IR3 = tz;

	docop2(0x498012);

	int t0 = t3 << 3;
	if (t3 < 0)
	{
		t3 = -t3;
		t3 <<= 3;
		t0 = -t3;
	}//loc_81B60

	int t1 = t4 << 3;
	if (t4 < 0)
	{
		t4 = -t4;
		t4 <<= 3;
		t1 = -t4;
	}

	//loc_81B74
	int t2 = t5 << 3;
	if (t5 < 0)
	{
		t5 = -t5;
		t5 <<= 3;
		t2 = -t5;
	}
	//loc_81B88
	t3 = MAC1;
	t4 = MAC2;
	t5 = MAC3;

	t0 += t3;
	t1 += t4;
	t2 += t5;

	TRX = t0;
	TRY = t1;
	TRZ = t2;

	((int*)v0)[5] = t0;
	((int*)v0)[6] = t1;
	((int*)v0)[7] = t2;
}


void mTranslateAbsXYZ2(int tx, int ty, int tz, int* fp)
{
	TRX = 0;
	TRY = 0;
	TRZ = 0;
	tx -= fp[27];
	ty -= fp[28];
	tz -= fp[29];

	mTranslateXYZ2(tx, ty, tz, fp);
}

void mmPopMatrix2(int* fp)//81C0C(<)
{
	mLoadMatrix2((int*)(fp[20]-0x20), fp);
}

void mmPushMatrix2(int* fp)
{
	int* a0 = (int*)fp[20];
	a0 += 8;
	a0[0] = (R11 & 0xFFFF) | (R12 & 0xFFFF) << 16;
	a0[1] = (R13 & 0xFFFF) | (R21 & 0xFFFF) << 16;
	a0[2] = (R22 & 0xFFFF) | (R23 & 0xFFFF) << 16;
	a0[3] = (R31 & 0xFFFF) | (R32 & 0xFFFF) << 16;
	a0[4] = R33;
	a0[5] = TRX;
	a0[6] = TRY;
	a0[7] = TRZ;

	fp[20] = (int)a0;
}

void mLoadMatrix2(int* a0, int* fp)//81C18(<)
{
	R11 = (a0[0] & 0xFFFF);
	R12 = (a0[0] >> 16) & 0xFFFF;
	R13 = (a0[1] & 0xFFFF);
	R21 = (a0[1] >> 16) & 0xFFFF;
	R22 = (a0[2] & 0xFFFF);
	R23 = (a0[2] >> 16) & 0xFFFF;
	R31 = (a0[3] & 0xFFFF);
	R32 = (a0[3] >> 16) & 0xFFFF;
	R33 = (a0[4] & 0xFFFF);
	TRX = a0[5];
	TRY = a0[6];
	TRZ = a0[7];

	fp[20] = (int)a0;
}

void stash_the_info(int meshp/*a0*/, int* fp)//81750
{
	int* at;

	at = (int*)fp[17];

	((int*)at)[0] = meshp;

	at[1] = (R11 & 0xFFFF) | (R12 & 0xFFFF) << 16;
	at[2] = (R13 & 0xFFFF) | (R21 & 0xFFFF) << 16;
	at[3] = (R22 & 0xFFFF) | (R23 & 0xFFFF) << 16;
	at[4] = (R31 & 0xFFFF) | (R32 & 0xFFFF) << 16;

	at[5] = (R33 & 0xFFFF);
	at[6] = TRX;
	at[7] = TRY;
	at[8] = TRZ;

	at += 9;

	fp[19]++;
	fp[17] = (int)at;
}

void GetBounds(int* t0, int* a2, int* a3, int* t1, int* t2, int* v0, int* a0, int* a1, int* t3, int* t4, int* t5)//8139C
{
	if (*t0 < *a2)
	{
		*a2 = *t0;
	}

	//loc_813AC
	if (*t0 >= *a3)
	{
		*a3 = *t0;
	}

	//loc_813B8
	if (*t1 < *a2)
	{
		*a2 = *t1;
	}

	//loc_813C4
	if (*t1 >= *a3)
	{
		*a3 = *t1;
	}

	if (*t2 < *a2)
	{
		*a3 = *t2;
	}

	//loc_813DC
	*t0 <<= 16;
	if (*t2 >= *a3)
	{
		*a3 = *t2;
	}

	//loc_813E8
	*t1 <<= 16;
	*t2 <<= 16;

	if (*t0 < *a0)
	{
		*a0 = *t0;
	}

	if (*t0 >= *a1)
	{
		*a1 = *t0;
	}

	if (*t1 < *a0)
	{
		*a0 = *t1;
	}

	if (*t1 >= *a1)
	{
		*a1 = *t1;
	}

	if (*t2 < *a0)
	{
		*a0 = *t2;
	}

	if (*t2 >= *a1)
	{
		*a1 = *t2;
	}

	if (*t3 < 0x5000)
	{
		v0[0]++;
	}

	if (*t4 < 0x5000)
	{
		v0[0]++;
	}

	if (*t5 < 0x5000)
	{
		v0[0]++;
	}
}

int mClipBoundingBox2(unsigned short* bounds, int* sp /*fp*/)//811FC
{
	int t0 = TRZ - 20480;
	int t3;
	int t1;
	int t4;
	int t2;
	int t7;
	int v0 = 0;
	int at;
	int t9;
	int t8;
	int a0;
	int a1;
	int a2;
	int t5;
	int a3;

	if (t0 < 0)
	{
		t0 = bounds[0];
		t3 = bounds[1];
		t1 = bounds[2];
		t4 = bounds[3];
		t2 = bounds[4];
		t7 = bounds[5];

		t1 <<= 16;
		t4 <<= 16;

		at = t0 | t1;
		VX0 = (at & 0xFFFF);
		VY0 = (at >> 16) & 0xFFFF;
		VZ0 = t2;

		at = t3 | t1;
		VX1 = (at & 0xFFFF);
		VY1 = (at >> 16) & 0xFFFF;
		VZ1 = t2;

		at = t0 | t4;
		VX2 = (at & 0xFFFF);
		VY2 = (at >> 16) & 0xFFFF;
		VZ2 = t2;

		docop2(0x280030);

		t9 = t3 | t4;
		t8 = t2;

		a0 = 0x7FFFFFFF;
		a1 = 0x81000000;
		a2 = a0;
		t0 = SXY0;
		t1 = SXY1;
		t2 = SXY2;
		t3 = SZ1;
		t4 = SZ2;
		t5 = SZ3;

		VZ0 = t7;
		VZ1 = t7;
		VZ2 = t7;

		a3 = a1;

		docop2(0x280030);

		t3 -= 33;
		t4 -= 33;
		t5 -= 33;

		v0 = 0;
		GetBounds(&t0, &a2, &a3, &t1, &t2, &v0, &a0, &a1, &t3, &t4, &t5);

		t0 = SXY0;
		t1 = SXY1;
		t2 = SXY2;
		t3 = SZ1;
		t4 = SZ2;
		t5 = SZ3;

		VX0 = (t9 & 0xFFFF);
		VY0 = (t9 >> 16) & 0xFFFF;
		VZ0 = t8;

		VX1 = (t9 & 0xFFFF);
		VY1 = (t9 >> 16) & 0xFFFF;

		t2 -= 0x21;
		docop2(0x280030);
		t4 -= 0x21;
		t5 -= 0x21;

		GetBounds(&t0, &a2, &a3, &t1, &t2, &v0, &a0, &a1, &t3, &t4, &t5);
	
		t0 = SXY0;
		t1 = SXY1;
		t3 = SZ1;
		t4 = SZ2;

		t3 -= 0x21;
		t4 -= 0x21;
		t2 = t1;
		t5 = t4;

		GetBounds(&t0, &a2, &a3, &t1, &t2, &v0, &a0, &a1, &t3, &t4, &t5);
		t0 = ((short*)sp)[73];
		t1 = ((short*)sp)[75];
		t2 = ((short*)sp)[72];
		t3 = ((short*)sp)[74];

		a0 >>= 16;
		a1 >>= 16;
		a2 >>= 16;
		a3 >>= 16;

		if (v0 == 0 || t0 < a0 || t1 < a2 || a1 < t2 || a3 < t3)
		{
			return 0;
		}

		if (v0 < 9 || a0 < t2 || a2 < t3 || t0 < a1 || t1 < a3)
		{
			return -1;
		}

		return 1;
	}

	return v0;
}

void init_scratchpad(int* fp)//8281C(<) (F)
{
	int t0;
	int t1;
	int t2;
	int t3;
	int t4;
	int t5;
	int t6;
	int t7;
	int* at = &fp[47];
	fp[20] = (int)at;

	t0 = (R11 & 0xFFFF) | (R12 & 0xFFFF) << 16;
	t1 = (R13 & 0xFFFF) | (R21 & 0xFFFF) << 16;
	t2 = (R22 & 0xFFFF) | (R23 & 0xFFFF) << 16;
	t3 = (R31 & 0xFFFF) | (R32 & 0xFFFF) << 16;
	t4 = R33;
	t5 = TRX;
	t6 = TRY;
	t7 = TRZ;

	at[0] = t0;
	at[1] = t1;
	at[2] = t2;
	at[3] = t3;
	at[4] = t4;
	at[5] = t5;
	at[6] = t6;
	at[7] = t7;

	fp[16] = (int)&stashed_objects_list[0];
	fp[17] = (int)&stashed_matrix_list[0];
	fp[18] = 0;
	fp[24] = (int)&items[0];
	fp[38] = (int)&room[0];

	((short*)fp)[53] = camera.pos.room_number;
	fp[39] = (int)anims;
	fp[40] = (int)meshes;
	fp[41] = (int)bones;

	t0 = ((int*)MatrixStack)[5];
	t1 = ((int*)MatrixStack)[6];
	t2 = ((int*)MatrixStack)[7];

	fp[46] = BinocularRange;
	fp[27] = t0;
	fp[28] = t1;
	fp[29] = t2;

	/* Must be set in outer function */
	//s1 = number_draw_rooms 
	//s0 = &draw_rooms[0];
}


void CalcAllAnimatingItems_ASM()
{
	int scratchPad[256];
	int* fp;
	int v1;
	struct room_info* r;//$s2
	struct static_info* s5;
	int at;
	int* a2;
	int v0;
	int i;//$s1
	int j;//$s3
	struct ITEM_INFO* item;//$s3
	struct object_info* object;//$s6
	int item_number;//$s2

	S_MemSet((char*)&scratchPad[0], 0, 1024);

	fp = &scratchPad[0];
	init_scratchpad(fp);

	if (number_draw_rooms > 0)
	{
		//loc_82658
		for (i = 0; i < number_draw_rooms; i++)
		{
			r = &room[draw_rooms[i]];
			((short*)fp)[52] = draw_rooms[i];
			((int*)fp)[25] = (int)r;
			mmPushMatrix2(fp);

			if (r->num_meshes > 0)
			{
				fp[36] = ((int*)&r->left)[0];
				fp[37] = ((int*)&r->top)[0];

				//loc_82698
				for (j = 0; j < r->num_meshes; j++)
				{
					s5 = &static_objects[r->mesh[j].static_number];

					if ((r->mesh[j].Flags) & 1)
					{
						mmPushMatrix2(fp);
						mTranslateAbsXYZ2(r->mesh[j].x, r->mesh[j].y, r->mesh[j].z, fp);
						mRotY2(r->mesh[j].y_rot, fp);
						v1 = ((s5->flags) >> 2) << 10;
						at = TRZ;

						if (v1 && v1 < TRZ)
						{
							s5++;
						}//loc_82704

						v0 = mClipBoundingBox2((unsigned short*)&s5->x_minp, fp);

						if (v0)
						{
							a2 = (int*)fp[16];
							fp[18]++;
							((short*)a2)[0] = v0;
							((int*)a2)[1] = (int)&r->mesh[j];
							((short*)a2)[1] = 0;
							a2 += 3;
							fp[16] = (int)a2;
							v1 = s5->mesh_number;
							v0 = fp[40];

							stash_the_info(((int*)v0)[s5->mesh_number], fp);//((int*)meshes[s5->mesh_number])[0]
						}
						//loc_8274C
						mmPopMatrix2(fp);
					}//loc_82750
				}//loc_82750
			}
			//loc_8275C
			//s2 = r->item_number

			//v0 = r->item_number << 7
			if (r->item_number != -1)
			{
				item_number = r->item_number;

				//loc_8276C
				do
				{
					item = &((struct ITEM_INFO*)fp[24])[item_number];//&items[r->item_number]
					//v1 = r->item_number << 4	
					//v1 = item->object_number
					//s6 = objects
					object = &objects[item->object_number];

					//v0 = object->using_drawanimating_item
					//a1 = item->status

					if (object->using_drawanimating_item && item->status != 6)
					{
						//s2 = item
						///CalcAnimatingItem_ASM();
						//item = s2 //Maybe restore backup check if modified in func above
					}
					//loc_827BC
					item_number = item->next_item;
				} while (item->next_item != -1);

			}//loc_827CC
			mmPopMatrix2(fp);
		}
	}//loc_827DC
#if !JULY_VERSION
	DrawAllAnimatingItems_ASM(fp[18]);
#endif
}


void DrawAllAnimatingItems_ASM(int s4)//82900(<)
{
	struct ITEM_INFO* item;//$a3
	STASHEDOBJ* sobject;//$s0
	STASHEDDAT* sdat;//$s2
	int i;//$s4
	int j;//$s1
	int s6;

	if (s4 > 0)
	{
		sobject = &stashed_objects_list[0];
		mPushMatrix();
		sdat = &stashed_matrix_list[0];

		//loc_8291C
		for(i = 0; i < s4; i++, sobject++)
		{
			s6 = 0x80;

			if (sobject->numnodestodraw == 0)
			{
				item = stashed_objects_list[i].item;
				S_CalculateStaticMeshLight(item->floor, item->touch_bits, item->mesh_bits, item->current_anim_state);
			}
			else
			{
				//loc_82950
				s6 -= item->after_death;
				CalculateObjectLighting(item, sobject->frmptr0, sobject, sobject->numnodestodraw);
			}

			//loc_82964
			for (j = 0; j < 1; j++, sdat++)
			{
				R11 = ((int*)&sdat->matrix[0])[0] & 0xFFFF;
				R12 = (((int*)&sdat->matrix[0])[0] >> 16) & 0xFFFF;
				R13 = ((int*)&sdat->matrix[0])[1] & 0xFFFF;
				R21 = (((int*)&sdat->matrix[0])[1] >> 16) & 0xFFFF;
				R22 = ((int*)&sdat->matrix[0])[2] & 0xFFFF;
				R23 = (((int*)&sdat->matrix[0])[2] >> 16) & 0xFFFF;
				R31 = ((int*)&sdat->matrix[0])[3] & 0xFFFF;
				R32 = (((int*)&sdat->matrix[0])[3] >> 16) & 0xFFFF;
				R33 = ((int*)&sdat->matrix[0])[4] & 0xFFFF;
				TRX = ((int*)&sdat->matrix[0])[5];
				TRY = ((int*)&sdat->matrix[0])[6];
				TRZ = ((int*)&sdat->matrix[0])[7];

				if (s6 != 0x80)
				{
					//loc_829C4
					if (s6 >= 5)
					{
						//phd_PutPolygons_seethrough(sdat->mesh, s6);
					}
				}//loc_829C4
				else
				{
					phd_PutPolygons(sdat->mesh, sobject->clip);
				}
			}
		}
		//loc_829E8
		mPopMatrix();
	}
	//loc_829E8
	phd_left = 0;
	phd_right = 0x1FF;
	phd_top = 0;
	phd_bottom = 0xEF;
}