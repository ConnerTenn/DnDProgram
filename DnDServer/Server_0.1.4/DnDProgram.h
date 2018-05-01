


namespace DnD
{
	class DnDProgram;
}

#pragma once


#include "NetworkManager.h"
#include <WinLib.h>
#include <fstream>
#include "World.h"
#include "CommandInterpreter.h"
#include "FileReader.h"
#include "Globals.h"

namespace DnD
{

	class DnDProgram
	{
	public:
		struct GraphicWindow
		{
			Window *WindowPtr;
			World *GameWorld;
			Surface *InterfaceSurface;
			bool Pan = false;
			Int2 InitialMousePos = { 0,0 };
			Double2 InitialViewPos = { 0,0 };
			std::vector<Int2> InitialObjectPos = {};
			int SubSelectIndex = 0;
			bool Zoom = false;
			enum Tool
			{
				Select,
				SubSelect,
				DrawLines,
				DrawEllipse,
				PlaceText,
				PlaceCharacter
			} SelectedTool = Select;
			/*struct ObjectSelect
			{
				WorldObject *Object = 0;
				int SelectIndex = 0;
			} ActiveObject;
			int ActiveEnvironment = 0;
			int ActiveLayer = 0;*/

			Lines *ActiveLines = 0;
			Ellipse *ActiveEllipse = 0;
			Text *ActiveText = 0;
			Character *ActiveCharacter = 0;

			void Callback(MessageData message, DnDProgram *program);
			void DrawSelectMenu(DnDProgram *program);

			Colour SelectedColour = { 255,255,255 };
			int FontSize = 20;
			bool ChangeFontSize = false;
			bool ChangeGridSize = false;
			bool SnapToGrid = false;

			int PlayerScroll = 0;
			int EnvironmentScroll = 0;
			int LayerScroll = 0;
			int ObjectScroll = 0;
			Player *SelectedPlayer = 0; int SelectedPlayerIndex = 0; std::vector<Player *> ActivePlayers; //Player *ActivePlayer = 0;
			Environment *SelectedEnvironment = 0; int SelectedEnvironmentIndex = -1;
			Layer *SelectedLayer = 0; int SelectedLayerIndex = -1;
			WorldObject *SelectedObject = 0; int SelectedObjectIndex = -1;

			bool EditPlayerName = false;
			bool EditEnvironmentName = false;
			bool EditLayerName = false;
			bool EditObjectName = false;

			bool Drag = false;

			//int eIndex, int lIndex, int oIndex
			void SelectObject(int eIndex, int lIndex, int oIndex);
			inline void SelectObject(int oIndex);
			void SubSelectObject(int eIndex, int lIndex, int oIndex);
			inline void SubSelectObject(int oIndex);
			void SetObjectSelected(int eIndex, int lIndex, int oIndex);
			inline void SetObjectSelected(int oIndex);
			void DeselectObject(int eIndex, int lIndex, int oIndex);
			inline void DeselectObject(int oIndex);
			
			void SelectLayer(int eIndex, int lIndex);
			inline void SelectLayer(int lIndex);
			void SetLayerSelected(int eIndex, int lIndex);
			inline void SetLayerSelected(int lIndex);
			void DeselectLayer(int eIndex, int lIndex);
			inline void DeselectLayer(int lIndex);
			
			void SelectEnvironment(int eIndex);
			void SetEnvironmentSelected(int eIndex);
			void DeselectEnvironment(int eIndex);
			
			void SelectPlayer(int index);
			void SubSelectPlayer(int index);
			void SetPlayerSelected(int index);
			void DeselectPlayer(int index);

			void SetObjectContainsPlayers(WorldObject *object);
			void SetPlayersActive(std::vector<Player *> players);
			void AddActivePlayer(Player *player);
			void RemoveActivePlayer(Player *player);
		} GraphicWin;

		struct ConsoleWindow
		{
			Window *WindowPtr;
			Console *CommandLine;
			Console *Command;
		} ConsoleWin;

		CommandInterpretor *Interpreter;
		FileReader *FileLoader;
		NetworkManager *Network;


		DnDProgram(HINSTANCE hInstance, INT iCmdShow);
		~DnDProgram();

		void InitGraphicWindow();
		void InitConsoleWindow();

		static void GraphicCallback(Window *window, MessageData message, void *data);
		static void ConsoleCallback(Window *window, MessageData message, void *data);

		void Run();
		void ProcessCommand(std::string command);

		void Save(std::string name);
		void Save2(std::string name);
		void Load(std::string name);
		void Load2(std::string name);
	};
}

