// Copyright (C) 2010 Lyutov Sergey
// This file is part of the "elirTet".
// For conditions of distribution and use, see copyright notice in game.cpp


#include "game.h"

/*
    конструктор класса, заполняем поля значениями по умолчанию и
    загружаем ресурсы игрового окна
*/
Game::Game(IrrlichtDevice *Device, irrklang::ISoundEngine* Esound)
{
    device = Device;
    esound = Esound;

    isUseTextures = true;
    isShowNext = false;
    isUseSound = true;

    cubes = device->getVideoDriver()->getTexture("res/cubes.bmp");
    skin = device->getVideoDriver()->getTexture("res/skin.jpg");
    fx_explose = device->getVideoDriver()->getTexture("res/fire.bmp");

    color = nextColor = -1;
}

/*
    обработчки событий клавиатуры и GUI элементов
    для подробностей см. Туториалы №4 и №5 на сайте www.irrlicht.ru
*/
bool Game::OnEvent(const SEvent& e)
{
    //события клавиатуры
    if (e.EventType == EET_KEY_INPUT_EVENT)
    {
        switch (e.KeyInput.Key)
        {
            case KEY_ESCAPE: // ESC - выход в меню
                if (!e.KeyInput.PressedDown)
                {
                    nstate=1;
                    explosionsRemove();
                }
            break;

            case KEY_SPACE: // пробел - пауза в течении игры
                if (!e.KeyInput.PressedDown)
                {
                    if (bGame==false) // если игра была окончена, то старт новой
                    {
                        explosionsRemove();
                        startNew();
                        return true;
                    }
                    isPause = !isPause;
                    if ( isPause ) pauseGame();
                    else device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GAME_ID_UNPAUSE, true)->getParent()->remove();
                }
            break;

            case KEY_UP: // стрелка вверх - вращение фигуры
                if (!e.KeyInput.PressedDown) shapeRotate();
            break;

            case KEY_DOWN: // стрелка вниз отжата - вкл. ускоренного падения фигуры
                if (!e.KeyInput.PressedDown) shapeAccelerate(true);
            break;

            case KEY_LEFT: // стрелка влево - сдвиг фигуры влево
                if (e.KeyInput.PressedDown && !checkLeft()) curX-=20;
            break;

            case KEY_RIGHT: // стрелка влево - сдвиг фигуры вправо
                if (e.KeyInput.PressedDown && !checkRight()) curX+=20;
            break;

            case KEY_PLUS: // плюс - увеличение скорости
                if (!e.KeyInput.PressedDown) levelUp();
            break;

            case KEY_MINUS: // минус - уменьшение скорости
                if (!e.KeyInput.PressedDown) levelDown();
            break;


        }
    }
    // события GUI
    else if (e.EventType == EET_GUI_EVENT)
    {
        s32 id = e.GUIEvent.Caller->getID();
        switch(e.GUIEvent.EventType)
        {
            // кнопки

            case gui::EGET_BUTTON_CLICKED: // кнопка рестарта
                if (id==GAME_ID_RESTART)
                {
                    explosionsRemove();
                    startNew();
                    return true;
                }
                else if (id==GAME_ID_MENU) // кнопка выхода в меню
                {
                    nstate=1;
                    explosionsRemove();
                    return true;
                }
                else if (id==GAME_ID_UNPAUSE) // кнопка отключения паузы
                {
                    isPause = false;
                    e.GUIEvent.Caller->getParent()->remove();
                    return true;
                }
            break;

            // чекбоксы (галочки)

            case gui::EGET_CHECKBOX_CHANGED:
                if (id==GAME_ID_USETEX) //использование текстур
                {
                    isUseTextures = ((gui::IGUICheckBox*)e.GUIEvent.Caller)->isChecked();
                }
                else if (id==GAME_ID_SHOWNEXT) // показ следующей фигуры
                {
                    isShowNext = ((gui::IGUICheckBox*)e.GUIEvent.Caller)->isChecked();
                }
                else if (id==GAME_ID_USESOUND) // использование звука
                {
                    isUseSound = ((gui::IGUICheckBox*)e.GUIEvent.Caller)->isChecked();
                }
            break;
        }
    }

    return false;
}

