#define DEF_CP_APP
#include "ConsolaProd.h"

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

	static const std::string Tokens[] = { "+", "-", "^", "*", "/", "%", "abs", "sin", "cos", "(", ")" };

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

		sState = "Unknown unary operator";
	}

	case 0:
		return strtod(e.token.c_str(), nullptr);

	}

	sState = "Unknown expression type";
	
}

class Graphs : public def::ConsolaProd
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
		return true;
	}

	virtual bool OnUserUpdate(float dt) override
	{
		Clear(def::Pixel::SOLID, def::FG::BLACK);

		float fAngle = 0.0f;

		for (int i = -1; i < nScreenWidth; i++)
		{
			int CurrentY[3];

			std::string expression = sExpression;

			size_t pos = expression.find("x");

			while (pos != std::string::npos) 
			{
				expression.replace(pos, 1, std::to_string(fAngle));
				pos = expression.find("x", pos);
			}

			CurrentY[0] = int(nAmplitude * parser.Get(expression.c_str())) + nScreenHeight / 2;
			DrawLine(i - 1, PrevY[0], i, CurrentY[0], def::Pixel::SOLID, def::FG::RED);
			PrevY[0] = CurrentY[0];

			fAngle += (2.0f * 3.1415926535f) / nAmplitude;
		}

		if (GetKey(VK_UP).bHeld)
			nAmplitude += 1;

		if (GetKey(VK_DOWN).bHeld)
			nAmplitude -= 1;

		if (nAmplitude <= 0)
			nAmplitude = 1;

		return true;
	}

private:
	int nAmplitude = 40;
	int PrevY[3] = { 0, 0, 0 };

	Parser parser;
	std::string sExpression;
	double dResult;
	
};

int main()
{
	Graphs demo("sin(x * 10) * cos(x)");
	demo.Run(256, 240, 4, 4);
}
