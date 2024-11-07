#include "player_hajimi.h"
#include "player_manbo.h"
#include "character_manager.h"
#include "camera.h"

CharacterManager* CharacterManager::manager = nullptr;

//������ɫ�ĵ���ģʽ
CharacterManager* CharacterManager::getInstance()
{
    if (!manager)
        manager = new CharacterManager();
    return manager;
}

////�������
//void CharacterManager::onInput(const ExMessage& msg)
//{
//    player->onInput(msg);
//}

//���½�ɫ
void CharacterManager::onUpdate(float delta)
{

    player_1->on_update(delta);
    player_2->on_update(delta);

}

//��Ⱦ��ɫ
void CharacterManager::onRender(Camera camera)
{
    //�������
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