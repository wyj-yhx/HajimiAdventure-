#ifndef SELECTOR_SCENE_H
#define SELECTOR_SCENE_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>

#include <sstream>      //�ַ�����ͷ�ļ�
#include <codecvt>      //����ת��ͷ�ļ�

#pragma comment(lib, "ws2_32.lib")

#include "timer.h"
#include "scene.h"
#include "resources_manager.h"
#include "protocol.h"
#include "scene_manager.h"
#include "wyjbutton.h"
#include "character_manager.h"
//#include "vector2.h"


/*ȫ�ֱ���*/
extern HWND hwnd;	//���ھ��
extern std::string str_address;			//��������ַ

extern std::string str_text;					//�ı�����
extern std::vector<std::string> str_line_list;	//�ı����б�
extern int num_total_char;				//���ַ���

extern std::string player_name;				//�������
extern std::string counterpart_name;	    //�Է��������
extern int id_player;						//���ID
int select_room = 0;						//����ID
int select_room_temp = 0;                  //��ʱ����ID
int roomList[12] = { 0 };					//�����б�

extern std::atomic<int> progress_1;	//���1����	//atomic���ڶ��߳�
extern std::atomic<int> progress_2;	//���2����
extern int player_game_id;						//�����ϷID

SOCKET client_socket;       //�ͻ����׽���

//�����ļ�����
void stringToStringList()
{
    str_line_list.clear();              //����б�

    std::string str_line;                   //ÿ���ı�
    int left = 0, right = 0;

    while (str_text[right] != '\0')
    {
        if ((right - left) % 50 == 49)
        {
            while (str_text[right] != L' ')
            {
                if (right == str_text.length() - 1)  break;
                right++;
            }
            right++;
            str_line = str_text.substr(left, right - left);

            left = right;
            str_line_list.push_back(str_line);  //��ÿ���ı������б�
            std::cout << "strline_length(): " << str_line << " " << str_line.size() << std::endl;
        }
        else
        {
            right++;
        }
    }
    if (right - left != 0)
    {
        str_line = str_text.substr(left, right - left);
        str_line_list.push_back(str_line);  //��ÿ���ı������б�
        std::cout << "strline_length(): " << str_line << " " << str_line.size() << std::endl;
    }

}


//���߳�
DWORD WINAPI threadFunc(LPVOID lpParam)
{
    SOCKET socketfd = *(SOCKET*)lpParam;

    while (1)
    {
        uint pduLen = 0;
        int ret = recv(client_socket, (char*)&pduLen, sizeof(uint), 0);		//��ȡPDU����
        if (ret <= 0) break;

        PDU* uiPDU = mkPDU(pduLen - sizeof(PDU));
        ret = recv(client_socket, (char*)uiPDU + sizeof(uint), pduLen - sizeof(uint), 0);		//��ȡʣ��PDU����
        if (ret <= 0) break;

        switch (uiPDU->uiPDUType)
        {
        case LOGIN_SUCCESS:
        {
            id_player = uiPDU->room_id;
            std::cout << uiPDU->caData << std::endl;
            str_text = (char*)uiPDU->strMsg;
            num_total_char = (int)strlen(uiPDU->strMsg);

            std::cout << str_text << std::endl;
            std::cout << uiPDU->uiMSGLen << " " << uiPDU->strMsg << std::endl;

            //�����ļ�����
            stringToStringList();
            
            std::cout << "num_total_char: " << num_total_char << std::endl;
        }
            break;
        case GET_ROOM_LIST_SUCCESS:
            for (int i = 0; i < 12; i++)
            {
                std::cout << uiPDU->roomList[i] << std::endl;
                roomList[i] = uiPDU->roomList[i];
            }
            break;
        case GET_GAME_INFO_SUCCESS:
            std::cout << uiPDU->caData << std::endl;
            progress_2 = uiPDU->opposing_player;
            std::cout << "progress_1: " << progress_1 << " progress_2: " << progress_2 << " " << num_total_char << std::endl;
            break;
        case CREATE_ROOM_SUCCESS:
            std::cout << uiPDU->caData << std::endl;
            select_room = uiPDU->room_id;
            break;
        case CREATE_ROOM_FAIL:
            std::cout << uiPDU->caData << std::endl;
            break;
        case CANCEL_ROOM_SUCCESS:
            std::cout << uiPDU->caData << std::endl;
            break;
        case UPDATE_ROOM_LIST_SUCCESS:
            for (int i = 0; i < 12; i++)
            {
                std::cout << uiPDU->roomList[i] << std::endl;
                roomList[i] = uiPDU->roomList[i];
            }
            break;
        case JOIN_ROOM_SUCCESS:
        {
            std::cout << uiPDU->caData << std::endl;
            counterpart_name = uiPDU->caData;
            CharacterManager::getInstance()->getPlayer_1()->set_name(player_name);
            CharacterManager::getInstance()->getPlayer_2()->set_name(counterpart_name);

            progress_1 = 0;
            player_game_id = uiPDU->player_game_id;				//�����ϷID
            select_room = uiPDU->room_id;						//����ID
            progress_2 = uiPDU->opposing_player;

            SceneManager::getInstance()->switchTo(SceneManager::SceneType::GAME_LAN);
        }
            break;
        case EXIT_GAME_LAN_SUCCESS:
            std::cout << "��ң�" << uiPDU->caData << " ���˳���" << std::endl;
            progress_2 = -1;
            break;
        default:
            break;
        }

        free(uiPDU);
        Sleep(50);
    }

    //4.�ر��׽�������
    closesocket(client_socket);

    return 0;
}



