

namespace DnD
{
	class World;
	class Player;
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

	class Player
	{
	public:
		std::string Name;
		Int2 Position;
		SelectState Selected = Unselected;
		bool Visible = true;
		bool Connected = false;
		//SOCKET ClientSocket = INVALID_SOCKET;

		//std::thread RecieveThread;

		int HashEnvironmentIndex = -1;
		int HashLayerIndex = -1;
		int HashObjectIndex = -1;

		Player();
		~Player();
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
		std::vector<Player *> PlayerList;

		WorldObject();

		virtual void Draw(Surface *surface, World *world, Player *activePlayer);
		//virtual bool ClickedOn(Double2 pos, double scale);
		virtual bool ClickedOn(Double2 pos, double scale, Surface *surface);
		//virtual void UpdateBounds();
		//virtual bool Intersect();
		//virtual bool Encompass();

		virtual uint32_t CalculateHash();
	};

	class Lines : public WorldObject
	{
	public:
		std::vector<Int2> LineList;
		bool Closed = false;

		Lines();
		//Lines(const Lines &other);

		void Draw(Surface *surface, World *world, Player *activePlayer);
	private:
		bool ClickedOnCalculation(Double2 pos1, Double2 pos2, Double2 click, double scale);
	public:
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
	
		//void UpdateBounds();
		uint32_t CalculateHash();
	};

	class Ellipse : public WorldObject
	{
	public:
		Int2 Pos;
		Int2 Radius;

		Ellipse();

		void Draw(Surface *surface, World *world, Player *activePlayer);
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
		//void UpdateBounds();

		uint32_t CalculateHash();
	};

	class Text : public WorldObject
	{
	public:
		Int2 Pos;
		std::string TextData;
		int FontSize = 20;

		Text();

		void Draw(Surface *surface, World *world, Player *activePlayer);
		bool ClickedOn(Double2 pos, double scale, Surface *surface);
		Int2 GetSize(Surface *surface, World *world);
		//void UpdateBounds();

		uint32_t CalculateHash();
	};

	class Layer
	{
	public:
		std::vector<WorldObject *> ObjectList;
		SelectState Selected = Unselected;
		std::string Name;
		bool Visible = true;
		std::vector<Player *> PlayerList;

		Layer();
		~Layer();

		void Draw(Surface *surface, World *world, Player *activePlayer);

		uint32_t CalculateHash();
	};

	class Environment
	{
	public:
		std::vector<Layer *> LayerList;
		SelectState Selected = Unselected;
		std::string Name;
		bool Visible = true;
		std::vector<Player *> PlayerList;

		Environment();
		~Environment();

		void Draw(Surface *surface, World *world, Player *activePlayer);

		uint32_t CalculateHash();
	};



	class World

	{
	public:
		Window *ParentWindow;
		DnDProgram *Program;

		//std::vector<Layer> LayerList;
		std::vector<Environment *> EnvironmentList;
		std::vector<Player *> PlayerList;

		Double2 ViewPos = { 0,0 };
		double Scale = 1;

		bool DrawGrid = true;
		int GridSize = 10;

		World(Window *parentWindow, DnDProgram *program);
		~World();

		void Draw(Environment *activeEnvironment, Player *activePlayer);

		DnD::Player *ConnectPlayer(std::string name);
		void AddPlayer(DnD::Player *player);
		void RemovePlayer(DnD::Player *player);
		DnD::Player *RemovePlayer(std::string name);

		Double2 Transform(Double2 point);
		Double2 ReverseTransform(Double2 point);

		void DeselectPlayers();
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
