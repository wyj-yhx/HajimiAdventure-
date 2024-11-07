#ifndef SCENE_H
#define SCENE_H

#include "camera.h"

#include <graphics.h>

//Stage表示当前游戏状态
enum class Stage
{
    Waiting,	//等待玩家加入
    Ready,		//准备开始
    Racing		//比赛进行中
};

//定义一个场景类，用于管理游戏中的场景
class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    virtual void onEnter() {}   //定义一个纯虚函数，用于进入场景时执行的操作

    virtual void onUpdate(float delta) {}  //定义一个纯虚函数，用于场景更新时执行的操作
    virtual void onRender(Camera& camera) {}    //定义一个纯虚函数，用于场景绘制时执行的操作
    virtual void onInput(const ExMessage& msg) {}   //定义一个纯虚函数，用于场景输入时执行的操作

    virtual void onExit() {}    //定义一个纯虚函数，用于退出场景时执行的操作

private:

};


#endif // !SCENE_H