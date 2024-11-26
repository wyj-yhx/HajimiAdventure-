#ifndef SELECTOR_SCENE_H
#define SELECTOR_SCENE_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>

#include <sstream>      //字符串流头文件
#include <codecvt>      //编码转换头文件

#pragma comment(lib, "ws2_32.lib")

#include "timer.h"
#include "scene.h"
#include "resources_manager.h"
#include "protocol.h"
#include "scene_manager.h"
#include "wyjbutton.h"
#include "character_manager.h"
//#include "vector2.h"


/*全局变量*/
extern HWND hwnd;	//窗口句柄
extern std::string str_address;			//服务器地址

extern std::string str_text;					//文本内容
extern std::vector<std::string> str_line_list;	//文本行列表
extern int num_total_char;				//总字符数

extern std::string player_name;				//玩家名字
extern std::string counterpart_name;	    //对方玩家名字
extern int id_player;						//玩家ID
int select_room = 0;						//房间ID
int select_room_temp = 0;                  //临时房间ID
int roomList[12] = { 0 };					//房间列表

extern std::atomic<int> progress_1;	//玩家1进度	//atomic用于多线程
extern std::atomic<int> progress_2;	//玩家2进度
extern int player_game_id;						//玩家游戏ID

SOCKET client_socket;       //客户端套接字

//加载文件内容
void stringToStringList()
{
    str_line_list.clear();              //清空列表

    std::string str_line;                   //每行文本
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
            str_line_list.push_back(str_line);  //将每行文本存入列表
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
        str_line_list.push_back(str_line);  //将每行文本存入列表
        std::cout << "strline_length(): " << str_line << " " << str_line.size() << std::endl;
    }

}


//多线程
DWORD WINAPI threadFunc(LPVOID lpParam)
{
    SOCKET socketfd = *(SOCKET*)lpParam;

    while (1)
    {
        uint pduLen = 0;
        int ret = recv(client_socket, (char*)&pduLen, sizeof(uint), 0);		//读取PDU长度
        if (ret <= 0) break;

        PDU* uiPDU = mkPDU(pduLen - sizeof(PDU));
        ret = recv(client_socket, (char*)uiPDU + sizeof(uint), pduLen - sizeof(uint), 0);		//读取剩余PDU内容
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

            //加载文件内容
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
            player_game_id = uiPDU->player_game_id;				//玩家游戏ID
            select_room = uiPDU->room_id;						//房间ID
            progress_2 = uiPDU->opposing_player;

            SceneManager::getInstance()->switchTo(SceneManager::SceneType::GAME_LAN);
        }
            break;
        case EXIT_GAME_LAN_SUCCESS:
            std::cout << "玩家：" << uiPDU->caData << " 已退出！" << std::endl;
            progress_2 = -1;
            break;
        default:
            break;
        }

        free(uiPDU);
        Sleep(50);
    }

    //4.关闭套接字连接
    closesocket(client_socket);

    return 0;
}



//游戏初始化函数，登录到服务器
int login_to_server(HWND hwnd)
{
    //windows上使用网络需要开始网络权限
    WSADATA wsaData;
    int iResual = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResual != 0)
    {
        std::cout << "WSAStartup error!!!" << "errcode:" << GetLastError() << std::endl;
        return -1;
    }

    //1.创建socket套接字
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        std::cout << "client socket error!!!" << std::endl;
        return -1;
    }

    //2_1.加载文本文件
    std::ifstream file_cfg("config.cfg");		//打开文本文件
    if (!file_cfg.good())
    {
        MessageBox(nullptr, L"无法打开文本文件 config.cfg", L"启动失败", MB_OK | MB_ICONERROR);
        return -1;
    }
    std::stringstream config_stream;	//创建字符串流
    config_stream << file_cfg.rdbuf();		//将文件内容读入流中
    std::string config_addr = config_stream.str();	//将流中的内容转换为字符串

    file_cfg.close();		//关闭文件

    //2.连接服务器
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

    //3.开始通讯	send() recv()
    CreateThread(NULL, 0, threadFunc, &client_socket, 0, NULL);     //创建线程

    return 0;

}


