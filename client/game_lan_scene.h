#ifndef GAME_LAN_SCENE_H
#define GAME_LAN_SCENE_H

#include "scene.h"
#include "timer.h"
#include "util.h"
#include "path.h"
#include "character_manager.h"
#include "resources_manager.h"
#include "scene_manager.h"
//#include "wyjbutton.h"

#include <sstream>      //字符串流头文件
#include <codecvt>      //编码转换头文件


//全局变量定义
extern std::atomic<int> progress_1;	//玩家1进度	//atomic用于多线程
extern std::atomic<int> progress_2;	//玩家2进度

extern HWND hwnd;	                    //窗口句柄
extern int num_total_char;				//总字符数
extern Path path;
extern Camera camera_scene;             //scene相机跟随场景，ui相机固定
extern int win_width, win_height;       //窗口宽高

extern int idx_line;						//当前行数
extern int idx_char;						//当前字符数
extern std::vector<std::string> str_line_list;	//文本行列表

extern SOCKET client_socket;            //客户端套接字

extern std::string player_name;			//玩家名字
extern std::string counterpart_name;	//对方玩家名字
extern int id_player;			        //玩家ID
extern int select_room;					//房间ID
int player_game_id;						//玩家游戏ID

/************游戏选场景************/
class GameLanScene : public Scene
{
private:

    int val_countdown = 4;			//倒计时 4 s
    Stage stage = Stage::Waiting;	//当前游戏状态
    Timer timer_countdown;  //倒计时计时器

    WyjButton* btn_back;		//返回按钮
    WyjButton* btn_exit;		//返回按钮
    WyjButton* btn_text;		//文本按钮

public:
    GameLanScene()
    {
        //按钮初始化
        btn_exit = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("退出"));
        btn_back = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_up_2"), *ResourcesManager::getInstance()->findImage("ui_button_down_2"), _T("返回"));
        btn_text = new WyjButton(*ResourcesManager::getInstance()->findImage("ui_button_selector_up"), *ResourcesManager::getInstance()->findImage("ui_button_selector_down"), _T(" "));

        btn_exit->setPosition({ (float)(50), (float)(25) });
        btn_text->setPosition({ (float)(getwidth() / 2), (float)(getheight() / 2) });
        btn_back->setPosition({ (float)(getwidth() / 2 + 100), (float)(getheight() / 2 + 60) });

