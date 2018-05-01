
#include "DnDProgram.h"

inline double MapVal(double iRangeMin, double iRangeMax, double fRangeMin, double fRangeMax, double val)
{
	return ((val - iRangeMin) / (iRangeMax - iRangeMin) * (fRangeMax - fRangeMin)) + fRangeMin;
}

DnD::DnDProgram::DnDProgram(HINSTANCE hInstance, INT iCmdShow)
{
	WindowInfo windowInfo;
	
	windowInfo.Name = "Server_0.0.2  GraphicWindow";
	windowInfo.Dimensions = { 1000,800 };
	windowInfo.ClearColour = Colour(0, 0, 0);
	windowInfo.HInstance = hInstance;
	windowInfo.ICmdShow = iCmdShow;
	windowInfo.Style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	windowInfo.Callback = DnDProgram::GraphicCallback;
	windowInfo.CallbackPtr = this;
	GraphicWin.WindowPtr = new Window(&windowInfo);

	windowInfo.Name = "Server_0.0.2  ConsoleWindow";
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
	GraphicWin.SelectedEnvironment = GraphicWin.GameWorld->EnvironmentList[0]; GraphicWin.SelectedEnvironment->Selected = Selected;
	GraphicWin.SelectedLayer = GraphicWin.SelectedEnvironment->LayerList[0]; GraphicWin.SelectedLayer->Selected = Selected;

	Interpreter = new CommandInterpretor(this);

	GraphicWin.WindowPtr->Draw();
	ConsoleWin.WindowPtr->Draw();
}

DnD::DnDProgram::~DnDProgram()
{
	delete Interpreter;
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

	//Selecting and Adding Code
	/*if (message == WM_LBUTTONDOWN && !program->GraphicWin.Pan)
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
	}*/

}

