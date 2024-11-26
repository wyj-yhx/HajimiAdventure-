#ifndef SELECTOR_INTERNET_SCENE_H
#define SELECTOR_INTERNET_SCENE_H


#include "scene.h"
#include "resources_manager.h"
#include "scene_manager.h"
#include "wyjbutton.h"
//#include "vector2.h"


/*全局变量*/
extern HWND hwnd;	//窗口句柄



class SelectorInternetScene : public Scene
{
public:
    SelectorInternetScene()
    {
        btn_back = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("返回"));

        btn_back->setPosition({ (float)(50), (float)(25) });
    }

    ~SelectorInternetScene()
    {
        delete btn_back;
    }


    //用于进入场景时执行的操作
    void onEnter()override
    {
        btn_back->setClicked(false);
    }

    //用于场景更新时执行的操作
    void onUpdate(float delta)override
    {

    }

    //用于场景绘制时执行的操作
    void onRender(Camera& camera_ui)override
    {
        setcolor(RGB(255,255,255));
        outtextxy((int)(getwidth() - textwidth(_T("敬请期待。。。"))) / 2, (int)getheight() / 2, _T("敬请期待。。。"));

        btn_back->onRender(camera_ui);
    }

    //用于场景输入时执行的操作
    void onInput(const ExMessage& msg) override
    {
        switch (msg.message)
        {
        case WM_LBUTTONDOWN:        //鼠标左键
        {
            //点击返回
            if (btn_back->isClicked({ (float)msg.x, (float)msg.y }))
            {
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::MENU);
            }
        }
        break;
        case WM_LBUTTONUP:        //鼠标左键
            btn_back->setClicked(false);
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
        //stop_audio(_T("bgm_menu"));
    }


private:
    WyjButton* btn_back;


};







#endif // SELECTOR_INTERNET_SCENE_H
