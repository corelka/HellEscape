// Copyright (C) 2010 Lyutov Sergey
// This file is part of the "elirTet".
// For conditions of distribution and use, see copyright notice in game.cpp

#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

enum
{
        GUI_ID_GAME_EXIT = 101,     // id кнопки выхода из приложения
        GUI_ID_GAME_NEW,             // id кнопки начала новой игры
        GUI_ID_GAME_CONTINUE             // id кнопки продолжения игры
};

class Menu : public IEventReceiver
{
    public:
        // переменная через которую класс Menu сообщает
        // менеджеру состояний, на какое надо переключится
        // вариантов два либо 2 - игра, либо 3 - выход из приложения
        s32 nstate;

        //конструктор
        Menu(IrrlichtDevice *Device)
        {
            device = Device;
            createMenu();

            speed=0;
            timer=0;

            bg = device->getVideoDriver()->getTexture("res/wall.jpg");
        }

        //обработчик событий
        virtual bool OnEvent(const SEvent& e)
        {
            if (e.EventType == EET_GUI_EVENT) // если условие истинно, то это событие интерфейса
            {
                s32 id = e.GUIEvent.Caller->getID(); // получаем идентификатор элемента, вызвавшего событие
                switch(e.GUIEvent.EventType) // обрабатываем события относительно типа
                {
                    case gui::EGET_BUTTON_CLICKED: // событие клика кнопки мыши
                        switch(id)
                        {
                            case GUI_ID_GAME_EXIT: // выход из приложения
                                nstate = 3;
                                return true;
                            break;

                            case GUI_ID_GAME_NEW: // начало новой игры
                                nstate = 2;
                                return true;
                            break;
                        }
                    break;
                }
            }

            // события клавиатуры, если клавиша была отжата
            if (e.EventType == EET_KEY_INPUT_EVENT && !e.KeyInput.PressedDown)
            {
                switch (e.KeyInput.Key)
                {
                    case KEY_ESCAPE: // ESC - выход из приложения
                        nstate=3;
                        return true;
                    break;

                    case KEY_SPACE: // пробел - начало игры
                        nstate=2;
                        return true;
                    break;
                }
            }
            return false;
        }//OnEvent

        // создает меню
        void createMenu()
        {
            gui::IGUIEnvironment* env = device->getGUIEnvironment();
            env->clear(); //удаляем все элементы
            env->addButton( core::rect<s32>(320/2-50, 480/2-40, 320/2+50,480/2-8),  0, GUI_ID_GAME_NEW, L"играть");
            env->addButton( core::rect<s32>(320/2-50, 480/2+8,  320/2+50,480/2+40), 0, GUI_ID_GAME_EXIT, L"выход");
            env->addStaticText(
                       L"www.irrlicht.ru (c) 2010 автор: эльмиго",
                       core::rect<s32>(5, 423, 315, 470),
                       false, true, 0, 1, false
                    );
            device->setEventReceiver( (IEventReceiver*)this );
        }

        void update(const f32 delta)
        {
            // собственно заглушка
        }

        // отрисовка фоновой картинки
        void render()
        {
            video::IVideoDriver* driver = device->getVideoDriver();
            driver->draw2DImage( bg, core::position2d<s32>(0,0) );
        }

    private:
        IrrlichtDevice *device;
        video::ITexture* bg;

        f32 timer;
        f32 speed;
};

#endif // MENU_H_INCLUDED
