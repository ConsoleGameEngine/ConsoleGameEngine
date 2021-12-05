# **Consola Prod**

## Description

Consola Prod is a simple Microsoft terminal based engine created for implementing complex algorithms and demonstrate how they work.

## Version

It's the second version of this engine (2.0)

## Documentation

### Example

Let's go through simple example:

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

Firstly, we need to define ```DEF_CP_APP ``` because engine needs to know if you are using only it and protect you from unexpected errors with function overloading.

Then, we need to include ```ConsoleProd.hpp``` file. 

After including file you need to create a class that publicly inherits from ```ConsoleProd``` class from namespace `def`. In this class firstly we need to override a constructor and set title of a window and then two pure functions: `OnUserCreate` and `OnUserUpdate`, second function gets as argument delta time value (`dt`) . All these functions return `true` by default, but when you return false in some of them, the application stops. Notice that `OnUserCreate` is called only once and `OnUserUpdate` every clock of your CPU.

In `main` function we need to create an object of our class and check if there is enough memory for creating window and your monitor supports this resolution. Function `Construct` takes as arguments 4 values: width of window, height of window, width of one pixel and height of one pixel. If function returns positive value we start our application with `Start` function, else we give a error message.

### Functions' descriptions

Notice that `c` argument is a type of pixel which you can get from `def::Pixel` namespace and `col` argument is a color of pixel which you can get from `def::FG` or `def::BG` namespaces. 

1. Draw 

   - Draws one pixel on set coordinates on the screen.

2. DrawLine

   - Draws line using 2 points on the screen.

3. DrawTriangle

   - Draws triangle using 3 points on the screen.

4. FillTriangle

   - Draws filled triangle using 3 points on the screen.

5. DrawRectangle

   - Draws rectangle using 2 point on the screen (first point is in top left corner, second point is in bottom right corner).

6. DrawRectangleS

   - Draws rectangle using 2 points on the screen, but in this case first point is in top left corner and second value

     means size of this rectangle.

7. FillRectangle

   - Draws filled rectangle using 2 point on the screen (first point is in top left corner, second point is in bottom right corner).

8. FillRectangleS

   - Draws filled rectangle using 2 points on the screen, but in this case first point is in top left corner and second value means size of this rectangle.

9. DrawCircle

   - Draws circle using 1 point and radius, first argument is point that tell to engine where the center of circle is and second argument is a radius of this circle.

10. FillCircle

   - Draws filled circle using 1 point and radius, first argument is point that tell to engine where the center of circle is and second argument is a radius of this circle.

11. DrawSprite

    -  Draws sprite on the screen, first argument is position and second argument is a pointer to class `Sprite`.

12. DrawPartialSprite

    - Draws sprite on the screen, first argument is position on the screen, next 2 arguments are the position in the file, last argument is a pointer to class `Sprite`

13. DrawPartialSpriteS

    - Draws sprite on the screen, first argument is position on the screen, next 2 arguments are the position and size of sprite in the file, last argument is a pointer to class `Sprite`.

14. DrawString

    - One tip - takes as second argument UNICODE string, e.g. `L"Hello, World!"`.

15. Clear

    - Erase all data from the screen and fill it with passing color. 

16. Focused

    - Returns `true` value if you are focused on terminal, else returns `false`.

17. GetMouse

    - Returns position of mouse in terminal

18. GetScreenSize

    - Returns size of screen

19. AnyKey...

    - Returns `std::vector<int>` of ...  keys

### Keyboard and mouse

Consola Prod provides 256 keys and 5 mouse buttons, each key and button has 3 different states: `bReleased`(when button was released), `bPressed`(when button was pressed) and `bHeld`(when button was held).

Example:

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

5.12.2021 18:30 MOSCOW

