
#include "DnDProgram.h"


DnD::DnDProgram::DnDProgram(HINSTANCE hInstance, INT iCmdShow)
{
	Interpreter = new CommandInterpretor(this);
	//FileLoader = new FileReader(this);
	Network = new NetworkManager(this);

	WindowInfo windowInfo;
	
	windowInfo.Name = "Client_" + ProgramVersion + "  GraphicWindow";
	windowInfo.Dimensions = { 1000,800 };
	windowInfo.ClearColour = Colour(0, 0, 0);
	windowInfo.HInstance = hInstance;
	windowInfo.ICmdShow = iCmdShow;
	windowInfo.Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;//WS_POPUP;//
	windowInfo.Callback = DnDProgram::GraphicCallback;
	windowInfo.CallbackPtr = this;
	GraphicWin.WindowPtr = new Window(&windowInfo);

	windowInfo.Name = "Client_" + ProgramVersion + "  ConsoleWindow";
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

	GraphicWin.GameWorld = new World(GraphicWin.WindowPtr, this);
	//GraphicWin.SelectedEnvironment = GraphicWin.GameWorld->EnvironmentList[0]; GraphicWin.SelectedEnvironment->Selected = Selected; GraphicWin.SelectedEnvironmentIndex = 0;
	//GraphicWin.SelectedLayer = GraphicWin.SelectedEnvironment->LayerList[0]; GraphicWin.SelectedLayer->Selected = Selected; GraphicWin.SelectedLayerIndex = 0;
	GraphicWin.SelectEnvironment(0);
	GraphicWin.SelectLayer(0);
	//GraphicWin.SelectObject(0);
	GraphicWin.SelectedObject = 0; GraphicWin.SelectedObjectIndex = -1;

	LoadConfig(this);


	GraphicWin.WindowPtr->Draw();
	ConsoleWin.WindowPtr->Draw();
}

DnD::DnDProgram::~DnDProgram()
{
	delete Interpreter;
	//delete FileLoader;
	delete Network;
	delete GraphicWin.GameWorld;
	delete GraphicWin.InterfaceSurface;
	delete GraphicWin.WindowPtr;
	delete ConsoleWin.WindowPtr;
}

void DnD::DnDProgram::InitGraphicWindow()
{
	GraphicWin.WindowPtr->WindowSurface->Clear();

	SurfaceInfo surfaceInfo;
	surfaceInfo.Dimensions = { 200, GraphicWin.WindowPtr->Dimensions.Y };
	surfaceInfo.ClearColour = Colour(20, 20, 50);
	surfaceInfo.ParentWindow = GraphicWin.WindowPtr;
	GraphicWin.InterfaceSurface = new Surface(&surfaceInfo);
}

void DnD::DnDProgram::InitConsoleWindow()
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

void DnD::DnDProgram::GraphicCallback(Window *window, MessageData message, void *data)
{
	DnDProgram *program = (DnDProgram *)data;
	World *gameWorld = program->GraphicWin.GameWorld;

	program->GraphicWin.Callback(message, program);
	

}

