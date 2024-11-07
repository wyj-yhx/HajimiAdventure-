#include "player_hajimi.h"
#include "player_manbo.h"
#include "character_manager.h"
#include "camera.h"

CharacterManager* CharacterManager::manager = nullptr;

//创建角色的单例模式
CharacterManager* CharacterManager::getInstance()
{
    if (!manager)
        manager = new CharacterManager();
    return manager;
}

////输入控制
//void CharacterManager::onInput(const ExMessage& msg)
//{
//    player->onInput(msg);
//}

//更新角色
void CharacterManager::onUpdate(float delta)
{

    player_1->on_update(delta);
    player_2->on_update(delta);

}

//渲染角色
void CharacterManager::onRender(Camera camera)
{
    //绘制玩家
    if (player_1->get_position().y > player_2->get_position().y)
    {
        player_2->on_render(camera);
        player_1->on_render(camera);
    }
    else
    {
        player_1->on_render(camera);
        player_2->on_render(camera);
    }

}

CharacterManager::CharacterManager()
{
    player_1 = new PlayerHajimi();
    player_2 = new PlayerManbo();
}

CharacterManager::~CharacterManager()
{
    delete player_1;
    delete player_2;
}