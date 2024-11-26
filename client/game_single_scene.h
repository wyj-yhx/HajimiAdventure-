#ifndef GAME_SINGLE_SCENE_H
#define GAME_SINGLE_SCENE_H

#include "scene.h"
#include "timer.h"
#include "util.h"
#include "path.h"
#include "character_manager.h"
#include "resources_manager.h"
#include "scene_manager.h"
//#include "wyjbutton.h"

#include <chrono>       //ʱ��ͷ�ļ�
#include <sstream>      //�ַ�����ͷ�ļ�
#include <codecvt>      //����ת��ͷ�ļ�



//ȫ�ֱ�������
extern std::atomic<int> progress_1;	//���1����	//atomic���ڶ��߳�

extern HWND hwnd;	                    //���ھ��
extern int num_total_char;				//���ַ���
extern Path path;
extern Camera camera_scene;             //scene������泡����ui����̶�
extern int win_width, win_height;       //���ڿ��

extern int idx_line;						//��ǰ����
extern int idx_char;						//��ǰ�ַ���
extern std::vector<std::string> str_line_list;	//�ı����б�

extern std::string player_name;			//�������
extern Character* player_single;		//���ָ��

/************��Ϸѡ����************/
class GameSingleScene : public Scene
{
private:

    int val_countdown = 4;			//����ʱ 4 s
    Stage stage = Stage::Waiting;	//��ǰ��Ϸ״̬
    Timer timer_countdown;          //����ʱ��ʱ��

    int correctness = 0;       //��ȷ��
    int num_minute = 0;        //�����ٶȣ���/����
    int num_char = 0;               //����������

    std::chrono::steady_clock::time_point start_clock;   //���ֿ�ʼʱ��
    std::chrono::steady_clock::time_point frame_clock;   //����֡ʱ��
    std::chrono::duration<float> delta_clock;            //����ʱ���

