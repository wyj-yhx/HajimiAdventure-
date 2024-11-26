#include "selector_lan_scene.h"
#include "selector_single_scene.h"
#include "selector_internet_scene.h"
#include "scene_manager.h"
#include "menu_scene.h"
#include "game_single_scene.h"
#include "game_lan_scene.h"


SceneManager* SceneManager::manager = nullptr;

SceneManager* SceneManager::getInstance()
{
    if (manager == nullptr)
    {
        manager = new SceneManager();
    }
    return manager;
}


SceneManager::SceneManager()
{
    menu_scene = new MenuScene();
    game_single_scene = new GameSingleScene();
    selector_lan_scene = new SelectorLanScene();
    game_lan_scene = new GameLanScene();
    selector_single_scene = new SelectorSingleScene();
    selector_internet_scene = new SelectorInternetScene();

    current_scene = menu_scene;
    switchTo(SceneType::MENU);
}

SceneManager::~SceneManager() = default;


//切换场景
void SceneManager::switchTo(SceneType type)
{
    current_scene->onExit();
    switch (type)
    {
    case SceneType::MENU:
        current_scene = menu_scene;
        break;
    case SceneType::GAME_SINGLE:
        current_scene = game_single_scene;
        break;
    case SceneType::SELECTOR_LAN:
        current_scene = selector_lan_scene;
        break;
    case SceneType::GAME_LAN:
        current_scene = game_lan_scene;
        break;
    case SceneType::SELECTOR_SINGLE:
        current_scene = selector_single_scene;
        break;
    case SceneType::SELECTOR_INTERNET:
        current_scene = selector_internet_scene;
        break;
    default:
        break;
    }
    current_scene->onEnter();
}

//用于场景更新时执行的操作
void SceneManager::onUpdate(float delta)
{
    current_scene->onUpdate(delta);
}

//用于场景绘制时执行的操作
void SceneManager::onDraw(Camera& camera)
{
    current_scene->onRender(camera);
}

//用于场景输入时执行的操作
void SceneManager::onInput(const ExMessage& msg)
{
    current_scene->onInput(msg);
}

