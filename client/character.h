#ifndef CHARACTER_H
#define CHARACTER_H

#include "animation.h"

class Character
{
public:
	enum class Facing
	{
		Up, Down, Left, Right
	};

public:
	Character() {}

	~Character() {}

	void on_update(float delta)
	{
		//判断当前位置与目标位置是否接近
		if (!position.approx(pos_target))
			velocity = (pos_target - position).normalize() * SPEED_RUN;
		else
			velocity = Vector2(0, 0);

		//判断与目标的距离是否小于速度
		if ((pos_target - position).length() <= (velocity * delta).length())
			position = pos_target;
		else
			position += velocity * delta;

		if (velocity.approx(Vector2(0, 0)))
		{
			switch (facing)
			{
			case Character::Facing::Up:	current_anim = &anim_idle_up;
				break;
			case Character::Facing::Down:	current_anim = &anim_idle_down;
				break;
			case Character::Facing::Left:	current_anim = &anim_idle_left;
				break;
			case Character::Facing::Right:	current_anim = &anim_idle_right;
				break;
			}
		}
		else
		{
			//abs表示,取绝对值
			if (abs(velocity.y) >= 0.0001f)
				facing = (velocity.y > 0) ? Character::Facing::Down : Character::Facing::Up;
			if (abs(velocity.x) >= 0.0001f)
				facing = (velocity.x > 0) ? Character::Facing::Right : Character::Facing::Left;

			switch (facing)
			{
			case Character::Facing::Up:	current_anim = &anim_run_up;	break;
			case Character::Facing::Down:	current_anim = &anim_run_down;	break;
			case Character::Facing::Left:	current_anim = &anim_run_left;	break;
			case Character::Facing::Right: current_anim = &anim_run_right; break;
			}
		}

		if (!current_anim) return;
		current_anim->set_position(position);
		current_anim->on_update(delta);

	}

	void on_render(const Camera& camera)
	{
		if (!current_anim) return;
		current_anim->on_render(camera);
	}

	void set_position(const Vector2& position)
	{
		this->position = position;
	}

	const Vector2& get_position() const
	{
		return position;
	}

	void set_target(const Vector2& pos_target)
	{
		this->pos_target = pos_target;
	}

protected:
	const float SPEED_RUN = 100.0f;

protected:
	Vector2 position;	//当前位置
	Vector2 velocity;	//速度
	Vector2 pos_target;	//目标位置

	Animation anim_idle_up;
	Animation anim_idle_down;
	Animation anim_idle_left;
	Animation anim_idle_right;
	Animation anim_run_up;
	Animation anim_run_down;
	Animation anim_run_left;
	Animation anim_run_right;

	Animation* current_anim = nullptr;

	Facing facing = Facing::Down;
};






#endif // _CHARACTER_H