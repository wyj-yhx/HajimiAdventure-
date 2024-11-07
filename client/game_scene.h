#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "scene.h"
#include "timer.h"
#include "util.h"
#include "path.h"
#include "character_manager.h"
#include "resources_manager.h"

#include <sstream>      //�ַ�����ͷ�ļ�
#include <codecvt>      //����ת��ͷ�ļ�


//ȫ�ֱ�������
extern std::atomic<int> progress_1;	//���1����	//atomic���ڶ��߳�
extern std::atomic<int> progress_2;	//���2����

extern HWND hwnd;	//���ھ��
extern int id_player;		//���id
extern int num_total_char;				//���ַ���
extern Path path;
extern Camera camera_ui, camera_scene; //scene������泡����ui����̶�
extern int win_width, win_height; //���ڿ��
//extern Player player_1, player_2;	//���1�����2

extern int idx_line;						//��ǰ����
extern int idx_char;						//��ǰ�ַ���
extern std::vector<std::string> str_line_list;	//�ı����б�

/************��Ϸѡ����************/
class GameScene : public Scene
{
private:

    int val_countdown = 4;			//����ʱ 4 s
    Stage stage = Stage::Waiting;	//��ǰ��Ϸ״̬
    Timer timer_countdown;  //����ʱ��ʱ��

public:
    GameScene()
    {
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

        CharacterManager::getInstance()->getPlayer_1()->set_position({ 842, 842 });
        CharacterManager::getInstance()->getPlayer_2()->set_position({ 842, 842 });
    }
    ~GameScene() = default;

    //���ڽ��볡��ʱִ�еĲ���
    void onEnter()override 
    {
        
    }   

    //���ڳ�������ʱִ�еĲ���
    void onUpdate(float delta)override 
    {

        if (stage == Stage::Waiting)
        {
            if (progress_1 >= 0 && progress_2 >= 0)
                stage = Stage::Ready;
        }
        else
        {
            if (stage == Stage::Ready)
                timer_countdown.on_update(delta);

            if ((id_player == 1 && progress_1 >= num_total_char) ||
                (id_player == 2 && progress_2 >= num_total_char))
            {
                stop_audio(_T("bgm"));
                play_audio((id_player == 1) ? _T("1p_win") : _T("2p_win"));
                MessageBox(hwnd, _T("Ӯ���ˣ�"), _T("��Ϸ����"), MB_OK | MB_ICONINFORMATION);
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::MENU);
            }
            else if ((id_player == 1 && progress_2 >= num_total_char) ||
                (id_player == 2 && progress_1 >= num_total_char))
            {
                stop_audio(_T("bgm"));
                MessageBox(hwnd, _T("�����ˣ�"), _T("��Ϸ����"), MB_OK | MB_ICONINFORMATION);
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::MENU);
            }

            //���½�ɫ��Ŀ��
            CharacterManager::getInstance()->onUpdate(delta);

            CharacterManager::getInstance()->getPlayer_1()->set_target(path.get_position_at_progress((float)progress_1 / num_total_char));
            CharacterManager::getInstance()->getPlayer_2()->set_target(path.get_position_at_progress((float)progress_2 / num_total_char));

            camera_scene.look_at((id_player == 1) ? CharacterManager::getInstance()->getPlayer_1()->get_position() : CharacterManager::getInstance()->getPlayer_2()->get_position());

        }
    }  

    //���ڳ�������ʱִ�еĲ���
    void onRender(Camera& camera)override
    {
        if (stage == Stage::Waiting)
        {
            settextcolor(RGB(195, 195, 195));
            outtextxy(15, 675, _T("�ȴ���Ҽ���..."));
        }
        else
        {
            //���Ʊ���ͼ
            IMAGE& img_bg = *ResourcesManager::getInstance()->findImage("background");
            static const Rect rect_bg =
            {
                0,0,
                img_bg.getwidth(),
                img_bg.getheight()
            };
            putimage_ex(camera_scene, &img_bg, &rect_bg);

            //��Ⱦ��ɫ
            CharacterManager::getInstance()->onRender(camera_scene);

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

            //���ƽ���
            if (stage == Stage::Racing)
            {
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

                settextcolor(RGB(125, 125, 125));
                outtextxy(rect_textbox_1.x + 150 + 2, rect_textbox_1.y + 65 + 2, wstr_line.c_str());
                settextcolor(RGB(25, 25, 25));
                outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_line.c_str());
                settextcolor(RGB(0, 149, 217));
                outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_completed.c_str());
            }
        }
    }   

    //���ڳ�������ʱִ�еĲ���
    void onInput(const ExMessage& msg) override
    {
        if (stage != Stage::Racing)
        {
            return;
        }

        if (msg.message == WM_CHAR && idx_line < str_line_list.size())
        {
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

                (id_player == 1) ? (progress_1++) : (progress_2++);

                idx_char++;
                if (idx_char >= str_line.length())
                {
                    idx_char = 0;
                    idx_line++;
                }
            }
        }
    }   

    //�����˳�����ʱִ�еĲ���
    void onExit() override
    {
        progress_1 = 0;
        progress_2 = 0;

        stage = Stage::Waiting;
        idx_line = 0;
        idx_char = 0;
        val_countdown = 4;

        timer_countdown.restart();

        CharacterManager::getInstance()->getPlayer_1()->set_position({ 842, 842 });
        CharacterManager::getInstance()->getPlayer_2()->set_position({ 842, 842 });
    }    

};






#endif // GAME_SCENE_H