/*
    инициализация новой игры
*/
void Game::startNew()
{
    // значения по умолчанию
	score=0;
	level=1;
	isPause = false;
    accelerate = false;
	speed=1;
	timer=0;

	ClearStock(); // очищаем стакан
	bGame=true; // игра запущена
	shapeNew(); // создаем новую фигуру вверху стакана

    esound->setSoundVolume(0.3f); // уровень звука

    //устанавливаем обработчик событий
    device->setEventReceiver( (IEventReceiver*)this ); // передаем себя движку как обработчик событий

    //зачищаем интерфейс и рисуем по новой
    gui::IGUIEnvironment* env = device->getGUIEnvironment();
    env->clear(); //удаляем все элементы

    //чисто игровая атрибутика - счет и уровень игрока
    gui_score = env->addStaticText(
                       (core::stringw(L"Счет: ")+core::stringw(score)).c_str(),
                       core::rect<s32>(20, 423, 150, 447),
                       false, false, 0, 1, false
                    );
    gui_score->setOverrideColor( video::SColor(255, 0,255,0) );

    gui_level = env->addStaticText(
                       (core::stringw(L"Уровень: ")+core::stringw(level)).c_str(),
                       core::rect<s32>(20, 448, 150, 475),
                       false, false, 0, 1, false
                    );
    gui_level->setOverrideColor( video::SColor(255, 0,255,0) );


    //демонстрационные фишки
    gui::IGUIStaticText* tmp = env->addStaticText(
               L"Текстуры",
               core::rect<s32>(320-97, 20, 320-5, 45),
               false, false, 0, 1, false
            );
    tmp->setOverrideColor( video::SColor(255, 255,255,255) );
    env->addCheckBox(isUseTextures, core::rect<s32>(320-60, 40, 320-40, 75), 0, GAME_ID_USETEX);

    tmp = env->addStaticText(
               L"Оп-ля!!!",
               core::rect<s32>(320-85, 85, 320-5, 110),
               false, false, 0, 1, false
            );
    tmp->setOverrideColor( video::SColor(255, 255,255,255) );
    env->addCheckBox(isShowNext, core::rect<s32>(320-60, 110, 320-40, 135), 0, GAME_ID_SHOWNEXT);

    tmp = env->addStaticText(
               L"Звук???",
               core::rect<s32>(320-85, 235, 320-5, 265),
               false, false, 0, 1, false
            );
    tmp->setOverrideColor( video::SColor(255, 255,255,255) );
    env->addCheckBox(isUseSound, core::rect<s32>(320-60, 260, 320-40, 280), 0, GAME_ID_USESOUND);

}

/*
    заполняем матрицу в соответствии с запрошенным типом
*/
void Game::shapeFill(bool c[4][4], ShapeType tip)
{
    shapeClear(c);

	switch(tip)
	{
        case line:
            c[0][1] = c[1][1] = c[2][1] = c[3][1] = true;
        break;

        case square:
            c[0][0] = c[0][1] =
            c[1][0] = c[1][1] = true;
        break;

        case leftL:
            c[0][0] = c[0][1] = c[0][2] =
            c[1][0] = true;
        break;

        case rightL:
            c[0][0] = c[0][1] = c[0][2] =
                                c[1][2] = true;
        break;

        case pyramide:
            c[0][0] = c[0][1] = c[0][2] =
                      c[1][1] = true;
        break;

        case leftZ:
            c[0][0] = c[0][1] =
                      c[1][1] = c[1][2] = true;
        break;

        case rightZ:
                      c[0][1] = c[0][2] =
            c[1][0] = c[1][1] = true;
        break;
	}
}//shapeFill

/*
    создает новую фигуру вверху стакана
*/
void Game::shapeNew()
{
	curY=0;
	curX=60;

    srand(device->getTimer()->getTime());

	if (nextColor==-1)
	{
        color = rand()%6;
        curType = ShapeType(rand()%7);
	}
	else
	{
	    color = nextColor;
	    curType = nextType;
	}
    nextColor = rand()%6;
    nextType = ShapeType(rand()%7);

    shapeFill(cells,     curType);
    shapeFill(nextCells, nextType);
}

