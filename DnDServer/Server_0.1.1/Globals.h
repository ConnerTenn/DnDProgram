
#pragma once

#include <vector>
#include <WinLibTypes.h>
#include <math.h>
#include <string>
#include <strstream>
#include <iomanip>

const std::string ProgramVersion = "0.1.1";

struct Double2
{
	double X = 0;
	double Y = 0;

	Double2();
	Double2(double x, double y);
	Double2(double pos[2]);
	Double2(Double2 &pos);
	Double2(Int2 &pos);

	double operator[](int i);
	Double2 operator+(Double2 other);
	Double2 operator-(Double2 other);
	Double2 operator*(Double2 other);
	Double2 operator/(Double2 other);
	Double2 operator+(double val);
	Double2 operator-(double val);
	Double2 operator*(double val);
	Double2 operator/(double val);
	bool operator==(Double2 other);

	Int2 ToInt2();
};

struct Line
{
	Int2 Pos1;
	Int2 Pos2;
};

double MapVal(double iRangeMin, double iRangeMax, double fRangeMin, double fRangeMax, double val);

double Round(double val, double nearest = 1);
double Floor(double val, double nearest = 1);
double Ceil(double val, double nearest = 1);


std::string IntToHex(unsigned long long num);

std::string CharToString(char *buf, int length);
void StringToChar(std::string str,char **buf);

//template<typename T>
//T GetEnd(std::vector<T> &list);


