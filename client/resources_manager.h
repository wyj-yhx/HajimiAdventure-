#ifndef RESOURCES_MANAGER_H
#define RESOURCES_MANAGER_H

#include <string>				//字符串
#include <graphics.h>			//图形库
#include <unordered_map>		//哈希表

#include "atlas.h"			//图集类
#include "util.h"			//工具类

//资源管理器类
class ResourcesManager
{
public:
	//获取单例，可以在不创建对象的情况下使用，获取到对象
	static ResourcesManager* getInstance();

	//加载资源
	void loadResources();

	//根据名字找到图集
	Atlas* findAtlas(const std::string& id) const;
	//根据名字找到图片
	IMAGE* findImage(const std::string& id) const;

private:
	static ResourcesManager* manager;		//单例模式

	std::unordered_map<std::string, Atlas*> atlas_pool;		//图集哈希表
	std::unordered_map<std::string, IMAGE*> image_pool;		//图片哈希表
private:
	ResourcesManager();
	~ResourcesManager();

};


#endif // !RESOURCES_MANAGER_H