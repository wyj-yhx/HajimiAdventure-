#ifndef SELECTOR_INTERNET_SCENE_H
#define SELECTOR_INTERNET_SCENE_H


#include "scene.h"
#include "resources_manager.h"
#include "scene_manager.h"
#include "wyjbutton.h"
//#include "vector2.h"


/*ȫ�ֱ���*/
extern HWND hwnd;	//���ھ��



class SelectorInternetScene : public Scene
{
public:
    SelectorInternetScene()
    {
        btn_back = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("����"));

        btn_back->setPosition({ (float)(50), (float)(25) });
    }

    ~SelectorInternetScene()
    {
        delete btn_back;
    }


    //���ڽ��볡��ʱִ�еĲ���
    void onEnter()override
    {
        btn_back->setClicked(false);
    }

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta)override
    {

    }

    //���ڳ�������ʱִ�еĲ���
    void onRender(Camera& camera_ui)override
    {
        setcolor(RGB(255,255,255));
        outtextxy((int)(getwidth() - textwidth(_T("�����ڴ�������"))) / 2, (int)getheight() / 2, _T("�����ڴ�������"));

        btn_back->onRender(camera_ui);
    }

    //���ڳ�������ʱִ�еĲ���
    void onInput(const ExMessage& msg) override
    {
        switch (msg.message)
        {
        case WM_LBUTTONDOWN:        //������
        {
            //�������
            if (btn_back->isClicked({ (float)msg.x, (float)msg.y }))
            {
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::MENU);
            }
        }
        break;
        case WM_LBUTTONUP:        //������
            btn_back->setClicked(false);
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
        //stop_audio(_T("bgm_menu"));
    }


private:
    WyjButton* btn_back;


};







#endif // SELECTOR_INTERNET_SCENE_H
