#ifndef CHARACTER_MANAGER_H_
#define CHARACTER_MANAGER_H_

#include "character.h"


//½ÇÉ«¹ÜÀíÆ÷
class CharacterManager
{
public:
	static CharacterManager* getInstance();

	Character* getPlayer_1() { return player_1; }
    Character* getPlayer_2() { return player_2; }

	//void onInput(const ExMessage& msg);
	void onUpdate(float delta);
	void onRender(Camera camera);

private:
	static CharacterManager* manager;

	Character* player_1 = nullptr;
	Character* player_2 = nullptr;

private:
	CharacterManager();
	~CharacterManager();
};



#endif /* CHARACTER_MANAGER_H_ */
