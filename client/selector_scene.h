#pragma once
#ifndef SELECTOR_SCENE_H
#define SELECTOR_SCENE_H

#include "scene.h"


class SelectorScene : public Scene
{
public:
    SelectorScene() = default;
    ~SelectorScene() = default;


    //���ڽ��볡��ʱִ�еĲ���
    void onEnter()override
    {

    }

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta)override
    {


    }

    //���ڳ�������ʱִ�еĲ���
    void onRender(Camera& camera)override
    {

    }

    //���ڳ�������ʱִ�еĲ���
    void onInput(const ExMessage& msg) override
    {

    }

    //�����˳�����ʱִ�еĲ���
    void onExit() override
    {

    }
};







#endif // SELECTOR_SCENE_H