////////////////////////////////////////////////////////////////////////////
//	Module		: weapon_collision.cpp
//	Created		: 12/10/2012
//	Modified 	: 12/10/2012
//	Author		: lost alpha (SkyLoader)
//	Description	: weapon HUD collision (based on weapon_bobbing.cpp)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "weapon_collision.h"
#include "actor.h"
#include "ActorCondition.h"
#include "Weapon.h"

CWeaponCollision::CWeaponCollision()
{
	Load();
}

CWeaponCollision::~CWeaponCollision()
{
}

void CWeaponCollision::Load()
{
	is_limping		= false;
	fReminderDist		= 0;
	fReminderNeedDist	= 0;
	bFirstUpdate		= true;
}

static const float SPEED_REMINDER = 0.75f;

void CWeaponCollision::CheckState()
{
	dwMState = Actor()->MovingState();
	is_limping = Actor()->conditions().IsLimping();
}

void CWeaponCollision::Update(Fmatrix &o, float range)
{
	CheckState();

	Fvector	xyz	= o.c;
	Fvector	dir;
	o.getHPB(dir.x,dir.y,dir.z);

	if (bFirstUpdate) {
		fReminderDist		= xyz.z;
		fReminderNeedDist	= xyz.z;
		bFirstUpdate		= false;
	}

	if (range < 0.8f/* && !m_bZoomMode*/) //-> TODO: Придумать, что делать с этим.
		fReminderNeedDist	= xyz.z - ((1 - range - 0.2) * 0.6);
	else
		fReminderNeedDist	= xyz.z;

	if (!fsimilar(fReminderDist, fReminderNeedDist)) {
		if (fReminderDist < fReminderNeedDist) {
			fReminderDist += SPEED_REMINDER * Device.fTimeDelta;
			if (fReminderDist > fReminderNeedDist)
				fReminderDist = fReminderNeedDist;
		} else if (fReminderDist > fReminderNeedDist) {
			fReminderDist -= SPEED_REMINDER * Device.fTimeDelta;
			if (fReminderDist < fReminderNeedDist)
				fReminderDist = fReminderNeedDist;
		}
	}

	if (!fsimilar(fReminderDist, xyz.z))
	{
		xyz.z 		= fReminderDist;
		o.c.set(xyz);
	}
}