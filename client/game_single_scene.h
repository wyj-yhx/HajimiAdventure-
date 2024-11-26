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

#include <chrono>       //时间头文件
#include <sstream>      //字符串流头文件
#include <codecvt>      //编码转换头文件



//全局变量定义
extern std::atomic<int> progress_1;	//玩家1进度	//atomic用于多线程

extern HWND hwnd;	                    //窗口句柄
extern int num_total_char;				//总字符数
extern Path path;
extern Camera camera_scene;             //scene相机跟随场景，ui相机固定
extern int win_width, win_height;       //窗口宽高

extern int idx_line;						//当前行数
extern int idx_char;						//当前字符数
extern std::vector<std::string> str_line_list;	//文本行列表

extern std::string player_name;			//玩家名字
extern Character* player_single;		//玩家指针

/************游戏选场景************/
class GameSingleScene : public Scene
{
private:

    int val_countdown = 4;			//倒计时 4 s
    Stage stage = Stage::Waiting;	//当前游戏状态
    Timer timer_countdown;          //倒计时计时器

    int correctness = 0;       //正确率
    int num_minute = 0;        //打字速度：字/分钟
    int num_char = 0;               //打字总字数

    std::chrono::steady_clock::time_point start_clock;   //打字开始时间
    std::chrono::steady_clock::time_point frame_clock;   //打字帧时间
    std::chrono::duration<float> delta_clock;            //打字时间差

    WyjButton* btn_back;		//返回按钮
    WyjButton* btn_exit;		//返回按钮
    WyjButton* btn_text;		//文本按钮

public:
    GameSingleScene()
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

    }
    ~GameSingleScene()
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

        start_clock = std::chrono::steady_clock::now();  //上一帧时间点

    }

    //用于场景更新时执行的操作
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
            //更新角色的目标
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

            //更新角色的目标
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
                btn_text->setText(_T("恭喜，完成挑战！"));
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
            outtextxy(15, 675, _T("等待玩家进入..."));
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
            player_single->on_render(camera_scene);

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
            player_single->on_render(camera_scene);

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
            std::wstring wstr_minute = convert.from_bytes("TypingSpeed: " + std::to_string(num_minute) + " n/m");       //UTF-8编码转换为宽字符
            std::wstring wstr_correctness = convert.from_bytes("Correctness: " + std::to_string(correctness) + " %");       //UTF-

            //绘制打字文本
            settextcolor(RGB(125, 125, 125));
            outtextxy(rect_textbox_1.x + 150 + 2, rect_textbox_1.y + 65 + 2, wstr_line.c_str());
            settextcolor(RGB(25, 25, 25));
            outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_line.c_str());
            settextcolor(RGB(0, 149, 217));
            outtextxy(rect_textbox_1.x + 150, rect_textbox_1.y + 65, wstr_completed.c_str());

            //绘制右上角数据
            settextcolor(RGB(255, 0, 0));
            outtextxy(getwidth() - 280, 10, wstr_minute.c_str());
            outtextxy(getwidth() - 280, 40, wstr_correctness.c_str());

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
            player_single->on_render(camera_scene);

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
                //正确率
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

    //用于退出场景时执行的操作
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