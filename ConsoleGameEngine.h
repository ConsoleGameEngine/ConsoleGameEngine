#pragma once
#pragma comment(lib, "winmm.lib")

#pragma region LICENSE
/***
*	BSD 3-Clause License

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
***/
#pragma endregion

#pragma region EXAMPLE
/**
* Example (engine only supports .spr files, check [this](https://github.com/defini7/lab/tree/main/Sprite_Editor) for editing .spr files):
	#include "defConsoleGameEngine.h"

	class Example : public def::ConsoleGameEngine
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
		if (demo.Construct(256, 240, 4, 4))
			demo.Start();

		return 0;
	}
**/
#pragma endregion

#ifndef UNICODE
#error Please, enable UNICODE
#endif

#define PI 3.1415926535

#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <thread>
#include <atomic>

namespace def
{
	namespace FG
	{
		int BLACK = 0x0000,
			DARK_BLUE = 0x0001,
			DARK_GREEN = 0x0002,
			DARK_CYAN = 0x0003,
			DARK_RED = 0x0004,
			DARK_MAGENTA = 0x0005,
			DARK_YELLOW = 0x0006,
			GREY = 0x0007,
			DARK_GREY = 0x0008,
			BLUE = 0x0009,
			GREEN = 0x000A,
			CYAN = 0x000B,
			RED = 0x000C,
			MAGENTA = 0x000D,
			YELLOW = 0x000E,
			WHITE = 0x000F;
	}
	namespace BG
	{
		int BLACK = 0x0000,
			DARK_BLUE = 0x0010,
			DARK_GREEN = 0x0020,
			DARK_CYAN = 0x0030,
			DARK_RED = 0x0040,
			DARK_MAGENTA = 0x0050,
			DARK_YELLOW = 0x0060,
			GREY = 0x0070,
			DARK_GREY = 0x0080,
			BLUE = 0x0090,
			GREEN = 0x00A0,
			CYAN = 0x00B0,
			RED = 0x00C0,
			MAGENTA = 0x00D0,
			YELLOW = 0x00E0,
			WHITE = 0x00F0;
	}

	namespace Pixel
	{
		int SOLID = 0x2588,
			THREEQUARTERS = 0x2593,
			HALF = 0x2592,
			QUARTER = 0x2591;
	}

	struct vi2d
	{
		int x;
		int y;
	};

	struct vf2d
	{
		float x;
		float y;
	};

	struct vi3d
	{
		int x;
		int y;
		int z;
	};

	struct vf3d
	{
		float x;
		float y;
		float z;
	};

	struct triangle
	{
		vf3d p[3];

		wchar_t sym;
		short col;
	};

	struct KeyState
	{
		bool bHeld;
		bool bReleased;
		bool bPressed;
	};

	struct mesh
	{
		std::vector<triangle> tris;

		bool LoadFromObjectFile(std::wstring sFilename)
		{
			std::ifstream f(sFilename);
			if (!f.is_open())
				return false;

			// Local cache of verts
			std::vector<vf3d> verts;

			while (!f.eof())
			{
				char line[128];
				f.getline(line, 128);

				std::strstream s;
				s << line;

				char junk;

				if (line[0] == 'v')
				{
					vf3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}

				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}

			return true;
		}

	};

	struct mat4x4
	{
		float m[4][4] = { 0 };
	};

	class Sprite
	{
	public:
		Sprite()
		{
			Create(8, 8);
		}

		Sprite(int w, int h)
		{
			Create(w, h);
		}

