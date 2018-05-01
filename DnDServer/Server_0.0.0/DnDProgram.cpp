
#include "DnDProgram.h"


DnDProgram::DnDProgram(HINSTANCE hInstance, INT iCmdShow)
{
	WindowInfo windowInfo;
	
	windowInfo.Name = "GraphicWindow";
	windowInfo.Dimensions = { 1000,600 };
	windowInfo.ClearColour = Colour(0, 0, 0);
	windowInfo.HInstance = hInstance;
	windowInfo.ICmdShow = iCmdShow;
	windowInfo.Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	windowInfo.Callback = DnDProgram::GraphicCallback;
	windowInfo.CallbackPtr = this;
	GraphicWin.Window = new Window(&windowInfo);

	windowInfo.Name = "ConsoleWindow";
	windowInfo.Dimensions = { 587, 720 + 19 };
	windowInfo.ClearColour = { 10, 10, 40, 255 };
	windowInfo.HInstance = hInstance;
	windowInfo.ICmdShow = iCmdShow;
	windowInfo.Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	windowInfo.Callback = DnDProgram::ConsoleCallback;
	windowInfo.CallbackPtr = this;
	ConsoleWin.Window = new Window(&windowInfo);

	GraphicWin.Window->Create();
	ConsoleWin.Window->Create();

	InitGraphicWindow();
	InitConsoleWindow();

	GraphicWin.GameWorld = new World(GraphicWin.Window);

	Interpreter = new CommandInterpretor(this);

	GraphicWin.Window->Draw();
	ConsoleWin.Window->Draw();
}

DnDProgram::~DnDProgram()
{
	delete Interpreter;
	delete GraphicWin.GameWorld;
	delete GraphicWin.InterfaceSurface;
	delete GraphicWin.Window;
	delete ConsoleWin.Window;
}

void DnDProgram::InitGraphicWindow()
{
	GraphicWin.Window->WindowSurface->Clear();

	SurfaceInfo surfaceInfo;
	surfaceInfo.Dimensions = { 200, GraphicWin.Window->Dimensions.Y };
	surfaceInfo.ClearColour = Colour(20, 20, 50);
	surfaceInfo.ParentWindow = GraphicWin.Window;
	GraphicWin.InterfaceSurface = new Surface(&surfaceInfo);
}

void DnDProgram::InitConsoleWindow()
{
	ConsoleInfo consoleInfo;

	consoleInfo.Dimensions = { 100,60 };
	consoleInfo.ParentWindow = ConsoleWin.Window;
	consoleInfo.TextFont = Font("Consolas", 8, Colour(200, 200, 255));
	ConsoleWin.CommandLine = new Console(&consoleInfo);

	consoleInfo.Dimensions = { 100,1 };
	consoleInfo.ParentWindow = ConsoleWin.Window;
	consoleInfo.TextFont = Font("Consolas", 8, Colour(200, 200, 255));
	ConsoleWin.Command = new Console(&consoleInfo);

	ConsoleWin.Window->WindowSurface->Clear();
	ConsoleWin.CommandLine->Draw(ConsoleWin.Window->WindowSurface, { 0, 0 });
	//ConsoleWin.Window->WindowSurface->CopySurface({ { 0, 0 },{ ConsoleWin.Console->TextSurface->Dimensions.X, ConsoleWin.Console->TextSurface->Dimensions.Y } }, ConsoleWin.Console->TextSurface, { { 0, 0 },{ ConsoleWin.Console->TextSurface->Dimensions.X, ConsoleWin.Console->TextSurface->Dimensions.Y } });
	//ConsoleWin.Window->WindowSurface->DrawRectangle(0, 0, ConsoleWin.Console->TextSurface->Dimensions.X, ConsoleWin.Console->TextSurface->Dimensions.Y, Colour(0, 255, 0));
	//ConsoleWin.Window->WindowSurface->DrawText(0, 720, std::string(">") + ConsoleWin.Command, consoleInfo.TextFont);
	ConsoleWin.Command->Draw(ConsoleWin.Window->WindowSurface, { 0, 720 });
}