/*
    вращает падающую фигуру
*/
void Game::shapeRotate()
{
    if (curType==square) return; // исключение: квадрат не вращаем

    s32 curXX = curX;

    bool tempShape[4][4];
	shapeClear(tempShape);

	switch(curType)
	{
        case line:
            // ислючение: линию перекладываем
			if(cells[0][0]==true)
			{
				for(s32 k=0; k<4; k++) tempShape[k][1]=true;
			}
			else
			{
				for(s32 k=0; k<4; k++) tempShape[0][k]=true;
                if (curX==-20) curXX=0;
                else if (curX>139) curXX=120;
			}
		break;

		default:
            //переворачиваем фигуру квадрат 3х3 во временную матрицу
            // 123    360    654
            // 456 -> 250 -> 321
            // 000    140    000
            if ( !cells[2][0] && !cells[2][1] && !cells[2][2] )
            {
                tempShape[0][0]=cells[0][2];
                tempShape[1][0]=cells[0][1];
                tempShape[2][0]=cells[0][0];
                tempShape[0][1]=cells[1][2];
                tempShape[1][1]=cells[1][1];
                tempShape[2][1]=cells[1][0];

                tempShape[0][2]=false;
                tempShape[1][2]=false;
                tempShape[2][2]=false;
            }
            else
            {
                tempShape[0][0]=cells[0][1];
                tempShape[0][1]=cells[1][1];
                tempShape[0][2]=cells[2][1];
                tempShape[1][0]=cells[0][0];
                tempShape[1][1]=cells[1][0];
                tempShape[1][2]=cells[2][0];

                tempShape[2][0]=false;
                tempShape[2][1]=false;
                tempShape[2][2]=false;
            }

            //патчик, если матрица ушла на 2 ед. за край стакана
            if (curX==-20) curXX=0;
            else if (curX==160) curXX=140;
	}

    bool tempStock[4][4];
    shapeClear(tempStock);

    //забираем область "стакана" на которую ляжет перевенутая фигура
    for(s32 y=0; y<4; y++)
    {
        s32 my = y+curY/size;

        for(s32 x=0; x<4; x++)
        {
            s32 mx = x+curXX/size;

            if (mx > -1 && mx < 10) // если матрица фигуры вся в стакане
                tempStock[y][x] = (stock[my][mx]==-1?false:true);
            else // если матрица фигуры вышла за пределы стакана
                tempStock[y][x] = true;
        }
    }

    //сравниваем матрицы
    if (curType==line) //для линии 4х4
    {
        for(s32 y=0; y<4; y++)
            for(s32 x=0; x<4; x++)
                // матрицы пересеклись, поворот запрещен
                if (tempStock[y][x] && tempShape[y][x]) return; //выходим
    }
    else // для остальных фигур 3х3
    {
        for(s32 y=0; y<3; y++)
            for(s32 x=0; x<3; x++)
                // матрицы пересеклись, поворот запрещен
                if (tempStock[y][x] && tempShape[y][x]) return; //выходим
    }

    //помещаем в стакан повернутую фигуру
    shapeClear(cells);
    for(s32 y=0; y<4; y++)
        for(s32 x=0; x<4; x++)
            cells[y][x]=tempShape[y][x];

    curX = curXX;
}//shapeRotate

/*
    очищает матрицу фигуры
*/
void Game::shapeClear( bool c[4][4] )
{
	for( s32 i=0; i<4; i++)
		for( s32 j=0; j<4; j++)
			c[i][j]=false;
}

/*
    включает или выключает ускорение см. update()
*/
void Game::shapeAccelerate(bool a)
{
    accelerate=a;
}

/*
    возвращает цвет относительно запрошенного индекса
*/
video::SColor Game::shapeColor(s32 color)
{
	switch(color)
	{
	case 0: return video::SColor(255, 255,0,0);
	case 1: return video::SColor(255, 0,255,0);
	case 2: return video::SColor(255, 0,0,255);
	case 3: return video::SColor(255, 255,0,255);
	case 4: return video::SColor(255, 0,255,255);
	case 5: return video::SColor(255, 255,255,0);
	case 999: return video::SColor(255, 0,0,0);
	}
	return video::SColor(255, 255,255,255);
}

