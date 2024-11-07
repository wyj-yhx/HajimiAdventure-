#ifndef _ATLAS_H
#define _ATLAS_H

#include <vector>	//C++标准库的头文件
#include <graphics.h>	//easyX的头文件

class Atlas
{
public:
	Atlas() = default;
	~Atlas() = default;

	void load(LPCTSTR path_template, int num)
	{
		img_list.clear();
		img_list.resize(num);		//初始化大小

		TCHAR path_file[256];
		for (int i = 0; i < num; ++i)
		{
			_stprintf_s(path_file, path_template, i + 1);
			loadimage(&img_list[i], path_file);
		}
	}

	void clear()
	{
		img_list.clear();
	}

	int get_size() const
	{
		return (int)img_list.size();
	}

	IMAGE* get_image(int idx)
	{
		if (idx < 0 || idx >= img_list.size())
			return nullptr;

		return &img_list[idx];

	}

	void add_image(const IMAGE& img)
	{
		img_list.push_back(img);
	}

private:
	std::vector<IMAGE> img_list;

};









#endif // !_ATLAS_H