		Sprite(std::wstring sFileName)
		{
			if (sFileName[sFileName.length() - 3] == L'.' &&
				sFileName[sFileName.length() - 2] == L'b' &&
				sFileName[sFileName.length() - 1] == L'm' &&
				sFileName[sFileName.length() - 0] == L'p')
			{
				std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;

				std::string sz = converterX.to_bytes(sFileName);

				unsigned char* data = ReadBMP(sz.c_str());

				m_Glyphs = new short[nWidth * nHeight];
				m_Colours = new short[nWidth * nHeight];

				for (int i = 0; i < nWidth; i++)
				{
					for (int j = 0; j < nHeight; j++)
					{
						m_Colours[j * nWidth + i] = (data[3 * (j * nWidth + i)] +
													 data[3 * (j * nWidth + i) + 1] +
													 data[3 * (j * nWidth + i) + 2]) / 3;
						m_Glyphs[j * nWidth + i] = Pixel::SOLID;
					}
				}
			}
			else
				if (!Load(sFileName))
					Create(8, 8);
		}

		int nWidth = 0;
		int nHeight = 0;

	private:
		short* m_Glyphs = nullptr;
		short* m_Colours = nullptr;

		unsigned char* ReadBMP(const char* filename)
		{
			int i;
			FILE* f = fopen(filename, "rb");
			unsigned char info[54];

			// read the 54-byte header
			fread(info, sizeof(unsigned char), 54, f);

			// extract image height and width from header
			int width = *(int*)&info[18];
			int height = *(int*)&info[22];

			nWidth = width;
			nHeight = height;

			// allocate 3 bytes per pixel
			int size = 3 * width * height;
			unsigned char* data = new unsigned char[size];

			// read the rest of the data at once
			fread(data, sizeof(unsigned char), size, f);
			fclose(f);

			for (i = 0; i < size; i += 3)
			{
				// flip the order of every 3 bytes
				unsigned char tmp = data[i];
				data[i] = data[i + 2];
				data[i + 2] = tmp;
			}

			return data;
		}

		void Create(int w, int h)
		{
			nWidth = w;
			nHeight = h;
			m_Glyphs = new short[w * h];
			m_Colours = new short[w * h];
			for (int i = 0; i < w * h; i++)
			{
				m_Glyphs[i] = L' ';
				m_Colours[i] = FG::BLACK;
			}
		}

	public:
		void SetGlyph(vi2d pos, short c)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return;
			else
				m_Glyphs[pos.y * nWidth + pos.x] = c;
		}