//��Ϸ��ʼ����������¼��������
int login_to_server(HWND hwnd)
{
    //windows��ʹ��������Ҫ��ʼ����Ȩ��
    WSADATA wsaData;
    int iResual = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResual != 0)
    {
        std::cout << "WSAStartup error!!!" << "errcode:" << GetLastError() << std::endl;
        return -1;
    }

    //1.����socket�׽���
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        std::cout << "client socket error!!!" << std::endl;
        return -1;
    }

    //2_1.�����ı��ļ�
    std::ifstream file_cfg("config.cfg");		//���ı��ļ�
    if (!file_cfg.good())
    {
        MessageBox(nullptr, L"�޷����ı��ļ� config.cfg", L"����ʧ��", MB_OK | MB_ICONERROR);
        return -1;
    }
    std::stringstream config_stream;	//�����ַ�����
    config_stream << file_cfg.rdbuf();		//���ļ����ݶ�������
    std::string config_addr = config_stream.str();	//�����е�����ת��Ϊ�ַ���

    file_cfg.close();		//�ر��ļ�

    //2.���ӷ�����
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(8080);
    target.sin_addr.s_addr = inet_addr(config_addr.c_str());

    if (-1 == connect(client_socket, (struct sockaddr*)&target, sizeof(target)))
    {
        std::cout << "connect error!!!" << std::endl;
        closesocket(client_socket);
        return -1;
    }

    //3.��ʼͨѶ	send() recv()
    CreateThread(NULL, 0, threadFunc, &client_socket, 0, NULL);     //�����߳�

    return 0;

}


