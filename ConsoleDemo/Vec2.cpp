#include "Vec2.h"
#include "script/Util.h"

Vec2::Vec2() : Vec2(0.0f, 0.0f) {}

Vec2::Vec2(float x, float y)
	:
x(x), y(y)
{
	addFunction("add", script::Util::makeFunction(this, &Vec2::add, "add(vec2)"));
	addFunction("subtract", script::Util::makeFunction(this, &Vec2::subtract, "subtract(vec2)"));
	addFunction("negate", script::Util::makeFunction(this, &Vec2::negate, "negate()"));
	addFunction("getX", script::Util::makeFunction(this, &Vec2::getX, "float getX()"));
	addFunction("getY", script::Util::makeFunction(this, &Vec2::getY, "float getY()"));
	addFunction("setX", script::Util::makeFunction(this, &Vec2::setX, "setX(float x)"));
	addFunction("setY", script::Util::makeFunction(this, &Vec2::setY, "setY(float y)"));
}

Vec2& Vec2::getValue()
{
	return *this;
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
