
#include "DnDProgram.h"


DnDProgram::DnDProgram(HINSTANCE hInstance, INT iCmdShow)
{
	WindowInfo windowInfo;
	
	windowInfo.Name = "Server_0.0.1  GraphicWindow";
	windowInfo.Dimensions = { 1000,600 };
	windowInfo.ClearColour = Colour(0, 0, 0);
	windowInfo.HInstance = hInstance;
	windowInfo.ICmdShow = iCmdShow;
	windowInfo.Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	windowInfo.Callback = DnDProgram::GraphicCallback;
	windowInfo.CallbackPtr = this;
	GraphicWin.WindowPtr = new Window(&windowInfo);

	windowInfo.Name = "Server_0.0.1  ConsoleWindow";
	windowInfo.Dimensions = { 587, 720 + 19 };
	windowInfo.ClearColour = { 10, 10, 40, 255 };
	windowInfo.HInstance = hInstance;
	windowInfo.ICmdShow = iCmdShow;
	windowInfo.Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	windowInfo.Callback = DnDProgram::ConsoleCallback;
	windowInfo.CallbackPtr = this;
	ConsoleWin.WindowPtr = new Window(&windowInfo);

	GraphicWin.WindowPtr->Create();
	ConsoleWin.WindowPtr->Create();

	InitGraphicWindow();
	InitConsoleWindow();

	GraphicWin.GameWorld = new World(GraphicWin.WindowPtr);

	Interpreter = new CommandInterpretor(this);

	GraphicWin.WindowPtr->Draw();
	ConsoleWin.WindowPtr->Draw();
}

DnDProgram::~DnDProgram()
{
	delete Interpreter;
	delete GraphicWin.GameWorld;
	delete GraphicWin.InterfaceSurface;
	delete GraphicWin.WindowPtr;
	delete ConsoleWin.WindowPtr;
}

void DnDProgram::InitGraphicWindow()
{
	GraphicWin.WindowPtr->WindowSurface->Clear();

	SurfaceInfo surfaceInfo;
	surfaceInfo.Dimensions = { 200, GraphicWin.WindowPtr->Dimensions.Y };
	surfaceInfo.ClearColour = Colour(20, 20, 50);
	surfaceInfo.ParentWindow = GraphicWin.WindowPtr;
	GraphicWin.InterfaceSurface = new Surface(&surfaceInfo);
}

void DnDProgram::InitConsoleWindow()
{
	ConsoleInfo consoleInfo;

	consoleInfo.Dimensions = { 100,60 };
	consoleInfo.ParentWindow = ConsoleWin.WindowPtr;
	consoleInfo.TextFont = Font("Consolas", 8, Colour(200, 200, 255));
	ConsoleWin.CommandLine = new Console(&consoleInfo);

	consoleInfo.Dimensions = { 100,1 };
	consoleInfo.ParentWindow = ConsoleWin.WindowPtr;
	consoleInfo.TextFont = Font("Consolas", 8, Colour(200, 200, 255));
	ConsoleWin.Command = new Console(&consoleInfo);

	ConsoleWin.WindowPtr->WindowSurface->Clear();
	ConsoleWin.CommandLine->Draw(ConsoleWin.WindowPtr->WindowSurface, { 0, 0 });
	//ConsoleWin.Window->WindowSurface->CopySurface({ { 0, 0 },{ ConsoleWin.Console->TextSurface->Dimensions.X, ConsoleWin.Console->TextSurface->Dimensions.Y } }, ConsoleWin.Console->TextSurface, { { 0, 0 },{ ConsoleWin.Console->TextSurface->Dimensions.X, ConsoleWin.Console->TextSurface->Dimensions.Y } });
	//ConsoleWin.Window->WindowSurface->DrawRectangle(0, 0, ConsoleWin.Console->TextSurface->Dimensions.X, ConsoleWin.Console->TextSurface->Dimensions.Y, Colour(0, 255, 0));
	//ConsoleWin.Window->WindowSurface->DrawText(0, 720, std::string(">") + ConsoleWin.Command, consoleInfo.TextFont);
	ConsoleWin.Command->Draw(ConsoleWin.WindowPtr->WindowSurface, { 0, 720 });
}

