
#pragma once

#include <vector>
#include <WinLibTypes.h>

class World;


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

	Int2 ToInt2();
};

struct Line
{
	Int2 Pos1;
	Int2 Pos2;
};

enum WorldObjectTypes
{
	Wall,
	Objects,
	Outlines,
	Borders,
	GroundPlane,
	Text
};

class WorldObject
{
public:
	std::vector<Int2> LineList;
	bool Closed;
	WorldObjectTypes Type;
	UUID ObjectID;
	
	WorldObject();

	virtual void Draw(Surface *surface, World *world);
	bool Intersect();
	bool Encompass();
};

class Walls : public WorldObject
{
public:
	void Draw(Surface *surface, World *world);
};
/*
class Objects : public WorldObject
{
public:
	void Draw();
};

class Boarders : public WorldObject
{
public:
	//list of sub elements

	void Draw();//will draw sub elements
};

class GroundPlane : public WorldObject
{
public:
	
	void Draw();//no drawing actually done, will restrict drawing

	void Occlude();
};*/

class Layer
{
public:
	std::vector<WorldObject *> ObjectList;

	void Draw(Surface *surface, World *world);
};

class Environment
{
public:
	std::vector<Layer> LayerList;

	void Draw(Surface *surface, World *world);
};

class World
{
public:
	Window *ParentWindow;

	//Walls
	//Objects
	//Outlines
	//Boarders
	//ground plane?

	//layers

	std::vector<Layer> LayerList;
	std::vector<Environment> EnvironmentList;

	//use oct trees?

	//ActiveLayer
	//ActiveLineLoop
	//ActiveType
	Double2 ViewPos = { 0,0 };
	double Scale = 1;

	World(Window *parentWindow);
	~World();

	void Draw();
	//Draw Each layer
	//Draw the CurrentLoop
	Double2 Transform(Double2 point);
	Double2 ReverseTransform(Double2 point);

	void AddWall();
	//void AddObject();
	//void AddBoarders();
	//void AddGroundPlane();

	void BeginNewLineLoop(Int2 point);
	void AddPointToCurrentLineLoop(Int2 point);
	void UpdateNewPointPos(Int2 point);
	void EndLineLoop(bool closed);
	void CancelLineLoop();

	WorldObject *SelectWorldObjectAt();

	void Save();
	void Load();
};

