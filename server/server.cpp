#include "../thirdparty/httplib.h"

#include <mutex>
#include <string>
#include <fstream>


//ȫ�ֱ�������
std::string g_str;
std::mutex g_mutex;	//ȫ�ֻ�����

std::string str_text;	//�ı�����

int progress_1 = -1;	//���1������
int progress_2 = -1;	//���2������


// �ص�����
void login(const httplib::Request& req, httplib::Response& res)
{
	std::lock_guard<std::mutex> lock(g_mutex);		//������

	if (progress_1 >= 0 && progress_2 >= 0)
	{
		res.set_content("-1", "text/plain");
        return;
	}

	res.set_content(progress_1 >= 0 ? "2" : "1", "text/plain");
	(progress_1 >= 0) ? (progress_2 = 0) : (progress_1 = 0);


	/*
	
    // ����ͻ�����������
	g_str = req.body;

	// ����ͻ�����Ϣ
	std::cout << "Hello From Client!" << std::endl;

	// ������Ӧ����
	res.set_content("Hello From Server!", "text/plain");

	*/
}


// �������˴���
int main(int argc, char** argv)
{
	std::ifstream file("text.txt");		//���ı��ļ�

	if (!file.good())
	{
		MessageBox(nullptr, L"�޷����ı��ļ� text.txt", L"����ʧ��", MB_OK | MB_ICONERROR);
		return -1;
	}

	std::stringstream str_stream;	//�����ַ�����
	str_stream << file.rdbuf();		//���ļ����ݶ�������
	str_text = str_stream.str();	//�����е�����ת��Ϊ�ַ���

	file.close();	//�ر��ļ�

    // ����httplib::Server����
    httplib::Server server;

	// ע��POST��������
	server.Post("/login", login);

	server.Post("/query_text", [&](const httplib::Request& req, httplib::Response& res)
		{
			res.set_content(str_text, "text/plain");
		});

	server.Post("/update_1", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::lock_guard<std::mutex> lock(g_mutex);
			progress_1 = std::stoi(req.body);	//���ַ���ת��Ϊ����
			res.set_content(std::to_string(progress_2), "text/plain");
		});

	server.Post("/update_2", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::lock_guard<std::mutex> lock(g_mutex);
			progress_2 = std::stoi(req.body);	//���ַ���ת��Ϊ����
			res.set_content(std::to_string(progress_1),"text/plain");
		});

	// ����ָ��IP�Ͷ˿�
	server.listen("0.0.0.0", 25565);	//��������IP��ַ��25565�˿�

	return 0;
	/*

	// ����httplib::Server����
	httplib::Server server;

	// ע��POST��������
	server.Post("/hello", onHello);

		// ����ָ��IP�Ͷ˿�
	server.listen("localhost",25565);

	return 0;

	*/
}
