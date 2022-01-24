# **Consola Prod**

## Description

Consola Prod - это простой игровой движок сделанный в командной строке Windows для создания сложных алгоритмов и их демонстрации.

## Version 2.0

## Documentation

### Example

Давайте рассмотрим простой пример:

```c++
#define DEF_CP_APP
#include "ConsolaProd.hpp"

class Example : public def::ConsolaProd
{
public:
	Example()
	{
		sAppName = L"Example";
	}
protected:
	virtual bool OnUserCreate() override
	{
		return true;
	}
	virtual bool OnUserUpdate(float dt) override
	{
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	else
		std::wcerr << "Could not construct console" << std::endl;
	return 0;
}
```

Сначала, нам нужно объявить ```DEF_CP_APP ```, потому что движок должен знать, что ты используешь только его, и чтобы защитить вас от непредвиденных перегрузок функций.

Затем нам нужно подключить файл ```ConsoleProd.hpp```. 

После нам нужно создать класс, который публично наследует от ```ConsoleProd``` класса из пространства имен `def`. В этом случае нам сначала нужно переопределить конструктор, чтобы задать название нашего приложения и затем проделать тоже самое с функциями: `OnUserCreate` и `OnUserUpdate`, вторая функция получает параметром аргумент дельта-время. По умолчанию эти функцию возвращают true, но когда ты пытаешься указать false приложение останавливается. Заметьте, что `OnUserCreate` вызывается только один раз, а `OnUserUpdate` на каждом обновлении вашего процессора.

В `main` функции создаем объект нашего класса и проверяем достаточно ли памяти для создания окна, поддерживает ли ваш монитор такое разрешение. Функция `Construct` принимает 4 аргумента: ширину окна, высоту окна, ширину пикселя, высоту пикселя. Если функция возвращает положительное значение то мы запускаем наше приложение с помощью метода `Start`, а иначе мы выводим сообщение об ошибке.

### Functions' descriptions

Заметьте, что `c` - это тип пикселя, который вы можете получить из `def::Pixel` и аргумент `col` - это цвет пикселя, который можно получить из `def::FG` или из `def::BG`. 

1. Draw 

   - Меняет цвет пикселя по заданным координатам.

2. DrawLine

   - Рисует линию по 2 точки.

3. DrawTriangle

   - Рисует треугольник по 3 точкам.

4. FillTriangle

   - Рисует закрашенный треугольник по 3 точкам.

5. DrawRectangle

   - Рисует прямоугольник по 2 точкам (первая - левый верхний угол, вторая - правый нижний угол).

6. DrawRectangleS

   - В данном случае рисует прямоугольник по двум точкам, но уже по размеру.

7. FillRectangle

   - Рисует закрашенный прямоугольник по 2 точкам (первая - левый верхний угол, вторая - правый нижний угол).

8. FillRectangleS

   - В данном случае рисует закрашенный прямоугольник по двум точкам, но уже по размеру.

9. DrawCircle

   - Рисует круг по одной точке (центру окружности) и радиусу.

10. FillCircle

   - Рисует закрашенный круг по одной точке (центру окружности) и радиусу.

11. DrawSprite

    - Рисует спрайт, используя объект класса Sprite.

12. DrawPartialSprite

    - Рисует спрайт, используя объект класса Sprite, первый аргумент - это позиция на экране, а другие два аргумента - позиция и размер в файле. Используются координаты.

13. DrawPartialSpriteS

    - Рисует спрайт, используя объект класса Sprite, первый аргумент - это позиция на экране, а другие два аргумента - позиция и размер в файле. Используется размер.

14. DrawString

    - Одна фишка - принимает параметром UNICODE строку, например `L"Hello, World!"`.

15. Clear

    - Очищает экран. 

16. Focused

    - Возвращает `true`, если командная строка в фокусе, иначе `false`.

17. GetMouse

    - Возвращает позицию курсора в командной строке.

18. GetScreenSize

    - Возвращает размер экрана.

19. AnyKey...

    - Возвращает `std::vector<int>` ... клавиш.

### Keyboard and mouse

Consola Prod предоставляет 256 клавиш и 5 кнопок на мыше, у каждой клавиши или кнопки 3 состояния: `bReleased`(когда кнопка была опущена), `bPressed`(когда кнопка была поднята) и `bHeld`(когда кнопка была зажата).

Пример:

```c++
if (keys[L'A'].bHeld)
{
  vPlayer.x -= 30.0f * dt;
}

if (mouse[0].bPressed)
{
  Draw(GetMouse(), def::Pixel::SOLID, def::FG::RED);
}
```

## License

BSD 3-Clause License

Copyright (c) 2021, Alex
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Last Update

24.01.2022 18:15 MOSCOW

