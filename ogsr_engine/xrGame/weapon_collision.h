////////////////////////////////////////////////////////////////////////////
//	Файл		: weapon_collision.h
//	Создан		: 12/10/2012
//	Изменен 	: 15.07.21
//	Автор		: lost alpha (SkyLoader)
//	Описание	: Коллизия худа оружия
//	
//	Правки и адаптация под player_hud
//			i-love-kfc
////////////////////////////////////////////////////////////////////////////
#pragma once

class CWeaponCollision
{
public:
		CWeaponCollision();
		virtual ~CWeaponCollision();
		void Load();
		void Update(Fmatrix &o, float range/*, bool is_zoom*/);
		void CheckState();
private:
		float	fReminderDist;
		float	fReminderNeedDist;
		bool	bFirstUpdate;
		u32	dwMState;
		bool	is_limping;
};