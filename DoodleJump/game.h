// Copyright (C) 2010 Lyutov Sergey
// This file is part of the "elirTet".
// For conditions of distribution and use, see copyright notice in game.cpp

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <irrlicht.h>
#include <irrKlang.h>

using namespace irr;

//размер кубика из которого состоят фигуры в пикселях
#define size 20

//смещение стакана от левого верхнего угла в пикселях
#define stock_x 10
#define stock_y 10


// типы фигур
enum ShapeType { line = 0, square, rightL, leftL, pyramide, leftZ, rightZ};

enum GAME_IDS
{
    GAME_ID_RESTART = 200,
    GAME_ID_MENU,
    GAME_ID_USETEX,
    GAME_ID_USESOUND,
    GAME_ID_SHOWNEXT,
    GAME_ID_UNPAUSE,
    GAME_ID_EXPLOSION
};

class Game : public IEventReceiver
{
    public:
        Game(IrrlichtDevice *Device, irrklang::ISoundEngine* Esound);

        //обработчик событий игры
        virtual bool OnEvent(const SEvent& event);

        void startNew(); //инициализирует и стартует новую игру


        // переменная через которую класс Game сообщает
        // менеджеру состояний, на какое надо переключится
        // тут только вариант 1 -  выход в меню
        s32 nstate;

        void update(const f32 delta); // обновляет состояние игры
        void render(); // рисует игру

    private:
        IrrlichtDevice* device;
        irrklang::ISoundEngine* esound;


        gui::IGUIStaticText* gui_score; // статическое поле для набранных очков
        gui::IGUIStaticText* gui_level; // статическое поле для набранного уровня скорости
        video::ITexture* cubes; // текстура с цветными кубиками
        video::ITexture* skin; // скин интерфейса игры
        video::ITexture* fx_explose; // текстура используемая для изображения взрыва

        bool isUseTextures; // режим использования текстур, иначе рисование цветом
        bool isShowNext; // режим показа следующей фигуры
        bool isUseSound; // вкл/выкл звука

        bool		cells[4][4]; // 4x4 матрица для хранения фигуры
        ShapeType	curType;     // текущая фигура
        s32 color;              // цвет текущей фигуры

        bool		nextCells[4][4]; // 4x4 матрица для хранения фигуры
        ShapeType	nextType; // следующая фигура
        s32 nextColor; // цвет следующей фигуры

        short	stock[20][10]; // матрица "стакана"
        s32	    curY; // положение фигуры по Y
        s32	    curX; // положение фигуры по X
        bool	bGame; // если false то игра закончена
        s32	    score; // здесь храним набранные очки
        s32     level; // здесь храним уровень скорости падения фигур

        bool accelerate; // вкл/выкл ускоренного падения фигуры
        bool isPause; // вкл/выкл паузы в игре
        f32 speed; // текущая скорость падения
        f32 timer; // в связке со speed не дает фигуре моментально упасть на дно см. update()

        void shapeNew(); // создает новую фигуру вверху "стакана"
        void shapeRotate(); // вращает фигуру
        void shapeAccelerate(bool a); //вкл. ускоренного падения фигуры
        video::SColor shapeColor(s32 color); // возвращает цвет по индексу
        core::rect<s32> shapeRect(s32 color); // возвращает координаты цвета в текстуре по индексу
        void shapeClear( bool c[4][4] ); //обнуляет матрицу текущей фигуры
        void shapeFill( bool c[4][4], ShapeType tip ); // заполняет матрицу фигуры указанным типом формы

        void levelUp(); // поднимает уровень скорости падения
        void levelDown(); // снижает уровень скорости падения
        void endGame(); // заканчивает игру
        void pauseGame(); // приостанавливает игру

        bool checkLeft(); // проверяет возможность движения влево
        bool checkRight(); // проверяет возможность движения вправо

        void checkForLine(); // удаляет полные строки
        // добавляет эффект взрыва для удаляемой строки
        void exploseLine(s32 x, s32 y, s32 w, s32 h, s32 live, f32 min_size=1.f, f32 max_size=2.f);
        void explosionsRemove(); // принудительно убирает спецэффекты взрывов

        bool IsAtBottom(); // проверка достижения дна фигурой

        void ClearStock(); // очищает "стакан"
};

#endif // GAME_H_INCLUDED
