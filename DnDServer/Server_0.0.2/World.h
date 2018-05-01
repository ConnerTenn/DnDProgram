
#pragma once

#include <vector>
#include <WinLibTypes.h>

namespace DnD
{
	class World;
}


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

namespace DnD
{

	enum WorldObjectTypes
	{
		LineObject,
		EllipseObject,
		TextObject
	};

	enum SelectState
	{
		Unselected = 0,
		Selected = 1,
		SubSelected = 2
	};

	class WorldObject
	{
	public:
		//std::vector<Int2> LineList;
		//bool Closed;
		Colour ObjectColour = { 255,255,255 };
		WorldObjectTypes Type;
		UUID ObjectID;
		SelectState Selected = Unselected;
		std::string Name;
		bool Visible = true;
		//Line Bounds;

		WorldObject();

		virtual void Draw(Surface *surface, World *world);
		//virtual bool ClickedOn(Double2 pos, double scale);
		virtual bool ClickedOn(Double2 pos, double scale, Surface *surface);
		//virtual void UpdateBounds();
		//virtual bool Intersect();
		//virtual bool Encompass();
	};

	class Lines : public WorldObject
	{
	public:
		std::vector<Int2> LineList;
		bool Closed = false;

		Lines();

		void Draw(Surface *surface, World *world);
	private:
		bool ClickedOnCalculation(Double2 pos1, Double2 pos2, Double2 click, double scale);
	public:
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
	
		//void UpdateBounds();
	};

	class Ellipse : public WorldObject
	{
	public:
		Int2 Pos;
		Int2 Radius;
		bool Closed;

		Ellipse();

		void Draw(Surface *surface, World *world);
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
		//void UpdateBounds();
	};

	class Text : public WorldObject
	{
	public:
		Int2 Pos;
		std::string TextData;
		bool Closed;

		Text();

		void Draw(Surface *surface, World *world);
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
		//void UpdateBounds();
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
		UUID ObjectID;
		SelectState Selected = Unselected;
		std::string Name;
		bool Visible = true;

		Layer();
		~Layer();

		void Draw(Surface *surface, World *world);
	};

	class Environment
	{
	public:
		std::vector<Layer *> LayerList;
		UUID ObjectID;
		SelectState Selected = Unselected;
		std::string Name;
		bool Visible = true;

		Environment();
		~Environment();

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

		//std::vector<Layer> LayerList;
		std::vector<Environment *> EnvironmentList;


		//ActiveLayer
		//ActiveLineLoop
		//ActiveType
		Double2 ViewPos = { 0,0 };
		double Scale = 1;

		World(Window *parentWindow);
		~World();

		void Draw();

		Double2 Transform(Double2 point);
		Double2 ReverseTransform(Double2 point);

		void DeselectObjects();
		void Remove(WorldObject *object);

		//void AddWall();
		//void AddObject();
		//void AddBoarders();
		//void AddGroundPlane();

		//void BeginNewLineLoop(Int2 point);
		//void AddPointToCurrentLineLoop(Int2 point);
		//void UpdateNewPointPos(Int2 point);
		//void EndLineLoop(bool closed);
		//void CancelLineLoop();

		//WorldObject *SelectWorldObjectAt();

		void Save();
		void Load();
	};
}
