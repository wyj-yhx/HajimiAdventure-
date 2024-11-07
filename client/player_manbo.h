#ifndef _PLAYER_MANBO_H
#define _PLAYER_MANBO_H

#include "character.h"
#include "animation.h"
#include "resources_manager.h"

class PlayerManbo : public Character
{

public:
	PlayerManbo()
	{

		anim_idle_up.set_loop(true);
		anim_idle_up.set_interval(0.1f);
		anim_idle_up.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_idle_up"));

		anim_idle_down.set_loop(true);
		anim_idle_down.set_interval(0.1f);
		anim_idle_down.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_idle_down"));

		anim_idle_left.set_loop(true);
		anim_idle_left.set_interval(0.1f);
		anim_idle_left.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_idle_left"));

		anim_idle_right.set_loop(true);
		anim_idle_right.set_interval(0.1f);
		anim_idle_right.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_idle_right"));

		anim_run_up.set_loop(true);
		anim_run_up.set_interval(0.1f);
		anim_run_up.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_run_up"));

		anim_run_down.set_loop(true);
		anim_run_down.set_interval(0.1f);
		anim_run_down.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_run_down"));

		anim_run_left.set_loop(true);
		anim_run_left.set_interval(0.1f);
		anim_run_left.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_run_left"));

		anim_run_right.set_loop(true);
		anim_run_right.set_interval(0.1f);
		anim_run_right.add_frame(ResourcesManager::getInstance()->findAtlas("manbo_run_right"));

	}

	~PlayerManbo() = default;

};






#endif // _PLAYER_MANBO_H