void DnDProgram::GraphicCallback(Window *window, MessageData message, void *data)
{
	DnDProgram *program = (DnDProgram *)data;
	World *gameWorld = program->GraphicWin.GameWorld;

	program->GraphicWin.Callback(message, program);
	/*if (Message == WL_UPDATESURFACE)
	{
		window->WindowSurface->Clear();
		gameWorld->Draw();
		if (program->GraphicWin.ActiveObject.Object)
		{
			WorldObject *object = program->GraphicWin.ActiveObject.Object;
			for (int i = 0; i < object->LineList.size() - 1; i++)
			{
				program->GraphicWin.WindowPtr->WindowSurface->DrawLine(gameWorld->Transform(object->LineList[i]).ToInt2(), gameWorld->Transform(object->LineList[i + 1]).ToInt2(), Colour(255, 255, 0), 2.0);
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
		program->GraphicWin.WindowPtr->Draw();
	}*/
	/*if (message == WM_KEYDOWN)
	{
		if (message.WParam() == 'W') { gameWorld->ViewPos.Y -= 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'S') { gameWorld->ViewPos.Y += 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'A') { gameWorld->ViewPos.X -= 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'D') { gameWorld->ViewPos.X += 10 * (1.0/gameWorld->Scale); window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'E') { gameWorld->Scale *= 1.3; window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'Q') { gameWorld->Scale *= 0.7; window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
	}
	//panning code
	if (message == WM_LBUTTONDOWN  && program->GraphicWin.WindowPtr->Input->MouseState.GetPos().X < 800)
	{
		//record the initial mouse and view pos
		program->GraphicWin.InitialMousePos = { GET_X_LPARAM(message.LParam()) , GET_Y_LPARAM(message.LParam()) };
		program->GraphicWin.InitialViewPos = program->GraphicWin.GameWorld->ViewPos.ToInt2();
		if (program->GraphicWin.WindowPtr->Input->KeyState[' '].Pressed) { program->GraphicWin.Pan = true; program->ConsoleWin.CommandLine->Write("DEBUG::Pan True\n"); }
	}
	if (program->GraphicWin.Pan && (message == WM_LBUTTONUP || (message == WM_KEYUP && message.WParam() == VK_SPACE)))
	{
		program->GraphicWin.Pan = false;
		program->ConsoleWin.CommandLine->Write("DEBUG::Pan False\n");
	}
	if (message == WM_MOUSEMOVE && program->GraphicWin.Pan)
	{
        Int2 currentPos = { GET_X_LPARAM(message.LParam()) , GET_Y_LPARAM(message.LParam()) };//set the current mouse pos
		//set the view to translate based on the difference between the current mouse and initial mouse pos. The translation is then added to initial view pos before being applied.
		program->GraphicWin.GameWorld->ViewPos = program->GraphicWin.InitialViewPos + Double2{(1.0/program->GraphicWin.GameWorld->Scale), (1.0/program->GraphicWin.GameWorld->Scale)} * (program->GraphicWin.InitialMousePos - currentPos);
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		//program->ConsoleWin.CommandLine->Write("DEBUG:: PanX: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + " PanY: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + "\n");
	}
	//zooming code
	if (message == WM_KEYDOWN && message.WParam() == VK_CONTROL) { program->GraphicWin.Zoom = true; program->ConsoleWin.CommandLine->Write("DEBUG::Scroll True\n"); }
	if (message == WM_KEYUP && message.WParam() == VK_CONTROL) { program->GraphicWin.Zoom = false; program->ConsoleWin.CommandLine->Write("DEBUG::Scroll False\n"); }
	if (message == WM_MOUSEWHEEL && program->GraphicWin.Zoom && program->GraphicWin.WindowPtr->Input->MouseState.GetPos().X < 800)
	{
		Int2 currentPos = program->GraphicWin.WindowPtr->Input->MouseState.GetPos();//get the current mouse pos
		double scale = 1.0 + 0.3 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA); //calculate the scaling based on the scroll
		Double2 pos1 = program->GraphicWin.GameWorld->ReverseTransform(currentPos); //calculate the initial pos of the cursor before scaling
		program->GraphicWin.GameWorld->Scale *= scale; //apply the scaling
		Double2 pos2 = program->GraphicWin.GameWorld->ReverseTransform(currentPos); //calculate the initial pos of the cursor after scaling
		program->GraphicWin.GameWorld->ViewPos = (Double2(program->GraphicWin.GameWorld->ViewPos) + (pos1 - pos2)).ToInt2(); //translate the view by the difference in the 2 positions
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); program->ConsoleWin.CommandLine->Write("DEBUG::Scroll\n"); 
	}*/

	//Selecting and Adding Code
	if (message == WM_LBUTTONDOWN && !program->GraphicWin.Pan)
	{
		Int2 currentPos = program->GraphicWin.GameWorld->ReverseTransform(program->GraphicWin.WindowPtr->Input->MouseState.GetPos()).ToInt2();//get the current mouse pos

		if (program->GraphicWin.SelectedTool == DnDProgram::GraphicWindow::Tool::DrawWall)
		{
			if (program->GraphicWin.ActiveObject.Object)
			{
				program->ConsoleWin.CommandLine->Write("Add to New Wall to the active wall\n");
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
		if (program->GraphicWin.SelectedTool == DnDProgram::GraphicWindow::Tool::Select)
		{

		}
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	}
	if (message == WM_MOUSEMOVE)
	{
		Int2 currentPos = program->GraphicWin.GameWorld->ReverseTransform(program->GraphicWin.WindowPtr->Input->MouseState.GetPos()).ToInt2();//get the current mouse pos

		if (program->GraphicWin.ActiveObject.Object)
		{
			program->GraphicWin.ActiveObject.Object->LineList[program->GraphicWin.ActiveObject.SelectIndex] = currentPos;
		}
		window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	}
	if (message == WM_KEYDOWN && message.WParam() == VK_ESCAPE && program->GraphicWin.ActiveObject.Object)
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

void DnDProgram::GraphicWindow::Callback(MessageData message, DnDProgram *program)
{
	if (message == WL_UPDATESURFACE)
	{
		WindowPtr->WindowSurface->Clear();
		GameWorld->Draw();
		if (ActiveObject.Object)
		{
			WorldObject *object = ActiveObject.Object;
			for (int i = 0; i < object->LineList.size() - 1; i++)
			{
				WindowPtr->WindowSurface->DrawLine(GameWorld->Transform(object->LineList[i]).ToInt2(), GameWorld->Transform(object->LineList[i + 1]).ToInt2(), Colour(255, 255, 0), 2.0);
			}
		}
		InterfaceSurface->Clear();
		InterfaceSurface->DrawText({ 10,10 }, "Player Select", Font("Consolas", 12, Colour{ 255,255,255 }));
		InterfaceSurface->DrawRectangle({ 10,10 }, { 190,150 }, Colour(255, 255, 255));
		InterfaceSurface->DrawText({ 10,160 }, "Environment Select", Font("Consolas", 12, Colour{ 255,255,255 }));
		InterfaceSurface->DrawRectangle({ 10,160 }, { 190,300 }, Colour(255, 255, 255));
		InterfaceSurface->DrawText({ 10,310 }, "Layer Select", Font("Consolas", 12, Colour{ 255,255,255 }));
		InterfaceSurface->DrawRectangle({ 10,310 }, { 190,450 }, Colour(255, 255, 255));
		WindowPtr->WindowSurface->CopySurface(InterfaceSurface->GetRect() + Int2{ 800,0 }, InterfaceSurface, InterfaceSurface->GetRect());
		WindowPtr->Draw();
	}
	if (message == WM_KEYDOWN)
	{
		if (message.WParam() == 'W') { GameWorld->ViewPos.Y -= 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'S') { GameWorld->ViewPos.Y += 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'A') { GameWorld->ViewPos.X -= 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'D') { GameWorld->ViewPos.X += 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'E') { GameWorld->Scale *= 1.3; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'Q') { GameWorld->Scale *= 0.7; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
	}

	//panning code
	if (message == WM_LBUTTONDOWN  && WindowPtr->Input->MouseState.GetPos().X < 800)
	{
		//record the initial mouse and view pos
		InitialMousePos = { GET_X_LPARAM(message.LParam()) , GET_Y_LPARAM(message.LParam()) };
		InitialViewPos = GameWorld->ViewPos.ToInt2();
		if (WindowPtr->Input->KeyState[' '].Pressed) { Pan = true; program->ConsoleWin.CommandLine->Write("DEBUG::Pan True\n"); }
	}
	if (Pan && (message == WM_LBUTTONUP || (message == WM_KEYUP && message.WParam() == VK_SPACE)))
	{
		Pan = false;
		program->ConsoleWin.CommandLine->Write("DEBUG::Pan False\n");
	}
	if (message == WM_MOUSEMOVE && Pan)
	{
		Int2 currentPos = { GET_X_LPARAM(message.LParam()) , GET_Y_LPARAM(message.LParam()) };//set the current mouse pos
																							  //set the view to translate based on the difference between the current mouse and initial mouse pos. The translation is then added to initial view pos before being applied.
		GameWorld->ViewPos = InitialViewPos + Double2{ (1.0 / GameWorld->Scale), (1.0 / GameWorld->Scale) } *(InitialMousePos - currentPos);
		WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		//program->ConsoleWin.CommandLine->Write("DEBUG:: PanX: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + " PanY: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + "\n");
	}
	//zooming code
	if (message == WM_KEYDOWN && message.WParam() == VK_CONTROL) { Zoom = true; program->ConsoleWin.CommandLine->Write("DEBUG::Scroll True\n"); }
	if (message == WM_KEYUP && message.WParam() == VK_CONTROL) { Zoom = false; program->ConsoleWin.CommandLine->Write("DEBUG::Scroll False\n"); }
	if (message == WM_MOUSEWHEEL && Zoom && WindowPtr->Input->MouseState.GetPos().X < 800)
	{
		Int2 currentPos = WindowPtr->Input->MouseState.GetPos();//get the current mouse pos
		double scale = 1.0 + 0.3 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA); //calculate the scaling based on the scroll
		Double2 pos1 = GameWorld->ReverseTransform(currentPos); //calculate the initial pos of the cursor before scaling
		GameWorld->Scale *= scale; //apply the scaling
		Double2 pos2 = GameWorld->ReverseTransform(currentPos); //calculate the initial pos of the cursor after scaling
		GameWorld->ViewPos = (Double2(GameWorld->ViewPos) + (pos1 - pos2)).ToInt2(); //translate the view by the difference in the 2 positions
		WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); program->ConsoleWin.CommandLine->Write("DEBUG::Scroll\n");
	}
}

void DnDProgram::ConsoleCallback(Window *window, MessageData Message, void *data)
{
	DnDProgram *program = (DnDProgram *)data;

	if (Message == WL_UPDATESURFACE)
	{
		window->WindowSurface->Clear();
		program->ConsoleWin.CommandLine->Draw(program->ConsoleWin.WindowPtr->WindowSurface, { 0,0 });
		program->ConsoleWin.Command->Draw(program->ConsoleWin.WindowPtr->WindowSurface, { 0, 720 });
		program->ConsoleWin.WindowPtr->WindowSurface->DrawRectangle(program->ConsoleWin.CommandLine->TextSurface->GetRect(), Colour(255, 255, 255));
		program->ConsoleWin.WindowPtr->WindowSurface->DrawRectangle(program->ConsoleWin.Command->TextSurface->GetRect() + Int2{ 0, 720 }, Colour(255, 255, 255));
		program->ConsoleWin.WindowPtr->Draw();
		
	}
	else if (Message == WM_KEYDOWN)
	{
		Key *keyState;
		keyState = program->ConsoleWin.WindowPtr->Input->KeyState;
		for (int i = 0; i < 49; i++)
		{
			if (Message.WParam() == program->ConsoleWin.WindowPtr->Input->CharacterMap[i][0])
			{
				if (program->ConsoleWin.WindowPtr->Input->KeyState[WL_SHIFT].Pressed)
				{
					program->ConsoleWin.Command->WriteChar(program->ConsoleWin.WindowPtr->Input->CharacterMapShift[i][1]);
				}
				else
				{
					program->ConsoleWin.Command->WriteChar(program->ConsoleWin.WindowPtr->Input->CharacterMap[i][1]);
				}
			}
		}
		if (Message.WParam() == WL_RETURN)
		{
			program->ConsoleWin.CommandLine->Write(std::string(">>") + program->ConsoleWin.Command->Text[0] + "\n");
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
	GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	ConsoleWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

	ConsoleWin.CommandLine->Write("FLOW::Begin Main Loop\n");
	while (!(GraphicWin.WindowPtr->State.Quit || ConsoleWin.WindowPtr->State.Quit))
	{
		GraphicWin.WindowPtr->UpdateMsg();
		ConsoleWin.WindowPtr->UpdateMsg();
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