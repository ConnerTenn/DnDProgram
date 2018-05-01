
namespace DnD
{
	class DnDProgram;
}

#pragma once

#include <WinLib.h>
#include "World.h"
#include "CommandInterpreter.h"

inline double MapVal(double iRangeMin, double iRangeMax, double fRangeMin, double fRangeMax, double val);

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
			bool Zoom = false;
			enum Tool
			{
				Select,
				SubSelect,
				DrawLines,
				DrawEllipse,
				PlaceText
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

			void Callback(MessageData message, DnDProgram *program);
			void DrawSelectMenu();

			Colour SelectedColour = { 255,255,255 };

			int PlayerScroll = 0;
			int EnvironmentScroll = 0;
			int LayerScroll = 0;
			int ObjectScroll = 0;
			Environment *SelectedEnvironment = 0;
			Layer *SelectedLayer = 0;
			WorldObject *SelectedObject = 0;

			bool Drag = false;

			bool DrawGrid = false;
			int GridSize = 10;
		} GraphicWin;

		struct ConsoleWindow
		{
			Window *WindowPtr;
			Console *CommandLine;
			Console *Command;
		} ConsoleWin;

		CommandInterpretor *Interpreter;


		DnDProgram(HINSTANCE hInstance, INT iCmdShow);
		~DnDProgram();

		void InitGraphicWindow();
		void InitConsoleWindow();

		static void GraphicCallback(Window *window, MessageData message, void *data);
		static void ConsoleCallback(Window *window, MessageData message, void *data);

		void Run();
		void ProcessCommand(std::string command);
	};
}