/*
    в режиме с текстурами, вместо рисования цветом я ессно рисую текстурами
    вернее субтекстурами текстуры cubes.jpg т.е. по запрошенному индексу
    функция возвращает координаты прямоугольника, который описывает положение
    субтекстуры в текстуре
*/
core::rect<s32> Game::shapeRect(s32 color)
{
	switch(color)
	{
        case 0: return core::rect<s32>( 1, 1, 24, 24);
        case 1: return core::rect<s32>(25, 1, 48, 24);
        case 2: return core::rect<s32>(49, 1, 72, 24);
        case 3: return core::rect<s32>(73, 1, 96, 24);
        case 4: return core::rect<s32>(97, 1, 120,24);
        case 5: return core::rect<s32>(121,1, 144,24);
        case 6: return core::rect<s32>(145,1, 168,24);
	}
}

/*
    ощищает "стакан"
*/
void Game::ClearStock()
{
	for(s32 i=0; i<20; i++)
		for(s32 j=0; j<10; j++)
			stock[i][j]=-1;
}

/*
    проверка достижения дна фигурой
*/
bool Game::IsAtBottom()
{
	for(s32 i=0; i<4; i++)
		for(s32 j=0; j<4; j++)
			if(cells[i][j]==true)
				if((curY/20+i)>=19 || stock[curY/20+i+1][curX/20+j]!=-1) return true;

	return false;
}

/*
    удаление полных линий, обновление счетчика очков и
    запуск спецэффетов
*/
void Game::checkForLine()
{
	bool flag; s32 j,c=0;

	for(s32 i=0; i<20; i++)
	{
		flag=true;

		for(j=0; j<10; j++) flag &= (stock[i][j]!=-1);

		if(flag==true && j!=0)
		{
			c++;
			score++;
			gui_score->setText( (core::stringw("Score: ")+core::stringw(score)).c_str() );
			exploseLine(stock_x, i*20+stock_y, stock_x+200, i*20+stock_y+20, 500, 2,5);

			if (score%10==0) levelUp();

			for(s32 k=i; k>0; k--)
				for(s32 j=0; j<10; j++)
					stock[k][j]=stock[k-1][j];
		}
	}

	for(s32 k=0; k<c; k++)
		for(s32 j=0; j<10; j++)
			stock[k][j]=-1;
}

/*
    Функция реализующая спецэффект взрыва заполненной линии
    для подробностей см. Туториалы №8 на сайте www.irrlicht.ru
*/
void Game::exploseLine(s32 x, s32 y, s32 w, s32 h, s32 live, f32 min_size, f32 max_size)
{
    scene::ICameraSceneNode* camera = device->getSceneManager()->getActiveCamera();
    scene::ISceneCollisionManager * cm = device->getSceneManager()->getSceneCollisionManager();

    core::line3d<f32> lt = cm->getRayFromScreenCoordinates(core::position2di(x, y), camera);
    core::line3d<f32> rb = cm->getRayFromScreenCoordinates(core::position2di(w, h), camera);

    scene::IParticleSystemSceneNode* ps = device->getSceneManager()->addParticleSystemSceneNode(false, 0, GAME_ID_EXPLOSION);

    scene::IParticleEmitter* em = ps->createBoxEmitter(
        core::aabbox3d<f32>( lt.end.X,rb.end.Y,0, rb.end.X,lt.end.Y,-3 ), // размер эмиттера(куба)
        core::vector3df(0.0f,0.0f,-0.3f),   // начальное направление
        50,100,                             // частота испускания (мин,макс)
        video::SColor(0 ,255,255,255),       // самый темный цвет
        video::SColor(0, 255,255,255),       // самый яркий цвет
        1000,3000,0,                         // време жизни (мин,макс), угол
        core::dimension2df(min_size, min_size),         // минимальный размер частиц
        core::dimension2df(max_size, max_size));        // максимальный размер частиц

    ps->setEmitter(em); // отдаем эмиттер системе
    em->drop(); // а лично нам он не нужен

    ps->setPosition(core::vector3df(0,0,0));
    ps->setMaterialFlag(video::EMF_LIGHTING, false);
    ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
    ps->setMaterialTexture(0, fx_explose);
    ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

	if (isUseSound) esound->play2D("res/explosion.wav");

    //удаляем взрыв через live мили сек.
    if (live>0)
    {
        scene::ISceneNodeAnimator* anim = device->getSceneManager()->createDeleteAnimator(live);
        ps->addAnimator(anim);
        anim->drop();
    }
}