class SelectorLanScene : public Scene
{
public:
    SelectorLanScene() 
    {
        btn_create = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"),_T("创建"));
        btn_cancel = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("取消"));
        btn_update = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("更新"));
        btn_join = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("加入"));
        btn_back = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("返回"));
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

        //连接服务器
        int rev = login_to_server(hwnd);  //登录到服务器
        if (rev == -1)
        {
            MessageBox(hwnd, _T("连接服务器失败"), _T("提示"), MB_OK);
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


    //用于进入场景时执行的操作
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

    //用于场景更新时执行的操作
    void onUpdate(float delta)override
    {

    }

    //用于场景绘制时执行的操作
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

        static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;   //UTF-8编码转换器
        std::wstring wstr_line = convert.from_bytes("Name:" + player_name);       //UTF-8编码转换为宽字符
        
        //std::cout << player_name << wstr_line.c_str() << std::endl;

        outtextxy(getwidth() - 250, 10, wstr_line.c_str());
        wstr_line = convert.from_bytes("ID:" + std::to_string(id_player));       //UTF-8编码转换为宽字符
        outtextxy(getwidth() - 220, 40, wstr_line.c_str());

        outtextxy((getwidth() - textwidth(_T("输入你的名字..."))) / 2, 50, _T("输入你的名字..."));
        outtextxy((getwidth() - textwidth(_T("【点击房间加入】"))) / 2, getheight() - 110, _T("【点击房间加入】"));

        for (int i = 0; i < 6; i++)
        {
            Vector2 posi = btn_selector_list[i]->getPosition();
            wstr_line = convert.from_bytes(std::to_string(roomList[i * 2]) + " VS " + std::to_string(roomList[i * 2 + 1]));
            outtextxy((int)posi.x + 80, (int)posi.y + 40, wstr_line.c_str());
        }

    }

    //用于场景输入时执行的操作
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
        case WM_LBUTTONDOWN:        //鼠标左键
        {
            //点击房间号
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

            //点击创建房间
            if (btn_create->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->uiPDUType = CREATE_ROOM;
                strcpy_s(uiPDU->caData, player_name.c_str());

                //创建房间
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }
            //点击取消房间
            if (btn_cancel->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->uiPDUType = CANCEL_ROOM;
                strcpy_s(uiPDU->caData, player_name.c_str());

                //取消房间
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }
            //更新房间
            if (btn_update->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->room_id = select_room;
                uiPDU->uiPDUType = UPDATE_ROOM_LIST;
                strcpy_s(uiPDU->caData, player_name.c_str());

                //更新房间
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }
            //点击加入房间
            if (btn_join->isClicked({ (float)msg.x, (float)msg.y }))
            {
                uiPDU->room_id = select_room_temp;
                uiPDU->uiPDUType = JOIN_ROOM;
                strcpy_s(uiPDU->caData, player_name.c_str());
                //创建房间
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
            }

            //点击返回
            if (btn_back->isClicked({ (float)msg.x, (float)msg.y }))
            {
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::MENU);
            }
            free(uiPDU);
            uiPDU = nullptr;
        }
            break;
        case WM_LBUTTONUP:        //鼠标左键
            btn_create->setClicked(false);
            btn_cancel->setClicked(false);
            btn_update->setClicked(false);
            btn_join->setClicked(false);
            btn_back->setClicked(false);
            break;
        case WM_RBUTTONDOWN:        //鼠标右键
            break;

        default:
            break;
        }
    }

    //用于退出场景时执行的操作
    void onExit() override
    {
        //stop_audio(_T("bgm_menu"));
    }


private:
    std::vector<WyjButton*> btn_selector_list;
    WyjButton* btn_create;      //创建按钮
    WyjButton* btn_cancel;      //取消按钮
    WyjButton* btn_join;
    WyjButton* btn_back;        //返回按钮
    WyjButton* btn_update;      //更新按钮

private:
    
};







#endif // SELECTOR_SCENE_H