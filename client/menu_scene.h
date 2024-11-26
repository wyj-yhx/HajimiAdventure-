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
        button_single = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up"), *ResourcesManager::getInstance()->findImage("ui_button_down"), _T("����ģʽ"));
        button_lan = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up"), *ResourcesManager::getInstance()->findImage("ui_button_down"), _T("������ģʽ"));
        button_internet = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up"), *ResourcesManager::getInstance()->findImage("ui_button_down"), _T("������ģʽ"));
    }
    ~MenuScene()
    {
        delete button_single;
        delete button_lan;
        delete button_internet;
    }


    //���ڽ��볡��ʱִ�еĲ���
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

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta)override
    {

    }

    //���ڳ�������ʱִ�еĲ���
    void onRender(Camera& camera_ui)override
    {
        //��ʾ�����˵�ͼ
        putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("background_menu"));

        settextcolor(RGB(0, 0, 0));
        outtextxy(15, 675, _T("��ѡ����Ϸģʽ��"));

        button_single->onRender(camera_ui);
        button_lan->onRender(camera_ui);
        button_internet->onRender(camera_ui);
    }

    //���ڳ�������ʱִ�еĲ���
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
        case WM_LBUTTONDOWN:        //������
            if(button_single->isClicked({(float)msg.x, (float)msg.y})) SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_SINGLE);
            if(button_lan->isClicked({ (float)msg.x, (float)msg.y })) SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_LAN);
            if(button_internet->isClicked({ (float)msg.x, (float)msg.y }))SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_INTERNET);
            break;
        case WM_LBUTTONUP:        //������
            
            break;
        case WM_RBUTTONDOWN:        //����Ҽ�
            break;

        default:
            break;
        }
    }

    //�����˳�����ʱִ�еĲ���
    void onExit() override
    {
        stop_audio(_T("bgm_menu"));
    }

private:
    WyjButton* button_single;   //����ģʽ��ť
    WyjButton* button_lan;    //������ģʽ��ť
    WyjButton* button_internet;    //����ģʽ��ť


};







#endif // MENU_SCENE_H