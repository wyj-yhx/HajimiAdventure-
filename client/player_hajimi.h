#ifndef _PLAYER_HAJIMI_H
#define _PLAYER_HAJIMI_H

#include "character.h"
#include "animation.h"
#include "resources_manager.h"

class PlayerHajimi: public Character
{

public:
	PlayerHajimi()
	{

		anim_idle_up.set_loop(true);
		anim_idle_up.set_interval(0.1f);
		anim_idle_up.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_idle_up"));

		anim_idle_down.set_loop(true);
		anim_idle_down.set_interval(0.1f);
		anim_idle_down.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_idle_down"));

		anim_idle_left.set_loop(true);
		anim_idle_left.set_interval(0.1f);
		anim_idle_left.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_idle_left"));

		anim_idle_right.set_loop(true);
		anim_idle_right.set_interval(0.1f);
		anim_idle_right.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_idle_right"));

		anim_run_up.set_loop(true);
		anim_run_up.set_interval(0.1f);
		anim_run_up.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_run_up"));

		anim_run_down.set_loop(true);
		anim_run_down.set_interval(0.1f);
		anim_run_down.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_run_down"));

		anim_run_left.set_loop(true);
		anim_run_left.set_interval(0.1f);
		anim_run_left.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_run_left"));

		anim_run_right.set_loop(true);
		anim_run_right.set_interval(0.1f);
		anim_run_right.add_frame(ResourcesManager::getInstance()->findAtlas("hajimi_run_right"));

	}

	~PlayerHajimi() = default;

};






#endif // _PLAYER_HAJIMI_H