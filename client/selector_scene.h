#pragma once
#ifndef SELECTOR_SCENE_H
#define SELECTOR_SCENE_H

#include "scene.h"


class SelectorScene : public Scene
{
public:
    SelectorScene() = default;
    ~SelectorScene() = default;


    //用于进入场景时执行的操作
    void onEnter()override
    {

    }

    //用于场景更新时执行的操作
    void onUpdate(float delta)override
    {


    }

    //用于场景绘制时执行的操作
    void onRender(Camera& camera)override
    {

    }

    //用于场景输入时执行的操作
    void onInput(const ExMessage& msg) override
    {

    }

    //用于退出场景时执行的操作
    void onExit() override
    {

    }
};







#endif // SELECTOR_SCENE_H