    WyjButton* btn_back;		//���ذ�ť
    WyjButton* btn_exit;		//���ذ�ť
    WyjButton* btn_text;		//�ı���ť

public:
    GameSingleScene()
    {
        //��ť��ʼ��
        btn_exit = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("�˳�"));
        btn_back = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("����"));
        btn_text = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_selector_up"), *ResourcesManager::getInstance()->findImage("ui_button_selector_down"), _T(" "));

        btn_exit->setPosition({ (float)(50), (float)(25) });
        btn_text->setPosition({ (float)(getwidth() / 2), (float)(getheight() / 2) });
        btn_back->setPosition({ (float)(getwidth() / 2 + 100), (float)(getheight() / 2 + 60) });

        //��ʼ�����ܵ���ʱ
        timer_countdown.set_one_shot(false);
        timer_countdown.set_wait_time(1.0f);
        timer_countdown.set_on_timeout([&]()
            {
                val_countdown--;

                switch (val_countdown)
                {
                case 3: play_audio(_T("ui_3"));     break;
                case 2: play_audio(_T("ui_2"));     break;
                case 1: play_audio(_T("ui_1"));     break;
                case 0: play_audio(_T("ui_fight")); break;
                case -1:
                    stage = Stage::Racing;
                    play_audio(_T("bgm"), true);     //���ű�������,ѭ������
                }
            });

    }
    ~GameSingleScene()
    {
        delete btn_exit;
        delete btn_text;
        delete btn_back;

    }

    //���ڽ��볡��ʱִ�еĲ���
    void onEnter()override
    {
        btn_exit->setClicked(false);
        btn_back->setClicked(false);
        btn_text->setClicked(false);

        start_clock = std::chrono::steady_clock::now();  //��һ֡ʱ���

    }

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta)override
    {
        //std::cout<<"onUpdate"<<std::endl;
        if (stage == Stage::Waiting)
        {
            if (progress_1 >= 0)
                stage = Stage::Ready;
        }
        else if (stage == Stage::Ready)
        {
            timer_countdown.on_update(delta);
            //���½�ɫ��Ŀ��
            player_single->on_update(delta);

            player_single->set_target(path.get_position_at_progress((float)progress_1 / num_total_char));

            camera_scene.look_at(player_single->get_position());
        }
        else if (stage == Stage::Racing)
        {
            frame_clock = std::chrono::steady_clock::now();
            delta_clock = std::chrono::duration<float>(frame_clock - start_clock);
            num_minute = (int)(progress_1 / delta_clock.count() * 60);

            //std::cout <<"delta_clock: " << delta_clock.count() << "num_secont: " << num_minute << std::endl;

            if (progress_1 >= num_total_char)
            {
                stop_audio(_T("bgm"));
                play_audio(_T("1p_win"));
                stage = Stage::End;
            }

            //���½�ɫ��Ŀ��
            player_single->on_update(delta);

            player_single->set_target(path.get_position_at_progress((float)progress_1 / num_total_char));

            camera_scene.look_at(player_single->get_position());

        }
        else if (stage == Stage::End)
        {
            player_single->set_target(path.get_position_at_progress((float)progress_1 / num_total_char));

            camera_scene.look_at(player_single->get_position());

            if (progress_1 >= num_total_char)
            {
                btn_text->setText(_T("��ϲ�������ս��"));
            } 
        }

    }

    //���ڳ�������ʱִ�еĲ���
    void onRender(Camera& camera_ui)override
    {
        //std::cout << "onRender" << std::endl;
        if (stage == Stage::Waiting)
        {
            settextcolor(RGB(195, 195, 195));
            outtextxy(15, 675, _T("�ȴ���ҽ���..."));
        }
        else if (stage == Stage::Ready)
        {
            //���Ʊ���ͼ
            IMAGE& img_bg = *ResourcesManager::getInstance()->findImage("background");
            const Rect rect_bg =
            {
                0,0,
                img_bg.getwidth(),
                img_bg.getheight()
            };
            putimage_ex(camera_scene, &img_bg, &rect_bg);

            //��Ⱦ��ɫ
            player_single->on_render(camera_scene);

            //���Ƶ���ʱ
            switch (val_countdown)
            {
            case 3:
            {
                putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("ui_3"));
            }
            break;
            case 2:
            {
                putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("ui_2"));
            }
            break;
            case 1:
            {
                //IMAGE& img_ui_1 = *ResourcesManager::getInstance()->findImage("ui_1");
                putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("ui_1"));
            }
            break;
            case 0:
            {
                putimage_center(camera_ui, ResourcesManager::getInstance()->findImage("ui_fight"));
            }
            break;
            default: break;
            }
        }
        else if (stage == Stage::Racing)
        {
            //���Ʊ���ͼ
            IMAGE& img_bg = *ResourcesManager::getInstance()->findImage("background");
            const Rect rect_bg =
            {
                0,0,
                img_bg.getwidth(),
                img_bg.getheight()
            };
            putimage_ex(camera_scene, &img_bg, &rect_bg);

            //��Ⱦ��ɫ
            player_single->on_render(camera_scene);

            //���ƽ���
            IMAGE& img_ui_textbox_1 = *ResourcesManager::getInstance()->findImage("ui_textbox_1");
            static const Rect rect_textbox_1 =
            {
                0,
                win_height - img_ui_textbox_1.getheight(),
                img_ui_textbox_1.getwidth(),
                img_ui_textbox_1.getheight()
            };
            putimage_ex(camera_ui, &img_ui_textbox_1, &rect_textbox_1);

            if (win_width > 1080)
            {
                IMAGE& img_ui_textbox_3 = *ResourcesManager::getInstance()->findImage("ui_textbox_3");
                static const Rect rect_textbox_3 =
                {
                    win_width - img_ui_textbox_3.getwidth(),
                    win_height - img_ui_textbox_3.getheight(),
                    img_ui_textbox_3.getwidth(),
                    img_ui_textbox_3.getheight()
                };
                putimage_ex(camera_ui, &img_ui_textbox_3, &rect_textbox_3);
            }

            IMAGE& img_ui_textbox_2 = *ResourcesManager::getInstance()->findImage("ui_textbox_2");
            static const Rect rect_textbox_2 =
            {
                win_width - img_ui_textbox_2.getwidth(),
                win_height - img_ui_textbox_2.getheight(),
                img_ui_textbox_2.getwidth(),
                img_ui_textbox_2.getheight()
            };
            putimage_ex(camera_ui, &img_ui_textbox_2, &rect_textbox_2);


            static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;   //UTF-8����ת����
            std::wstring wstr_line = convert.from_bytes(str_line_list[idx_line]);       //UTF-8����ת��Ϊ���ַ�
            std::wstring wstr_completed = convert.from_bytes(str_line_list[idx_line].substr(0, idx_char));  //UTF-8����ת��Ϊ���ַ�
            std::wstring wstr_minute = convert.from_bytes("TypingSpeed: " + std::to_string(num_minute) + " n/m");       //UTF-8����ת��Ϊ���ַ�
            std::wstring wstr_correctness = convert.from_bytes("Correctness: " + std::to_string(correctness) + " %");       //UTF-

            //���ƴ����ı�
            settextcolor(RGB(125, 125, 125));
            outtextxy(rect_textbox_1.x + 150 + 2, rect_textbox_1.y + 65 + 2, wstr_line.c_str());
            settextcolor(RGB(25, 25, 25));
            outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_line.c_str());
            settextcolor(RGB(0, 149, 217));
            outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_completed.c_str());

            //�������Ͻ�����
            settextcolor(RGB(255, 0, 0));
            outtextxy(getwidth() - 280, 10, wstr_minute.c_str());
            outtextxy(getwidth() - 280, 40, wstr_correctness.c_str());

            btn_exit->onRender(camera_ui);
        }
        else if (stage == Stage::End)
        {
            //���Ʊ���ͼ
            IMAGE& img_bg = *ResourcesManager::getInstance()->findImage("background");
            const Rect rect_bg =
            {
                0,0,
                img_bg.getwidth(),
                img_bg.getheight()
            };
            putimage_ex(camera_scene, &img_bg, &rect_bg);

            //��Ⱦ��ɫ
            player_single->on_render(camera_scene);

            btn_text->onRender(camera_ui);
            btn_back->onRender(camera_ui);
        }
    }

    //���ڳ�������ʱִ�еĲ���
    void onInput(const ExMessage& msg) override
    {
        //std::cout << "onInput" << std::endl;

        if (msg.message == WM_LBUTTONDOWN)
        {
            if (btn_exit->isClicked({ (float)msg.x, (float)msg.y }))
            {
                stop_audio(_T("bgm"));
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_SINGLE);
            }

            if (stage == Stage::End && btn_back->isClicked({ (float)msg.x, (float)msg.y }))
            {
                stop_audio(_T("bgm"));
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_SINGLE);
            }

        }

        if (stage != Stage::Racing)
        {
            return;
        }

        if (msg.message == WM_CHAR && idx_line < str_line_list.size())
        {
            std::cout << "input: " << msg.ch << std::endl;
            num_char++;
            const std::string& str_line = str_line_list[idx_line];
            if (str_line[idx_char] == msg.ch)   //������ȷ,����msg.chΪ������ַ�
            {
                switch (rand() % 4)
                {
                case 0: play_audio(_T("click_1")); break;
                case 1: play_audio(_T("click_2")); break;
                case 2: play_audio(_T("click_3")); break;
                case 3: play_audio(_T("click_4")); break;
                }

                progress_1++;
                //��ȷ��
                correctness = (int)progress_1 * 100 / num_char;
                //std::cout << "correctness: " << correctness << std::endl;
                idx_char++;
                if (idx_char >= str_line.length())
                {
                    idx_char = 0;
                    idx_line++;
                }

            }
            std::cout << ": " << msg.ch << std::endl;
        }
    }

    //�����˳�����ʱִ�еĲ���
    void onExit() override
    {
        progress_1 = -1;
        progress_2 = -1;

        correctness = 0;

        stage = Stage::Waiting;
        idx_line = 0;
        idx_char = 0;
        val_countdown = 4;

        timer_countdown.restart();

        player_single->set_position({ 842, 842 });

    }

};






#endif // GAME_SINGLE_SCENE_H