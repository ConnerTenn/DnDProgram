
#include "World.h"
#include <sstream>
#include <iomanip>

Double2::Double2() {}
Double2::Double2(double x, double y) : X(x), Y(y) {}
Double2::Double2(double pos[2]) : X(pos[0]), Y(pos[1]) {}
Double2::Double2(Double2 &pos) : X(pos.X), Y(pos.Y) {}
Double2::Double2(Int2 &pos) : X(pos.X), Y(pos.Y) {}

double Double2::operator[](int i) { if (i == 0) { return X; } else if (i == 1) { return Y; } else { Throw("ERROR::Index out Of Range\n"); return 0; } }
Double2 Double2::operator+(Double2 other) { other.X = X + other.X; other.Y = Y + other.Y; return other; }
Double2 Double2::operator-(Double2 other) { other.X = X - other.X; other.Y = Y - other.Y; return other; }
Double2 Double2::operator*(Double2 other) { other.X = X * other.X; other.Y = Y * other.Y; return other; }
Double2 Double2::operator/(Double2 other) { other.X = X / other.X; other.Y = Y / other.Y; return other; }
Double2 Double2::operator+(double val) { Double2 temp = { X + val, Y + val }; return temp; }
Double2 Double2::operator-(double val) { Double2 temp = { X - val, Y - val }; return temp; }
Double2 Double2::operator*(double val) { Double2 temp = { X * val, Y * val }; return temp; }
Double2 Double2::operator/(double val) { Double2 temp = { X / val, Y / val }; return temp; }
bool Double2::operator==(Double2 other) { return X == other.X && Y == other.Y; }

Int2 Double2::ToInt2() { return { (int)X, (int)Y }; }


DnD::WorldObject::WorldObject()
{
	UuidCreate(&ObjectID);

	static int count = 0;
	std::stringstream ss;
	ss << std::setw(5) << std::setfill('0') << count++;
	Name = ss.str();
}

void DnD::WorldObject::Draw(Surface *surface, World *world)
{

}

bool DnD::WorldObject::ClickedOn(Double2 pos, double scale, Surface *surface)
{
	return false;
}

/*void DnD::WorldObject::UpdateBounds()
{

}*/

void DnD::Lines::Draw(Surface *surface, World *world)
{
	std::vector<Int2> points(LineList.size() + Closed);
	for (int i = 0; i < LineList.size(); i++)
	{
		points[i] = world->Transform(LineList[i]).ToInt2();
	}
	if (Closed) { points.back() = world->Transform(LineList[0]).ToInt2(); }
	surface->DrawLines(points, ObjectColour, Selected?3:0.5f);

	if (Selected == SubSelected)
	{
		std::vector<Rect> rects(LineList.size());
		for (int i = 0; i < LineList.size(); i++)
		{
			//rects[i] = { { LineList[i].X - 3 / world->Scale, LineList[i].Y - 3 / world->Scale }, { LineList[i].X + 3 / world->Scale, LineList[i].Y + 3 / world->Scale } };
			//rects[i].Pos1 = world->Transform(rects[i].Pos1).ToInt2();
			//rects[i].Pos2 = world->Transform(rects[i].Pos2).ToInt2();
			rects[i] = { world->Transform(LineList[i]).ToInt2(), world->Transform(LineList[i]).ToInt2() };
			rects[i].Pos1 = rects[i].Pos1 - 3;
			rects[i].Pos2 = rects[i].Pos2 + 3;
		}
		surface->FillRectangles(rects, Colour(0, 0, 0));
		surface->DrawRectangles(rects, ObjectColour);
	}


	/*if ((world->Transform(Bounds.Pos2).X - world->Transform(Bounds.Pos1).X) * (world->Transform(Bounds.Pos2).Y - world->Transform(Bounds.Pos1).Y) >= 4)
	{
		for (int i = 0; i < LineList.size() - 1; i++)
		{
			surface->DrawLine(world->Transform(LineList[i]).ToInt2(), world->Transform(LineList[i + 1]).ToInt2(), ObjectColour);
		}
		if (Closed)
		{
			surface->DrawLine(world->Transform(LineList[0]).ToInt2(), world->Transform(LineList.back()).ToInt2(), ObjectColour);
		}
		//surface->DrawRectangle(world->Transform(Bounds.Pos1).ToInt2(), world->Transform(Bounds.Pos2).ToInt2(), ObjectColour);
	}
	*/
	/*
	surface->StartFigure();
	for (int i = 0; i < LineList.size() - 1; i++)
	{
		surface->AddLine(world->Transform(LineList[i]).ToInt2(), world->Transform(LineList[i + 1]).ToInt2());
	}
	if (Closed)
	{
		surface->AddLine(world->Transform(LineList[0]).ToInt2(), world->Transform(LineList.back()).ToInt2());
	}
	*/
}

