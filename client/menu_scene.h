#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "scene.h"
#include "wyjbutton.h"
#include "resources_manager.h"


class MenuScene : public Scene
{
public:
    MenuScene()
    {
        button_single = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up"), *ResourcesManager::getInstance()->findImage("ui_button_down"), _T("单人模式"));
        button_lan = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up"), *ResourcesManager::getInstance()->findImage("ui_button_down"), _T("局域网模式"));
        button_internet = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up"), *ResourcesManager::getInstance()->findImage("ui_button_down"), _T("互联网模式"));
    }
    ~MenuScene()
    {
        delete button_single;
        delete button_lan;
        delete button_internet;
    }


    //用于进入场景时执行的操作
    void onEnter()override
    {
        button_single->setPosition({ (float)(getwidth() / 2), (float)(getheight() - 300)});
        button_lan->setPosition({ (float)(getwidth() / 2), (float)(getheight() - 225)});
        button_internet->setPosition({ (float)(getwidth() / 2), (float)(getheight() - 150)});

        play_audio(_T("bgm_menu"), true);

        button_single->setClicked(false);
        button_lan->setClicked(false);
        button_internet->setClicked(false);
    }

    //用于场景更新时执行的操作
    void onUpdate(float delta)override
    {

    }

    //用于场景绘制时执行的操作
    void onRender(Camera& camera_ui)override
    {
        //显示背景菜单图
        putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("background_menu"));

        settextcolor(RGB(0, 0, 0));
        outtextxy(15, 675, _T("【选择游戏模式】"));

        button_single->onRender(camera_ui);
        button_lan->onRender(camera_ui);
        button_internet->onRender(camera_ui);
    }

    //用于场景输入时执行的操作
    void onInput(const ExMessage& msg) override
    {
        switch (msg.message)
        {
        case WM_KEYDOWN:
            switch (msg.vkcode)
            {
            case VK_RETURN:
                break;
            }
            break;
        case WM_KEYUP:
            switch (msg.vkcode)
            {
            case VK_RETURN:
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_LAN);
                break;
            }
            break;
        case WM_LBUTTONDOWN:        //鼠标左键
            if(button_single->isClicked({(float)msg.x, (float)msg.y})) SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_SINGLE);
            if(button_lan->isClicked({ (float)msg.x, (float)msg.y })) SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_LAN);
            if(button_internet->isClicked({ (float)msg.x, (float)msg.y }))SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_INTERNET);
            break;
        case WM_LBUTTONUP:        //鼠标左键
            
            break;
        case WM_RBUTTONDOWN:        //鼠标右键
            break;

        default:
            break;
        }
    }

    //用于退出场景时执行的操作
    void onExit() override
    {
        stop_audio(_T("bgm_menu"));
    }

private:
    WyjButton* button_single;   //单人模式按钮
    WyjButton* button_lan;    //局域网模式按钮
    WyjButton* button_internet;    //多人模式按钮


};







#endif // MENU_SCENE_H