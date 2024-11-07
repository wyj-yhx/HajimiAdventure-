#ifndef PATH_H
#define PATH_H

#include "vector2.h"
#include <vector>


/*��ɫ�ƶ�·���ķ�װ*/
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

	//���ݽ��Ȼ�ȡλ��
	Vector2 get_position_at_progress(float progress) const
	{
		if (progress <= 0) return point_list.front();	//�������С�ڵ���0���������
        if (progress >= 1) return point_list.back();	//������ȴ��ڵ���1�������յ�

		float target_distance = total_length * progress;	//Ŀ�����

		float accumulated_len = 0.0f;		//�Ѿ��߹��ĳ���
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
	float total_length;		//�ܳ���
	std::vector<Vector2> point_list;	//���б�
	std::vector<float> segment_len_list;	//ÿһ�εĳ���
};



#endif // PATH_H
