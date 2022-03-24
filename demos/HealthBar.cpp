#include "ConsolaProd.h"

class Example : public def::ConsolaProd
{
public:
	Example()
	{
		sAppName = L"Consola Prod Sample";
	}

private:
	float m_fHealth = 1.0f;

	int m_nBarWidth = 100;
	int m_nBarHeight = 20;

protected:
	virtual bool OnUserCreate() override
	{
		return true;
	}

	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		Clear(def::FG::BLACK);

		FillRectangleS(10, 10, m_nBarWidth, m_nBarHeight, def::Pixel::SOLID, def::FG::WHITE);
		FillRectangleS(10, 10, m_nBarWidth * m_fHealth, m_nBarHeight, def::Pixel::SOLID, def::FG::RED);

		if (GetKey(VK_LEFT).bHeld)
			m_fHealth -= 2.0f * fDeltaTime;

		if (GetKey(VK_RIGHT).bHeld)
			m_fHealth += 2.0f * fDeltaTime;

		if (m_fHealth < 0.0f)
			m_fHealth = 0.0f;

		if (m_fHealth > 1.0f)
			m_fHealth = 1.0f;

		return true;
	}
};


int main()
{
	Example demo;
	demo.Run(256, 240, 4, 4);
	return 0;
}
