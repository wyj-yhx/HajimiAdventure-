#include "scene_manager.h"
#include "menu_scene.h"
#include "game_scene.h"
#include "selector_scene.h"

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
    game_scene = new GameScene();
    selector_scene = new SelectorScene();

    current_scene = menu_scene;
    switchTo(SceneType::GAME);
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
    case SceneType::GAME:
        current_scene = game_scene;
        break;
    case SceneType::SELECTOR:
        current_scene = selector_scene;
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

