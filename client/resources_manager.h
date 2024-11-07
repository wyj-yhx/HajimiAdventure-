#ifndef RESOURCES_MANAGER_H
#define RESOURCES_MANAGER_H

#include <string>				//�ַ���
#include <graphics.h>			//ͼ�ο�
#include <unordered_map>		//��ϣ��

#include "atlas.h"			//ͼ����
#include "util.h"			//������

//��Դ��������
class ResourcesManager
{
public:
	//��ȡ�����������ڲ���������������ʹ�ã���ȡ������
	static ResourcesManager* getInstance();

	//������Դ
	void loadResources();

	//���������ҵ�ͼ��
	Atlas* findAtlas(const std::string& id) const;
	//���������ҵ�ͼƬ
	IMAGE* findImage(const std::string& id) const;

private:
	static ResourcesManager* manager;		//����ģʽ

	std::unordered_map<std::string, Atlas*> atlas_pool;		//ͼ����ϣ��
	std::unordered_map<std::string, IMAGE*> image_pool;		//ͼƬ��ϣ��
private:
	ResourcesManager();
	~ResourcesManager();

};


#endif // !RESOURCES_MANAGER_H