void DnD::DnDProgram::GraphicWindow::Callback(MessageData message, DnDProgram *program)
{
	Int2 mousePos = WindowPtr->Input->MouseState.GetPos();
	Double2 mouseTransform = GameWorld->ReverseTransform(mousePos);

	if (SnapToGrid)
	{
		mouseTransform = { Round(mouseTransform.X, GameWorld->GridSize), Round(mouseTransform.Y, GameWorld->GridSize) };
	}
	else
	{
		mouseTransform = { Round(mouseTransform.X, 1), Round(mouseTransform.Y, 1) };
	}

	if (message == WL_UPDATESURFACE)
	{
		WindowPtr->WindowSurface->Clear();
		GameWorld->Draw(SelectedEnvironment);//pass 0 to signify draw all
		/*if (ActiveObject.Object)
		{
			WorldObject *object = ActiveObject.Object;
			for (int i = 0; i < object->LineList.size() - 1; i++)
			{
				WindowPtr->WindowSurface->DrawLine(GameWorld->Transform(object->LineList[i]).ToInt2(), GameWorld->Transform(object->LineList[i + 1]).ToInt2(), Colour(255, 255, 0), 2.0);
			}
		}*/
		InterfaceSurface->Clear();
		DrawSelectMenu(program);

		WindowPtr->WindowSurface->CopySurface(InterfaceSurface->GetRect() + Int2{ 800,0 }, InterfaceSurface, InterfaceSurface->GetRect());
		WindowPtr->Draw();
	}
	/*if (message == WM_KEYDOWN)
	{
		if (message.WParam() == 'W') { GameWorld->ViewPos.Y -= 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'S') { GameWorld->ViewPos.Y += 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'A') { GameWorld->ViewPos.X -= 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'D') { GameWorld->ViewPos.X += 10 * (1.0 / GameWorld->Scale); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'E') { GameWorld->Scale *= 1.3; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		else if (message.WParam() == 'Q') { GameWorld->Scale *= 0.7; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
	}*/

	//panning code
	if (message == WM_KEYDOWN && message.WParam() == VK_SPACE)
	{
		Pan = true;
	}
	if (message == WM_LBUTTONDOWN  && WindowPtr->Input->MouseState.GetPos().X < 800)
	{
		//record the initial mouse and view pos
		InitialMousePos = mousePos;// { GET_X_LPARAM(message.LParam()), GET_Y_LPARAM(message.LParam()) };
		InitialViewPos = GameWorld->ViewPos.ToInt2();
		//if (WindowPtr->Input->KeyState[' '].Pressed) { Pan = true; }
	}
	//if (Pan && (message == WM_LBUTTONUP || (message == WM_KEYUP && message.WParam() == VK_SPACE)))
	if (message == WM_KEYUP && message.WParam() == VK_SPACE)
	{
		Pan = false;
		//program->ConsoleWin.CommandLine->Write("DEBUG::Pan False\n");
	}
	if (message == WM_MOUSEMOVE && Pan && program->GraphicWin.WindowPtr->Input->MouseState.LPressed)
	{
		//Int2 currentPos = { GET_X_LPARAM(message.LParam()) , GET_Y_LPARAM(message.LParam()) };//set the current mouse pos
																							  //set the view to translate based on the difference between the current mouse and initial mouse pos. The translation is then added to initial view pos before being applied.
		GameWorld->ViewPos = InitialViewPos + Double2{ (1.0 / GameWorld->Scale), (1.0 / GameWorld->Scale) } *(InitialMousePos - mousePos);
		WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		//program->ConsoleWin.CommandLine->Write("DEBUG:: PanX: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + " PanY: " + std::to_string(program->GraphicWin.InitialMousePos.X - currentPos.X) + "\n");
	}
	//zooming code
	if (message == WM_KEYDOWN && message.WParam() == VK_CONTROL) { Zoom = true; }
	if (message == WM_KEYUP && message.WParam() == VK_CONTROL) { Zoom = false; }
	if (message == WM_MOUSEWHEEL && Zoom && mousePos.X < 800)
	{
		double scale = 1.0 + 0.3 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA); //calculate the scaling based on the scroll
		Double2 pos1 = mouseTransform; //calculate the initial pos of the cursor before scaling
		GameWorld->Scale *= abs(scale); //apply the scaling
		Double2 pos2 = GameWorld->ReverseTransform(mousePos); //calculate the initial pos of the cursor after scaling
		if (GameWorld->Scale < 0.000001) 
		{ 
			GameWorld->Scale = 0.000001;
			program->ConsoleWin.CommandLine->Write("WARNING::Grid dissabled; grid spacing too small at current zoom level\n");
		}
		else
		{
			GameWorld->ViewPos = Double2(GameWorld->ViewPos) + (pos1 - pos2); //translate the view by the difference in the 2 positions
		}
		WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		program->ConsoleWin.CommandLine->Write("Scale:" + std::to_string(GameWorld->Scale) + "\n");
		
	}

	{/*Selecting and Editing*/}
	if (!Pan)// || ActiveText)
	{
		//Select
		//can use object bounds to optimize it by excluding ones outside of the view
		if (SelectedTool == Select)
		{
			if (message == WM_LBUTTONDOWN && mousePos.X < 800)
			{
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

				SelectedObject = 0; SelectedObjectIndex = 0; ActiveLines = 0; ActiveEllipse = 0; ActiveText = 0;
				GameWorld->DeselectObjects();

				for (int e = 0; e < GameWorld->EnvironmentList.size(); e++)
				{
					if (GameWorld->EnvironmentList[e]->Visible)
					{
						for (int l = 0; l < GameWorld->EnvironmentList[e]->LayerList.size(); l++)
						{
							if (GameWorld->EnvironmentList[e]->LayerList[l]->Visible)
							{
								for (int o = 0; o < GameWorld->EnvironmentList[e]->LayerList[l]->ObjectList.size(); o++)
								{
									if (GameWorld->EnvironmentList[e]->LayerList[l]->ObjectList[o]->Visible)
									{
										if (GameWorld->EnvironmentList[e]->LayerList[l]->ObjectList[o]->ClickedOn(mouseTransform, GameWorld->Scale, WindowPtr->WindowSurface))
										{
											//SelectedObject = GameWorld->EnvironmentList[e]->LayerList[l]->ObjectList[o]; SelectedObject->Selected = Selected; SelectedObjectIndex = o;
											SelectObject(e, l, o);
										}
									}
								}
							}
						}
					}
				}
			}

			if (message == WM_KEYDOWN && message.WParam() == VK_DELETE)
			{
				GameWorld->Remove(SelectedObject); SelectedObject = 0; SelectedObjectIndex = -1;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}

		//SubSelect
		/*if (SelectedTool == Select)
		{

		}*/

		//Lines
		/*if (SelectedTool == DrawLines && SelectedLayer && !Pan)
		{
			if (message == WM_LBUTTONDOWN && mousePos.X < 800)
			{
				if (ActiveLines == 0)
				{
					//start new line
					ActiveLines = new Lines();
					ActiveLines->ObjectColour = SelectedColour;
					ActiveLines->LineList.push_back(mouseTransform.ToInt2());
					SelectedLayer->ObjectList.push_back(ActiveLines);
					//ActiveLines->Selected = SubSelected; SelectedObject = ActiveLines; SelectedObjectIndex = (int)SelectedLayer->ObjectList.size() - 1;
					SubSelectObject((int)SelectedLayer->ObjectList.size() - 1);
				}
				else
				{
					//if click on beginning of line loop
					if (Rect(GameWorld->Transform(ActiveLines->LineList[0]).ToInt2() - 3, GameWorld->Transform(ActiveLines->LineList[0]).ToInt2() + 3).Intersect(mousePos))
					{
						ActiveLines->Closed = true;
						//deselect
						//ActiveLines->Selected = Unselected;
						DeselectObject(SelectedObjectIndex);
						ActiveLines->LineList.pop_back();
						if (ActiveLines->LineList.size() < 2) { GameWorld->Remove(ActiveLines); }
						//SelectedObject = 0; SelectedObjectIndex = -1; ActiveLines = 0;
						ActiveLines = 0;
					}
				}
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			//stop dragging point
			if (message == WM_LBUTTONUP && mousePos.X < 800)
			{
				if (ActiveLines)
				{
					ActiveLines->LineList.push_back(mouseTransform.ToInt2());
					WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				}
			}
			//update the line to show a line following to the cursor
			if (message == WM_MOUSEMOVE && ActiveLines)
			{
				ActiveLines->LineList[ActiveLines->LineList.size() - 1] = mouseTransform.ToInt2();
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			//deselect
			if (message == WM_KEYDOWN && message.WParam() == VK_ESCAPE)
			{
				//ActiveLines->Selected = Unselected;
				DeselectObject(SelectedObjectIndex);
				ActiveLines->LineList.pop_back();
				if (ActiveLines->LineList.size() < 2) { GameWorld->Remove(ActiveLines); }
				//SelectedObject = 0; SelectedObjectIndex = -1; ActiveLines = 0;
				ActiveLines = 0;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}*/

		//Ellipse
		/*if (SelectedTool == DrawEllipse && SelectedLayer)
		{
			//create new ellipse
			if (message == WM_LBUTTONDOWN && mousePos.X < 800)
			{
				ActiveEllipse = new Ellipse();
				ActiveEllipse->ObjectColour = SelectedColour;
				ActiveEllipse->Pos = mouseTransform.ToInt2();
				ActiveEllipse->Radius = { abs(mouseTransform.ToInt2().X- ActiveEllipse->Pos.X), abs(mouseTransform.ToInt2().Y - ActiveEllipse->Pos.Y) };
				SelectedLayer->ObjectList.push_back(ActiveEllipse);
				//ActiveEllipse->Selected = SubSelected; SelectedObject = ActiveEllipse; SelectedObjectIndex = (int)SelectedLayer->ObjectList.size() - 1;
				SubSelectObject((int)SelectedLayer->ObjectList.size() - 1);
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			//done resizing
			if (message == WM_LBUTTONUP)
			{
				if (ActiveEllipse)
				{
					//ActiveEllipse->Selected = Unselected;
					DeselectObject(SelectedObjectIndex);
					if (ActiveEllipse->Radius == Int2{ 0, 0 }) { GameWorld->Remove(ActiveEllipse); }
					//SelectedObject = 0; SelectedObjectIndex = -1; ActiveEllipse = 0;
					ActiveEllipse = 0;
					WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				}
			}
			//update size and shape
			if (message == WM_MOUSEMOVE)
			{
				if (ActiveEllipse)
				{
					if (WindowPtr->Input->KeyState[WL_SHIFT].Pressed)
					{
						//perfect circle
						int radius = (int)sqrt((mouseTransform.ToInt2().X - ActiveEllipse->Pos.X) * (mouseTransform.ToInt2().X - ActiveEllipse->Pos.X) + (mouseTransform.ToInt2().Y - ActiveEllipse->Pos.Y) * (mouseTransform.ToInt2().Y - ActiveEllipse->Pos.Y));
						ActiveEllipse->Radius = { radius, radius };
					}
					else
					{
						//ellipse
						ActiveEllipse->Radius = { abs(mouseTransform.ToInt2().X - ActiveEllipse->Pos.X), abs(mouseTransform.ToInt2().Y - ActiveEllipse->Pos.Y) };
					}
					WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				}
			}
		}*/

		//Text
		/*if (SelectedTool == PlaceText && SelectedLayer)
		{
			if (message == WM_LBUTTONDOWN && mousePos.X < 800)
			{
				//deselect text
				if (ActiveText)
				{
					//ActiveText->Selected = Unselected;
					DeselectObject(SelectedObjectIndex);
					if (ActiveText->TextData.size() < 1) { GameWorld->Remove(ActiveText); }
					//SelectedObject = 0; SelectedObjectIndex = -1; ActiveText = 0;
					ActiveText = 0;
					WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				}

				//add new text
				ActiveText = new Text();
				ActiveText->ObjectColour = SelectedColour;
				ActiveText->Pos = mouseTransform.ToInt2();
				ActiveText->FontSize = FontSize;
				SelectedLayer->ObjectList.push_back(ActiveText);
				//ActiveText->Selected = SubSelected; SelectedObject = ActiveText; SelectedObjectIndex = (int)SelectedLayer->ObjectList.size() - 1;
				SubSelectObject((int)SelectedLayer->ObjectList.size() - 1);
				Drag = true;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			//stop dragging text
			if (message == WM_LBUTTONUP)
			{
				Drag = false;
			}
			//drag text
			if (message == WM_MOUSEMOVE && Drag)
			{
				ActiveText->Pos = mouseTransform.ToInt2();
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			//deselect text
			if (message == WM_KEYDOWN && message.WParam() == VK_ESCAPE)
			{
				//ActiveText->Selected = Unselected;
				DeselectObject(SelectedObjectIndex);
				if (ActiveText->TextData.size() < 1) { GameWorld->Remove(ActiveText); }
				//SelectedObject = 0; SelectedObjectIndex = -1; ActiveText = 0;
				ActiveText = 0;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			if (message == WM_KEYDOWN && message.WParam() && ActiveText && !ChangeFontSize)
			{
				char character = (char)message.WParam();
				if (character >= 'A' && character <= 'Z') { ActiveText->TextData.push_back(WindowPtr->Input->KeyState[WL_SHIFT].Pressed ? character : character - ('A' - 'a')); }
				if (character >= '0' && character <= '9' || character == ' ') { ActiveText->TextData.push_back(character); }
				if (character == WL_BACKSPACE && ActiveText->TextData.size()) { ActiveText->TextData.pop_back(); }
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}
	}*/
	}

	{/*InterfaceSurface */}
	{
		//Name Select
		if (message == WM_LBUTTONDOWN)
		{
			if (Rect({ 810 - 1, 10 }, { 990, 30 + 1 }).Intersect(mousePos)) { ChangeName = true; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			else { ChangeName = false; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		}
		if (message == WM_KEYDOWN && message.WParam() && ChangeName)
		{
			char character = (char)message.WParam();
			if (character >= 'A' && character <= 'Z')
			{
				GameWorld->Name += character - (WindowPtr->Input->KeyState[WL_SHIFT].Pressed ? 0 : ('A'-'a'));
				while (GameWorld->Name.size() > 16) { GameWorld->Name.pop_back(); }
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			if (character == WL_BACKSPACE && GameWorld->Name.size())
			{
				GameWorld->Name.pop_back();
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}

		//Selection Menus
		//Player
		/*if (message == WM_MOUSEWHEEL && Rect({ 810, 20 }, { 990, 150 }).Intersect(mousePos))
		{
			PlayerScroll -= int(14 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA));
			WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

			if (PlayerScroll < 0) { PlayerScroll = 0; }
			if (PlayerScroll > 25 * (GameWorld->PlayerList.size() - 1)) { PlayerScroll = 25 * ((int)GameWorld->PlayerList.size() - 1); }
		}

		if (message == WM_LBUTTONDOWN && Rect({ 810, 20 }, { 990, 150 }).Intersect(mousePos))
		{
			int clickedIndex = (mousePos.Y - 20 + PlayerScroll) / 25;
			
			if (clickedIndex >= 0 && clickedIndex < GameWorld->PlayerList.size())
			{
				if (Rect({ 814, 26 + 25 * clickedIndex - PlayerScroll }, { 827, 37 + 25 * clickedIndex - PlayerScroll }).Intersect(mousePos))
				{
					//{ 14, 26 + i * 25 - PlayerScroll }, { 27, 39 + i * 25 - PlayerScroll }
					//SelectedLayer->ObjectList[clickedIndex]->Visible = !SelectedLayer->ObjectList[clickedIndex]->Visible;
					ActivePlayer->Visible = false;
					ActivePlayer = GameWorld->PlayerList[clickedIndex];
					ActivePlayer->Visible = true;
				}
				else if (WindowPtr->Input->KeyState[WL_SHIFT].Pressed)
				{
					GameWorld->DeselectPlayers();
					for (int i = min(SelectedPlayerIndex, clickedIndex); i <= max(SelectedPlayerIndex, clickedIndex); i++)
					{
						//SelectedLayer->ObjectList[i]->Selected = Selected;
						SetPlayerSelected(i);
					}
				}
				else if (WindowPtr->Input->KeyState[VK_CONTROL].Pressed && clickedIndex != SelectedPlayerIndex)
				{
					//SelectedLayer->ObjectList[clickedIndex]->Selected = SelectedLayer->ObjectList[clickedIndex]->Selected ? Unselected : Selected;
					if (GameWorld->PlayerList[clickedIndex]->Selected) { DeselectPlayer(clickedIndex); }
					else { SetPlayerSelected(clickedIndex); }
				}
				else
				{
					GameWorld->DeselectPlayers();
					//SelectedObject = SelectedLayer->ObjectList[clickedIndex]; SelectedObject->Selected = Selected; SelectedObjectIndex = clickedIndex;
					SelectPlayer(clickedIndex);
				}
			}
			WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		}*/

		//Environment
		{
			//InterfaceSurface->DrawRectangle({ { 122, 148 },{ 130, 158 } }, Colour{ 255,255,255 });//new
			//InterfaceSurface->DrawRectangle({ { 132, 148 },{ 140, 158 } }, Colour{ 255,255,255 });//visible
			//InterfaceSurface->DrawRectangle({ { 142, 148 },{ 150, 158 } }, Colour{ 255,255,255 });//up
			//InterfaceSurface->DrawRectangle({ { 152, 148 },{ 160, 158 } }, Colour{ 255,255,255 });//down
			//InterfaceSurface->DrawRectangle({ { 162, 148 },{ 170, 158 } }, Colour{ 255,255,255 });//up up
			//InterfaceSurface->DrawRectangle({ { 172, 148 },{ 180, 158 } }, Colour{ 255,255,255 });//down down
			//InterfaceSurface->DrawRectangle({ { 182, 148 },{ 190, 158 } }, Colour{ 255,255,255 });//delete
			/*if (message == WM_LBUTTONDOWN && Rect({ 922, 148 }, { 930, 158 }).Intersect(mousePos))//new
			{
				GameWorld->EnvironmentList.push_back(new Environment());
				GameWorld->EnvironmentList[GameWorld->EnvironmentList.size() - 1]->LayerList.push_back(new Layer());
			}
			if (message == WM_LBUTTONDOWN && Rect({ 982, 148 }, { 990, 158 }).Intersect(mousePos) && SelectedEnvironment)//delete
			{
				delete SelectedEnvironment;
				GameWorld->EnvironmentList.erase(GameWorld->EnvironmentList.begin() + SelectedEnvironmentIndex);
				//if (GameWorld->EnvironmentList.size() < 1) { GameWorld->EnvironmentList.push_back(new Environment()); GameWorld->EnvironmentList[GameWorld->EnvironmentList.size() - 1]->LayerList.push_back(new Layer()); }
				SelectEnvironment(min(SelectedEnvironmentIndex, (int)GameWorld->EnvironmentList.size() - 1));
			}*/

			if (message == WM_MOUSEWHEEL && Rect({ 810, 160 }, { 990, 290 }).Intersect(mousePos))
			{
				EnvironmentScroll -= int(14 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA));
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

				if (EnvironmentScroll < 0) { EnvironmentScroll = 0; }
				if (EnvironmentScroll > 25 * (GameWorld->EnvironmentList.size() - 1)) { EnvironmentScroll = 25 * ((int)GameWorld->EnvironmentList.size() - 1); }
			}

			if (message == WM_LBUTTONDOWN && Rect({ 810, 160 }, { 990, 290 }).Intersect(mousePos))
			{
				int clickedIndex = (mousePos.Y - 160 + EnvironmentScroll) / 25;
				if (clickedIndex >= 0 && clickedIndex < GameWorld->EnvironmentList.size())
				{
					if (Rect({ 814, 166 + 25 * clickedIndex - EnvironmentScroll }, { 827, 177 + 25 * clickedIndex - EnvironmentScroll }).Intersect(mousePos))
					{
						GameWorld->EnvironmentList[clickedIndex]->Visible = !GameWorld->EnvironmentList[clickedIndex]->Visible;
					}
					else if (WindowPtr->Input->KeyState[WL_SHIFT].Pressed)
					{
						GameWorld->DeselectEnvironments();
						for (int i = min(SelectedEnvironmentIndex, clickedIndex); i <= max(SelectedEnvironmentIndex, clickedIndex); i++)
						{
							//SelectedLayer->ObjectList[i]->Selected = Selected;
							SetEnvironmentSelected(i);
						}
					}
					else if (WindowPtr->Input->KeyState[VK_CONTROL].Pressed && clickedIndex != SelectedEnvironmentIndex)
					{
						//SelectedLayer->ObjectList[clickedIndex]->Selected = SelectedLayer->ObjectList[clickedIndex]->Selected ? Unselected : Selected;
						if (GameWorld->EnvironmentList[clickedIndex]->Selected) { DeselectEnvironment(clickedIndex); }
						else { SetEnvironmentSelected(clickedIndex); }
					}
					else
					{
						ActiveLines = 0;
						ActiveEllipse = 0;
						ActiveText = 0;
						ActiveCharacter = 0;
						GameWorld->DeselectEnvironments();
						//SelectedObject = SelectedLayer->ObjectList[clickedIndex]; SelectedObject->Selected = Selected; SelectedObjectIndex = clickedIndex;
						SelectEnvironment(clickedIndex);
					}
				}
				else
				{
					GameWorld->DeselectObjects();
					GameWorld->DeselectLayers();
					GameWorld->DeselectEnvironments();
					SelectedLayer = 0;
					SelectedEnvironment = 0;
				}
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}

		//Layer
		{
			//InterfaceSurface->DrawRectangle({ { 122, 298 },{ 130, 308 } }, Colour{ 255,255,255 });//new
			//InterfaceSurface->DrawRectangle({ { 132, 298 },{ 140, 308 } }, Colour{ 255,255,255 });//visible
			//InterfaceSurface->DrawRectangle({ { 142, 298 },{ 150, 308 } }, Colour{ 255,255,255 });//up
			//InterfaceSurface->DrawRectangle({ { 152, 298 },{ 160, 308 } }, Colour{ 255,255,255 });//down
			//InterfaceSurface->DrawRectangle({ { 162, 298 },{ 170, 308 } }, Colour{ 255,255,255 });//up up
			//InterfaceSurface->DrawRectangle({ { 172, 298 },{ 180, 308 } }, Colour{ 255,255,255 });//down down
			//InterfaceSurface->DrawRectangle({ { 182, 298 },{ 190, 308 } }, Colour{ 255,255,255 });//delete
			/*if (message == WM_LBUTTONDOWN && Rect({ 922, 298 }, { 930, 308 }).Intersect(mousePos) && SelectedEnvironment)//new
			{
				SelectedEnvironment->LayerList.push_back(new Layer());
			}
			if (message == WM_LBUTTONDOWN && Rect({ 982, 298 }, { 990, 308 }).Intersect(mousePos) && SelectedLayer)//delete
			{
				delete SelectedLayer;
				SelectedEnvironment->LayerList.erase(SelectedEnvironment->LayerList.begin() + SelectedLayerIndex);
				//if (SelectedEnvironment->LayerList.size() < 1) { SelectedEnvironment->LayerList.push_back(new Layer()); }
				SelectLayer(min(SelectedLayerIndex, (int)SelectedEnvironment->LayerList.size() - 1));
			}*/

			if (message == WM_MOUSEWHEEL && Rect({ 810, 310 }, { 990, 440 }).Intersect(mousePos) && SelectedEnvironment)
			{
				LayerScroll -= int(14 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA));
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

				if (LayerScroll < 0) { LayerScroll = 0; }
				if (LayerScroll > 25 * (SelectedEnvironment->LayerList.size() - 1)) { LayerScroll = 25 * ((int)SelectedEnvironment->LayerList.size() - 1); }
			}

			if (message == WM_LBUTTONDOWN && Rect({ 810, 310 }, { 990, 440 }).Intersect(mousePos) && SelectedEnvironment)
			{
				int clickedIndex = (mousePos.Y - 310 + LayerScroll) / 25;
				if (clickedIndex >= 0 && clickedIndex < SelectedEnvironment->LayerList.size())
				{
					if (Rect({ 814, 316 + 25 * clickedIndex - LayerScroll }, { 827, 327 + 25 * clickedIndex - LayerScroll }).Intersect(mousePos))
					{
						SelectedEnvironment->LayerList[clickedIndex]->Visible = !SelectedEnvironment->LayerList[clickedIndex]->Visible;
					}
					else if (WindowPtr->Input->KeyState[WL_SHIFT].Pressed)
					{
						GameWorld->DeselectLayers();
						for (int i = min(SelectedLayerIndex, clickedIndex); i <= max(SelectedLayerIndex, clickedIndex); i++)
						{
							//SelectedLayer->ObjectList[i]->Selected = Selected;
							SetLayerSelected(i);
						}
					}
					else if (WindowPtr->Input->KeyState[VK_CONTROL].Pressed && clickedIndex != SelectedLayerIndex)
					{
						//SelectedLayer->ObjectList[clickedIndex]->Selected = SelectedLayer->ObjectList[clickedIndex]->Selected ? Unselected : Selected;
						if (SelectedEnvironment->LayerList[clickedIndex]->Selected) { DeselectLayer(clickedIndex); }
						else { SetLayerSelected(clickedIndex); }
					}
					else
					{
						ActiveLines = 0;
						ActiveEllipse = 0;
						ActiveText = 0;
						ActiveCharacter = 0;
						GameWorld->DeselectLayers();
						//SelectedObject = SelectedLayer->ObjectList[clickedIndex]; SelectedObject->Selected = Selected; SelectedObjectIndex = clickedIndex;
						SelectLayer(clickedIndex);
					}
				}
				else
				{
					GameWorld->DeselectObjects();
					GameWorld->DeselectLayers();
					SelectedLayer = 0;
				}
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}

		//Object
		{
			//InterfaceSurface->DrawRectangle({ { 132, 448 },{ 140, 458 } }, Colour{ 255,255,255 });//visible
			//InterfaceSurface->DrawRectangle({ { 142, 448 },{ 150, 458 } }, Colour{ 255,255,255 });//up
			//InterfaceSurface->DrawRectangle({ { 152, 448 },{ 160, 458 } }, Colour{ 255,255,255 });//down
			//InterfaceSurface->DrawRectangle({ { 162, 448 },{ 170, 458 } }, Colour{ 255,255,255 });//up up
			//InterfaceSurface->DrawRectangle({ { 172, 448 },{ 180, 458 } }, Colour{ 255,255,255 });//down down
			//InterfaceSurface->DrawRectangle({ { 182, 448 },{ 190, 458 } }, Colour{ 255,255,255 });//delete
			if (message == WM_MOUSEWHEEL && Rect({ 810, 460 }, { 990, 590 }).Intersect(mousePos) && SelectedLayer)
			{
				ObjectScroll -= int(14 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA));
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

				if (ObjectScroll < 0) { ObjectScroll = 0; }
				if (ObjectScroll > 25 * (SelectedLayer->ObjectList.size() - 1)) { ObjectScroll = 25 * ((int)SelectedLayer->ObjectList.size() - 1); }
			}

			if (message == WM_LBUTTONDOWN && Rect({ 810, 460 }, { 990, 590 }).Intersect(mousePos) && SelectedLayer)
			{
				int clickedIndex = (mousePos.Y - 460 + ObjectScroll) / 25;
				if (clickedIndex >= 0 && clickedIndex < SelectedLayer->ObjectList.size())
				{
					if (Rect({ 814, 466 + 25 * clickedIndex - ObjectScroll }, { 827, 477 + 25 * clickedIndex - ObjectScroll }).Intersect(mousePos))
					{
						//{ 14, 26 + i * 25 - PlayerScroll }, { 27, 39 + i * 25 - PlayerScroll }
						SelectedLayer->ObjectList[clickedIndex]->Visible = !SelectedLayer->ObjectList[clickedIndex]->Visible;
					}
					else if (WindowPtr->Input->KeyState[WL_SHIFT].Pressed)
					{
						GameWorld->DeselectObjects();
						for (int i = min(SelectedObjectIndex, clickedIndex); i <= max(SelectedObjectIndex, clickedIndex); i++)
						{
							//SelectedLayer->ObjectList[i]->Selected = Selected;
							SetObjectSelected(i);
						}
					}
					else if (WindowPtr->Input->KeyState[VK_CONTROL].Pressed && clickedIndex != SelectedObjectIndex)
					{
						//SelectedLayer->ObjectList[clickedIndex]->Selected = SelectedLayer->ObjectList[clickedIndex]->Selected ? Unselected : Selected;
						if (SelectedLayer->ObjectList[clickedIndex]->Selected) { DeselectObject(clickedIndex); }
						else { SetObjectSelected(clickedIndex); }
					}
					else
					{
						GameWorld->DeselectObjects();
						//SelectedObject = SelectedLayer->ObjectList[clickedIndex]; SelectedObject->Selected = Selected; SelectedObjectIndex = clickedIndex;
						SelectObject(clickedIndex);
					}
				}
				else
				{
					GameWorld->DeselectObjects();
				}
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}


		//Colour Select
		/*if (message == WM_LBUTTONDOWN || (message == WM_MOUSEMOVE && WindowPtr->Input->MouseState.LPressed))
		{
			if (Rect({ 810,600 }, { 898, 614 }).Intersect(mousePos)) { SelectedColour.R = (int)MapVal(810 + 1, 898 - 1, 0, 255, mousePos.X); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			if (Rect({ 810,620 }, { 898, 634 }).Intersect(mousePos)) { SelectedColour.G = (int)MapVal(810 + 1, 898 - 1, 0, 255, mousePos.X); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			if (Rect({ 810,640 }, { 898, 654 }).Intersect(mousePos)) { SelectedColour.B = (int)MapVal(810 + 1, 898 - 1, 0, 255, mousePos.X); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }

		}*/

		//Font Size Select
		/*if (message == WM_LBUTTONDOWN)
		{
			if (Rect({ 810 - 1, 670 }, { 900, 685 + 1 }).Intersect(mousePos)) { ChangeFontSize = true; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			else { ChangeFontSize = false; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		}
		if (message == WM_KEYDOWN && message.WParam() && ChangeFontSize)
		{
			char character = (char)message.WParam();
			if (character >= '0' && character <= '9')
			{
				FontSize *= 10; FontSize += character - '0';
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				if (FontSize > 999) { FontSize = 999; }
			}
			if (character == WL_BACKSPACE)
			{
				FontSize /= 10;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}*/

		//Grid Size
		/*if (message == WM_LBUTTONDOWN)
		{
			if (Rect({ 810 - 1, 700 }, { 916, 715 + 1 }).Intersect(mousePos)) { ChangeGridSize = true; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			else { ChangeGridSize = false; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
		}
		if (message == WM_KEYDOWN && message.WParam() && ChangeGridSize)
		{
			char character = (char)message.WParam();
			if (character >= '0' && character <= '9')
			{
				GameWorld->GridSize *= 10; GameWorld->GridSize += character - '0';
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				if (GameWorld->GridSize > 99999) { GameWorld->GridSize = 99999; }
			}
			if (character == WL_BACKSPACE)
			{
				GameWorld->GridSize /= 10;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}
		if (message == WM_LBUTTONDOWN && Rect({ 920, 700 }, { 935, 715 }).Intersect(mousePos))
		{
			GameWorld->DrawGrid = !GameWorld->DrawGrid;
		}
		if (message == WM_LBUTTONDOWN && Rect({ 980, 700 }, { 995, 715 }).Intersect(mousePos))
		{
			SnapToGrid = !SnapToGrid;
		}*/


		//tool select
		//{ 10, 702 }, {100, 714}
		/*if (message == WM_LBUTTONDOWN)
		{
			Tool Previous = SelectedTool;
			if (Rect({ 810, 720 + 15 * 0 }, { 900, 736 + 15 * 0 }).Intersect(mousePos)) { SelectedTool = Select; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); if (SelectedObject) { SelectedObject->Selected = Selected; } }
			if (Rect({ 810, 720 + 15 * 1 }, { 900, 736 + 15 * 1 }).Intersect(mousePos)) { SelectedTool = SubSelect; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); if (SelectedObject) { SelectedObject->Selected = SubSelected; } }
			if (Rect({ 810, 720 + 15 * 2 }, { 900, 736 + 15 * 2 }).Intersect(mousePos)) { SelectedTool = DrawLines; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			if (Rect({ 810, 720 + 15 * 3 }, { 900, 736 + 15 * 3 }).Intersect(mousePos)) { SelectedTool = DrawEllipse; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			if (Rect({ 810, 720 + 15 * 4 }, { 900, 736 + 15 * 4 }).Intersect(mousePos)) { SelectedTool = PlaceText; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
			if (Previous == DrawLines && SelectedTool != DrawLines && ActiveLines)
			{
				ActiveLines->Selected = Unselected;
				ActiveLines->LineList.pop_back();
				if (ActiveLines->LineList.size() < 2) { GameWorld->Remove(ActiveLines); }
				SelectedObject = 0; SelectedObjectIndex = -1; ActiveLines = 0;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			if (Previous == PlaceText && SelectedTool != PlaceText && ActiveText)
			{
				ActiveText->Selected = Unselected;
				if (ActiveText->TextData.size() < 1) { GameWorld->Remove(ActiveText); }
				SelectedObject = 0; SelectedObjectIndex = -1; ActiveText = 0;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}

		}*/
	}
}

void DnD::DnDProgram::GraphicWindow::DrawSelectMenu(DnDProgram *program)
{
	//Name Select
	InterfaceSurface->DrawText({ 10, 10 }, "Name:" + GameWorld->Name, Font("consolas", 12, { 255,255,255 }));
	InterfaceSurface->DrawRectangle({ 10 - 1, 10 }, { 190, 30 + 1 }, ChangeName ? Colour(255, 255, 255) : Colour(180, 180, 180), ChangeName ? 2 : 0.5f);

	InterfaceSurface->DrawText({ 10, 40 }, std::string("Net:") + (program->Network->Run ? "Up" : "Down"), Font("consolas", 12, { 255,255,255 }));

	//Player Select
	/*InterfaceSurface->DrawText({ 10, 5 }, "Player", Font("Consolas", 10, Colour{ 255,255,255 }));
	InterfaceSurface->DrawText({ 123, 8 }, "  V u d U D X", Font("Consolas", 6.9f, Colour{ 255,255,255 }));
	//InterfaceSurface->DrawRectangle({ { 122, 8 },{ 130, 18 } }, Colour{ 255,255,255 });//new
	InterfaceSurface->DrawRectangle({ { 132, 8 },{ 140, 18 } }, Colour{ 255,255,255 });//visible
	InterfaceSurface->DrawRectangle({ { 142, 8 },{ 150, 18 } }, Colour{ 255,255,255 });//up
	InterfaceSurface->DrawRectangle({ { 152, 8 },{ 160, 18 } }, Colour{ 255,255,255 });//down
	InterfaceSurface->DrawRectangle({ { 162, 8 },{ 170, 18 } }, Colour{ 255,255,255 });//up up
	InterfaceSurface->DrawRectangle({ { 172, 8 },{ 180, 18 } }, Colour{ 255,255,255 });//down down
	InterfaceSurface->DrawRectangle({ { 182, 8 },{ 190, 18 } }, Colour{ 255,255,255 });//delete
	{
		InterfaceSurface->SetClip({ 10, 20 }, { 190, 140 });
		//for (int i = 0; i < GameWorld->EnvironmentList.size(); i++)
		for (int i = max(PlayerScroll / 25, 0); i < min(PlayerScroll / 25 + 7, GameWorld->PlayerList.size()); i++)
		{
			if (GameWorld->PlayerList[i]->Selected && GameWorld->PlayerList[i] == SelectedPlayer)
			{
				InterfaceSurface->FillRectangle({ { 10, 20 + i * 25 - PlayerScroll },{ 190, 20 + 24 + i * 25 - PlayerScroll } }, { 200,150,20 });
			}
			else if (GameWorld->PlayerList[i]->Selected)
			{
				InterfaceSurface->FillRectangle({ { 10, 20 + i * 25 - PlayerScroll },{ 190, 20 + 24 + i * 25 - PlayerScroll } }, { 40,100,230 });
			}
			else
			{
				InterfaceSurface->FillRectangle({ { 10, 20 + i * 25 - PlayerScroll },{ 190, 20 + 24 + i * 25 - PlayerScroll } }, { 20,60,180 });
			}
			if (GameWorld->PlayerList[i]->Visible)
			{
				InterfaceSurface->FillRectangle({ { 14, 26 + i * 25 - PlayerScroll },{ 27, 39 + i * 25 - PlayerScroll } }, { 255,255,255 });
			}
			else
			{
				InterfaceSurface->DrawRectangle({ { 14, 26 + i * 25 - PlayerScroll },{ 27, 39 + i * 25 - PlayerScroll } }, { 255,255,255 });
			}
			
			//InterfaceSurface->DrawText({ 30, 160 + 8 + i * 25 - PlayerScroll }, "Name____________.0000", Font("consolas", 10, { 255,255,255 }));
			InterfaceSurface->DrawText({ 30, 20 + 4 + i * 25 - PlayerScroll }, GameWorld->PlayerList[i]->Name, Font("consolas", 10, { 255,255,255 }));
		}
		InterfaceSurface->ResetClip();
	}
	InterfaceSurface->DrawRectangle({ 10, 20 }, { 190, 140 }, Colour(255, 255, 255));
	*/

	//Environment Select
	InterfaceSurface->DrawText({ 10, 145 }, "Environment", Font("Consolas", 10, Colour{ 255,255,255 }));
	/*InterfaceSurface->DrawText({ 123, 148 }, "N V u d U D X", Font("Consolas", 6.9f, Colour{ 255,255,255 }));
	InterfaceSurface->DrawRectangle({ { 122, 148 },{ 130, 158 } }, Colour{ 255,255,255 });//new
	InterfaceSurface->DrawRectangle({ { 132, 148 },{ 140, 158 } }, Colour{ 255,255,255 });//visible
	InterfaceSurface->DrawRectangle({ { 142, 148 },{ 150, 158 } }, Colour{ 255,255,255 });//up
	InterfaceSurface->DrawRectangle({ { 152, 148 },{ 160, 158 } }, Colour{ 255,255,255 });//down
	InterfaceSurface->DrawRectangle({ { 162, 148 },{ 170, 158 } }, Colour{ 255,255,255 });//up up
	InterfaceSurface->DrawRectangle({ { 172, 148 },{ 180, 158 } }, Colour{ 255,255,255 });//down down
	InterfaceSurface->DrawRectangle({ { 182, 148 },{ 190, 158 } }, Colour{ 255,255,255 });//delete*/
	{
		InterfaceSurface->SetClip({ 10, 160 }, { 190, 290 });
		//for (int i = 0; i < GameWorld->EnvironmentList.size(); i++)
		for (int i = max(EnvironmentScroll / 25,0); i < min(EnvironmentScroll / 25 + 7, GameWorld->EnvironmentList.size()); i++)
		{
			if (GameWorld->EnvironmentList[i]->Selected && GameWorld->EnvironmentList[i] == SelectedEnvironment)
			{
				InterfaceSurface->FillRectangle({ { 10, 160 + i * 25 - EnvironmentScroll },{ 190, 160 + 24 + i * 25 - EnvironmentScroll } }, { 200,150,20 });
			}
			else if (GameWorld->EnvironmentList[i]->Selected)
			{
				InterfaceSurface->FillRectangle({ { 10, 160 + i * 25 - EnvironmentScroll },{ 190, 160 + 24 + i * 25 - EnvironmentScroll } }, { 40,100,230 });
			}
			else
			{
				InterfaceSurface->FillRectangle({ { 10, 160 + i * 25 - EnvironmentScroll },{ 190, 160 + 24 + i * 25 - EnvironmentScroll } }, { 20,60,180 });
			}
			if (GameWorld->EnvironmentList[i]->Visible)
			{
				InterfaceSurface->FillRectangle({ { 14, 166 + i * 25 - EnvironmentScroll },{ 27, 179 + i * 25 - EnvironmentScroll } }, { 255,255,255 });
			}
			else
			{
				InterfaceSurface->DrawRectangle({ { 14, 166 + i * 25 - EnvironmentScroll },{ 27, 179 + i * 25 - EnvironmentScroll } }, { 255,255,255 });
			}
			
			//InterfaceSurface->DrawText({ 30, 160 + 8 + i * 25 - EnvironmentScroll }, "Name____________.0000", Font("consolas", 10, { 255,255,255 }));
			InterfaceSurface->DrawText({ 30, 160 + 4 + i * 25 - EnvironmentScroll }, GameWorld->EnvironmentList[i]->Name, Font("consolas", 10, { 255,255,255 }));
		}
		InterfaceSurface->ResetClip();
	}
	InterfaceSurface->DrawRectangle({ 10, 160 }, { 190, 290 }, Colour(255, 255, 255));

	//Layer Select
	InterfaceSurface->DrawText({ 10, 295 }, "Layer", Font("Consolas", 10, Colour{ 255,255,255 }));
	/*InterfaceSurface->DrawText({ 123, 298 }, "N V u d U D X", Font("Consolas", 6.9f, Colour{ 255,255,255 }));
	InterfaceSurface->DrawRectangle({ { 122, 298 },{ 130, 308 } }, Colour{ 255,255,255 });//new
	InterfaceSurface->DrawRectangle({ { 132, 298 },{ 140, 308 } }, Colour{ 255,255,255 });//visible
	InterfaceSurface->DrawRectangle({ { 142, 298 },{ 150, 308 } }, Colour{ 255,255,255 });//up
	InterfaceSurface->DrawRectangle({ { 152, 298 },{ 160, 308 } }, Colour{ 255,255,255 });//down
	InterfaceSurface->DrawRectangle({ { 162, 298 },{ 170, 308 } }, Colour{ 255,255,255 });//up up
	InterfaceSurface->DrawRectangle({ { 172, 298 },{ 180, 308 } }, Colour{ 255,255,255 });//down down
	InterfaceSurface->DrawRectangle({ { 182, 298 },{ 190, 308 } }, Colour{ 255,255,255 });//delete*/
	if (SelectedEnvironment)
	{
		InterfaceSurface->SetClip({ 10, 310 }, { 190, 440 });
		//for (int i = 0; i < GameWorld->EnvironmentList[SelectedEnvironment].LayerList.size(); i++)
		for (int i = LayerScroll / 25; i < min(LayerScroll / 25 + 7, SelectedEnvironment->LayerList.size()); i++)
		{
			InterfaceSurface->FillRectangle({ { 10, 310 + i * 25 - LayerScroll },{ 190, 310 + 24 + i * 25 - LayerScroll } }, { 40,100,230 });
			if (SelectedEnvironment->LayerList[i]->Selected && SelectedEnvironment->LayerList[i] == SelectedLayer)
			{
				InterfaceSurface->FillRectangle({ { 10, 310 + i * 25 - LayerScroll },{ 190, 310 + 24 + i * 25 - LayerScroll } }, { 200,150,20 });
			}
			else if (SelectedEnvironment->LayerList[i]->Selected)
			{
				InterfaceSurface->FillRectangle({ { 10, 310 + i * 25 - LayerScroll },{ 190, 310 + 24 + i * 25 - LayerScroll } }, { 40,100,230 });
			}
			else
			{
				InterfaceSurface->FillRectangle({ { 10, 310 + i * 25 - LayerScroll },{ 190, 310 + 24 + i * 25 - LayerScroll } }, { 20,60,180 });
			}
			if (SelectedEnvironment->LayerList[i]->Visible)
			{
				InterfaceSurface->FillRectangle({ { 14, 316 + i * 25 - LayerScroll },{ 27, 329 + i * 25 - LayerScroll } }, { 255,255,255 });
			}
			else
			{
				InterfaceSurface->DrawRectangle({ { 14, 316 + i * 25 - LayerScroll },{ 27, 329 + i * 25 - LayerScroll } }, { 255,255,255 });
			}
			
			//InterfaceSurface->DrawText({ 30, 310 + 8 + i * 25 - LayerScroll }, "Name____________.0000", Font("consolas", 10, { 255,255,255 }));
			InterfaceSurface->DrawText({ 30, 310 + 4 + i * 25 - LayerScroll }, SelectedEnvironment->LayerList[i]->Name, Font("consolas", 10, { 255,255,255 }));
		}
		InterfaceSurface->ResetClip();
	}
	InterfaceSurface->DrawRectangle({ 10, 310 }, { 190, 440 }, Colour(255, 255, 255));

	//Object Select
	InterfaceSurface->DrawText({ 10, 445 }, "Object", Font("Consolas", 10, Colour{ 255,255,255 }));
	/*InterfaceSurface->DrawText({ 123, 448 }, "  V u d U D X", Font("Consolas", 6.9f, Colour{ 255,255,255 }));
	//InterfaceSurface->DrawRectangle({ { 122, 448 },{ 130, 458 } }, Colour{ 255,255,255 });//new
	InterfaceSurface->DrawRectangle({ { 132, 448 },{ 140, 458 } }, Colour{ 255,255,255 });//visible
	InterfaceSurface->DrawRectangle({ { 142, 448 },{ 150, 458 } }, Colour{ 255,255,255 });//up
	InterfaceSurface->DrawRectangle({ { 152, 448 },{ 160, 458 } }, Colour{ 255,255,255 });//down
	InterfaceSurface->DrawRectangle({ { 162, 448 },{ 170, 458 } }, Colour{ 255,255,255 });//up up
	InterfaceSurface->DrawRectangle({ { 172, 448 },{ 180, 458 } }, Colour{ 255,255,255 });//down down
	InterfaceSurface->DrawRectangle({ { 182, 448 },{ 190, 458 } }, Colour{ 255,255,255 });//delete*/
	if (SelectedLayer)
	{
		InterfaceSurface->SetClip({ 10, 460 }, { 190, 590 });
		//for (int i = 0; i < GameWorld->EnvironmentList[SelectedEnvironment].LayerList[SelectedLayer].ObjectList.size(); i++)
		for (int i = ObjectScroll / 25; i < min(ObjectScroll / 25 + 7, SelectedLayer->ObjectList.size()); i++)
		{
			InterfaceSurface->FillRectangle({ { 10, 460 + i * 25 - ObjectScroll },{ 190, 460 + 24 + i * 25 - ObjectScroll } }, { 40,100,230 });
			if (SelectedLayer->ObjectList[i]->Selected && SelectedLayer->ObjectList[i] == SelectedObject)
			{
				InterfaceSurface->FillRectangle({ { 10, 460 + i * 25 - ObjectScroll },{ 190, 460 + 24 + i * 25 - ObjectScroll } }, { 200,150,20 });
			}
			else if (SelectedLayer->ObjectList[i]->Selected)
			{
				InterfaceSurface->FillRectangle({ { 10, 460 + i * 25 - ObjectScroll },{ 190, 460 + 24 + i * 25 - ObjectScroll } }, { 40,100,230 });
			}
			else
			{
				InterfaceSurface->FillRectangle({ { 10, 460 + i * 25 - ObjectScroll },{ 190, 460 + 24 + i * 25 - ObjectScroll } }, { 20,60,180 });
			}
			if (SelectedLayer->ObjectList[i]->Visible)
			{
				InterfaceSurface->FillRectangle({ { 14, 466 + i * 25 - ObjectScroll },{ 27, 479 + i * 25 - ObjectScroll } }, { 255,255,255 });
			}
			else
			{
				InterfaceSurface->DrawRectangle({ { 14, 466 + i * 25 - ObjectScroll },{ 27, 479 + i * 25 - ObjectScroll } }, { 255,255,255 });
			}
			
			//InterfaceSurface->DrawText({ 30, 460 + 8 + i * 25 - ObjectScroll }, "Name____________.0000", Font("consolas", 10, { 255,255,255 }));
			InterfaceSurface->DrawText({ 30, 460 + 4 + i * 25 - ObjectScroll }, SelectedLayer->ObjectList[i]->Name, Font("consolas", 10, { 255,255,255 }));
		}
		InterfaceSurface->ResetClip();
	}
	InterfaceSurface->DrawRectangle({ 10, 460 }, { 190, 590 }, Colour(255, 255, 255));

	//Colour Select
	/*InterfaceSurface->FillRectangle({ 10,600 }, { 99, 614 }, Colour(SelectedColour.R,0,0)); InterfaceSurface->DrawRectangle({ 10,600 }, { 99, 614 }, Colour(255, 255, 255)); InterfaceSurface->DrawText({ 103, 599 }, std::to_string(SelectedColour.R), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawLine({ (int)MapVal(0,255,11,97,SelectedColour.R),600-1 }, { (int)MapVal(0,255,11,97,SelectedColour.R),614 }, Colour(255, 255, 255));
	InterfaceSurface->FillRectangle({ 10,620 }, { 99, 634 }, Colour(0,SelectedColour.G,0)); InterfaceSurface->DrawRectangle({ 10,620 }, { 99, 634 }, Colour(255, 255, 255)); InterfaceSurface->DrawText({ 103, 619 }, std::to_string(SelectedColour.G), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawLine({ (int)MapVal(0,255,11,97,SelectedColour.G),620-1 }, { (int)MapVal(0,255,11,97,SelectedColour.G),634 }, Colour(255, 255, 255));
	InterfaceSurface->FillRectangle({ 10,640 }, { 99, 654 }, Colour(0,0,SelectedColour.B)); InterfaceSurface->DrawRectangle({ 10,640 }, { 99, 654 }, Colour(255, 255, 255)); InterfaceSurface->DrawText({ 103, 639 }, std::to_string(SelectedColour.B), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawLine({ (int)MapVal(0,255,11,97,SelectedColour.B),640-1 }, { (int)MapVal(0,255,11,97,SelectedColour.B),654 }, Colour(255, 255, 255));
	InterfaceSurface->FillRectangle({ 135,600 }, { 190, 654 }, SelectedColour);
	InterfaceSurface->DrawRectangle({ 135,600 }, { 190, 654 }, Colour(255,255,255), 3);
	InterfaceSurface->DrawRectangle({ 135,600 }, { 190, 654 }, Colour(0,0,0), 1);*/

	//Font Select
	/*InterfaceSurface->DrawText({ 10, 670 }, "FontSize:" + std::to_string(FontSize), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawRectangle({ 10 - 1, 670 }, { 100, 685 + 1 }, ChangeFontSize ? Colour(255, 255, 255) : Colour(180, 180, 180), ChangeFontSize ? 2 : 0.5f);*/


	//Grid Control
	/*InterfaceSurface->DrawText({ 10, 700 }, "GridSize:" + std::to_string(GameWorld->GridSize), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawRectangle({ 10 - 1, 700 }, { 116, 715 + 1 }, ChangeGridSize ? Colour(255, 255, 255) : Colour(180, 180, 180), ChangeGridSize ? 2 : 0.5f);
	if (GameWorld->DrawGrid) { InterfaceSurface->FillRectangle({ 120, 700 }, { 135, 715 }, Colour(255, 255, 255)); }
	else { InterfaceSurface->DrawRectangle({ 120, 700 }, { 135, 715 }, Colour(255, 255, 255)); }
	InterfaceSurface->DrawText({ 145, 700 }, "Snap", Font("consolas", 10, { 255,255,255 }));
	if (SnapToGrid) { InterfaceSurface->FillRectangle({ 180, 700 }, { 195, 715 }, Colour(255, 255, 255)); }
	else { InterfaceSurface->DrawRectangle({ 180, 700 }, { 195, 715 }, Colour(255, 255, 255)); }*/

	//Display Values
	//InterfaceSurface->DrawText({ 10, 612 }, "ScreenPos:\n{" + std::to_string(GameWorld->ViewPos.X) + "," + std::to_string(GameWorld->ViewPos.Y) + "}", Font("Consolas", 10, Colour{ 255,255,255 }));
	//InterfaceSurface->DrawText({ 10, 645 }, "Zoom: " + std::to_string(GameWorld->Scale), Font("Consolas", 10, Colour{ 255,255,255 }));
	//{ Double2 pos = GameWorld->ReverseTransform(WindowPtr->Input->MouseState.GetPos()); InterfaceSurface->DrawText({ 10, 660 }, "MousePos:\n{" + std::to_string(pos.X) + "," + std::to_string(pos.Y) + "}", Font("Consolas", 10, Colour{ 255,255,255 })); }
	
	//Tool Selection
	/*if (SelectedTool == Select) { InterfaceSurface->FillRectangle({ { 10, 722 }, {100, 734} }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 720 }, "Select", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 720 }, "Select", Font("consolas", 10, { 255,255,255 })); }
	
	if (SelectedTool == SubSelect) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 1 },{ 100, 734 + 15 * 1 } }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 735 }, "SubSelect", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 735 }, "SubSelect", Font("consolas", 10, { 255,255,255 })); }
	
	if (SelectedTool == DrawLines) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 2 },{ 100, 734 + 15 * 2} }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 750 }, "DrawLines", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 750 }, "DrawLines", Font("consolas", 10, { 255,255,255 })); }

	if (SelectedTool == DrawEllipse) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 3 },{ 100, 734 + 15 * 3} }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 765 }, "DrawEllipse", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 765 }, "DrawEllipse", Font("consolas", 10, { 255,255,255 })); }

	if (SelectedTool == PlaceText) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 4 },{ 100, 734 + 15 * 4 } }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 780 }, "PlaceText", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 780 }, "PlaceText", Font("consolas", 10, { 255,255,255 })); }*/
}


void DnD::DnDProgram::GraphicWindow::SelectObject(int eIndex, int lIndex, int oIndex)
{
	SelectEnvironment(eIndex);
	SelectLayer(lIndex);
	if (SelectedEnvironment && SelectedLayer && SelectedLayer->ObjectList.size() && oIndex < SelectedLayer->ObjectList.size())
	{
		if (oIndex >= SelectedLayer->ObjectList.size()) { oIndex = 0; }

		SelectedObject = SelectedLayer->ObjectList[oIndex];
		SelectedObject->Selected = Selected;
		SelectedObjectIndex = oIndex;
	}
	else
	{
		SelectedObject = 0;
		SelectedObjectIndex = -1;
	}
}

void DnD::DnDProgram::GraphicWindow::SelectObject(int oIndex)
{
	SelectObject(SelectedEnvironmentIndex, SelectedLayerIndex, oIndex);
}

void DnD::DnDProgram::GraphicWindow::SubSelectObject(int eIndex, int lIndex, int oIndex)
{
	if (eIndex >= GameWorld->EnvironmentList.size()) { eIndex = 0; }
	if (lIndex >= GameWorld->EnvironmentList[eIndex]->LayerList.size()) { lIndex = 0; }
	if (oIndex >= GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList.size()) { oIndex = 0; }
	if (GameWorld->EnvironmentList.size() && GameWorld->EnvironmentList[eIndex]->LayerList.size() && GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList.size())
	{
		SelectedObject = GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList[oIndex];
		SelectedObjectIndex = oIndex;
		SelectedObject->Selected = SubSelected;
	}
	else
	{
		SelectedObject = 0;
		SelectedObjectIndex = -1;
	}
}

void DnD::DnDProgram::GraphicWindow::SubSelectObject(int oIndex)
{
	SubSelectObject(SelectedEnvironmentIndex, SelectedLayerIndex, oIndex);
}

void DnD::DnDProgram::GraphicWindow::SetObjectSelected(int eIndex, int lIndex, int oIndex)
{
	if (GameWorld->EnvironmentList.size() && eIndex < GameWorld->EnvironmentList.size() &&
		GameWorld->EnvironmentList[eIndex]->LayerList.size() && lIndex < GameWorld->EnvironmentList[eIndex]->LayerList.size() &&
		GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList.size() && oIndex < GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList.size())
	{
		GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList[oIndex]->Selected = Selected;
	}
}

void DnD::DnDProgram::GraphicWindow::SetObjectSelected(int oIndex)
{
	SetObjectSelected(SelectedEnvironmentIndex, SelectedLayerIndex, oIndex);
}

void DnD::DnDProgram::GraphicWindow::DeselectObject(int eIndex, int lIndex, int oIndex)
{
	if (GameWorld->EnvironmentList.size() && eIndex < GameWorld->EnvironmentList.size() &&
		GameWorld->EnvironmentList[eIndex]->LayerList.size() && lIndex < GameWorld->EnvironmentList[eIndex]->LayerList.size() &&
		GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList.size() && lIndex < GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList.size())
	{
		GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList[oIndex]->Selected = Unselected;
		if (GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->ObjectList[oIndex] == SelectedObject)
		{
			SelectedObject = 0;
			SelectedObjectIndex = -1;
		}
	}
}

void DnD::DnDProgram::GraphicWindow::DeselectObject(int oIndex)
{
	DeselectObject(SelectedEnvironmentIndex, SelectedLayerIndex, oIndex);
}

void DnD::DnDProgram::GraphicWindow::SelectLayer(int eIndex, int lIndex)
{
	SelectEnvironment(eIndex);
	if (SelectedEnvironment && SelectedEnvironment->LayerList.size() && lIndex < SelectedEnvironment->LayerList.size())
	{
		if (lIndex >= SelectedEnvironment->LayerList.size()) { lIndex = 0; }

		SelectedLayer = SelectedEnvironment->LayerList[lIndex];
		SelectedLayer->Selected = Selected;
		SelectedLayerIndex = lIndex;
	}
	else
	{
		SelectedLayer = 0;
		SelectedLayerIndex = -1;
	}

	GameWorld->DeselectObjects();
	SelectedObject = 0;
	SelectedObjectIndex = -1;
}

void DnD::DnDProgram::GraphicWindow::SelectLayer(int lIndex)
{
	SelectLayer(SelectedEnvironmentIndex, lIndex);
}

void DnD::DnDProgram::GraphicWindow::SetLayerSelected(int eIndex, int lIndex)
{
	if (GameWorld->EnvironmentList.size() && eIndex < GameWorld->EnvironmentList.size() &&
		GameWorld->EnvironmentList[eIndex]->LayerList.size() && lIndex < GameWorld->EnvironmentList[eIndex]->LayerList.size())
	{
		GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->Selected = Selected;
	}
}

void DnD::DnDProgram::GraphicWindow::SetLayerSelected(int lIndex)
{
	SetLayerSelected(SelectedEnvironmentIndex, lIndex);
}

void DnD::DnDProgram::GraphicWindow::DeselectLayer(int eIndex, int lIndex)
{
	if (GameWorld->EnvironmentList.size() && eIndex < GameWorld->EnvironmentList.size() &&
		GameWorld->EnvironmentList[eIndex]->LayerList.size() && lIndex < GameWorld->EnvironmentList[eIndex]->LayerList.size())
	{
		GameWorld->EnvironmentList[eIndex]->LayerList[lIndex]->Selected = Unselected;
		if (GameWorld->EnvironmentList[eIndex]->LayerList[lIndex] == SelectedLayer)
		{
			SelectedLayer = 0;
			SelectedLayerIndex = -1;

			GameWorld->DeselectObjects();
			SelectedObject = 0;
			SelectedObjectIndex = -1;
		}
	}
}

void DnD::DnDProgram::GraphicWindow::DeselectLayer(int lIndex)
{
	DeselectLayer(SelectedEnvironmentIndex, lIndex);
}

void DnD::DnDProgram::GraphicWindow::SelectEnvironment(int eIndex)
{
	if (GameWorld->EnvironmentList.size())
	{
		if (eIndex >= GameWorld->EnvironmentList.size()) { eIndex = 0; }

		SelectedEnvironment = GameWorld->EnvironmentList[eIndex];
		SelectedEnvironment->Selected = Selected;
		SelectedEnvironmentIndex = eIndex;
	}
	else
	{
		SelectedEnvironment = 0;
		SelectedEnvironmentIndex = -1;
	}

	GameWorld->DeselectLayers();
	SelectedLayer = 0;
	SelectedLayerIndex = -1;

	GameWorld->DeselectObjects();
	SelectedObject = 0;
	SelectedObjectIndex = -1;
}

void DnD::DnDProgram::GraphicWindow::SetEnvironmentSelected(int eIndex)
{
	if (GameWorld->EnvironmentList.size() && eIndex < GameWorld->EnvironmentList.size())
	{
		GameWorld->EnvironmentList[eIndex]->Selected = Selected;
	}
}

void DnD::DnDProgram::GraphicWindow::DeselectEnvironment(int eIndex)
{
	if (GameWorld->EnvironmentList.size() && eIndex < GameWorld->EnvironmentList.size())
	{
		GameWorld->EnvironmentList[eIndex]->Selected = Unselected;
		if (GameWorld->EnvironmentList[eIndex] == SelectedEnvironment)
		{
			SelectedEnvironment = 0;
			SelectedEnvironmentIndex = -1;

			SelectedLayer = 0;
			SelectedLayerIndex = -1;

			GameWorld->DeselectLayers();
			SelectedLayer = 0;
			SelectedLayerIndex = -1;
		}
	}
}


void DnD::DnDProgram::ConsoleCallback(Window *window, MessageData message, void *data)
{
	DnD::DnDProgram *program = (DnD::DnDProgram *)data;

	if (message == WL_UPDATESURFACE)
	{
		window->WindowSurface->Clear();
		program->ConsoleWin.CommandLine->Draw(program->ConsoleWin.WindowPtr->WindowSurface, { 0,0 });
		program->ConsoleWin.Command->Draw(program->ConsoleWin.WindowPtr->WindowSurface, { 0, 720 });
		program->ConsoleWin.WindowPtr->WindowSurface->DrawRectangle(program->ConsoleWin.CommandLine->TextSurface->GetRect(), Colour(255, 255, 255));
		program->ConsoleWin.WindowPtr->WindowSurface->DrawRectangle(program->ConsoleWin.Command->TextSurface->GetRect() + Int2{ 0, 720 }, Colour(255, 255, 255));
		program->ConsoleWin.WindowPtr->Draw();
		
	}
	else if (message == WM_KEYDOWN)
	{
		Key *keyState;
		keyState = program->ConsoleWin.WindowPtr->Input->KeyState;
		for (int i = 0; i < 49; i++)
		{
			if (message.WParam() == program->ConsoleWin.WindowPtr->Input->CharacterMap[i][0])
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
		if (message.WParam() == WL_RETURN)
		{
			program->ConsoleWin.CommandLine->Write(std::string(">>") + program->ConsoleWin.Command->Text[0] + "\n");
			program->ProcessCommand(program->ConsoleWin.Command->Text[0]);
			program->ConsoleWin.Command->Text[0] = "";
			program->ConsoleWin.Command->Cursor.X = 0;
		}
		if (message.WParam() == WL_BACKSPACE)
		{
			program->ConsoleWin.Command->BackspaceChar();
		}
		if (message.WParam() == WL_LEFT)
		{
			program->ConsoleWin.Command->MoveCursor(LEFT);
		}
		if (message.WParam() == WL_RIGHT)
		{
			program->ConsoleWin.Command->MoveCursor(RIGHT);
		}
	}
	else if (message == NW_CLOSE)
	{
		program->ConsoleWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		program->Network->Shutdown();
	}
	else if (message == NW_RECEIVE)
	{
		program->Network->Receive((int)message.WParam(), program->Network->SocketData.Identifier, program->Network->SocketData.Buf, program->Network->SocketData.DataSize);

		program->Network->SocketData.FreeBuf();
		program->Network->SocketData.IdentifierSize = 0;
		program->Network->SocketData.DataSize = 0;
	}
	else if (message == NW_MESSAGE)
	{
		MessageString str;
		WPARAM param = message.WParam();
		str = *(MessageString *)&param;

		program->ConsoleWin.CommandLine->Write(*str.String);

		str.Free();
	}
}


void DnD::DnDProgram::Run()
{
	GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	ConsoleWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

	ConsoleWin.CommandLine->Write("FLOW::Begin Main Loop\n");
	while (!(GraphicWin.WindowPtr->State.Quit || ConsoleWin.WindowPtr->State.Quit))
	{
		GraphicWin.WindowPtr->PreMsg();
		ConsoleWin.WindowPtr->PreMsg();
		Window::GetMsg();
		GraphicWin.WindowPtr->UpdateMsg();
		ConsoleWin.WindowPtr->UpdateMsg();
	}
}

void DnD::DnDProgram::ProcessCommand(std::string command)
{
	Interpreter->Interpret(command);
	/*if (command == "exit") { ConsoleWin.Window->State.Quit = true; }
	else if (command == "draw")
	{
		GraphicWin.Window->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
		ConsoleWin.CommandLine->Write("DEBUG::Initiated Draw\n");
	}*/
}



