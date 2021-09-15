# ConsoleGameEngine description
CGE (Console Game Engine) is an universal tool to create games, GUI or whatever you want.

# Documentation
1. Example (engine supports only .spr files, check [this](https://github.com/defini7/lab/tree/main/Sprite_Editor) for editing .spr files):
	```cpp 
	#include "ConsoleGameEngine.h"

	class Example : public cge::ConsoleGameEngine
	{
	public:
		virtual bool OnUserCreate() override
		{
			spr = new cge::Sprite(L"ball.png");
			return true;
		}

		virtual bool OnUserUpdate(float fDeltaTime) override
		{
			DrawTriangle({ 0, 0 }, { 0, 20 }, { 20, 20 }, PIXEL_SOLID, FG_BLUE);
			DrawLine({10, 10}, {20, 10}, PIXEL_SOLID, FG_RED);
			Draw({ 30, 10 }, PIXEL_SOLID, FG_DARK_GREEN);
			return true;
		}

	private:
		cge::Sprite* spr = nullptr;

	};

	int main()
	{
		Example demo;
		demo.Construct(120, 120, 4, 4);
	}
	```
	
Commands:
- SetTitle(title); -- set title of window, by default it's: "Undefined"
- Clear(COLOUR); -- clears screen
- Draw(pos1, PIXEL_TYPE, COLOUR); -- draws simple point
- DrawSprite(pos, sprite); -- draws sprite
- DrawPartialSprite(pos, file_pos, sprite); -- takes sprite from file and draws it
- DrawLine(pos1, pos2, PIXEL_TYPE, COLOUR); -- draw line
- DrawTriangle(pos1, pos2, pos3, PIXEL_TYPE, COLOUR); -- draw triangle
- FillTriangle(pos1, pos2, pos3, PIXEL_TYPE, COLOUR); -- draws and fills triangle
- DrawRectangle(pos, size, PIXEL_TYPE, COLOUR); -- draws rectangle
- FillRectangle(pos, size, PIXEL_TYPE, COLOUR); -- draws and fills rectangle
- DrawCircle(pos, radius, PIXEL_TYPE, COLOUR); -- draws circle
- FillCircle(pos, radius, PIXEL_TYPE, COLOUR); -- draws and fills circle
- DrawString(pos, text, PIXEL_TYPE, COLOUR); -- draws string (notice that one character is one pixel)
- Clear(COLOUR); -- clears screen with colour
- IsFocused(); -- returns true if console window is focused
- GetMouseX(); -- returns position of mouse by X offset
- GetMouseY(); -- returns position of mouse by Y offset
- GetWidth(); -- returns width of screen
- GetHeight(); -- returns height of screen
- GetScreenSize(); -- returns size of screen
- InitObject(filename); -- uses in OnUserCreate() function, takes as argument .obj file
- DrawObject(rotate, speed); -- uses in OnUserCreate() function, takes as first argument boolean value to rotate object or not, as second value takes float argument of rotatable speed.

2. Keys buffer:
This buffer contains 256 keys and each key has 3 states:
- Held
- Pressed
- Released.
To select key from this buffer you need to use [Virtual Keys](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).
Example of usage:
	```cpp 
	if (keys[VK_UP].bHeld)
	{
		// do something
	}

	if (keys[VK_LEFT].bPressed)
	{
		// do something
	}
	```
	
3. Mouse buttons buffer:
This buffer contains 5 keys on mouse and each key has 3 states as keys in keys buffer:
- Held
- Pressed
- Released.
To select key from this buffer use numbers between 0 and 4.
Example of usage:
	```cpp
	if (mouse[0].bHeld)
	{
		// do something
	}

	if (mouse[1].bPressed)
	{
		// do something
	}
	```

4. Structs:
This engine contains different types of vectors:
1. vi2d -- vector that contains two integer values
2. vf2d -- vector that contains two float values
3. vi3d -- vector that contains three integer values
4. vf3d -- vector that contains three float values
5. KeyState -- has three boolean values of state of button
6. mat4x4 -- simple 4x4 matrix
7. mesh -- contains function that loads .obj file in memory

# License
BSD 3-Clause License

Copyright (c) 2021, deffy
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