/*
    принудительное удаление взрывов со сцены при необходимости
*/
void Game::explosionsRemove()
{
    scene::ISceneNode* n;
    while ( n=device->getSceneManager()->getSceneNodeFromId(GAME_ID_EXPLOSION) ) n->remove();
}

/*
    проверка возможности на движение фигуры вправо
*/
bool Game::checkRight()
{
	for(s32 i=0; i<4; i++)
		for(s32 j=0; j<4; j++)
			if(cells[i][j]==true)
				if((curX/20+j)>=9 || stock[curY/20+i][curX/20+j+1]!=-1) return true;

	return false;
}

/*
    проверка возможности на движение фигуры влево
*/
bool Game::checkLeft()
{
	for(s32 i=0; i<4; i++)
		for(s32 j=0; j<4; j++)
			if(cells[i][j]==true)
				if((curX/20+j)<=0 || stock[curY/20+i][curX/20+j-1]!=-1) return true;

	return false;
}

/*
    повышение уровня скорости
    снижаем по 0.1 с 1 до 0.1
    снижаем по 0.01 с 0.1 до 0
*/
void Game::levelUp()
{
    if (speed<=0.1) speed -= 0.01f;
    else speed -= 0.1f;

    if (speed<0.01) speed=0.01f;
    else level++;

    gui_level->setText( (core::stringw(L"Уровень: ")+core::stringw(level)).c_str() );
}

/*
    понижение уровня скорости
    повышаеем зеркально понижению levelUP
*/
void Game::levelDown()
{
    if (speed<0.1) speed += 0.01;
    else speed += 0.1;

    if (speed>1.f) speed=1;
    else level--;
    gui_level->setText( (core::stringw(L"Уровень: ")+core::stringw(level)).c_str() );

}

/*
    окончание игры
*/
void Game::endGame()
{
    bGame=false;

    gui::IGUIEnvironment* env = device->getGUIEnvironment();
    gui::IGUIWindow* win = env->addWindow(
                                        core::rect< s32 >(320/2-100, 480/2-50, 320/2+100, 480/2+50),
                                        false,
                                        L"Опа!",
                                        0, -1
                                    );
    win->getCloseButton()->remove();

    gui::IGUIStaticText* txt = env->addStaticText(
               core::stringw(L"Игра окончена!!!").c_str(),
               core::rect<s32>(10, 20, 195, 45),
               false, false, win, 0, false
            );

    txt->setOverrideColor( video::SColor(255, 199,46,22) );

    env->addButton( core::rect< s32 >(5,60,95,85), win, GAME_ID_MENU, L"В меню" );
    env->addButton( core::rect< s32 >(105,60,195,85), win, GAME_ID_RESTART, L"Играть" );

    exploseLine(10,10, 310,470, 0, 10,15);
}

/*
    пауза в игре
*/
void Game::pauseGame()
{
    gui::IGUIEnvironment* env = device->getGUIEnvironment();
    gui::IGUIWindow* win = env->addWindow(
                                        core::rect< s32 >(320/2-100, 480/2-50, 320/2+100, 480/2+50),
                                        false,
                                        L"Пауза!",
                                        0, -1
                                    );
    win->getCloseButton()->remove();

    env->addButton( core::rect<s32>(10, 50, 195, 75), win, GAME_ID_UNPAUSE, L"в игру ->" );
}

