#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "scene.h"


class MenuScene : public Scene
{
public:
    MenuScene() = default;
    ~MenuScene() = default;


    //用于进入场景时执行的操作
    void onEnter()override
    {

    }

    //用于场景更新时执行的操作
    void onUpdate(float delta)override
    {


    }

    //用于场景绘制时执行的操作
    void onRender(Camera& camera)override
    {
        settextcolor(RGB(195, 195, 195));
        outtextxy(15, 675, _T("输入[Enter]开始..."));
    }

    //用于场景输入时执行的操作
    void onInput(const ExMessage& msg) override
    {

        if (msg.vkcode == VK_RETURN)
        {
            SceneManager::getInstance()->switchTo(SceneManager::SceneType::GAME);
        }
    }

    //用于退出场景时执行的操作
    void onExit() override
    {

    }
};







#endif // MENU_SCENE_H