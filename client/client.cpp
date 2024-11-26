#include "scene_manager.h"
#include "path.h"
#include "character.h"
#include "resources_manager.h"


#include <chrono>       //时间头文件
#include <string>       //字符串头文件
#include <vector>       //向量头文件
#include <thread>       //线程头文件
#include <codecvt>      //编码转换头文件
#include <fstream>      //文件头文件
#include <sstream>      //字符串流头文件


//全局变量
HWND hwnd;	//窗口句柄
int win_width = 1080, win_height = 720; //窗口宽高
Camera camera_ui, camera_scene; //scene相机跟随场景，ui相机固定

std::string player_name = "wyj";	//玩家名字
std::string counterpart_name;		//对方玩家名字
int id_player = 0;					//玩家id
std::atomic<int> progress_1 = -1;	//玩家1进度	//atomic用于多线程
std::atomic<int> progress_2 = -1;	//玩家2进度

int num_total_char = 0;				    //总字符数
int idx_line = 0;						//当前行数
int idx_char = 0;						//当前字符数
std::string str_text;					//文本内容
std::vector<std::string> str_line_list;	//文本行列表

std::string str_address;			//服务器地址





//角色移动路径对象 c 
Path path = Path(
	{
		{842,842},{1322,842},{1322,442},
		{2762,442},{2762,842},{3162,842},
		{3162,1722},{2122,1722},{2122,1562},
		{842,1562},{842,842}
	});

void load_config(HWND hwnd)
{

    //加载服务器地址文件
    std::ifstream file("config.cfg");

    if (!file.good())
    {
        MessageBox(hwnd, _T("服务器地址文件读取失败！"), _T("错误"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    std::stringstream str_stream;
    str_stream << file.rdbuf();     //读取文件内容到字符串流
    str_address = str_stream.str();

    file.close();
}


//游戏主循环
int main(int argc, char** argv)
{
    //////////////处理数据初始化////////////////
    using namespace std::chrono;

    hwnd = initgraph(win_width, win_height/*, EW_SHOWCONSOLE*/);  //初始化窗口,EW_SHOWCONSOLE显示控制台
    SetWindowText(hwnd, _T("哈基米大冒险！"));  //设置窗口标题
    settextstyle(28, 0, _T("IPix"));          //设置字体样式

    setbkmode(TRANSPARENT);  //设置背景透明


    ResourcesManager::getInstance()->loadResources();  //加载资源
    load_config(hwnd);      //加载网络地址
    

    ExMessage msg;          //接收输入消息


    camera_ui.set_size({ (float)win_width, (float)win_height });
    camera_scene.set_size({ (float)win_width, (float)win_height });



    const nanoseconds frame_duration(1000000000 / 144);     //每帧持续时间,nanoseconds表示纳秒
    steady_clock::time_point last_tick = steady_clock::now();  //上一帧时间点

    BeginBatchDraw();  //开始批量绘制

    while (true)
    {
        //////////////处理玩家输入////////////////
        while (peekmessage(&msg/*, EM_MOUSE | EM_KEY*/))
        {
            SceneManager::getInstance()->onInput(msg);
        }

        //////////////处理游戏更新////////////////
        steady_clock::time_point frame_start = steady_clock::now();
        duration<float> delta = duration<float>(frame_start - last_tick);

        SceneManager::getInstance()->onUpdate(delta.count());

        //////////////处理画面绘制////////////////
        setbkcolor(RGB(0, 0, 0));
        cleardevice();

        SceneManager::getInstance()->onDraw(camera_ui);

        FlushBatchDraw();   //绘制缓存区内容

        last_tick = frame_start;
        nanoseconds sleep_duration = frame_duration - (steady_clock::now() - frame_start);
        if (sleep_duration > nanoseconds(0))
            std::this_thread::sleep_for(sleep_duration);
    }

    EndBatchDraw();   //结束绘制缓存区内容

    return 0;
}