DnD::Lines::Lines()
{
	Type = LineObject;
}

bool DnD::Lines::ClickedOnCalculation(Double2 pos1, Double2 pos2, Double2 click, double scale)
{
	const double range = 3.0;
	double a = -(pos2.Y - pos1.Y), b = (pos2.X - pos1.X), c = a*pos1.X + b*pos1.Y;
	double c2 = b*click.X - a*click.Y;
	//intersection is the intersect between the segment and a perpendicular line through the point
	Double2 intersect = { (b*c2 + a*c) / (a*a + b*b), -(a*c2 - b*c) / (a*a + b*b) };

	//if intersection is not on the segment
	if (!(intersect.X >= min(pos2.X, pos1.X) && intersect.X <= max(pos2.X, pos1.X) && intersect.Y >= min(pos2.Y, pos1.Y) && intersect.Y <= max(pos2.Y, pos1.Y)))
	{
		//distance between endpoints and click is less than accepted range
		if ((click.X - pos2.X)*(click.X - pos2.X) + (click.Y - pos2.Y)*(click.Y - pos2.Y) < (range*range) / (scale*scale))
		{
			return true;
		}
		if ((click.X - pos1.X)*(click.X - pos1.X) + (click.Y - pos1.Y)*(click.Y - pos1.Y) < (range*range) / (scale*scale))
		{
			return true;
		}
		return false;
	}
	//distance between intersection and click is less than accepted range
	if ((intersect.X - click.X)*(intersect.X - click.X) + (intersect.Y - click.Y)*(intersect.Y - click.Y) < (range*range) / (scale*scale))
	{
		return true;
	}
	return false;
}

bool DnD::Lines::ClickedOn(Double2 pos, double scale, Surface *surface)
{
	for (int i = 1; i < LineList.size(); i++)
	{
		if (ClickedOnCalculation(LineList[i - 1], LineList[i], pos, scale))
		{
			return true;
		}
	}
	if (Closed && ClickedOnCalculation(LineList[0], LineList[LineList.size()-1], pos, scale)) { return true; }
	return false;
}

/*void DnD::Lines::UpdateBounds()
{
	Bounds.Pos1.X = LineList[0].X;
	Bounds.Pos1.Y = LineList[0].Y;
	Bounds.Pos2.X = LineList[0].X;
	Bounds.Pos2.Y = LineList[0].Y;
	for (int i = 1; i < LineList.size(); i++)
	{
		if (LineList[i].X < Bounds.Pos1.X) { Bounds.Pos1.X = LineList[i].X; }
		if (LineList[i].Y < Bounds.Pos1.Y) { Bounds.Pos1.Y = LineList[i].Y; }
		if (LineList[i].X > Bounds.Pos2.X) { Bounds.Pos2.X = LineList[i].X; }
		if (LineList[i].Y > Bounds.Pos2.Y) { Bounds.Pos2.Y = LineList[i].Y; }
	}
}*/

DnD::Ellipse::Ellipse()
{
	Type = EllipseObject;
}

void DnD::Ellipse::Draw(Surface *surface, World *world)
{
	Int2 pos = world->Transform(Pos).ToInt2();
	Int2 dimensions = (Double2(Radius) * world->Scale).ToInt2();
    surface->DrawEllipse(pos - dimensions, pos + dimensions, ObjectColour, Selected ? 3 : 0.5f);
	/*surface->StartFigure();
	Int2 pos = world->Transform(Pos - Radius).ToInt2();
	Int2 dimensions = (Double2(Radius * 2) * world->Scale).ToInt2();
	surface->AddEllipse(pos, dimensions);*/
	if (Selected == SubSelected)
	{
		std::vector<Rect> rects(5);
		rects[0] = { pos - 3, pos + 3 };//center
		rects[1] = { pos - dimensions - 3, pos - dimensions + 3 };//top left
		rects[2] = { pos + Int2{ dimensions.X, -dimensions.Y } -3, pos + Int2{ dimensions.X, -dimensions.Y } +3 };//top right
		rects[3] = { pos + Int2{ dimensions.X, dimensions.Y } -3, pos + Int2{ dimensions.X, dimensions.Y } +3 };//bottom right
		rects[4] = { pos + Int2{ -dimensions.X, dimensions.Y } -3, pos + Int2{ -dimensions.X, dimensions.Y } +3 };//bottom left
		surface->FillRectangles(rects, Colour(0, 0, 0));
		surface->DrawRectangles(rects, ObjectColour);
	}
}

