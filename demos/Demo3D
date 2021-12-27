#define DEF_CP_APP
#include "ConsolaProd.h"
using namespace def;

struct vf3d
{
	float x;
	float y;
	float z;
};

struct triangle
{
	vf3d p[3];
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

struct mesh
{
	std::vector<triangle> tris;
};

class Engine3D : public ConsolaProd
{
public:
	Engine3D()
	{
		sAppName = L"3D Engine";
	}

private:
	mesh meshCube;
	mat4x4 matProj;

	float fFar = 1000.0f;
	float fNear = 0.1f;
	float fFOV = 0.0f;
	float fAspectRatio = 0.0f;
	float fQ = 0.0f;
	float fTheta = 0.0f;

protected:
	void MultiplyVecMat(mat4x4& mat, vf3d& vec, vf3d& res)
	{
		res.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
		res.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
		res.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];
		float w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + mat.m[3][3];

		if (w != 0.0f)
			res.x /= w; res.y /= w; res.z /= w;
	}

	void RotateX(mat4x4& mat, float a)
	{
		mat.m[0][0] = 1;
		mat.m[1][1] = cosf(fTheta * 0.5f);
		mat.m[1][2] = sinf(fTheta * 0.5f);
		mat.m[2][1] = -sinf(fTheta * 0.5f);
		mat.m[2][2] = cosf(fTheta * 0.5f);
		mat.m[3][3] = 1;
	}

	void RotateY(mat4x4& mat, float a)
	{
		mat.m[0][0] = cosf(a * 0.5f); mat.m[1][0] = 0.0f; mat.m[2][0] = sinf(a * 0.5f);
		mat.m[0][1] = 0.0f; mat.m[1][1] = 1.0f; mat.m[2][1] = 0.0f;
		mat.m[0][2] = -sinf(a * 0.5f); mat.m[1][2] = 0.0f; mat.m[2][2] = cosf(a * 0.5f);
	}

	void RotateZ(mat4x4& mat, float a)
	{
		mat.m[0][0] = cosf(fTheta);
		mat.m[0][1] = sinf(fTheta);
		mat.m[1][0] = -sinf(fTheta);
		mat.m[1][1] = cosf(fTheta);
		mat.m[2][2] = 1;
		mat.m[3][3] = 1;
	}

	bool OnUserCreate() override
	{
		meshCube.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f }

		};

		fAspectRatio = GetScreenWidth() / GetScreenHeight();
		fFOV = 1 / tanf(90 * 0.5f / 180.0f * 3.14159f); // in radians
		fQ = fFar / (fFar - fNear);

		matProj.m[0][0] = fAspectRatio * fFOV;
		matProj.m[1][1] = fFOV;
		matProj.m[2][2] = fQ;
		matProj.m[3][2] = -(fNear * fQ);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		return true;
	}

	bool OnUserUpdate(float dt) override
	{
		Clear(def::Pixel::SOLID, def::FG::BLACK);

		mat4x4 matRotX, matRotY, matRotZ;
		fTheta += 1.0f * dt;

		RotateX(matRotX, fTheta);
		RotateZ(matRotZ, fTheta);

		// Draw triangles
		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			MultiplyVecMat(matRotZ, tri.p[0], triRotatedZ.p[0]);
			MultiplyVecMat(matRotZ, tri.p[1], triRotatedZ.p[1]);
			MultiplyVecMat(matRotZ, tri.p[2], triRotatedZ.p[2]);

			MultiplyVecMat(matRotX, triRotatedZ.p[0], triRotatedZX.p[0]);
			MultiplyVecMat(matRotX, triRotatedZ.p[1], triRotatedZX.p[1]);
			MultiplyVecMat(matRotX, triRotatedZ.p[2], triRotatedZX.p[2]);

			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			MultiplyVecMat(matProj, triTranslated.p[0], triProjected.p[0]);
			MultiplyVecMat(matProj, triTranslated.p[1], triProjected.p[1]);
			MultiplyVecMat(matProj, triTranslated.p[2], triProjected.p[2]);

			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

			triProjected.p[0].x *= 0.5f * GetScreenWidth();
			triProjected.p[0].y *= 0.5f * GetScreenHeight();
			triProjected.p[1].x *= 0.5f * GetScreenWidth();
			triProjected.p[1].y *= 0.5f * GetScreenHeight();
			triProjected.p[2].x *= 0.5f * GetScreenWidth();
			triProjected.p[2].y *= 0.5f * GetScreenHeight();

			DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				def::Pixel::SOLID, def::FG::WHITE
			);
		}

		return true;
	}
};

int main()
{
	Engine3D demo;
	demo.Run(256, 240, 4, 4);
	return 0;
}
