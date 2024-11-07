#ifndef SCENE_H
#define SCENE_H

#include "camera.h"

#include <graphics.h>

//Stage��ʾ��ǰ��Ϸ״̬
enum class Stage
{
    Waiting,	//�ȴ���Ҽ���
    Ready,		//׼����ʼ
    Racing		//����������
};

//����һ�������࣬���ڹ�����Ϸ�еĳ���
class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    virtual void onEnter() {}   //����һ�����麯�������ڽ��볡��ʱִ�еĲ���

    virtual void onUpdate(float delta) {}  //����һ�����麯�������ڳ�������ʱִ�еĲ���
    virtual void onRender(Camera& camera) {}    //����һ�����麯�������ڳ�������ʱִ�еĲ���
    virtual void onInput(const ExMessage& msg) {}   //����һ�����麯�������ڳ�������ʱִ�еĲ���

    virtual void onExit() {}    //����һ�����麯���������˳�����ʱִ�еĲ���

private:

};


#endif // !SCENE_H