bool DnD::Ellipse::ClickedOn(Double2 pos, double scale, Surface *surface)
{
	const double range = 3.0;
	double value1 = ((pos.X - (double)Pos.X)*(pos.X - (double)Pos.X)) / (((double)Radius.X + range/scale)*((double)Radius.X + range/scale)) + ((pos.Y - (double)Pos.Y)*(pos.Y - (double)Pos.Y)) / (((double)Radius.Y + range/scale)*((double)Radius.Y + range/scale));
	double value2 = ((pos.X - (double)Pos.X)*(pos.X - (double)Pos.X)) / (((double)Radius.X - range/scale)*((double)Radius.X - range/scale)) + ((pos.Y - (double)Pos.Y)*(pos.Y - (double)Pos.Y)) / (((double)Radius.Y - range/scale)*((double)Radius.Y - range/scale));

	if (value1 < 1.0 && value2 > 1.0)
	{
		return true;
	}
	return false;
}

/*void DnD::Ellipse::UpdateBounds()
{

}*/

DnD::Text::Text()
{
	Type = TextObject;
}

void DnD::Text::Draw(Surface *surface, World *world)
{
	surface->DrawText(world->Transform(Pos).ToInt2(), TextData, Font("consolas", float(40.0 * world->Scale), ObjectColour, Selected?WL_UNDERLINE:0));
	//surface->StartFigure();
	//surface->AddString(world->Transform(Pos).ToInt2(), TextData, Font("consolas", int(100 * world->Scale), Colour(255,0,0)));
	if (Selected == SubSelected)
	{
		Int2 dimensions = MeasureString(surface, TextData.size() ? TextData : "a", Font("consolas", float(40.0 * world->Scale), ObjectColour, Selected ? WL_UNDERLINE : 0));
		surface->DrawRectangle({ world->Transform(Pos).ToInt2(), world->Transform(Pos).ToInt2() + dimensions }, ObjectColour);
	}
}

bool DnD::Text::ClickedOn(Double2 pos, double scale, Surface *surface)
{
	Int2 dimensions = MeasureString(surface, TextData, Font("consolas", 40.0f, ObjectColour));
	if (pos.X > Pos.X && pos.X < Pos.X + dimensions.X && pos.Y > Pos.Y && pos.Y < Pos.Y + dimensions.Y)
	{
		return true;
	}
	return false;
}

/*void DnD::Text::UpdateBounds()
{

}*/

DnD::Layer::Layer()
{
	UuidCreate(&ObjectID);

	static int count = 0;
	std::stringstream ss;
	ss << std::setw(5) << std::setfill('0') << count++;
	Name = ss.str();
}

DnD::Layer::~Layer()
{
	for (int i = 0; i < ObjectList.size(); i++)
	{
		delete ObjectList[i]; ObjectList[i] = 0;
	}
	ObjectList.clear();
}

void DnD::Layer::Draw(Surface *surface, World *world)
{
	for (int i = 0; i < ObjectList.size(); i++)
	{
		ObjectList[i]->Draw(surface, world);
	}
}

DnD::Environment::Environment()
{
	UuidCreate(&ObjectID);

	static int count = 0;
	std::stringstream ss;
	ss << std::setw(5) << std::setfill('0') << count++;
	Name = ss.str();
}

DnD::Environment::~Environment()
{
	for (int i = 0; i < LayerList.size(); i++)
	{
		delete LayerList[i]; LayerList[i] = 0;
	}
	LayerList.clear();
}

void DnD::Environment::Draw(Surface *surface, World *world)
{
	for (int i = 0; i < LayerList.size(); i++)
	{
		LayerList[i]->Draw(surface, world);
	}
}



