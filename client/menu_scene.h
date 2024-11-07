#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "scene.h"


class MenuScene : public Scene
{
public:
    MenuScene() = default;
    ~MenuScene() = default;


    //���ڽ��볡��ʱִ�еĲ���
    void onEnter()override
    {

    }

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta)override
    {


    }

    //���ڳ�������ʱִ�еĲ���
    void onRender(Camera& camera)override
    {
        settextcolor(RGB(195, 195, 195));
        outtextxy(15, 675, _T("����[Enter]��ʼ..."));
    }

    //���ڳ�������ʱִ�еĲ���
    void onInput(const ExMessage& msg) override
    {

        if (msg.vkcode == VK_RETURN)
        {
            SceneManager::getInstance()->switchTo(SceneManager::SceneType::GAME);
        }
    }

    //�����˳�����ʱִ�еĲ���
    void onExit() override
    {

    }
};







#endif // MENU_SCENE_H