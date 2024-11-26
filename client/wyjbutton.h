#ifndef _WYJBUTTON_H_
#define _WYJBUTTON_H_

#include "vector2.h"
#include "camera.h"
#include "util.h"

#include <string>
#include <graphics.h>	//easyX��ͷ�ļ�
#include <iostream>

//ʵ�ְ�ť�Ĺ���
class WyjButton
{
public:

    WyjButton(IMAGE& image_up, IMAGE& image_down, LPCTSTR text = _T("New Button")){
        this->image_up = image_up;
        this->image_down = image_down;
        this->text = text;

        size.x = (float)image_up.getwidth();
        size.y = (float)image_up.getheight();

    }
    ~WyjButton()
    {

    }

    void setClicked(bool is_clicked = false)
    {
        this->is_clicked = is_clicked;
    }

    void setPosition(Vector2 position)
    {
        this->position.x = position.x - size.x / 2;
        this->position.y = position.y - size.y / 2;
    }
    void setText(LPCTSTR text)
    {
        this->text = text;
    }

    Vector2 getPosition()
    {
        return position;
    }

    //��ť�Ƿ񱻵��
    bool isClicked(Vector2 mouse_position)
    {
        bool in_button_x = mouse_position.x >= position.x && mouse_position.x <= position.x + size.x;
        bool in_button_y = mouse_position.y >= position.y && mouse_position.y <= position.y + size.y;

        if (in_button_x && in_button_y)
        {
            play_audio(_T("click_1"));
            is_clicked = true;
        }

        return in_button_x && in_button_y;
    }


    void onRender(Camera& camera)
    {
        Rect rect_button =
        {
            (int)position.x,
            (int)position.y,
            (int)size.x,
            (int)size.y
        };

        if (!is_clicked)
        {
            putimage_ex(camera, &image_up, &rect_button);
        }
        else
        {
            putimage_ex(camera, &image_down, &rect_button);
        }

        settextcolor(RGB(125, 125, 125));
        outtextxy((int)(position.x + (size.x - textwidth(text)) / 2), (int)(position.y + (size.y - textheight(text)) / 2), text);
        settextcolor(RGB(25, 25, 25));
        outtextxy((int)(position.x + (size.x - textwidth(text)) / 2 - 1), (int)(position.y + (size.y - textheight(text)) / 2 - 1), text);
    }

private:
    Vector2 size;       //��ť�Ĵ�С
    Vector2 position;   //��ť��λ��
    bool is_clicked = false;     //��ť�Ƿ񱻵��


    IMAGE image_up;    //��ť��ͼƬ
    IMAGE image_down;  //��ť�İ���ͼƬ
    LPCTSTR text;   //��ť���ı�

};
















#endif // !_WYJBUTTON_H_