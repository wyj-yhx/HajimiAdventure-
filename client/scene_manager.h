#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H


#include "scene.h"

//ȫ�ֱ���



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
    //����ģʽ
    static SceneManager* manager;

    //���泡����ָ��
    Scene* current_scene = nullptr;

    Scene* menu_scene;		// �˵�����
    Scene* selector_scene;	// ѡ�񳡾�
    Scene* game_scene;		// ��Ϸ����


public:
    SceneManager();
    ~SceneManager();


    //�л�����
    void switchTo(SceneType type);

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta);

    //���ڳ�������ʱִ�еĲ���
    void onDraw(Camera& camera);

    //���ڳ�������ʱִ�еĲ���
    void onInput(const ExMessage& msg);


};



#endif // SCENE_MANAGER_H