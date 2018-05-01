

namespace DnD
{
	class World;
	class Server;
	class WorldObject;
	class Lines;
	class Ellipse;
	class Text;
	class Environment;
	class Layer;
}

#pragma once

#include "DnDProgram.h"
#include <WinLibTypes.h>
#include "Globals.h"


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

	/*class Player
	{
	public:
		std::string Name;
		Int2 Postition;
		SelectState Selected = Unselected;
		bool Visible = true;
		SOCKET ClientSocket = INVALID_SOCKET;

		Player();
	};*/
	class Server
	{
	public:
		SOCKET ConnectSocket = INVALID_SOCKET;
	};

	uint32_t HashColour(Colour colour, uint32_t hash = 0);
	uint32_t HashInt(uint32_t val, uint32_t hash = 0);
	uint32_t HashLong(uint64_t val, uint32_t hash = 0);
	uint32_t HashString(std::string str, uint32_t hash = 0);
	uint32_t HashBool(bool val, uint32_t hash = 0);
	uint32_t HashInt2(Int2 val, uint32_t hash = 0);
	uint32_t HashDouble2(Double2 val, uint32_t hash = 0);

	class WorldObject
	{
	public:
		//std::vector<Int2> LineList;
		//bool Closed;
		Colour ObjectColour = { 255,255,255 };
		WorldObjectTypes Type;
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
		virtual uint32_t CalculateHash();
		virtual int LoadFromData(char *data, WorldObjectTypes type);
	};

	class Lines : public WorldObject
	{
	public:
		std::vector<Int2> LineList;
		bool Closed = false;

		Lines();
		//Lines(const Lines &other);

		void Draw(Surface *surface, World *world);
	private:
		bool ClickedOnCalculation(Double2 pos1, Double2 pos2, Double2 click, double scale);
	public:
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
	
		//void UpdateBounds();
		uint32_t CalculateHash();
		int LoadFromData(char *data, WorldObjectTypes type);
	};

	class Ellipse : public WorldObject
	{
	public:
		Int2 Pos;
		Int2 Radius;

		Ellipse();

		void Draw(Surface *surface, World *world);
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
		//void UpdateBounds();
		uint32_t CalculateHash();
		int LoadFromData(char *data, WorldObjectTypes type);
	};

	class Text : public WorldObject
	{
	public:
		Int2 Pos;
		std::string TextData;
		int FontSize = 20;

		Text();

		void Draw(Surface *surface, World *world);
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
		//void UpdateBounds();
		uint32_t CalculateHash();
		int LoadFromData(char *data, WorldObjectTypes type);
	};

	class Layer
	{
	public:
		std::vector<WorldObject *> ObjectList;
		SelectState Selected = Unselected;
		std::string Name;
		bool Visible = true;

		Layer();
		~Layer();

		void Draw(Surface *surface, World *world);
		uint32_t CalculateHash();
		int LoadFromData(char *data);
	};

	class Environment
	{
	public:
		std::vector<Layer *> LayerList;
		SelectState Selected = Unselected;
		std::string Name;
		bool Visible = true;

		Environment();
		~Environment();

		void Draw(Surface *surface, World *world);
		uint32_t CalculateHash();
		int LoadFromData(char *data);
	};



	class World

	{
	public:
		Window *ParentWindow;
		DnDProgram *Program;

		//std::vector<Layer> LayerList;
		std::vector<Environment *> EnvironmentList;

		Double2 ViewPos = { 0,0 };
		double Scale = 1;

		bool DrawGrid = true;
		int GridSize = 10;

		std::string Name = "Player";
		Int2 Position;

		World(Window *parentWindow, DnDProgram *program);
		~World();

		void Draw(Environment *activeEnvironment);

		Double2 Transform(Double2 point);
		Double2 ReverseTransform(Double2 point);

		void DeselectEnvironments();
		void DeselectLayers();
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
	};
}