		void SetColour(vi2d pos, short c)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return;
			else
				m_Colours[pos.y * nWidth + pos.x] = c;
		}

		short GetGlyph(vi2d pos)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return L' ';
			else
				return m_Glyphs[pos.y * nWidth + pos.x];
		}

		short GetColour(vi2d pos)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return FG::BLACK;
			else
				return m_Colours[pos.y * nWidth + pos.x];
		}

		bool Save(std::wstring sFile)
		{
			FILE* f = nullptr;
			_wfopen_s(&f, sFile.c_str(), L"wb");
			if (f == nullptr)
				return false;

			fwrite(&nWidth, sizeof(int), 1, f);
			fwrite(&nHeight, sizeof(int), 1, f);
			fwrite(m_Colours, sizeof(short), nWidth * nHeight, f);
			fwrite(m_Glyphs, sizeof(short), nWidth * nHeight, f);

			fclose(f);

			return true;
		}

		bool Load(std::wstring sFile)
		{
			delete[] m_Glyphs;
			delete[] m_Colours;
			nWidth = 0;
			nHeight = 0;

			FILE* f = nullptr;
			_wfopen_s(&f, sFile.c_str(), L"rb");
			if (f == nullptr)
				return false;

			std::fread(&nWidth, sizeof(int), 1, f);
			std::fread(&nHeight, sizeof(int), 1, f);

			Create(nWidth, nHeight);

			std::fread(m_Colours, sizeof(short), nWidth * nHeight, f);
			std::fread(m_Glyphs, sizeof(short), nWidth * nHeight, f);

			std::fclose(f);
			return true;
		}
	};

	class ConsoleGameEngine
	{
	public:
		ConsoleGameEngine()
		{
			hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
			hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

			sAppName = L"Undefined";
		}

		virtual ~ConsoleGameEngine()
		{
			delete[] screen;
		}

	public:
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fDeltaTime) = 0;

		bool Construct(int width = 120, int height = 40, int fontw = 4, int fonth = 4)
		{
			if (width <= 0 || height <= 0 || fontw <= 0 || fonth <= 0)
				return false;

			nScreenWidth = width;
			nScreenHeight = height;

			nFontW = fontw;
			nFontH = fonth;

			hConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

			if (hConsoleOut == INVALID_HANDLE_VALUE)
				return false;

			rectWindow = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rectWindow);

			COORD coord = { (short)nScreenWidth, (short)nScreenHeight };
			if (!SetConsoleScreenBufferSize(hConsoleOut, coord))
				return false;

			if (!SetConsoleActiveScreenBuffer(hConsoleOut))
				return false;

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = nFontW;
			cfi.dwFontSize.Y = nFontH;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;

			wcscpy_s(cfi.FaceName, L"Consolas");
			if (!SetCurrentConsoleFontEx(hConsoleOut, false, &cfi))
				return false;

			if (!SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
				return false;

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(hConsoleOut, &csbi))
				return false;
			if (nScreenHeight > csbi.dwMaximumWindowSize.Y)
				return false;
			if (nScreenWidth > csbi.dwMaximumWindowSize.X)
				return false;

			rectWindow = { 0, 0, short(nScreenWidth - 1), short(nScreenHeight - 1) };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rectWindow);
			screen = new CHAR_INFO[nScreenWidth * nScreenHeight];
			memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

			return true;
		}

		void Start()
		{
			bGameThreadActive = true;
			std::thread t = std::thread(&def::ConsoleGameEngine::AppThread, this);

			t.join();
		}

		void Stop()
		{
			bGameThreadActive = false;
		}

		std::vector<KeyState> keys;
		std::vector<KeyState> mouse;

	private:
		mesh meshCube;
		mat4x4 matProj;

		float fTheta;

		void MultiplyMatrixVector(vf3d& i, vf3d& o, mat4x4& m)
		{
			o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
			o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
			o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
			float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

			if (w != 0.0f)
			{
				o.x /= w; o.y /= w; o.z /= w;
			}
		}

		// Taken From Command Line Webcam Video
		CHAR_INFO GetColour(float lum)
		{
			short bg_col, fg_col;
			wchar_t sym;
			int pixel_bw = (int)(13.0f * lum);
			switch (pixel_bw)
			{
			case 0: bg_col = BG::BLACK; fg_col = FG::BLACK; sym = Pixel::SOLID; break;

			case 1: bg_col = BG::BLACK; fg_col = FG::DARK_GREY; sym = Pixel::QUARTER; break;
			case 2: bg_col = BG::BLACK; fg_col = FG::DARK_GREY; sym = Pixel::HALF; break;
			case 3: bg_col = BG::BLACK; fg_col = FG::DARK_GREY; sym = Pixel::THREEQUARTERS; break;
			case 4: bg_col = BG::BLACK; fg_col = FG::DARK_GREY; sym = Pixel::SOLID; break;

			case 5: bg_col = BG::DARK_GREY; fg_col = FG::GREY; sym = Pixel::QUARTER; break;
			case 6: bg_col = BG::DARK_GREY; fg_col = FG::GREY; sym = Pixel::HALF; break;
			case 7: bg_col = BG::DARK_GREY; fg_col = FG::GREY; sym = Pixel::THREEQUARTERS; break;
			case 8: bg_col = BG::DARK_GREY; fg_col = FG::GREY; sym = Pixel::SOLID; break;
			
			case 9:  bg_col = BG::GREY; fg_col = FG::WHITE; sym = Pixel::QUARTER; break;
			case 10: bg_col = BG::GREY; fg_col = FG::WHITE; sym = Pixel::HALF; break;
			case 11: bg_col = BG::GREY; fg_col = FG::WHITE; sym = Pixel::THREEQUARTERS; break;
			case 12: bg_col = BG::GREY; fg_col = FG::WHITE; sym = Pixel::SOLID; break;
			default:
				bg_col = BG::BLACK; fg_col = FG::DARK_GREY; sym = Pixel::SOLID;
			}

			CHAR_INFO c;
			c.Attributes = bg_col | fg_col;
			c.Char.UnicodeChar = sym;
			return c;
		}

	public:

		vf3d vCamera;

		void InitObject(std::wstring filename)
		{
			//meshCube.tris = {

			//// SOUTH
			//{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			//{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			//// EAST                                                      
			//{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			//{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			//// NORTH                                                     
			//{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			//{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			//// WEST                                                      
			//{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			//{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			//// TOP                                                       
			//{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			//{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			//// BOTTOM                                                    
			//{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			//{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			//};

			meshCube.LoadFromObjectFile(filename);

			// Projection Matrix
			float fNear = 0.1f;
			float fFar = 1000.0f;
			float fFov = 90.0f;
			float fAspectRatio = (float)nScreenWidth / (float)nScreenHeight;
			float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

			matProj.m[0][0] = fAspectRatio * fFovRad;
			matProj.m[1][1] = fFovRad;
			matProj.m[2][2] = fFar / (fFar - fNear);
			matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
			matProj.m[2][3] = 1.0f;
			matProj.m[3][3] = 0.0f;
		}

		void DrawObject(bool bRotate, float fSpeed = 1.0f)
		{
			// Set up rotation matrices
			mat4x4 matRotZ, matRotX;

			// Set positive speed if it's less than 0
			if (fSpeed < 0.0f)
				fSpeed *= -1;

			// Rotate object if bRotate = true
			if (bRotate)
				fTheta += fSpeed * deltaTime;

			// Rotation Z
			matRotZ.m[0][0] = cosf(fTheta);
			matRotZ.m[0][1] = sinf(fTheta);
			matRotZ.m[1][0] = -sinf(fTheta);
			matRotZ.m[1][1] = cosf(fTheta);
			matRotZ.m[2][2] = 1;
			matRotZ.m[3][3] = 1;

			// Rotation X
			matRotX.m[0][0] = 1;
			matRotX.m[1][1] = cosf(fTheta * 0.5f);
			matRotX.m[1][2] = sinf(fTheta * 0.5f);
			matRotX.m[2][1] = -sinf(fTheta * 0.5f);
			matRotX.m[2][2] = cosf(fTheta * 0.5f);
			matRotX.m[3][3] = 1;

			// Store triagles for rastering later
			std::vector<triangle> vecTrianglesToRaster;

			// Draw Triangles
			for (auto tri : meshCube.tris)
			{
				triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

				// Rotate in Z-Axis
				MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
				MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
				MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

				// Rotate in X-Axis
				MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
				MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
				MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

				// Offset into the screen
				triTranslated = triRotatedZX;
				triTranslated.p[0].z = triRotatedZX.p[0].z + 8.0f;
				triTranslated.p[1].z = triRotatedZX.p[1].z + 8.0f;
				triTranslated.p[2].z = triRotatedZX.p[2].z + 8.0f;

				// Use Cross-Product to get surface normal
				vf3d normal, line1, line2;
				line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
				line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
				line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

				line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
				line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
				line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

				normal.x = line1.y * line2.z - line1.z * line2.y;
				normal.y = line1.z * line2.x - line1.x * line2.z;
				normal.z = line1.x * line2.y - line1.y * line2.x;

				// It's normally normal to normalise the normal
				float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
				normal.x /= l; normal.y /= l; normal.z /= l;

				//if (normal.z < 0)
				if (normal.x * (triTranslated.p[0].x - vCamera.x) +
					normal.y * (triTranslated.p[0].y - vCamera.y) +
					normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)
				{
					// Illumination
					vf3d light_direction = { 0.0f, 0.0f, -1.0f };
					float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
					light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

					// How similar is normal to light direction
					float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

					// Choose console colours as required (much easier with RGB)
					CHAR_INFO c = GetColour(dp);
					triTranslated.col = c.Attributes;
					triTranslated.sym = c.Char.UnicodeChar;

					// Project triangles from 3D --> 2D
					MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
					MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
					MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);
					triProjected.col = triTranslated.col;
					triProjected.sym = triTranslated.sym;

					// Scale into view
					triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
					triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
					triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
					triProjected.p[0].x *= 0.5f * (float)nScreenWidth;
					triProjected.p[0].y *= 0.5f * (float)nScreenHeight;
					triProjected.p[1].x *= 0.5f * (float)nScreenWidth;
					triProjected.p[1].y *= 0.5f * (float)nScreenHeight;
					triProjected.p[2].x *= 0.5f * (float)nScreenWidth;
					triProjected.p[2].y *= 0.5f * (float)nScreenHeight;

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}

			}

			// Sort triangles from back to front
			sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
				{
					float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
					float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
					return z1 > z2;
				});

			for (auto& triProjected : vecTrianglesToRaster)
			{
				// Rasterize triangle
				FillTriangle({ (int)triProjected.p[0].x, (int)triProjected.p[0].y },
					{ (int)triProjected.p[1].x, (int)triProjected.p[1].y },
					{ (int)triProjected.p[2].x, (int)triProjected.p[2].y },
					triProjected.sym, triProjected.col);

				/*DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_SOLID, FG_BLACK);*/
			}
		}

	public:
		void Draw(vi2d pos, short c = 0x2588, short col = 0x000F);
		void Draw(int x, int y, short c = 0x2588, short col = 0x000F);

		void DrawLine(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);

		void DrawTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c = 0x2588, short col = 0x000F);
		void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);

		void FillTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c = 0x2588, short col = 0x000F);
		void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);

		void DrawRectangle(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		void DrawRectangle(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
		void DrawRectangleS(vi2d pos, vi2d size, short c = 0x2588, short col = 0x000F);
		void DrawRectangleS(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);

		void FillRectangle(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		void FillRectangle(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
		void FillRectangleS(vi2d pos, vi2d size, short c = 0x2588, short col = 0x000F);
		void FillRectangleS(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);

		void DrawCircle(vi2d pos, int radius, short c = 0x2588, short col = 0x000F);
		void DrawCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);

		void FillCircle(vi2d pos, int radius, short c = 0x2588, short col = 0x000F);
		void FillCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);

		void DrawSprite(vi2d pos, Sprite* sprite);
		void DrawSprite(int x, int y, Sprite* sprite);

		void DrawPartialSprite(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite);
		void DrawPartialSprite(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite);

		void DrawString(vi2d pos, std::wstring text, short c = 0x2588, short col = 0x000F);
		void DrawString(int x, int y, std::wstring text, short c = 0x2588, short col = 0x000F);

		void Clear(short col = 0x000F);
		bool IsFocused();

		vi2d GetMouse();
		int GetMouseX();
		int GetMouseY();

		int GetWidth();
		int GetHeight();
		vi2d GetScreenSize();

	private:
		void AppThread()
		{
			auto tp1 = std::chrono::system_clock::now();
			auto tp2 = std::chrono::system_clock::now();

			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;

			if (!OnUserCreate())
				bGameThreadActive = false;

			for (int i = 0; i < 256; i++)
				keys.push_back({ false, false, false });
			
			for (int i = 0; i < 5; i++)
				mouse.push_back({ false });

			while (bGameThreadActive)
			{
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				deltaTime = elapsedTime.count();

				wchar_t buffer_title[256];
				swprintf_s(buffer_title, 256, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / deltaTime);
				SetConsoleTitleW(buffer_title);

				if (!OnUserUpdate(deltaTime))
					bGameThreadActive = false;

				// Handle Mouse Input - Check for window events
				INPUT_RECORD inBuf[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(hConsoleIn, &events);
				if (events > 0)
					ReadConsoleInputW(hConsoleIn, inBuf, events, &events);

				// Handle events - we only care about mouse clicks and movement
				// for now
				for (DWORD i = 0; i < events; i++)
				{
					switch (inBuf[i].EventType)
					{
					case FOCUS_EVENT:
						bFocused = inBuf[i].Event.FocusEvent.bSetFocus;
						break;

					case MOUSE_EVENT:
					{
						switch (inBuf[i].Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
						{
							nMousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
							nMousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
						}
						break;

						case 0:
						{
							for (int m = 0; m < 5; m++)
								mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
						}
						break;

						default:
							break;
						}
					}
					break;

					default:
						break;
					}
				}

				for (int i = 0; i < 256; i++)
				{
					keyNewState[i] = GetAsyncKeyState(i);

					keys[i].bPressed = false;
					keys[i].bReleased = false;

					if (keyNewState[i] != keyOldState[i])
					{
						if (keyNewState[i] & 0x8000)
						{
							keys[i].bPressed = !keys[i].bHeld;
							keys[i].bHeld = true;
						}
						else
						{
							keys[i].bReleased = true;
							keys[i].bHeld = false;
						}
					}

					keyOldState[i] = keyNewState[i];
				}

				for (int m = 0; m < 5; m++)
				{
					mouse[m].bPressed = false;
					mouse[m].bReleased = false;

					if (mouseNewState[m] != mouseOldState[m])
					{
						if (mouseNewState[m])
						{
							mouse[m].bPressed = true;
							mouse[m].bHeld = true;
						}
						else
						{
							mouse[m].bReleased = true;
							mouse[m].bHeld = false;
						}
					}

					mouseOldState[m] = mouseNewState[m];
				}

				WriteConsoleOutputW(hConsoleOut, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &rectWindow);
			}
		}

	protected:
		CHAR_INFO* screen = nullptr;
		HANDLE hConsoleOut;
		HANDLE hConsoleIn;
		SMALL_RECT rectWindow;
		std::wstring sAppName;

		short keyOldState[256]{ 0 };
		short keyNewState[256]{ 0 };

		bool mouseOldState[5] = { 0 };
		bool mouseNewState[5] = { 0 };

		int nMousePosX;
		int nMousePosY;

		int nScreenWidth;
		int nScreenHeight;
		int nFontW;
		int nFontH;

		float deltaTime;

		bool bGameThreadActive;
		bool bFocused;
	};

	bool ConsoleGameEngine::IsFocused()
	{
		return bFocused;
	}

	void ConsoleGameEngine::FillRectangle(vi2d pos1, vi2d pos2, short c, short col)
	{
		if (pos1.x >= 0 && pos1.x < nScreenWidth && pos1.y >= 0 && pos1.y < nScreenHeight)
		{
			for (int i = pos1.x; i < pos2.x; i++)
				for (int j = pos1.y; j < pos2.y; j++)
				{
					screen[j * nScreenWidth + i].Char.UnicodeChar = c;
					screen[j * nScreenWidth + i].Attributes = col;
				}
		}
	}

	void ConsoleGameEngine::FillRectangle(int x1, int y1, int x2, int y2, short c, short col)
	{
		FillRectangle({ x1, y1 }, { x2, y2 }, c, col);
	}

	void ConsoleGameEngine::FillRectangleS(vi2d pos, vi2d size, short c, short col)
	{
		FillRectangle(pos, { pos.x + size.x, pos.y + size.y }, c, col);
	}

	void ConsoleGameEngine::FillRectangleS(int x, int y, int size_x, int size_y, short c, short col)
	{
		FillRectangle({ x, y }, { x + size_x, y + size_y }, c, col);
	}

	void ConsoleGameEngine::DrawCircle(vi2d pos, int radius, short c, short col)
	{
		int x = 0;
		int y = radius;
		int p = 3 - 2 * radius;
		if (!radius) return;

		while (y >= x)
		{
			Draw({ pos.x - x, pos.y - y }, c, col);//upper left left
			Draw({ pos.x - y, pos.y - x }, c, col);//upper upper left
			Draw({ pos.x + y, pos.y - x }, c, col);//upper upper right
			Draw({ pos.x + x, pos.y - y }, c, col);//upper right right
			Draw({ pos.x - x, pos.y + y }, c, col);//lower left left
			Draw({ pos.x - y, pos.y + x }, c, col);//lower lower left
			Draw({ pos.x + y, pos.y + x }, c, col);//lower lower right
			Draw({ pos.x + x, pos.y + y }, c, col);//lower right right
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void ConsoleGameEngine::DrawCircle(int x, int y, int radius, short c, short col)
	{
		DrawCircle({ x, y }, radius, c, col);
	}

	void ConsoleGameEngine::FillCircle(vi2d pos, int radius, short c, short col)
	{
		for (int i = radius; i != 0; i--)
		{
			DrawCircle(pos, i, c, col);
		}
	}

	void ConsoleGameEngine::FillCircle(int x, int y, int radius, short c, short col)
	{
		FillCircle({ x, y }, radius, c, col);
	}

	void ConsoleGameEngine::Draw(vi2d pos, short c, short col)
	{
		if (pos.x >= 0 && pos.x < nScreenWidth && pos.y >= 0 && pos.y < nScreenHeight)
		{
			screen[pos.y * nScreenWidth + pos.x].Char.UnicodeChar = c;
			screen[pos.y * nScreenWidth + pos.x].Attributes = col;
		}
	}

	void ConsoleGameEngine::Draw(int x, int y, short c, short col)
	{
		Draw({ x, y }, c, col);
	}

	void ConsoleGameEngine::DrawLine(vi2d pos1, vi2d pos2, short c, short col)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = pos2.x - pos1.x; dy = pos2.y - pos1.y;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = pos1.x; y = pos1.y; xe = pos2.x;
			}
			else
			{
				x = pos2.x; y = pos2.y; xe = pos1.x;
			}

			Draw({ x, y }, c, col);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw({ x, y }, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = pos1.x; y = pos1.y; ye = pos2.y;
			}
			else
			{
				x = pos2.x; y = pos2.y; ye = pos1.y;
			}

			Draw({ x, y }, c, col);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw({ x, y }, c, col);
			}
		}
	}

	void ConsoleGameEngine::DrawLine(int x1, int y1, int x2, int y2, short c, short col)
	{
		DrawLine({ x1, y1 }, { x2, y2 }, c, col);
	}

	void ConsoleGameEngine::DrawTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c, short col)
	{
		DrawLine(pos1, pos2, c, col);
		DrawLine(pos2, pos3, c, col);
		DrawLine(pos3, pos1, c, col);
	}

	void ConsoleGameEngine::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
	{
		DrawTriangle({ x1, y1 }, { x2, y2 }, { x3, y3 }, c, col);
	}

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void ConsoleGameEngine::FillTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c, short col)
	{
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw({ i, ny } , c, col); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (pos1.y > pos2.y) { std::swap(pos1.y, pos2.y); std::swap(pos1.x, pos2.x); }
		if (pos1.y > pos3.y) { std::swap(pos1.y, pos3.y); std::swap(pos1.x, pos3.x); }
		if (pos2.y > pos3.y) { std::swap(pos2.y, pos3.y); std::swap(pos2.x, pos3.x); }

		t1x = t2x = pos1.x; y = pos1.y;   // Starting points
		dx1 = (int)(pos2.x - pos1.x); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(pos2.y - pos1.y);

		dx2 = (int)(pos3.x - pos1.x); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(pos3.y - pos1.y);

		if (dy1 > dx1) {   // swap values
			std::swap(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			std::swap(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (pos1.y == pos2.y) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
											// Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == pos2.y) break;

		}
	next:
		// Second half
		dx1 = (int)(pos3.x - pos2.x); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(pos3.y - pos2.y);
		t1x = pos2.x;

		if (dy1 > dx1) {   // swap values
			std::swap(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != pos3.x) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > pos3.y) return;
		}
	}

	void ConsoleGameEngine::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
	{
		FillTriangle({ x1, y1 }, { x2, y2 }, { x3, y3 }, c, col);
	}

	void ConsoleGameEngine::DrawRectangle(vi2d pos1, vi2d pos2, short c, short col)
	{
		DrawLine(pos1, { pos2.x, pos1.y });
		DrawLine({ pos2.x, pos1.y }, pos2);
		DrawLine(pos2, { pos1.x, pos2.y });
		DrawLine({ pos1.x, pos2.y }, pos1);
	}

	void ConsoleGameEngine::DrawRectangle(int x1, int y1, int x2, int y2, short c, short col)
	{
		DrawRectangle({ x1, y1 }, { x2, y2 }, c, col);
	}

	void ConsoleGameEngine::DrawRectangleS(vi2d pos, vi2d size, short c, short col)
	{
		DrawRectangle(pos, { pos.x + size.x, pos.y + size.y }, c, col);
	}

	void ConsoleGameEngine::DrawRectangleS(int x, int y, int size_x, int size_y, short c, short col)
	{
		DrawRectangle({ x, y }, { x + size_x, y + size_y }, c, col);
	}

	void ConsoleGameEngine::DrawSprite(vi2d pos, Sprite* sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->nWidth; i++)
		{
			for (int j = 0; j < sprite->nHeight; j++)
			{
				if (sprite->GetGlyph({ i, j }) != L' ')
					Draw({ pos.x + i, pos.y + j }, sprite->GetGlyph({ i, j }), sprite->GetColour({ i, j }));
			}
		}
	}

	void ConsoleGameEngine::DrawSprite(int x, int y, Sprite* sprite)
	{
		DrawSprite({ x, y }, sprite);
	}

	void ConsoleGameEngine::DrawPartialSprite(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite)
	{
		if (sprite == nullptr || fpos1.x < 0 || fpos1.y < 0 || fpos2.x > sprite->nWidth || fpos2.y > sprite->nHeight)
			return;

		for (int i = fpos1.x, x = 0; i < fpos2.x; i++, x++)
		{
			for (int j = fpos1.y, y = 0; j < fpos2.y; j++, y++)
			{
				if (sprite->GetGlyph({ i, j }) != L' ')
					Draw({ pos.x + x, pos.y + y }, sprite->GetGlyph({ i, j }), sprite->GetColour({ i, j }));
			}
		}
	}

	void ConsoleGameEngine::DrawPartialSprite(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite)
	{
		DrawPartialSprite({ x, y }, { fx1, fy1 }, { fx2, fy2 }, sprite);
	}

	void ConsoleGameEngine::DrawString(vi2d pos, std::wstring text, short c, short col)
	{
		if (pos.x > 0 && pos.y > 0 && pos.x <= nScreenWidth && pos.y <= nScreenHeight)
		{
			for (size_t i = 0; i < text.size(); i++)
			{
				screen[pos.y * nScreenWidth + pos.x + i].Char.UnicodeChar = text[i];
				screen[pos.y * nScreenWidth + pos.x + i].Attributes = col;
			}
		}
	}

	void ConsoleGameEngine::DrawString(int x, int y, std::wstring text, short c, short col)
	{
		DrawString({ x, y }, text, c, col);
	}

	void ConsoleGameEngine::Clear(short col)
	{
		FillRectangle({ 0, 0 }, { nScreenWidth, nScreenHeight }, 0x2588, col);
	}

	vi2d ConsoleGameEngine::GetMouse()
	{
		return { nMousePosX, nMousePosY };
	}

	int ConsoleGameEngine::GetMouseX()
	{
		return nMousePosX;
	}

	int ConsoleGameEngine::GetMouseY()
	{
		return nMousePosY;
	}

	int ConsoleGameEngine::GetWidth()
	{
		return nScreenWidth;
	}

	int ConsoleGameEngine::GetHeight()
	{
		return nScreenHeight;
	}

	vi2d ConsoleGameEngine::GetScreenSize()
	{
		return { nScreenWidth, nScreenHeight };
	}
}