/*
    обновление состояния

    параметр delta - прошедшее кол-во милисекунд между вызовами update
*/
void Game::update(const f32 delta)
{

    if (isPause) return;
    if (bGame==false) return;

    /*
        если включено ускорение двигаем вниз с максимальной скоростью

        если прошло количество секунд равное скорости speed
        то обнуляем timer и двигаем фигуру вниз, иначе увеличиваем timer
        на величину delta  и выходим
    */
    timer += delta;
    if (accelerate)
    {
        if (timer < 0.1) return;
    }
    else
    {
        if (timer < speed ) return;
    }
    timer = 0;

	if(bGame==true) // если идет игра
	{
		if(IsAtBottom()) // фигура достигла дна
		{
			for(s32 i=0; i<4; i++)
				for(s32 j=0; j<4; j++)
					if(cells[i][j]==true)
						stock[curY/20+i][curX/20+j]=color;

           	if (isUseSound) esound->play2D("res/place.wav"); // стучим фигурой об "дно"


			shapeNew(); // создаем новую фигуру
			if(IsAtBottom())
			{
                endGame();
			}
			checkForLine();
			accelerate=false;
			return;
		}
		curY += 20;
	}
}

/*
    рисуем игровой экран, т.е фон и содержимое стакана
    для подробностей смотреть туториал №6
*/
void Game::render()
{
	s32 i,j;

	video::IVideoDriver* driver = device->getVideoDriver();


    if (isUseTextures) //рисуем текстурами
    {
        //рисуем стакан
        driver->draw2DImage( skin, core::position2d<s32>(0,0) );
        driver->draw2DImage( skin, core::position2d<s32>(0,0), core::rect<s32>(1, 1, 320, 480), 0, video::SColor(255, 255, 255, 255), true);

        //рисуем падающую фигуру
        for(i=0; i<4; i++) for(j=0; j<4; j++) if(cells[i][j]==true)
            driver->draw2DImage(
                            cubes,
                            core::rect<s32>(stock_x+curX+j*size, stock_y+curY+i*size, stock_x+curX+(j+1)*size, stock_y+curY+(i+1)*size),
                            shapeRect(color));

        //рисуем фигуру в кеше
        if (isShowNext)
        for(i=0; i<4; i++) for(j=0; j<4; j++) if(nextCells[i][j]==true)
            driver->draw2DImage(
                            cubes,
                            core::rect<s32>(239+j*size, 140+i*size, 239+(j+1)*size, 140+(i+1)*size),
                            shapeRect(nextColor));

        //рисуем фигуры на дне стакана
        for(i=0; i<20; i++)	for(j=0; j<10; j++)	if(stock[i][j]!=-1)
            driver->draw2DImage(
                            cubes,
                            core::rect<s32>(stock_x+j*size, stock_y+i*size, stock_x+(j+1)*size, stock_y+(i+1)*size),
                            shapeRect(stock[i][j]));
    }
    else // рисуем цветами
    {
        //рисуем стакан
        driver->draw2DRectangle(shapeColor(555), core::rect<s32>(stock_x-5,     stock_y,     stock_x,        stock_y+400) );
        driver->draw2DRectangle(shapeColor(555), core::rect<s32>(stock_x+200,   stock_y,     stock_x+200+5,  stock_y+400) );
        driver->draw2DRectangle(shapeColor(555), core::rect<s32>(stock_x-5,     stock_y+400, stock_x+200+5,  stock_y+400+5) );
        driver->draw2DRectangle(shapeColor(999), core::rect<s32>(stock_x,       stock_y,     stock_x+200,    stock_y+400) );

        //рисуем падающую фигуру
        for(i=0; i<4; i++) for(j=0; j<4; j++) if(cells[i][j]==true)
            driver->draw2DRectangle(
                        shapeColor(color),
                        core::rect<s32>(stock_x+curX+j*size, stock_y+curY+i*size, stock_x+curX+(j+1)*size, stock_y+curY+(i+1)*size));

        //рисуем фигуру в кеше
        if (isShowNext)
        for(i=0; i<4; i++) for(j=0; j<4; j++) if(nextCells[i][j]==true)
            driver->draw2DRectangle(
                        shapeColor(nextColor),
                        core::rect<s32>(239+j*size, 140+i*size, 239+(j+1)*size, 140+(i+1)*size));

        //рисуем фигуры на дне стакана
        for(i=0; i<20; i++)	for(j=0; j<10; j++)	if(stock[i][j]!=-1)
            driver->draw2DRectangle(
                                        shapeColor(stock[i][j]),
                                        core::rect<s32>(stock_x+j*size, stock_y+i*size, stock_x+(j+1)*size, stock_y+(i+1)*size) );
    }
}
