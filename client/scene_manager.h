#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H


#include "scene.h"

//全局变量



class SceneManager
{

public:
    static SceneManager* getInstance();

    enum class SceneType
    {
        MENU = 1,
        SELECTOR,
        BACKGROUND,
        GAME
    };

private:
    //单例模式
    static SceneManager* manager;

    //保存场景的指针
    Scene* current_scene = nullptr;

    Scene* menu_scene;		// 菜单场景
    Scene* selector_scene;	// 选择场景
    Scene* game_scene;		// 游戏场景


public:
    SceneManager();
    ~SceneManager();


    //切换场景
    void switchTo(SceneType type);

    //用于场景更新时执行的操作
    void onUpdate(float delta);

    //用于场景绘制时执行的操作
    void onDraw(Camera& camera);

    //用于场景输入时执行的操作
    void onInput(const ExMessage& msg);


};



#endif // SCENE_MANAGER_H