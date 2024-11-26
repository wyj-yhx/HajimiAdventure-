#include "resources_manager.h"

ResourcesManager* ResourcesManager::manager = nullptr;

ResourcesManager::ResourcesManager() = default;
ResourcesManager::~ResourcesManager() = default;

ResourcesManager* ResourcesManager::getInstance()
{
    if (manager == nullptr)
        manager = new ResourcesManager();
    return manager;
}


//���ͼƬ��Դ�Ƿ���Ч
static inline bool checkImageValid(IMAGE* image)
{
    return GetImageBuffer(image);       //����ͼƬ�Ļ�����ָ��,���Ϊ�����ʾͼƬ��Ч
}

//���������ҵ�ͼ��
Atlas* ResourcesManager::findAtlas(const std::string& id) const
{
    const auto& itor = atlas_pool.find(id);
    if (itor == atlas_pool.end())
        return nullptr;
    return itor->second;
}

//���������ҵ�ͼƬ
IMAGE* ResourcesManager::findImage(const std::string& id) const
{
    const auto& itor = image_pool.find(id);
    if (itor == image_pool.end())
        return nullptr;
    return itor->second;
}


//ͼƬ��Դ��Ϣ�������ṹ
struct ImageResInfo
{
    std::string id;
    LPCTSTR path;
};

//ͼ����Դ��Ϣ�������ṹ
struct AtlasResInfo
{
    std::string id;
    LPCTSTR path;
    int num_frame = 0;
};

//��������#include <filesystem> ʵ����ȫ�Զ�������Դ���� 
//ͼƬ���ļ��������б�
static const std::vector<ImageResInfo> image_info_list =
{
    //ʹ��R��ǣ���ʾ�ַ���Ϊԭʼ�ַ�������������ת��
    {"background",        _T(R"(resources\background\background.png)")},
    {"background_menu",   _T(R"(resources\background\background_menu.png)")},
    {"background_selector",  _T(R"(resources\background\background_selector.png)")},


    {"ui_1",              _T(R"(resources\ui\ui_1.png)")},
    {"ui_2",              _T(R"(resources\ui\ui_2.png)")},
    {"ui_3",              _T(R"(resources\ui\ui_3.png)")},
    {"ui_fight",          _T(R"(resources\ui\ui_fight.png)")},
    {"ui_textbox",        _T(R"(resources\ui\ui_textbox.png)")},
    {"ui_textbox_1",      _T(R"(resources\ui\ui_textbox_1.png)")},
    {"ui_textbox_2",      _T(R"(resources\ui\ui_textbox_2.png)")},
    {"ui_textbox_3",      _T(R"(resources\ui\ui_textbox_3.png)")},
    {"ui_selector_bg",    _T(R"(resources\ui\ui_selector_bg.png)")},

    {"ui_button_up",            _T(R"(resources\ui\ui_button_up.png)")},
    {"ui_button_down",          _T(R"(resources\ui\ui_button_down.png)")},
    {"ui_button_up_2",          _T(R"(resources\ui\ui_button_up_2.png)")},
    {"ui_button_down_2",        _T(R"(resources\ui\ui_button_down_2.png)")},
    {"ui_button_selector_up",   _T(R"(resources\ui\ui_button_selector_up.png)")},
    {"ui_button_selector_down", _T(R"(resources\ui\ui_button_selector_down.png)")},
    {"ui_button_text_up",       _T(R"(resources\ui\ui_button_text_up.png)")},
    {"ui_button_text_down",     _T(R"(resources\ui\ui_button_text_down.png)")},
};

