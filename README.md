# **ConsoleGameEngine**

## Something that can drive you insane

That works absolutely horrible on Windows 11

## Description

ConsoleGameEngine is a simple game engine that can be used for building games and applications in Windows Command Prompt

## Documentation

### Example

Let's take a look on an example:

```c++
#include "ConsoleGameEngine.h"

class Example : public ConsoleGameEngine
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
	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		return true;
	}
};

int main()
{
	Example demo;
	
	if (demo.Construct(256, 240, 4, 4) == rcode::OK)
		demo.Run();
	
	return 0;
}
```

First of all we need to include `ConsoleGameEngine.h`. 

Then we need to create a class, that publicly inherits from `ConsoleGameEngine` class. Then we need to override a constructor, to set the application name and then we do the same with 2 virtual methods: `OnUserCreate` and `OnUserUpdate`, the second method takes as parameter `fDeltaTime`. They should return `true` to prevent exiting from application. `OnUserCreate` is being called only once, and `OnUserUpdate` on every frame.

Now we need to create a `main` function, after that we must create instance of our derived class, then we create an if statement with calling `Construct` method where we pass screen width, screen height, font width and font height, if it returns rcode::OK, we can call `Run` method.

## Additional

1. [Sprite Editor](https://github.com/defini7/SpriteEditor)

## License

BSD 3-Clause License

Copyright (c) 2021, 2022, 2023 Alex
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

