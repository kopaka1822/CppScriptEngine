#pragma once
#include "script/objects/GetValueObject.h"

class Vec2 : public script::GetValueObject<Vec2>
{
public:
	Vec2();
	Vec2(float x, float y);
	~Vec2() override = default;

	static FunctionT getCtor();

	Vec2& getValue() override;

	std::string toString() const override;
	script::ScriptObjectPtr clone() const override;
	bool equals(const script::ScriptObjectPtr& other) const override;

	void add(const Vec2& other);
	void subtract(const Vec2& other);
	void negate();

	float getX() const;
	float getY() const;
	void setX(float x);
	void setY(float y);

	float x;
	float y;
};