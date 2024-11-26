#ifndef SELECTOR_SINGLE_SCENE_H
#define SELECTOR_SINGLE_SCENE_H


#include <sstream>      //�ַ�����ͷ�ļ�
#include <codecvt>      //����ת��ͷ�ļ�
#include <windows.h>
#include <fstream>

#include "timer.h"
#include "scene.h"
#include "resources_manager.h"
#include "scene_manager.h"
#include "wyjbutton.h"
#include "character_manager.h"
#include "player_hajimi.h"


/*ȫ�ֱ���*/
extern HWND hwnd;	//���ھ��

extern std::atomic<int> progress_1;	//���1����	//atomic���ڶ��߳�
extern std::string str_text;					//�ı�����
extern std::vector<std::string> str_line_list;	//�ı����б�
extern int num_total_char;				    //���ַ���

extern std::string player_name;				//�������
Character* player_single = nullptr;		//���ָ��

class SelectorSingleScene : public Scene
{
public:
    SelectorSingleScene()
    {
        btn_back = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("����"));
        btn_update = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("����"));
        btn_start = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("��ʼ"));

        for (int i = 0; i < text_num_max; i++)
        {
            btn_text_list.push_back(new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_text_up"), *ResourcesManager::getInstance()->findImage("ui_button_text_down"), _T(" ")));
        }

        btn_back->setPosition({ (float)(50), (float)(25) });
        btn_update->setPosition({ (float)(getwidth() - 225), (float)(getheight() - 50) });
        btn_start->setPosition({ (float)(getwidth() - 100), (float)(getheight() - 50) });

        //���ð�ťλ��
        Vector2 pos = {365, 165};
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < text_num_max / 2; j++)
            {
                btn_text_list[i * text_num_max / 2 + j]->setPosition({ pos.x + i * 345, pos.y + j * 40 });
            }
        }

        player_single = new PlayerHajimi();
        player_single->set_position({ 842, 842 });

    }

    ~SelectorSingleScene()
    {
        delete btn_back;
        delete btn_update;
        delete btn_start;

        delete player_single;
    }


    //���ڽ��볡��ʱִ�еĲ���
    void onEnter()override
    {
        //�����ļ������б�
        loadFileNameList();

        btn_back->setClicked(false);
        btn_update->setClicked(false);
        btn_start->setClicked(false);
        
        for (int i = 0; i < text_num_max; i++)
        {
            btn_text_list[i]->setClicked(false);
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
        putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("ui_selector_bg"));

        static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;   //UTF-8����ת����
        std::wstring wstr_line = convert.from_bytes("Name:" + player_name);       //UTF-8����ת��Ϊ���ַ�

        //std::cout << player_name << wstr_line.c_str() << std::endl;

        outtextxy(getwidth() - 250, 10, wstr_line.c_str());
        wstr_line = convert.from_bytes("ID:" + std::to_string(id_player));       //UTF-8����ת��Ϊ���ַ�
        outtextxy(getwidth() - 220, 40, wstr_line.c_str());

        outtextxy((getwidth() - textwidth(_T("�����������..."))) / 2, 70, _T("�����������..."));
        outtextxy((getwidth() - textwidth(_T("�����������롿"))) / 2, getheight() - 110, _T("�����������롿"));

        btn_back->onRender(camera_ui);
        btn_update->onRender(camera_ui);
        btn_start->onRender(camera_ui);

        for (int i = 0; i < text_num; i++)
        {
            btn_text_list[i]->onRender(camera_ui);
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
            //���ѡ�е��ļ���
            for (int i = 0; i < text_num; i++)
            {
                if (btn_text_list[i]->isClicked({ (float)msg.x, (float)msg.y }))
                {
                    std::wcout << "click: " << text_list[i].c_str() << std::endl;
                    text_name = path_name + text_list[i] + L".txt";
                    std::wcout << "text_name: " << text_name.c_str() << std::endl;
                }
                else
                {
                    btn_text_list[i]->setClicked(false);
                }
            }
            //�������
            if (btn_back->isClicked({ (float)msg.x, (float)msg.y }))
            {
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::MENU);
            }
            //�������
            if (btn_update->isClicked({ (float)msg.x, (float)msg.y }))
            {
                loadFileNameList();
            }
            //�����ʼ
            if (btn_start->isClicked({ (float)msg.x, (float)msg.y }))
            {
                progress_1 = 0;
                player_single->set_name(player_name);
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::GAME_SINGLE);
                loadFileString();
            }
        }
            break;
        case WM_LBUTTONUP:        //������
            btn_back->setClicked(false);
            btn_update->setClicked(false);
            btn_start->setClicked(false);
            
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
    //�����ļ�����
    void loadFileString()
    {
        str_line_list.clear();              //����б�

        std::ifstream file(text_name);		//���ı��ļ�
        if (!file.good())
        {
            MessageBox(hwnd, L"�Զ���ս��һ���ı���", L"δѡ���ı�", MB_OK | MB_ICONWARNING);
            return;
        }
        std::stringstream str_stream;	//�����ַ�����
        str_stream << file.rdbuf();		//���ļ����ݶ�������
        str_text = str_stream.str();	//�����е�����ת��Ϊ�ַ���
        num_total_char = (int)str_text.length();

        std::wcout << "str: " << str_text.c_str() << " " << str_text.length() << std::endl;

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

        file.close();		//�ر��ļ�
    }

    //�����ļ������б�
    void loadFileNameList()
    {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(_T(R"(resources\text\*.txt)"), &findFileData);
        text_list.clear();
        text_num = 0;

        if (hFind == INVALID_HANDLE_VALUE) {
            std::cerr << "FindFirstFile failed (" << GetLastError() << ")" << std::endl;
            return;
        }

        do {
            //if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) continue;
            if (text_num >= text_num_max) break;
            std::wstring wstr = findFileData.cFileName;
            wstr.erase(wstr.size() - 4, 4);
            text_list.push_back(wstr);
            text_num++;
        } while (FindNextFile(hFind, &findFileData) != 0);

        for (auto it : text_list)
        {
            std::wcout << it.c_str() << std::endl;
        }
        std::cout << text_num << std::endl;

        FindClose(hFind);

        for (int i = 0; i < text_num; i++)
        {
            //�ļ�����ֵ����ť
            btn_text_list[i]->setText(text_list[i].c_str());
        }

    }

private:
    std::vector<std::wstring> text_list;      //�ĵ��б�
    std::wstring path_name = L"resources/text/";
    std::wstring text_name;             //ѡ�е��ĵ���
    int text_num = 0;                   //�ĵ�����
    const int text_num_max = 22;        //�ĵ��������

    std::vector<WyjButton*> btn_text_list;    //�ĵ���ť�б�

    WyjButton* btn_back;
    WyjButton* btn_start;       //��ʼ��ť
    WyjButton* btn_update;      //���°�ť

};







#endif // SELECTOR_SINGLE_SCENE_H