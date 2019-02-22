#include "Vec2.h"
#include "script/Util.h"

Vec2::Vec2() : Vec2(0.0f, 0.0f) {}

Vec2::Vec2(float x, float y)
	:
x(x), y(y)
{
	//addFunction("add", [this](const script::ArrayObjectPtr& args)
	//{
	//	if (args->getCount() != 1)
	//		throw script::InvalidArgumentCount("Vec2::add(Vec2)", 1, args->getCount());
	//	// try to obtain vec2
	//	const script::ScriptObjectPtr& arg1 = args->get(0);
	//	Vec2* vecArg = dynamic_cast<Vec2*>(arg1.get());
	//	if (vecArg == nullptr)
	//		throw script::InvalidArgumentType("Vec2::add(Vec2)", 0, *arg1, "Vec2");
	//
	//	// finally execute the function
	//	this->add(*vecArg);
	//
	//	return this->shared_from_this();
	//});
	addFunction("add", script::Util::makeFunction(this, &Vec2::add, "Vec2::add(Vec2)"));
	addFunction("subtract", script::Util::makeFunction(this, &Vec2::subtract, "Vec2::subtract(Vec2)"));
	addFunction("negate", script::Util::makeFunction(this, &Vec2::negate, "Vec2::negate()"));
	addFunction("getX", script::Util::makeFunction(this, &Vec2::getX, "Vec2::float getX()"));
	addFunction("getY", script::Util::makeFunction(this, &Vec2::getY, "Vec2::float getY()"));
	addFunction("setX", script::Util::makeFunction(this, &Vec2::setX, "Vec2::setX(float x)"));
	addFunction("setY", script::Util::makeFunction(this, &Vec2::setY, "Vec2::setY(float y)"));
}

script::ScriptObject::FunctionT Vec2::getCtor()
{
	return script::Util::combineFunctions({
		script::Util::fromLambda([](const Vec2& other)
		{
			return std::make_shared<Vec2>(other.getX(), other.getY());
		}, "Vec2(Vec2)"),
		script::Util::fromLambda([](float x, float y)
		{
			return std::make_shared<Vec2>(x, y);
		}, "Vec2(float x, float y)"),
		script::Util::fromLambda([](float val)
		{
			return std::make_shared<Vec2>(val, val);
		}, "Vec2(float)")
	});
}

std::string Vec2::toString() const
{
	return "Vec2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

script::ScriptObjectPtr Vec2::clone() const
{
	return std::make_shared<Vec2>(x, y);
}

bool Vec2::equals(const script::ScriptObjectPtr& other) const
{
	const auto v = std::dynamic_pointer_cast<Vec2>(other);
	// cast failed
	if (!v) return false;
	// memberwise equal
	return x == v->x && y == v->y;
}

void Vec2::add(const Vec2& other)
{
	x += other.x;
	y += other.y;
}

void Vec2::subtract(const Vec2& other)
{
	x -= other.x;
	y -= other.y;
}

void Vec2::negate()
{
	x *= -1.0f;
	y *= -1.0f;
}

float Vec2::getX() const
{
	return x;
}

float Vec2::getY() const
{
	return y;
}

void Vec2::setX(float x)
{
	this->x = x;
}

void Vec2::setY(float y)
{
	this->y = y;
}