        //初始化起跑倒计时
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
                    play_audio(_T("bgm"), true);     //播放背景音乐,循环播放
                }
            });


        CharacterManager::getInstance()->getPlayer_1()->set_position({ 842, 842 });
        CharacterManager::getInstance()->getPlayer_2()->set_position({ 842, 842 });
    }
    ~GameLanScene()
    {
        delete btn_exit;
        delete btn_text;
        delete btn_back;

    }

    //用于进入场景时执行的操作
    void onEnter()override
    {
        btn_exit->setClicked(false);
        btn_back->setClicked(false);
        btn_text->setClicked(false);
    }

    //用于场景更新时执行的操作
    void onUpdate(float delta)override
    {
        //std::cout<<"onUpdate"<<std::endl;
        if (stage == Stage::Waiting)
        {
            if (progress_1 >= 0 && progress_2 >= 0)
                stage = Stage::Ready;
        }
        else if (stage == Stage::Ready)
        {
            timer_countdown.on_update(delta);
            //更新角色的目标
            CharacterManager::getInstance()->onUpdate(delta);

            CharacterManager::getInstance()->getPlayer_1()->set_target(path.get_position_at_progress((float)progress_1 / num_total_char));
            CharacterManager::getInstance()->getPlayer_2()->set_target(path.get_position_at_progress((float)progress_2 / num_total_char));

            camera_scene.look_at(CharacterManager::getInstance()->getPlayer_1()->get_position());
        }
        else if (stage == Stage::Racing)
        {
            if (progress_2 == -1)
            {
                stop_audio(_T("bgm"));
                play_audio(_T("1p_win"));
                stage = Stage::End;
            }
                

            if (progress_1 >= num_total_char)
            {
                stop_audio(_T("bgm"));
                play_audio(_T("1p_win"));
                stage = Stage::End;
            } 
            else if (progress_2 >= num_total_char)
            {
                stop_audio(_T("bgm"));
                play_audio(_T("2p_win"));
                stage = Stage::End;
            }

            //更新角色的目标
            CharacterManager::getInstance()->onUpdate(delta);

            CharacterManager::getInstance()->getPlayer_1()->set_target(path.get_position_at_progress((float)progress_1 / num_total_char));
            CharacterManager::getInstance()->getPlayer_2()->set_target(path.get_position_at_progress((float)progress_2 / num_total_char));

            camera_scene.look_at(CharacterManager::getInstance()->getPlayer_1()->get_position());

        }
        else if (stage == Stage::End)
        {
            CharacterManager::getInstance()->getPlayer_1()->set_target(path.get_position_at_progress((float)progress_1 / num_total_char));
            CharacterManager::getInstance()->getPlayer_2()->set_target(path.get_position_at_progress((float)progress_2 / num_total_char));

            camera_scene.look_at(CharacterManager::getInstance()->getPlayer_1()->get_position());

            if (progress_1 >= num_total_char)
            {
                btn_text->setText(_T("你赢了，游戏结束"));
            }
            else if (progress_2 >= num_total_char)
            {
                btn_text->setText(_T("你输了，游戏结束"));
            }
            else if (progress_2 == -1)
            {
                btn_text->setText(_T("对方已退出，游戏结束"));
            }
        }
            
    }

    //用于场景绘制时执行的操作
    void onRender(Camera& camera_ui)override
    {
        //std::cout << "onRender" << std::endl;
        if (stage == Stage::Waiting)
        {
            settextcolor(RGB(195, 195, 195));
            outtextxy(15, 675, _T("等待玩家加入..."));
        }
        else if (stage == Stage::Ready)
        {
            //绘制背景图
            IMAGE& img_bg = *ResourcesManager::getInstance()->findImage("background");
            const Rect rect_bg =
            {
                0,0,
                img_bg.getwidth(),
                img_bg.getheight()
            };
            putimage_ex(camera_scene, &img_bg, &rect_bg);

            //渲染角色
            CharacterManager::getInstance()->onRender(camera_scene);

            //绘制倒计时
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
            //绘制背景图
            IMAGE& img_bg = *ResourcesManager::getInstance()->findImage("background");
            const Rect rect_bg =
            {
                0,0,
                img_bg.getwidth(),
                img_bg.getheight()
            };
            putimage_ex(camera_scene, &img_bg, &rect_bg);

            //渲染角色
            CharacterManager::getInstance()->onRender(camera_scene);
            
            //绘制界面
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


            static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;   //UTF-8编码转换器
            std::wstring wstr_line = convert.from_bytes(str_line_list[idx_line]);       //UTF-8编码转换为宽字符
            std::wstring wstr_completed = convert.from_bytes(str_line_list[idx_line].substr(0, idx_char));  //UTF-8编码转换为宽字符

            settextcolor(RGB(125, 125, 125));
            outtextxy(rect_textbox_1.x + 150 + 2, rect_textbox_1.y + 65 + 2, wstr_line.c_str());
            settextcolor(RGB(25, 25, 25));
            outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_line.c_str());
            settextcolor(RGB(0, 149, 217));
            outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_completed.c_str());

            btn_exit->onRender(camera_ui);

        }
        else if (stage == Stage::End)
        {
            //绘制背景图
            IMAGE& img_bg = *ResourcesManager::getInstance()->findImage("background");
            const Rect rect_bg =
            {
                0,0,
                img_bg.getwidth(),
                img_bg.getheight()
            };
            putimage_ex(camera_scene, &img_bg, &rect_bg);

            //渲染角色
            CharacterManager::getInstance()->onRender(camera_scene);
            
            btn_text->onRender(camera_ui);
            btn_back->onRender(camera_ui);
        }
    }

    //用于场景输入时执行的操作
    void onInput(const ExMessage& msg) override
    {
        //std::cout << "onInput" << std::endl;
        

        if (msg.message == WM_LBUTTONDOWN)
        {
            //获取游戏内的信息更新
            PDU* uiPDU = mkPDU(0);
            uiPDU->room_id = select_room;
            uiPDU->uiPDUType = EXIT_GAME_LAN;
            uiPDU->player_game_id = id_player;
            strcpy_s(uiPDU->caData, player_name.c_str());

            if (btn_exit->isClicked({ (float)msg.x, (float)msg.y }))
            {
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
                stop_audio(_T("bgm"));
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_LAN);
            }

            if (stage == Stage::End && btn_back->isClicked({ (float)msg.x, (float)msg.y }))
            {
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
                stop_audio(_T("bgm"));
                SceneManager::getInstance()->switchTo(SceneManager::SceneType::SELECTOR_LAN);
            }
            
            free(uiPDU);
            uiPDU = nullptr;
        }

        if (stage != Stage::Racing)
        {
            return;
        }

        if (msg.message == WM_CHAR && idx_line < str_line_list.size())
        {
            std::cout << "input: " << msg.ch << std::endl;
            const std::string& str_line = str_line_list[idx_line];
            if (str_line[idx_char] == msg.ch)   //输入正确,其中msg.ch为输入的字符
            {
                switch (rand() % 4)
                {
                case 0: play_audio(_T("click_1")); break;
                case 1: play_audio(_T("click_2")); break;
                case 2: play_audio(_T("click_3")); break;
                case 3: play_audio(_T("click_4")); break;
                }
              
                progress_1++;

                idx_char++;
                if (idx_char >= str_line.length())
                {
                    idx_char = 0;
                    idx_line++;
                }

                //将进度发送给对方
                //获取游戏内的信息更新
                PDU* uiPDU = mkPDU(0);
                uiPDU->room_id = select_room;
                uiPDU->uiPDUType = GET_GAME_INFO;
                uiPDU->opposing_player = progress_1;
                strcpy_s(uiPDU->caData, player_name.c_str());

                //获取游戏数据列表信息
                send(client_socket, (char*)uiPDU, uiPDU->uiPDULen, 0);
                free(uiPDU);
                uiPDU = nullptr;

            }
            std::cout << ": " << msg.ch << std::endl;
        }
    }

    //用于退出场景时执行的操作
    void onExit() override
    {
        progress_1 = -1;
        progress_2 = -1;

        stage = Stage::Waiting;
        idx_line = 0;
        idx_char = 0;
        val_countdown = 4;

        timer_countdown.restart();

        CharacterManager::getInstance()->getPlayer_1()->set_position({ 842, 842 });
        CharacterManager::getInstance()->getPlayer_2()->set_position({ 842, 842 });

    }

};






#endif // GAME_LAN_SCENE_H