#include "../thirdparty/httplib.h"

#include "path.h"
#include "character.h"
#include "resources_manager.h"
#include "scene_manager.h"

#include <chrono>       //时间头文件
#include <string>       //字符串头文件
#include <vector>       //向量头文件
#include <thread>       //线程头文件
#include <codecvt>      //编码转换头文件
#include <fstream>      //文件头文件
#include <sstream>      //字符串流头文件


//全局变量
HWND hwnd;	//窗口句柄
int id_player = 0;					//玩家id
std::atomic<int> progress_1 = -1;	//玩家1进度	//atomic用于多线程
std::atomic<int> progress_2 = -1;	//玩家2进度
int num_total_char = 0;				//总字符数
Camera camera_ui, camera_scene; //scene相机跟随场景，ui相机固定
int win_width = 1080, win_height = 720; //窗口宽高

int idx_line = 0;						//当前行数
int idx_char = 0;						//当前字符数
std::string str_text;					//文本内容
std::vector<std::string> str_line_list;	//文本行列表


std::string str_address;			//服务器地址
httplib::Client* client = nullptr;	//HTTP客户端对象




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

//游戏初始化函数，登录到服务器
void login_to_server(HWND hwnd)
{
    client = new httplib::Client(str_address);  //连接到服务器
    client->set_keep_alive(true);               //设置长连接

    httplib::Result result = client->Post("/login");    //发送登录请求
    if (!result || result->status != 200)                //检查请求是否成功  
    {
        MessageBox(hwnd, _T("连接到服务器失败！"), _T("启动失败"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    id_player = std::stoi(result->body);  //获取当前玩家ID

    if (id_player <= 0)
    {
        MessageBox(hwnd, _T("比赛已经开始！"), _T("拒绝加入"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    (id_player == 1) ? (progress_1 = 0) : (progress_2 = 0);  //初始化进度条

    str_text = client->Post("/query_text")->body;  //获取文本

    std::stringstream str_stream(str_text);	//字符串流
    std::string str_line;                   //每行文本
    while (std::getline(str_stream, str_line))
    {
        str_line_list.push_back(str_line);  //将每行文本存入列表
        num_total_char += (int)str_line.length();  //统计总字符数
    }

    //开启线程，接收服务器消息
    std::thread([&]() 
        {
            while (true)
            {
                using namespace std::chrono;

                std::string route = (id_player == 1) ? "/update_1" : "/update_2";
                std::string body = std::to_string((id_player == 1) ? progress_1 : progress_2);
                httplib::Result result = client->Post(route, body, "text/plain");

                if (result && result->status == 200)    //HTTP状态码200表示请求成功
                {
                    int progress = std::stoi(result->body);
                    (id_player == 1) ? (progress_2 = progress) : (progress_1 = progress);
                }

                std::this_thread::sleep_for(nanoseconds(1000000000 / 10));
            }
        }).detach();        //分离线程
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
    login_to_server(hwnd);  //登录到服务器

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

        SceneManager::getInstance()->onDraw(camera_scene);

        FlushBatchDraw();   //绘制缓存区内容

        last_tick = frame_start;
        nanoseconds sleep_duration = frame_duration - (steady_clock::now() - frame_start);
        if (sleep_duration > nanoseconds(0))
            std::this_thread::sleep_for(sleep_duration);
    }

    EndBatchDraw();   //结束绘制缓存区内容

    return 0;
}