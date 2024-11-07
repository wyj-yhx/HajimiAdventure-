#ifndef PATH_H
#define PATH_H

#include "vector2.h"
#include <vector>


/*角色移动路径的封装*/
class Path
{
public:
	Path(const std::vector<Vector2>& point_list)
	{
		total_length = 0;

		this->point_list = point_list;

		for (size_t i = 1; i < point_list.size(); ++i)
		{
			float segment_len = (point_list[i] - point_list[i - 1]).length();
			segment_len_list.push_back(segment_len);
			total_length += segment_len;
		}
	}

	~Path() = default;

	//根据进度获取位置
	Vector2 get_position_at_progress(float progress) const
	{
		if (progress <= 0) return point_list.front();	//如果进度小于等于0，返回起点
        if (progress >= 1) return point_list.back();	//如果进度大于等于1，返回终点

		float target_distance = total_length * progress;	//目标距离

		float accumulated_len = 0.0f;		//已经走过的长度
		for (size_t i = 1; i < point_list.size(); ++i)
		{
			accumulated_len += segment_len_list[i - 1];
			if (accumulated_len >= target_distance)
			{
				float segment_progress = (target_distance - (accumulated_len - segment_len_list[i - 1])) / segment_len_list[i - 1];
				return point_list[i - 1] + (point_list[i] - point_list[i - 1]) * segment_progress;
			}
		}
        return point_list.back();
	}

private:
	float total_length;		//总长度
	std::vector<Vector2> point_list;	//点列表
	std::vector<float> segment_len_list;	//每一段的长度
};



#endif // PATH_H