void DnD::DnDProgram::GraphicWindow::Callback(MessageData message, DnDProgram *program)
{
	Int2 mousePos = WindowPtr->Input->MouseState.GetPos();
	Double2 mouseTransform = GameWorld->ReverseTransform(mousePos);

	if (message == WL_UPDATESURFACE)
	{
		WindowPtr->WindowSurface->Clear();
		GameWorld->Draw();
		/*if (ActiveObject.Object)
		{
			WorldObject *object = ActiveObject.Object;
			for (int i = 0; i < object->LineList.size() - 1; i++)
			{
				WindowPtr->WindowSurface->DrawLine(GameWorld->Transform(object->LineList[i]).ToInt2(), GameWorld->Transform(object->LineList[i + 1]).ToInt2(), Colour(255, 255, 0), 2.0);
			}
		}*/
		InterfaceSurface->Clear();
		DrawSelectMenu();

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
	if (message == WM_LBUTTONDOWN  && WindowPtr->Input->MouseState.GetPos().X < 800)
	{
		//record the initial mouse and view pos
		InitialMousePos = mousePos;// { GET_X_LPARAM(message.LParam()), GET_Y_LPARAM(message.LParam()) };
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
		Double2 pos2 = mouseTransform; //calculate the initial pos of the cursor after scaling
		GameWorld->ViewPos = Double2(GameWorld->ViewPos) + (pos1 - pos2); //translate the view by the difference in the 2 positions
		WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
	}

	{/*Selecting and Editing*/}
	if (!Pan)
	{
		//Select
		//can use object bounds to optimize it by excluding ones outside of the view
		if (SelectedTool == Select)
		{
			if (message == WM_LBUTTONDOWN && mousePos.X < 800)
			{
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);

				SelectedObject = 0; ActiveLines = 0; ActiveEllipse = 0; ActiveText = 0;
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
											//program->ConsoleWin.CommandLine->Write("DEBUG::Clicked on Object:" + std::to_string(GameWorld->EnvironmentList[e]->LayerList[l]->ObjectList[o]->Type) + "\n");
											SelectedObject = GameWorld->EnvironmentList[e]->LayerList[l]->ObjectList[o]; SelectedObject->Selected = Selected;
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
				GameWorld->Remove(SelectedObject);
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}

		//SubSelect
		if (SelectedTool == Select)
		{

		}

		//Lines
		if (SelectedTool == DrawLines)
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
					ActiveLines->Selected = SubSelected; SelectedObject = ActiveLines;
				}
				else
				{
					//if click on beginning of line loop
					if (mouseTransform.X > ActiveLines->LineList[0].X - 3 && mouseTransform.X < ActiveLines->LineList[0].X + 3 && mouseTransform.Y > ActiveLines->LineList[0].Y - 3 && mouseTransform.Y < ActiveLines->LineList[0].Y + 3)
					{
						ActiveLines->Closed = true;
						//deselect
						ActiveLines->Selected = Unselected;
						ActiveLines->LineList.pop_back();
						if (ActiveLines->LineList.size() < 2) { GameWorld->Remove(ActiveLines); }
						SelectedObject = 0; ActiveLines = 0;
					}
				}
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			//stop dragging point
			if (message == WM_LBUTTONUP)
			{
				if (ActiveLines)
				{
					ActiveLines->LineList.push_back(mouseTransform.ToInt2());
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
				ActiveLines->Selected = Unselected;
				ActiveLines->LineList.pop_back();
				if (ActiveLines->LineList.size() < 2) { GameWorld->Remove(ActiveLines); }
				SelectedObject = 0; ActiveLines = 0;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}

		//Ellipse
		if (SelectedTool == DrawEllipse)
		{
			//create new ellipse
			if (message == WM_LBUTTONDOWN && mousePos.X < 800)
			{
				ActiveEllipse = new Ellipse();
				ActiveEllipse->ObjectColour = SelectedColour;
				ActiveEllipse->Pos = mouseTransform.ToInt2();
				ActiveEllipse->Radius = { abs(mouseTransform.ToInt2().X- ActiveEllipse->Pos.X), abs(mouseTransform.ToInt2().Y - ActiveEllipse->Pos.Y) };
				SelectedLayer->ObjectList.push_back(ActiveEllipse);
				ActiveEllipse->Selected = SubSelected; SelectedObject = ActiveEllipse;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			//done resizing
			if (message == WM_LBUTTONUP)
			{
				if (ActiveEllipse)
				{
					ActiveEllipse->Selected = Unselected;
					if (ActiveEllipse->Radius == Int2{ 0, 0 }) { GameWorld->Remove(ActiveEllipse); delete ActiveEllipse; }
					ActiveEllipse = 0; SelectedObject = 0;
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
		}

		//Text
		if (SelectedTool == PlaceText)
		{
			if (message == WM_LBUTTONDOWN && mousePos.X < 800)
			{
				ActiveText = new Text();
				ActiveText->ObjectColour = SelectedColour;
				ActiveText->Pos = mouseTransform.ToInt2();
				SelectedLayer->ObjectList.push_back(ActiveText);
				ActiveText->Selected = SubSelected; SelectedObject = ActiveText;
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
				ActiveText->Selected = Unselected;
				if (ActiveText->TextData.size() < 1) { GameWorld->Remove(ActiveText); }
				SelectedObject = 0; ActiveText = 0;
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
			if (message == WM_KEYDOWN && message.WParam() && ActiveText)
			{
				char character = (char)message.WParam();
				if (character >= 'A' && character <= 'Z') { ActiveText->TextData.push_back(WindowPtr->Input->KeyState[WL_SHIFT].Pressed ? character : character - ('A' - 'a')); }
				if (character >= '0' && character <= '9' || character == ' ') { ActiveText->TextData.push_back(character); }
				if (character == WL_BACKSPACE && ActiveText->TextData.size()) { ActiveText->TextData.pop_back(); }
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}
		}
	}

	{/*InterfaceSurface */}
	{
		if (mousePos.X > 800)
		{
			//Selection Menus
			if (message == WM_MOUSEWHEEL && mousePos.X > 810 && mousePos.X < 990 && mousePos.Y > 460 && mousePos.Y < 590)
			{
				ObjectScroll -= int(14 * ((double)GET_WHEEL_DELTA_WPARAM(message.WParam()) / WHEEL_DELTA));
				WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
			}

			//Colour Select
			if (message == WM_LBUTTONDOWN || (message == WM_MOUSEMOVE && WindowPtr->Input->MouseState.LPressed))
			{
				if (Rect({ 810,600 }, { 898, 614 }).Intersect(mousePos)) { SelectedColour.R = (int)MapVal(810+1, 898-1, 0, 255, mousePos.X); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
				if (Rect({ 810,620 }, { 898, 634 }).Intersect(mousePos)) { SelectedColour.G = (int)MapVal(810+1, 898-1, 0, 255, mousePos.X); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
				if (Rect({ 810,640 }, { 898, 654 }).Intersect(mousePos)) { SelectedColour.B = (int)MapVal(810+1, 898-1, 0, 255, mousePos.X); WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
				
			}

			//tool select
			//{ 10, 702 }, {100, 714}
			if (message == WM_LBUTTONDOWN)
			{
				Tool Previous = SelectedTool;
				if (Rect({ 810, 720 + 15 * 0 }, { 900, 736 + 15 * 0 }).Intersect(mousePos)) { SelectedTool = Select; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); if (SelectedObject) { SelectedObject->Selected = Selected; } }
				if (Rect({ 810, 720 + 15 * 1 }, { 900, 736 + 15 * 1 }).Intersect(mousePos))	{ SelectedTool = SubSelect; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); if (SelectedObject) { SelectedObject->Selected = SubSelected; } }
				if (Rect({ 810, 720 + 15 * 2 }, { 900, 736 + 15 * 2 }).Intersect(mousePos)) { SelectedTool = DrawLines; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
				if (Rect({ 810, 720 + 15 * 3 }, { 900, 736 + 15 * 3 }).Intersect(mousePos)) { SelectedTool = DrawEllipse; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
				if (Rect({ 810, 720 + 15 * 4 }, { 900, 736 + 15 * 4 }).Intersect(mousePos)) { SelectedTool = PlaceText; WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true); }
				if (Previous == DrawLines && SelectedTool != DrawLines && ActiveLines)
				{
					ActiveLines->Selected = Unselected;
					ActiveLines->LineList.pop_back();
					if (ActiveLines->LineList.size() < 2) { GameWorld->Remove(ActiveLines); }
					SelectedObject = 0; ActiveLines = 0;
					WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				}
				if (Previous == PlaceText && SelectedTool != PlaceText && ActiveLines)
				{
					ActiveText->Selected = Unselected;
					if (ActiveText->TextData.size() < 1) { GameWorld->Remove(ActiveText); }
					SelectedObject = 0; ActiveText = 0;
					WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
				}
			}
		}
	}
}

void DnD::DnDProgram::GraphicWindow::DrawSelectMenu()
{
	//Player Select
	InterfaceSurface->DrawText({ 10, 5 }, "Player", Font("Consolas", 10, Colour{ 255,255,255 }));
	{
		
	}
	InterfaceSurface->DrawRectangle({ 10, 20 }, { 190, 140 }, Colour(255, 255, 255));

	//Environment Select
	InterfaceSurface->DrawText({ 10, 145 }, "Environment", Font("Consolas", 10, Colour{ 255,255,255 }));
	InterfaceSurface->DrawText({ 123, 148 }, "N S u d U D X", Font("Consolas", 6.9f, Colour{ 255,255,255 }));
	/*InterfaceSurface->DrawRectangle({ { 122, 150 },{ 130, 158 } }, Colour{ 255,255,255 });//new
	InterfaceSurface->DrawRectangle({ { 132, 150 },{ 140, 158 } }, Colour{ 255,255,255 });//visible
	InterfaceSurface->DrawRectangle({ { 142, 150 },{ 150, 158 } }, Colour{ 255,255,255 });//up
	InterfaceSurface->DrawRectangle({ { 152, 150 },{ 160, 158 } }, Colour{ 255,255,255 });//down
	InterfaceSurface->DrawRectangle({ { 162, 150 },{ 170, 158 } }, Colour{ 255,255,255 });//up up
	InterfaceSurface->DrawRectangle({ { 172, 150 },{ 180, 158 } }, Colour{ 255,255,255 });//down down
	InterfaceSurface->DrawRectangle({ { 182, 150 },{ 190, 158 } }, Colour{ 255,255,255 });//delete*/
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
			InterfaceSurface->DrawRectangle({ { 14, 166 + i * 25 - EnvironmentScroll },{ 27, 179 + i * 25 - EnvironmentScroll } }, { 255,255,255 });
			char *str = 0;
			UuidToStringA(&GameWorld->EnvironmentList[i]->ObjectID, (RPC_CSTR *)&str);
			InterfaceSurface->DrawText({ 30, 160 + 1 + i * 25 - EnvironmentScroll }, std::string(str), Font("consolas", 6, { 255,255,255 }, WL_BOLD));
			//InterfaceSurface->DrawText({ 30, 160 + 8 + i * 25 - EnvironmentScroll }, "Name____________.0000", Font("consolas", 10, { 255,255,255 }));
			InterfaceSurface->DrawText({ 30, 160 + 8 + i * 25 - EnvironmentScroll }, GameWorld->EnvironmentList[i]->Name, Font("consolas", 10, { 255,255,255 }));
			RpcStringFree((RPC_CSTR*)&str);
		}
		InterfaceSurface->ResetClip();
	}
	InterfaceSurface->DrawRectangle({ 10, 160 }, { 190, 290 }, Colour(255, 255, 255));

	//Layer Select
	InterfaceSurface->DrawText({ 10, 295 }, "Layer", Font("Consolas", 10, Colour{ 255,255,255 }));
	InterfaceSurface->DrawRectangle({ { 132, 300 },{ 140, 308 } }, Colour{ 255,255,255 });//visible
	InterfaceSurface->DrawRectangle({ { 142, 300 },{ 150, 308 } }, Colour{ 255,255,255 });//up
	InterfaceSurface->DrawRectangle({ { 152, 300 },{ 160, 308 } }, Colour{ 255,255,255 });//down
	InterfaceSurface->DrawRectangle({ { 162, 300 },{ 170, 308 } }, Colour{ 255,255,255 });//up up
	InterfaceSurface->DrawRectangle({ { 172, 300 },{ 180, 308 } }, Colour{ 255,255,255 });//down down
	InterfaceSurface->DrawRectangle({ { 182, 300 },{ 190, 308 } }, Colour{ 255,255,255 });//delete
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
			InterfaceSurface->DrawRectangle({ { 14, 316 + i * 25 - LayerScroll },{ 27, 329 + i * 25 - LayerScroll } }, { 255,255,255 });
			char *str = 0;
			UuidToStringA(&GameWorld->EnvironmentList[i]->ObjectID, (RPC_CSTR *)&str);
			InterfaceSurface->DrawText({ 30, 310 + 1 + i * 25 - LayerScroll }, std::string(str), Font("consolas", 6, { 255,255,255 }, WL_BOLD));
			//InterfaceSurface->DrawText({ 30, 310 + 8 + i * 25 - LayerScroll }, "Name____________.0000", Font("consolas", 10, { 255,255,255 }));
			InterfaceSurface->DrawText({ 30, 310 + 8 + i * 25 - LayerScroll }, GameWorld->EnvironmentList[i]->Name, Font("consolas", 10, { 255,255,255 }));
			RpcStringFree((RPC_CSTR*)&str);
		}
		InterfaceSurface->ResetClip();
	}
	InterfaceSurface->DrawRectangle({ 10, 310 }, { 190, 440 }, Colour(255, 255, 255));

	//Object Select
	InterfaceSurface->DrawText({ 10, 445 }, "Object", Font("Consolas", 10, Colour{ 255,255,255 }));
	InterfaceSurface->DrawRectangle({ { 132, 450 },{ 140, 458 } }, Colour{ 255,255,255 });//visible
	InterfaceSurface->DrawRectangle({ { 142, 450 },{ 150, 458 } }, Colour{ 255,255,255 });//up
	InterfaceSurface->DrawRectangle({ { 152, 450 },{ 160, 458 } }, Colour{ 255,255,255 });//down
	InterfaceSurface->DrawRectangle({ { 162, 450 },{ 170, 458 } }, Colour{ 255,255,255 });//up up
	InterfaceSurface->DrawRectangle({ { 172, 450 },{ 180, 458 } }, Colour{ 255,255,255 });//down down
	InterfaceSurface->DrawRectangle({ { 182, 450 },{ 190, 458 } }, Colour{ 255,255,255 });//delete
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
			InterfaceSurface->DrawRectangle({ { 14, 466 + i * 25 - ObjectScroll },{ 27, 479 + i * 25 - ObjectScroll } }, { 255,255,255 });
			char *str = 0;
			UuidToStringA(&SelectedLayer->ObjectList[i]->ObjectID, (RPC_CSTR *)&str);
			InterfaceSurface->DrawText({ 30, 460 + 1 + i * 25 - ObjectScroll }, std::string(str), Font("consolas", 6, { 255,255,255 }, WL_BOLD));
			//InterfaceSurface->DrawText({ 30, 460 + 8 + i * 25 - ObjectScroll }, "Name____________.0000", Font("consolas", 10, { 255,255,255 }));
			InterfaceSurface->DrawText({ 30, 460 + 8 + i * 25 - ObjectScroll }, SelectedLayer->ObjectList[i]->Name, Font("consolas", 10, { 255,255,255 }));
			RpcStringFree((RPC_CSTR*)&str);
		}
		InterfaceSurface->ResetClip();
	}
	InterfaceSurface->DrawRectangle({ 10, 460 }, { 190, 590 }, Colour(255, 255, 255));

	//Colour Select
	InterfaceSurface->FillRectangle({ 10,600 }, { 99, 614 }, Colour(SelectedColour.R,0,0)); InterfaceSurface->DrawRectangle({ 10,600 }, { 99, 614 }, Colour(255, 255, 255)); InterfaceSurface->DrawText({ 103, 599 }, std::to_string(SelectedColour.R), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawLine({ (int)MapVal(0,255,11,97,SelectedColour.R),600-1 }, { (int)MapVal(0,255,11,97,SelectedColour.R),614 }, Colour(255, 255, 255));
	InterfaceSurface->FillRectangle({ 10,620 }, { 99, 634 }, Colour(0,SelectedColour.G,0)); InterfaceSurface->DrawRectangle({ 10,620 }, { 99, 634 }, Colour(255, 255, 255)); InterfaceSurface->DrawText({ 103, 619 }, std::to_string(SelectedColour.G), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawLine({ (int)MapVal(0,255,11,97,SelectedColour.G),620-1 }, { (int)MapVal(0,255,11,97,SelectedColour.G),634 }, Colour(255, 255, 255));
	InterfaceSurface->FillRectangle({ 10,640 }, { 99, 654 }, Colour(0,0,SelectedColour.B)); InterfaceSurface->DrawRectangle({ 10,640 }, { 99, 654 }, Colour(255, 255, 255)); InterfaceSurface->DrawText({ 103, 639 }, std::to_string(SelectedColour.B), Font("consolas", 10, { 255,255,255 }));
	InterfaceSurface->DrawLine({ (int)MapVal(0,255,11,97,SelectedColour.B),640-1 }, { (int)MapVal(0,255,11,97,SelectedColour.B),654 }, Colour(255, 255, 255));
	InterfaceSurface->FillRectangle({ 135,600 }, { 190, 654 }, SelectedColour);
	InterfaceSurface->DrawRectangle({ 135,600 }, { 190, 654 }, Colour(255,255,255), 3);
	InterfaceSurface->DrawRectangle({ 135,600 }, { 190, 654 }, Colour(0,0,0), 1);
	
	//Display Values
	//InterfaceSurface->DrawText({ 10, 612 }, "ScreenPos:\n{" + std::to_string(GameWorld->ViewPos.X) + "," + std::to_string(GameWorld->ViewPos.Y) + "}", Font("Consolas", 10, Colour{ 255,255,255 }));
	//InterfaceSurface->DrawText({ 10, 645 }, "Zoom: " + std::to_string(GameWorld->Scale), Font("Consolas", 10, Colour{ 255,255,255 }));
	//{ Double2 pos = GameWorld->ReverseTransform(WindowPtr->Input->MouseState.GetPos()); InterfaceSurface->DrawText({ 10, 660 }, "MousePos:\n{" + std::to_string(pos.X) + "," + std::to_string(pos.Y) + "}", Font("Consolas", 10, Colour{ 255,255,255 })); }
	
	//Tool Selection
	if (SelectedTool == Select) { InterfaceSurface->FillRectangle({ { 10, 722 }, {100, 734} }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 720 }, "Select", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 720 }, "Select", Font("consolas", 10, { 255,255,255 })); }
	
	if (SelectedTool == SubSelect) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 1 },{ 100, 734 + 15 * 1 } }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 735 }, "SubSelect", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 735 }, "SubSelect", Font("consolas", 10, { 255,255,255 })); }
	
	if (SelectedTool == DrawLines) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 2 },{ 100, 734 + 15 * 2} }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 750 }, "DrawLines", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 750 }, "DrawLines", Font("consolas", 10, { 255,255,255 })); }

	if (SelectedTool == DrawEllipse) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 3 },{ 100, 734 + 15 * 3} }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 765 }, "DrawEllipse", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 765 }, "DrawEllipse", Font("consolas", 10, { 255,255,255 })); }

	if (SelectedTool == PlaceText) { InterfaceSurface->FillRectangle({ { 10, 722 + 15 * 4 },{ 100, 734 + 15 * 4 } }, { 40,100,230 }); InterfaceSurface->DrawText({ 10, 780 }, "PlaceText", Font("consolas", 10, { 255,255,255 })); }
	else { InterfaceSurface->DrawText({ 10, 780 }, "PlaceText", Font("consolas", 10, { 255,255,255 })); }
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