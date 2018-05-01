
#include "Globals.h"


Double2::Double2() {}
Double2::Double2(double x, double y) : X(x), Y(y) {}
Double2::Double2(double pos[2]) : X(pos[0]), Y(pos[1]) {}
Double2::Double2(Double2 &pos) : X(pos.X), Y(pos.Y) {}
Double2::Double2(Int2 &pos) : X(pos.X), Y(pos.Y) {}

double Double2::operator[](int i) { if (i == 0) { return X; } else if (i == 1) { return Y; } else { Throw("ERROR::Index out Of Range\n"); return 0; } }
Double2 Double2::operator+(Double2 other) { other.X = X + other.X; other.Y = Y + other.Y; return other; }
Double2 Double2::operator-(Double2 other) { other.X = X - other.X; other.Y = Y - other.Y; return other; }
Double2 Double2::operator*(Double2 other) { other.X = X * other.X; other.Y = Y * other.Y; return other; }
Double2 Double2::operator/(Double2 other) { other.X = X / other.X; other.Y = Y / other.Y; return other; }
Double2 Double2::operator+(double val) { Double2 temp = { X + val, Y + val }; return temp; }
Double2 Double2::operator-(double val) { Double2 temp = { X - val, Y - val }; return temp; }
Double2 Double2::operator*(double val) { Double2 temp = { X * val, Y * val }; return temp; }
Double2 Double2::operator/(double val) { Double2 temp = { X / val, Y / val }; return temp; }
bool Double2::operator==(Double2 other) { return X == other.X && Y == other.Y; }

Int2 Double2::ToInt2() { return{ (int)X, (int)Y }; }


double MapVal(double iRangeMin, double iRangeMax, double fRangeMin, double fRangeMax, double val)
{
	return ((val - iRangeMin) / (iRangeMax - iRangeMin) * (fRangeMax - fRangeMin)) + fRangeMin;
}

double Round(double val, double nearest)
{
	return round(val / nearest)*nearest;
}

double Floor(double val, double nearest)
{
	return floor(val / nearest)*nearest;
}

double Ceil(double val, double nearest)
{
	return ceil(val / nearest)*nearest;
}

std::string CharToString(char *buf, int length)
{
	std::string str(length, 0x00);
	for (int i = 0; i < length; i++)
	{
		str[i] = buf[i];
	}
	return str;
}





