#include "ConsoleGameEngine.h"

struct Expression
{
	Expression(std::string token) : token(token) {}
	Expression(std::string token, Expression a) : token(token), args{ a } {}
	Expression(std::string token, Expression a, Expression b) : token(token), args{ a, b } {}

	std::string token;
	std::vector<Expression> args;
};

class Parser
{
public:
	Parser() { }
	virtual ~Parser() { }

	double Get(const char* input);

	const char* GetState() const;

private:
	std::string parse_token();

	Expression parse_simple_expression();
	Expression parse_binary_expression(int min_priority);
	double evaluate(const Expression& e);

	const char* input;

	std::string sState = "OK";
};

std::string Parser::parse_token()
{
	while (std::isspace(*input)) input++;

	if (std::isdigit(*input))
	{
		std::string sNumber;

		while (std::isdigit(*input) || *input == '.')
			sNumber.push_back(*input++);

		return sNumber;
	}

	static const std::string Tokens[] = { "+", "-", "^", "*", "/", "%", "abs", "sin", "cos", "tan", "(", ")", "sqrt" };

	for (auto& t : Tokens)
		if (std::strncmp(input, t.c_str(), t.size()) == 0)
		{
			input += t.size();
			return t;
		}

	return "";

}

Expression Parser::parse_simple_expression()
{
	std::string token = parse_token();

	if (token.empty())
		sState = "Invalid input";

	if (token == "(")
	{
		Expression result = parse_binary_expression(0);

		if (parse_token() != ")")
			sState = "Expected ')'";

		return result;
	}

	if (std::isdigit(token[0]))
		return Expression(token);

	return Expression(token, parse_simple_expression());
}

int get_priority(const std::string& binary_op)
{
	if (binary_op == "+") return 1;
	if (binary_op == "-") return 1;
	if (binary_op == "*") return 2;
	if (binary_op == "/") return 2;
	if (binary_op == "%") return 2;
	if (binary_op == "^") return 3;

	return 0;
}

Expression Parser::parse_binary_expression(int min_priority)
{
	Expression left_expr = parse_simple_expression();

	while (1)
	{
		std::string op = parse_token();
		int priority = get_priority(op);

		if (priority <= min_priority)
		{
			input -= op.size();
			return left_expr;
		}

		Expression right_expr = parse_binary_expression(priority);
		left_expr = Expression(op, left_expr, right_expr);
	}

}

double Parser::Get(const char* inp)
{
	input = inp;
	return evaluate(parse_binary_expression(0));
}

const char* Parser::GetState() const
{
	return sState.c_str();
}

double Parser::evaluate(const Expression& e)
{
	switch (e.args.size())
	{
	case 2:
	{
		double a = evaluate(e.args[0]);
		double b = evaluate(e.args[1]);

		if (e.token == "+") return a + b;
		if (e.token == "-") return a - b;
		if (e.token == "*") return a * b;
		if (e.token == "/") return a / b;
		if (e.token == "^") return pow(a, b);
		if (e.token == "%") return (int)a % (int)b;

		sState = "Unknown binary operator";
	}

	case 1:
	{
		double a = evaluate(e.args[0]);

		if (e.token == "+") return a;
		if (e.token == "-") return -a;
		if (e.token == "abs") return abs(a);
		if (e.token == "sin") return sin(a);
		if (e.token == "cos") return cos(a);
		if (e.token == "tan") return tan(a);
		if (e.token == "sqrt") return sqrt(a);

		sState = "Unknown unary operator";
	}

	case 0:
		return strtod(e.token.c_str(), nullptr);

	}

	sState = "Unknown expression type";

}

class Graphs : public def::ConsoleGameEngine
{
public:
	Graphs(std::string exp)
	{
		sAppName = L"Graphs";

		sExpression = exp;
	}

protected:
	virtual bool OnUserCreate() override
	{
		if (sExpression.find("sin") != std::string::npos ||
			sExpression.find("cos") != std::string::npos ||
			sExpression.find("tan") != std::string::npos)
			bHasFuncs = true;

		return true;
	}

	virtual bool OnUserUpdate(float dt) override
	{
		Clear(def::Pixel::SOLID, def::FG::WHITE | def::BG::WHITE);

		DrawLine(GetScreenWidth() / 2, 0, GetScreenWidth() / 2, GetScreenHeight(), def::Pixel::SOLID, def::FG::GREY | def::BG::GREY);
		DrawLine(0, GetScreenHeight() / 2, GetScreenWidth(), GetScreenHeight() / 2, def::Pixel::SOLID, def::FG::GREY | def::BG::GREY);

		float fAngle = 0.0f;

		for (int i = -1; i < GetScreenWidth(); i++)
		{
			int CurrentY;

			std::string expression = sExpression;

			size_t pos = expression.find("x");

			auto change_str_x = [&](float fValue)
			{
				while (pos != std::string::npos)
				{
					expression.replace(pos, 1, std::to_string(fValue));
					pos = expression.find("x", pos);
				}
			};

			auto draw_and_connect = [&](int to_mul = 1, int to_add_x = 0, int to_add_y = 0)
			{
				CurrentY = int(to_mul * parser.Get(expression.c_str())) + to_add_y;
				DrawLine(i + to_add_x - 1, PrevY, i + to_add_x, CurrentY, def::Pixel::SOLID, def::FG::BLACK);
				PrevY = CurrentY;
			};

			if (bHasFuncs)
			{
				change_str_x(fAngle);

				draw_and_connect(nAmplitude, 0, GetScreenHeight() / 2);

				fAngle += (2.0f * 3.1415926535f) / nAmplitude;

			}
			else
			{
				change_str_x(i);

				draw_and_connect(1, GetScreenWidth() / 2);
			}

		}

		DrawLine(GetScreenWidth() / 2 - 1, 0, GetScreenWidth() / 2 - 1, GetScreenHeight(), def::Pixel::SOLID, def::FG::WHITE | def::BG::WHITE);
		Draw(GetScreenWidth() / 2 - 1, GetScreenHeight() / 2, def::Pixel::SOLID, def::FG::GREY | def::BG::GREY);

		if (GetKey(VK_UP).bHeld)
			nAmplitude += 1;

		if (GetKey(VK_DOWN).bHeld)
			nAmplitude -= 1;

		if (nAmplitude <= 0)
			nAmplitude = 1;

		return true;
	}

private:
	// The highest point of a curve
	int nAmplitude = 40;
	int PrevY = 0;

	// Simple math parser
	Parser parser;
	std::string sExpression;

	bool bHasFuncs = false;

};

int main()
{
	Graphs demo("sin(x)");
	
	def::rcode err = demo.ConstructConsole(256, 240, 4, 4);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
