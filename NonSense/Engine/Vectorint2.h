#pragma once
class Vectorint2
{
public:
	int x, y;

	Vectorint2();
	Vectorint2(int, int);

	~Vectorint2() {}

public:
	bool operator==(const Vectorint2& rhs) const { return (rhs.x == x) && (rhs.y == y); }

	const Vectorint2& GetVector2() { return Vectorint2(x, y); }
	int GetX() { return x; }
	int GetY() { return y; }
};