DnD::World::World(Window *parentWindow)
{
	ParentWindow = parentWindow;

	EnvironmentList.push_back(new Environment());
	EnvironmentList[0]->LayerList.push_back(new Layer());
	//LayerList.push_back(Layer());

	Lines *line;
	//for (int i = 0; i < 50; i++)
	for (int i = 0; i < 0; i++)
	{
		line = new Lines();
		int sides = rand() % 10 + 3;
		double size = double(rand() % 100 + 1)/50.0;
		Int2 center = { rand() % 10000 - 5000, rand() % 10000 - 5000 };
		for (int j = 0; j < sides; j++)
		{
			//wall->LineList.push_back(center + Int2{rand() % 40 - 20, rand() % 40 - 20});
			line->LineList.push_back(center + Int2{ (int)((rand() % 50 + 2) * size * sin((double)j*TAU / (double)sides)), (int)((rand() % 40 + 10) * size * -cos((double)j*TAU / (double)sides)) });
		}
		line->Closed = true;
		line->ObjectColour = Colour{ BYTE(rand() % 235 + 20), BYTE(rand() % 235 + 20), BYTE(rand() % 235 + 20) };
		//line->UpdateBounds();
		//LayerList[0].ObjectList.push_back(wall);
		EnvironmentList[0]->LayerList[0]->ObjectList.push_back(line);
	}

	Ellipse *ellipse;
	//for (int i = 0; i < 50; i++)
	for (int i = 0; i < 0; i++)
	{
		ellipse = new Ellipse();
		ellipse->Pos = { rand() % 10000 - 5000, rand() % 10000 - 5000 };
		ellipse->Radius = { (rand() % 50 + 10), (rand() % 50 + 10) };
		ellipse->ObjectColour = Colour{ BYTE(rand() % 235 + 20), BYTE(rand() % 235 + 20), BYTE(rand() % 235 + 20) };
		//LayerList[0].ObjectList.push_back(wall);
		//ellipse->UpdateBounds();
		EnvironmentList[0]->LayerList[0]->ObjectList.push_back(ellipse);
	}

	Text *text;
	//for (int i = 0; i < 50; i++)
	for (int i = 0; i < 0; i++)
	{
		text = new Text();
		text->Pos = { rand() % 10000 - 5000, rand() % 10000 - 5000 };
		int count = rand() % 5 + 5;
		for (int j = 0; j < count; j++)
		{
			text->TextData.push_back(rand() % ('z' - 'a') + 'a');
		}
		text->ObjectColour = Colour{ BYTE(rand() % 235 + 20), BYTE(rand() % 235 + 20), BYTE(rand() % 235 + 20) };
		//LayerList[0].ObjectList.push_back(wall);
		//text->UpdateBounds();
		EnvironmentList[0]->LayerList[0]->ObjectList.push_back(text);
	}
}

DnD::World::~World()
{
	for (int i = 0; i < EnvironmentList.size(); i++)
	{
		delete EnvironmentList[i]; EnvironmentList[i] = 0;
	}
	EnvironmentList.clear();
}

void DnD::World::Draw()
{
	//ParentWindow->WindowSurface->ClearPath();
	for (int i = 0; i < EnvironmentList.size(); i++)
	{
		EnvironmentList[i]->Draw(ParentWindow->WindowSurface, this);
	}
	//ParentWindow->WindowSurface->DrawPath(Colour(255,255,255));
}

Double2 DnD::World::Transform(Double2 point)
{
	return{ (point.X - ViewPos.X) * Scale + 400, (point.Y - ViewPos.Y) * Scale + 400 };
}

Double2 DnD::World::ReverseTransform(Double2 point)
{
	return{ (point.X - 400) / Scale + ViewPos.X, (point.Y - 400) / Scale + ViewPos.Y };
}

void DnD::World::DeselectObjects()
{
	for (int e = 0; e < EnvironmentList.size(); e++)
	{
		for (int l = 0; l < EnvironmentList[e]->LayerList.size(); l++)
		{
			for (int o = 0; o < EnvironmentList[e]->LayerList[l]->ObjectList.size(); o++)
			{
				EnvironmentList[e]->LayerList[l]->ObjectList[o]->Selected = Unselected;
			}
		}
	}
}

void DnD::World::Remove(WorldObject *object)
{
	if (object)
	{
		for (int e = 0; e < EnvironmentList.size(); e++)
		{
			for (int l = 0; l < EnvironmentList[e]->LayerList.size(); l++)
			{
				for (int o = 0; o < EnvironmentList[e]->LayerList[l]->ObjectList.size(); o++)
				{
					if (EnvironmentList[e]->LayerList[l]->ObjectList[o] == object)
					{
						EnvironmentList[e]->LayerList[l]->ObjectList.erase(EnvironmentList[e]->LayerList[l]->ObjectList.begin() + o);
					}
				}
			}
		}
		delete object;
	}
}

/*void DnD::World::AddWall()
{

}*/
/*
void World::AddObject()
{

}

void World::AddBoarders()
{

}

void World::AddGroundPlane()
{

}*/

/*void DnD::World::BeginNewLineLoop(Int2 point)
{

}

void DnD::World::AddPointToCurrentLineLoop(Int2 point)
{

}

void DnD::World::UpdateNewPointPos(Int2 point)
{

}

void DnD::World::EndLineLoop(bool closed)
{

}

void DnD::World::CancelLineLoop()
{

}*/