void DnDProgram::GraphicCallback(Window *window, MessageData Message, void *data)
{
	DnDProgram *program = (DnDProgram *)data;
	World *gameWorld = program->GraphicWin.GameWorld;

	if (Message == WL_UPDATESURFACE)
	{
		window->WindowSurface->Clear();
		gameWorld->Draw();
		if (program->GraphicWin.ActiveObject.Object)
		{
			WorldObject *object = program->GraphicWin.ActiveObject.Object;
			for (int i = 0; i < object->LineList.size() - 1; i++)
			{
				program->GraphicWin.Window->WindowSurface->DrawLine(gameWorld->Transform(object->LineList[i]).ToInt2(), gameWorld->Transform(object->LineList[i + 1]).ToInt2(), Colour(255, 255, 0), 2.0);
			}
		}
		program->GraphicWin.InterfaceSurface->Clear();
		program->GraphicWin.InterfaceSurface->DrawText({ 10,10 }, "Player Select", Font("Consolas", 12, Colour{ 255,255,255 }));
		program->GraphicWin.InterfaceSurface->DrawRectangle({ 10,10 }, { 190,150 }, Colour(255, 255, 255));
		program->GraphicWin.InterfaceSurface->DrawText({ 10,160 }, "Environment Select", Font("Consolas", 12, Colour{ 255,255,255 }));
		program->GraphicWin.InterfaceSurface->DrawRectangle({ 10,160 }, { 190,300 }, Colour(255, 255, 255));
		program->GraphicWin.InterfaceSurface->DrawText({ 10,310 }, "Layer Select", Font("Consolas", 12, Colour{ 255,255,255 }));
		program->GraphicWin.InterfaceSurface->DrawRectangle({ 10,310 }, { 190,450 }, Colour(255, 255, 255));
		window->WindowSurface->CopySurface(program->GraphicWin.InterfaceSurface->GetRect() + Int2{800,0}, program->GraphicWin.InterfaceSurface, program->GraphicWin.InterfaceSurface->GetRect());
		program->GraphicWin.Window->Draw();
	}
	if (Message == WM_KEYDOWN)
	{
		if (Message.WParam() == 'W') { gameWorld->ViewPos.Y -= 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (Message.WParam() == 'S') { gameWorld->ViewPos.Y += 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (Message.WParam() == 'A') { gameWorld->ViewPos.X -= 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (Message.WParam() == 'D') { gameWorld->ViewPos.X += 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (Message.WParam() == 'E') { gameWorld->Scale *= 1.3; window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (Message.WParam() == 'Q') { gameWorld->Scale *= 0.7; window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
	}
	//panning code
	if (Message == WM_LBUTTONDOWN  && program->GraphicWin.Window->Input->MouseState.GetPos().X < 800)
	{
		//record the initial mouse and view pos
		program->GraphicWin.InitialMousePos = { GET_X_LPARAM(Message.LParam()) , GET_Y_LPARAM(Message.LParam()) };
		program->GraphicWin.InitialViewPos = program->GraphicWin.GameWorld->ViewPos.ToInt2();
		if (program->GraphicWin.Window->Input->KeyState[' '].Pressed) { program->GraphicWin.Pan = true; program->ConsoleWin.CommandLine->Write("DEBUG::Pan True\n"); }
	}
	if (program->GraphicWin.Pan && (Message == WM_LBUTTONUP || (Message == WM_KEYUP && Message.WParam() == VK_SPACE)))
	{
		program->GraphicWin.Pan = false;
		program->ConsoleWin.CommandLine->Write("DEBUG::Pan False\n");
	}
	if (Message == WM_MOUSEMOVE && program->GraphicWin.Pan)
	{
        Int2 currentPos = { GET_X_LPARAM(Message.LParam()) , GET_Y_LPARAM(Message.LParam()) };//set the current mouse pos
		//set the view to translate based on the difference between the current mouse and initial mouse pos. The translation is then added to initial view pos before being applied.
		program->GraphicWin.GameWorld->ViewPos = program->GraphicWin.InitialViewPos + Double2{(1.0/program->GraphicWin.GameWorld->Scale), (1.0/program->GraphicWin.GameWorld->Scale)} * (program->GraphicWin.InitialMousePos - currentPos);
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		//program->ConsoleWin.CommandLine->Write("DEBUG:: PanX: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + " PanY: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + "\n");
	}
	//zooming code
	if (Message == WM_KEYDOWN && Message.WParam() == VK_CONTROL) { program->GraphicWin.Zoom = true; program->ConsoleWin.CommandLine->Write("DEBUG::Scroll True\n"); }
	if (Message == WM_KEYUP && Message.WParam() == VK_CONTROL) { program->GraphicWin.Zoom = false; program->ConsoleWin.CommandLine->Write("DEBUG::Scroll False\n"); }
	if (Message == WM_MOUSEWHEEL && program->GraphicWin.Zoom && program->GraphicWin.Window->Input->MouseState.GetPos().X < 800)
	{
		Int2 currentPos = program->GraphicWin.Window->Input->MouseState.GetPos();//get the current mouse pos
		double scale = 1.0 + 0.3 * ((double)GET_WHEEL_DELTA_WPARAM(Message.WParam()) / WHEEL_DELTA); //calculate the scaling based on the scroll
		Double2 pos1 = program->GraphicWin.GameWorld->ReverseTransform(currentPos); //calculate the initial pos of the cursor before scaling
		program->GraphicWin.GameWorld->Scale *= scale; //apply the scaling
		Double2 pos2 = program->GraphicWin.GameWorld->ReverseTransform(currentPos); //calculate the initial pos of the cursor after scaling
		program->GraphicWin.GameWorld->ViewPos = (Double2(program->GraphicWin.GameWorld->ViewPos) + (pos1 - pos2)).ToInt2(); //translate the view by the difference in the 2 positions
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); program->ConsoleWin.CommandLine->Write("DEBUG::Scroll\n"); 
	}

	//Selecting and Adding Code
	if (Message == WM_LBUTTONDOWN && !program->GraphicWin.Pan)
	{
		Int2 currentPos = program->GraphicWin.GameWorld->ReverseTransform(program->GraphicWin.Window->Input->MouseState.GetPos()).ToInt2();//get the current mouse pos

		if (program->GraphicWin.SelectedTool == DnDProgram::GraphicWindow::Tool::DrawWall)
		{
			if (program->GraphicWin.ActiveObject.Object)
			{
				program->ConsoleWin.CommandLine->Write("Add to New Wall\n");
				program->GraphicWin.ActiveObject.Object->LineList.push_back(currentPos);
				program->GraphicWin.ActiveObject.SelectIndex = (int)program->GraphicWin.ActiveObject.Object->LineList.size() - 1;
			}
			else
			{
				program->ConsoleWin.CommandLine->Write("Create New Wall\n");
				//if (program->GraphicWin.SelectedTool == DnDProgram::GraphicWindow::Tool::DrawWall)
				//{
				Walls *wall = new Walls;
				wall->LineList.push_back(currentPos);
				program->GraphicWin.GameWorld->EnvironmentList[program->GraphicWin.ActiveEnvironment].LayerList[program->GraphicWin.ActiveLayer].ObjectList.push_back(wall);
				program->GraphicWin.ActiveObject.Object = wall;

				wall->LineList.push_back(currentPos);
				program->GraphicWin.ActiveObject.SelectIndex = (int)wall->LineList.size() - 1;
				//}
			}
		}
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	}
	if (Message == WM_MOUSEMOVE)
	{
		Int2 currentPos = program->GraphicWin.GameWorld->ReverseTransform(program->GraphicWin.Window->Input->MouseState.GetPos()).ToInt2();//get the current mouse pos

		if (program->GraphicWin.ActiveObject.Object)
		{
			program->GraphicWin.ActiveObject.Object->LineList[program->GraphicWin.ActiveObject.SelectIndex] = currentPos;
		}
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	}
	if (Message == WM_KEYDOWN && Message.WParam() == VK_ESCAPE && program->GraphicWin.ActiveObject.Object)
	{
		program->GraphicWin.ActiveObject.Object->LineList.erase(program->GraphicWin.ActiveObject.Object->LineList.begin() + program->GraphicWin.ActiveObject.SelectIndex);
		
		if (program->GraphicWin.ActiveObject.Object->LineList.size() == 1)
		{
			//line is only a point; remove it
			//program->GraphicWin.GameWorld->EnvironmentList[program->GraphicWin.ActiveEnvironment].LayerList[program->GraphicWin.ActiveLayer].ObjectList.
				//add access by UUID
				//add access through functions
		}
		program->GraphicWin.ActiveObject.Object = 0;
		program->GraphicWin.ActiveObject.SelectIndex = 0;
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	}

}

void DnDProgram::ConsoleCallback(Window *window, MessageData Message, void *data)
{
	DnDProgram *program = (DnDProgram *)data;

	if (Message == WL_UPDATESURFACE)
	{
		window->WindowSurface->Clear();
		program->ConsoleWin.CommandLine->Draw(program->ConsoleWin.Window->WindowSurface, { 0,0 });
		program->ConsoleWin.Command->Draw(program->ConsoleWin.Window->WindowSurface, { 0, 720 });
		program->ConsoleWin.Window->WindowSurface->DrawRectangle(program->ConsoleWin.CommandLine->TextSurface->GetRect(), Colour(255, 255, 255));
		program->ConsoleWin.Window->WindowSurface->DrawRectangle(program->ConsoleWin.Command->TextSurface->GetRect() + Int2{ 0, 720 }, Colour(255, 255, 255));
		program->ConsoleWin.Window->Draw();
		
	}
	else if (Message == WM_KEYDOWN)
	{
		Key *keyState;
		keyState = program->ConsoleWin.Window->Input->KeyState;
		for (int i = 0; i < 49; i++)
		{
			if (Message.WParam() == program->ConsoleWin.Window->Input->CharacterMap[i][0])
			{
				if (program->ConsoleWin.Window->Input->KeyState[WL_SHIFT].Pressed)
				{
					program->ConsoleWin.Command->WriteChar(program->ConsoleWin.Window->Input->CharacterMapShift[i][1]);
				}
				else
				{
					program->ConsoleWin.Command->WriteChar(program->ConsoleWin.Window->Input->CharacterMap[i][1]);
				}
			}
		}
		if (Message.WParam() == WL_RETURN)
		{
			program->ConsoleWin.CommandLine->Write(std::string(">") + program->ConsoleWin.Command->Text[0] + "\n");
			program->ProcessCommand(program->ConsoleWin.Command->Text[0]);
			program->ConsoleWin.Command->Text[0] = "";
			program->ConsoleWin.Command->Cursor.X = 0;
		}
		if (Message.WParam() == WL_BACKSPACE)
		{
			program->ConsoleWin.Command->BackspaceChar();
		}
		if (Message.WParam() == WL_LEFT)
		{
			program->ConsoleWin.Command->MoveCursor(LEFT);
		}
		if (Message.WParam() == WL_RIGHT)
		{
			program->ConsoleWin.Command->MoveCursor(RIGHT);
		}
	}
}


void DnDProgram::Run()
{
	GraphicWin.Window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	ConsoleWin.Window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

	ConsoleWin.CommandLine->Write("FLOW::Begin Main Loop\n");
	while (!(GraphicWin.Window->State.Quit || ConsoleWin.Window->State.Quit))
	{
		GraphicWin.Window->UpdateMsg();
		ConsoleWin.Window->UpdateMsg();
	}
}

void DnDProgram::ProcessCommand(std::string command)
{
	Interpreter->Interpret(command);
	/*if (command == "exit") { ConsoleWin.Window->State.Quit = true; }
	else if (command == "draw")
	{
		GraphicWin.Window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		ConsoleWin.CommandLine->Write("DEBUG::Initiated Draw\n");
	}*/
}