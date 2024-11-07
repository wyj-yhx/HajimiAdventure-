#ifndef _TIMER_H_
#define _TIMER_H_

#include <functional>

class Timer
{
public:
	Timer() = default;
	~Timer() = default;

	void restart()
	{
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(float val)
	{
		wait_time = val;
	}

	void set_one_shot(bool flag)
	{
		one_shot = flag;
	}

	void set_on_timeout(std::function<void()> on_timeout)
	{
		this->on_timeout = on_timeout;
	}

	//��ͣ��ʱ��
	void pause()
	{
		paused = true;
	}

	void resume()
	{
		paused = false;
	}

	void on_update(float delta)
	{
		if (paused) return;

		pass_time += delta;
		if (pass_time >= wait_time)
		{
			bool can_shot = (!one_shot || (one_shot && !shotted));
            if (can_shot && on_timeout)
				on_timeout();
			pass_time -= wait_time;
		}
	}

private:
	float pass_time = 0;		//�Ѿ���ȥ��ʱ��
	float wait_time = 0;		//�ȴ�ʱ��
	bool paused = false;		//�Ƿ���ͣ
	bool shotted = false;		//�Ƿ��Ѿ�����
	bool one_shot = false;		//�Ƿ���һ���Զ�ʱ��
	std::function<void()> on_timeout;
};


















#endif /* _TIMER_H_ */
