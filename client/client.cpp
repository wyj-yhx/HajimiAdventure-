#include "../thirdparty/httplib.h"

#include "path.h"
#include "character.h"
#include "resources_manager.h"
#include "scene_manager.h"

#include <chrono>       //ʱ��ͷ�ļ�
#include <string>       //�ַ���ͷ�ļ�
#include <vector>       //����ͷ�ļ�
#include <thread>       //�߳�ͷ�ļ�
#include <codecvt>      //����ת��ͷ�ļ�
#include <fstream>      //�ļ�ͷ�ļ�
#include <sstream>      //�ַ�����ͷ�ļ�


//ȫ�ֱ���
HWND hwnd;	//���ھ��
int id_player = 0;					//���id
std::atomic<int> progress_1 = -1;	//���1����	//atomic���ڶ��߳�
std::atomic<int> progress_2 = -1;	//���2����
int num_total_char = 0;				//���ַ���
Camera camera_ui, camera_scene; //scene������泡����ui����̶�
int win_width = 1080, win_height = 720; //���ڿ��

int idx_line = 0;						//��ǰ����
int idx_char = 0;						//��ǰ�ַ���
std::string str_text;					//�ı�����
std::vector<std::string> str_line_list;	//�ı����б�


std::string str_address;			//��������ַ
httplib::Client* client = nullptr;	//HTTP�ͻ��˶���




//��ɫ�ƶ�·������ c 
Path path = Path(
	{
		{842,842},{1322,842},{1322,442},
		{2762,442},{2762,842},{3162,842},
		{3162,1722},{2122,1722},{2122,1562},
		{842,1562},{842,842}
	});

void load_config(HWND hwnd)
{

    //���ط�������ַ�ļ�
    std::ifstream file("config.cfg");

    if (!file.good())
    {
        MessageBox(hwnd, _T("��������ַ�ļ���ȡʧ�ܣ�"), _T("����"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    std::stringstream str_stream;
    str_stream << file.rdbuf();     //��ȡ�ļ����ݵ��ַ�����
    str_address = str_stream.str();

    file.close();
}

//��Ϸ��ʼ����������¼��������
void login_to_server(HWND hwnd)
{
    client = new httplib::Client(str_address);  //���ӵ�������
    client->set_keep_alive(true);               //���ó�����

    httplib::Result result = client->Post("/login");    //���͵�¼����
    if (!result || result->status != 200)                //��������Ƿ�ɹ�  
    {
        MessageBox(hwnd, _T("���ӵ�������ʧ�ܣ�"), _T("����ʧ��"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    id_player = std::stoi(result->body);  //��ȡ��ǰ���ID

    if (id_player <= 0)
    {
        MessageBox(hwnd, _T("�����Ѿ���ʼ��"), _T("�ܾ�����"), MB_OK | MB_ICONERROR);
        exit(-1);
    }

    (id_player == 1) ? (progress_1 = 0) : (progress_2 = 0);  //��ʼ��������

    str_text = client->Post("/query_text")->body;  //��ȡ�ı�

    std::stringstream str_stream(str_text);	//�ַ�����
    std::string str_line;                   //ÿ���ı�
    while (std::getline(str_stream, str_line))
    {
        str_line_list.push_back(str_line);  //��ÿ���ı������б�
        num_total_char += (int)str_line.length();  //ͳ�����ַ���
    }

    //�����̣߳����շ�������Ϣ
    std::thread([&]() 
        {
            while (true)
            {
                using namespace std::chrono;

                std::string route = (id_player == 1) ? "/update_1" : "/update_2";
                std::string body = std::to_string((id_player == 1) ? progress_1 : progress_2);
                httplib::Result result = client->Post(route, body, "text/plain");

                if (result && result->status == 200)    //HTTP״̬��200��ʾ����ɹ�
                {
                    int progress = std::stoi(result->body);
                    (id_player == 1) ? (progress_2 = progress) : (progress_1 = progress);
                }

                std::this_thread::sleep_for(nanoseconds(1000000000 / 10));
            }
        }).detach();        //�����߳�
}




//��Ϸ��ѭ��
int main(int argc, char** argv)
{
    //////////////�������ݳ�ʼ��////////////////
    using namespace std::chrono;

    hwnd = initgraph(win_width, win_height/*, EW_SHOWCONSOLE*/);  //��ʼ������,EW_SHOWCONSOLE��ʾ����̨
    SetWindowText(hwnd, _T("�����״�ð�գ�"));  //���ô��ڱ���
    settextstyle(28, 0, _T("IPix"));          //����������ʽ

    setbkmode(TRANSPARENT);  //���ñ���͸��


    ResourcesManager::getInstance()->loadResources();  //������Դ
    load_config(hwnd);      //���������ַ
    login_to_server(hwnd);  //��¼��������

    ExMessage msg;          //����������Ϣ


    camera_ui.set_size({ (float)win_width, (float)win_height });
    camera_scene.set_size({ (float)win_width, (float)win_height });



    const nanoseconds frame_duration(1000000000 / 144);     //ÿ֡����ʱ��,nanoseconds��ʾ����
    steady_clock::time_point last_tick = steady_clock::now();  //��һ֡ʱ���

    BeginBatchDraw();  //��ʼ��������

    while (true)
    {
        //////////////�����������////////////////
        while (peekmessage(&msg/*, EM_MOUSE | EM_KEY*/))
        {
            SceneManager::getInstance()->onInput(msg);
        }

        //////////////������Ϸ����////////////////
        steady_clock::time_point frame_start = steady_clock::now();
        duration<float> delta = duration<float>(frame_start - last_tick);

        SceneManager::getInstance()->onUpdate(delta.count());

        //////////////���������////////////////
        setbkcolor(RGB(0, 0, 0));
        cleardevice();

        SceneManager::getInstance()->onDraw(camera_scene);

        FlushBatchDraw();   //���ƻ���������

        last_tick = frame_start;
        nanoseconds sleep_duration = frame_duration - (steady_clock::now() - frame_start);
        if (sleep_duration > nanoseconds(0))
            std::this_thread::sleep_for(sleep_duration);
    }

    EndBatchDraw();   //�������ƻ���������

    return 0;
}