//ͼƬ���ļ��������б�
static const std::vector<AtlasResInfo> atlas_info_list =
{
    //ʹ��R��ǣ���ʾ�ַ���Ϊԭʼ�ַ�������������ת��
    {"hajimi_idle_up",      _T(R"(resources\hajimi\idle_back\hajimi_idle_back_%d.png)"),     4},
    {"hajimi_idle_down",    _T(R"(resources\hajimi\idle_front\hajimi_idle_front_%d.png)"),   4},
    {"hajimi_idle_left",    _T(R"(resources\hajimi\idle_left\hajimi_idle_left_%d.png)"),     4},
    {"hajimi_idle_right",   _T(R"(resources\hajimi\idle_right\hajimi_idle_right_%d.png)"),   4},
    {"hajimi_run_up",       _T(R"(resources\hajimi\run_back\hajimi_run_back_%d.png)"),       4},
    {"hajimi_run_down",     _T(R"(resources\hajimi\run_front\hajimi_run_front_%d.png)"),     4},
    {"hajimi_run_left",     _T(R"(resources\hajimi\run_left\hajimi_run_left_%d.png)"),       4},
    {"hajimi_run_right",    _T(R"(resources\hajimi\run_right\hajimi_run_right_%d.png)"),     4},

    {"manbo_idle_up",       _T(R"(resources\manbo\idle_back\manbo_idle_back_%d.png)"),       4},
    {"manbo_idle_down",     _T(R"(resources\manbo\idle_front\manbo_idle_front_%d.png)"),     4},
    {"manbo_idle_left",     _T(R"(resources\manbo\idle_left\manbo_idle_left_%d.png)"),       4},
    {"manbo_idle_right",    _T(R"(resources\manbo\idle_right\manbo_idle_right_%d.png)"),     4},
    {"manbo_run_up",        _T(R"(resources\manbo\run_back\manbo_run_back_%d.png)"),         4},
    {"manbo_run_down",      _T(R"(resources\manbo\run_front\manbo_run_front_%d.png)"),       4},
    {"manbo_run_left",      _T(R"(resources\manbo\run_left\manbo_run_left_%d.png)"),         4},
    {"manbo_run_right",     _T(R"(resources\manbo\run_right\manbo_run_right_%d.png)"),       4},
    
};



//��Դ���غ���
void ResourcesManager::loadResources()
{
    AddFontResourceEx(_T("resources/IPix.ttf"), FR_PRIVATE, NULL);

    for (const auto& info : image_info_list)
    {
        IMAGE* image = new IMAGE();
        loadimage(image, info.path);
        if (!checkImageValid(image))
            throw info.path;        //�׳��쳣��������ͼƬ·��
        image_pool[info.id] = image;
    }

    for (const auto& info : atlas_info_list)
    {
        Atlas* atlas = new Atlas();
        atlas->load(info.path, info.num_frame);
        for (int i = 0; i < atlas->get_size(); ++i)
        {
            IMAGE* image = atlas->get_image(i);
            if (!checkImageValid(image))
                throw info.path;        //�׳��쳣��������ͼ��·��
        }
        atlas_pool[info.id] = atlas;

    }

    //������Ƶ
    load_audio(_T("resources/audio/bgm_menu.mp3"),   _T("bgm_menu"));
    load_audio(_T("resources/audio/bgm.mp3"),        _T("bgm"));
    load_audio(_T("resources/audio/1p_win.mp3"),     _T("1p_win"));
    load_audio(_T("resources/audio/2p_win.mp3"),     _T("2p_win"));
    load_audio(_T("resources/audio/click_1.mp3"),    _T("click_1"));
    load_audio(_T("resources/audio/click_2.mp3"),    _T("click_2"));
    load_audio(_T("resources/audio/click_3.mp3"),    _T("click_3"));
    load_audio(_T("resources/audio/click_4.mp3"),    _T("click_4"));
    load_audio(_T("resources/audio/ui_1.mp3"),       _T("ui_1"));
    load_audio(_T("resources/audio/ui_2.mp3"),       _T("ui_2"));
    load_audio(_T("resources/audio/ui_3.mp3"),       _T("ui_3"));
    load_audio(_T("resources/audio/ui_fight.mp3"),   _T("ui_fight"));
}