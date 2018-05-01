
struct OBID;
class DnDProgram;

#pragma once

#include <WinLib.h>
#include "World.h"
#include "CommandInterpreter.h"



class DnDProgram
{
public:
	struct GraphicWindow
	{
		Window *Window;
		World *GameWorld;
		Surface *InterfaceSurface;
		bool Pan = false;
		Int2 InitialMousePos = { 0,0 };
		Double2 InitialViewPos = { 0,0 };
		bool Zoom = false;
		enum Tool
		{
			Select,
			DrawWall
		} SelectedTool = DrawWall;
		struct ObjectSelect
		{
			WorldObject *Object = 0;
			int SelectIndex = 0;
		} ActiveObject;
		int ActiveEnvironment = 0;
		int ActiveLayer = 0;
	} GraphicWin;
	struct ConsoleWindow
	{
		Window *Window;
		Console *CommandLine;
		Console *Command;
	} ConsoleWin;

	CommandInterpretor *Interpreter;
	

	DnDProgram(HINSTANCE hInstance, INT iCmdShow);
	~DnDProgram();

	void InitGraphicWindow();
	void InitConsoleWindow();

	static void GraphicCallback(Window *window, MessageData Message, void *data);
	static void ConsoleCallback(Window *window, MessageData Message, void *data);

	void Run();
	void ProcessCommand(std::string command);
};
