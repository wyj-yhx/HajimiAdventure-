#include "../thirdparty/httplib.h"

#include <mutex>
#include <string>
#include <fstream>


//全局变量定义
std::string g_str;
std::mutex g_mutex;	//全局互斥锁

std::string str_text;	//文本内容

int progress_1 = -1;	//玩家1进度条
int progress_2 = -1;	//玩家2进度条


// 回调函数
void login(const httplib::Request& req, httplib::Response& res)
{
	std::lock_guard<std::mutex> lock(g_mutex);		//互斥锁

	if (progress_1 >= 0 && progress_2 >= 0)
	{
		res.set_content("-1", "text/plain");
        return;
	}

	res.set_content(progress_1 >= 0 ? "2" : "1", "text/plain");
	(progress_1 >= 0) ? (progress_2 = 0) : (progress_1 = 0);


	/*
	
    // 输出客户端请求内容
	g_str = req.body;

	// 输出客户端消息
	std::cout << "Hello From Client!" << std::endl;

	// 设置响应内容
	res.set_content("Hello From Server!", "text/plain");

	*/
}


// 服务器端代码
int main(int argc, char** argv)
{
	std::ifstream file("text.txt");		//打开文本文件

	if (!file.good())
	{
		MessageBox(nullptr, L"无法打开文本文件 text.txt", L"启动失败", MB_OK | MB_ICONERROR);
		return -1;
	}

	std::stringstream str_stream;	//创建字符串流
	str_stream << file.rdbuf();		//将文件内容读入流中
	str_text = str_stream.str();	//将流中的内容转换为字符串

	file.close();	//关闭文件

    // 创建httplib::Server对象
    httplib::Server server;

	// 注册POST请求处理函数
	server.Post("/login", login);

	server.Post("/query_text", [&](const httplib::Request& req, httplib::Response& res)
		{
			res.set_content(str_text, "text/plain");
		});

	server.Post("/update_1", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::lock_guard<std::mutex> lock(g_mutex);
			progress_1 = std::stoi(req.body);	//将字符串转换为整数
			res.set_content(std::to_string(progress_2), "text/plain");
		});

	server.Post("/update_2", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::lock_guard<std::mutex> lock(g_mutex);
			progress_2 = std::stoi(req.body);	//将字符串转换为整数
			res.set_content(std::to_string(progress_1),"text/plain");
		});

	// 监听指定IP和端口
	server.listen("0.0.0.0", 25565);	//监听所有IP地址的25565端口

	return 0;
	/*

	// 创建httplib::Server对象
	httplib::Server server;

	// 注册POST请求处理函数
	server.Post("/hello", onHello);

		// 监听指定IP和端口
	server.listen("localhost",25565);

	return 0;

	*/
}