class SelectorLanScene : public Scene
{
public:
    SelectorLanScene() 
    {
        btn_create = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"),_T("����"));
        btn_cancel = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("ȡ��"));
        btn_update = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("����"));
        btn_join = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("����"));
        btn_back = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("����"));
        for (int i = 0; i < 6; i++)
        {
            btn_selector_list.push_back(new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_selector_up"), *ResourcesManager::getInstance()->findImage("ui_button_selector_down"), _T(" ")));
        }

        btn_create->setPosition({ (float)(getwidth() - 475), (float)(getheight() - 50) });
        btn_cancel->setPosition({ (float)(getwidth() - 350), (float)(getheight() - 50) });
        btn_update->setPosition({ (float)(getwidth() - 225), (float)(getheight() - 50) });
        btn_join->setPosition({ (float)(getwidth() - 100), (float)(getheight() - 50) });
        btn_back->setPosition({ (float)(50), (float)(25) });

        for (int i = 1; i < 3; i++)
        {
            for (int j = 1; j < 4; j++)
            {
                btn_selector_list[(i - 1) * 3 + j - 1]->setPosition({ (float)(getwidth() * i / 3), (float)(getheight() * j / 4) });
            }
        }

        //���ӷ�����
        int rev = login_to_server(hwnd);  //��¼��������
        if (rev == -1)
        {
            MessageBox(hwnd, _T("���ӷ�����ʧ��"), _T("��ʾ"), MB_OK);
            return;
        }

    }

    ~SelectorLanScene()
    {
        delete btn_create;
        delete btn_join;
        delete btn_back;
        for (int i = 0; i < 6; i++)
        {
            delete btn_selector_list[i];
        }
        btn_selector_list.clear();
    }


    //���ڽ��볡��ʱִ�еĲ���
    void onEnter()override
    {
        //play_audio(_T("bgm_menu"), true);

        btn_create->setClicked(false);
        btn_cancel->setClicked(false);
        btn_update->setClicked(false);
        btn_join->setClicked(false);
        btn_back->setClicked(false);

        for (int i = 0; i < 6; i++)
        {
            btn_selector_list[i]->setClicked(false);
        }

    }

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta)override
    {

    }

    //���ڳ�������ʱִ�еĲ���
    void onRender(Camera& camera_ui)override
    {
        putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("background_selector"));

        btn_create->onRender(camera_ui);
        btn_cancel->onRender(camera_ui);
        btn_update->onRender(camera_ui);
        btn_join->onRender(camera_ui);
        btn_back->onRender(camera_ui);

        for (int i = 0; i < 6; i++)
        {
            btn_selector_list[i]->onRender(camera_ui);
        }

        settextcolor(RGB(0, 0, 0));

        static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;   //UTF-8����ת����
        std::wstring wstr_line = convert.from_bytes("Name:" + player_name);       //UTF-8����ת��Ϊ���ַ�
        
        //std::cout << player_name << wstr_line.c_str() << std::endl;

        outtextxy(getwidth() - 250, 10, wstr_line.c_str());
        wstr_line = convert.from_bytes("ID:" + std::to_string(id_player));       //UTF-8����ת��Ϊ���ַ�
        outtextxy(getwidth() - 220, 40, wstr_line.c_str());

        outtextxy((getwidth() - textwidth(_T("�����������..."))) / 2, 50, _T("�����������..."));
        outtextxy((getwidth() - textwidth(_T("�����������롿"))) / 2, getheight() - 110, _T("�����������롿"));

        for (int i = 0; i < 6; i++)
        {
            Vector2 posi = btn_selector_list[i]->getPosition();
            wstr_line = convert.from_bytes(std::to_string(roomList[i * 2]) + " VS " + std::to_string(roomList[i * 2 + 1]));
            outtextxy((int)posi.x + 80, (int)posi.y + 40, wstr_line.c_str());
        }

    }

    //���ڳ�������ʱִ�еĲ���
    void onInput(const ExMessage& msg) override
    {
        switch (msg.message)
        {

        case WM_CHAR:
            if (msg.ch == 8 && player_name.length() > 0)
            {
                player_name.pop_back();
                std::cout << player_name << std::endl;
                break;
            }
            if (player_name.length() < 30)
            {
                player_name += (char)msg.ch;
                std::cout << msg.ch << std::endl;
            }
            break;
        case VK_RETURN:
            break;
        case WM_LBUTTONDOWN:        //������
        {
            //��������
            for (int i = 0; i < btn_selector_list.size(); i++)
            {
                if (btn_selector_list[i]->isClicked({ (float)msg.x, (float)msg.y }))
                {
                    select_room_temp = i;
                }
                else
                {
                    btn_selector_list[i]->setClicked(false);
                }
            }

            PDU* uiPDU = mkPDU(0);

            //�����������
            if (btn_create->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->uiPDUType = CREATE_ROOM;
                strcpy_s(uiPDU->caData, player_name.c_str());

                //��������
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }
            //���ȡ������
            if (btn_cancel->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->uiPDUType = CANCEL_ROOM;
                strcpy_s(uiPDU->caData, player_name.c_str());

                //ȡ������
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }
            //���·���
            if (btn_update->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->room_id = select_room;
                uiPDU->uiPDUType = UPDATE_ROOM_LIST;
                strcpy_s(uiPDU->caData, player_name.c_str());

                //���·���
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }
            //������뷿��
            if (btn_join->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->room_id = select_room_temp;
                uiPDU->uiPDUType = JOIN_ROOM;
                strcpy_s(uiPDU->caData, player_name.c_str());
                //��������
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }

            //�������
            if (btn_back->isClicked({ (float)msg.x, (float)msg.y }))
            {
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::MENU);
            }
            free(uiPDU);
            uiPDU = nullptr;
        }
            break;
        case WM_LBUTTONUP:        //������
            btn_create->setClicked(false);
            btn_cancel->setClicked(false);
            btn_update->setClicked(false);
            btn_join->setClicked(false);
            btn_back->setClicked(false);
            break;
        case WM_RBUTTONDOWN:        //����Ҽ�
            break;

        default:
            break;
        }
    }

    //�����˳�����ʱִ�еĲ���
    void onExit() override
    {
        //stop_audio(_T("bgm_menu"));
    }


private:
    std::vector<WyjButton*> btn_selector_list;
    WyjButton* btn_create;      //������ť
    WyjButton* btn_cancel;      //ȡ����ť
    WyjButton* btn_join;
    WyjButton* btn_back;        //���ذ�ť
    WyjButton* btn_update;      //���°�ť

private:
    
};







#endif // SELECTOR_SCENE_H