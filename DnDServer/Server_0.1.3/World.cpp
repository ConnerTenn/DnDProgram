
#include "World.h"
//#include <sstream>
//#include <iomanip>



DnD::Player::Player()
{
	static int count = 0;
	//std::stringstream ss;
	//ss << std::setw(5) << std::setfill('0') << count++;
	//Name = ss.str();
	Name = std::to_string(count++);
}

DnD::Player::~Player()
{

}

void DnD::Player::Draw(Surface *surface, World *world)
{

	Int2 pos = world->Transform(Position).ToInt2();
	Int2 dimensions = (Double2(10 * world->Scale, 10 * world->Scale)).ToInt2();
	surface->DrawEllipse(pos - dimensions, pos + dimensions, { 255,0,0 }, Selected ? 3 : 0.5f);
	dimensions = (Double2(max(5, 10 * world->Scale), max(5, 10 * world->Scale))).ToInt2();
	surface->FillEllipse(pos - dimensions, pos + dimensions, { 255,0,0,150 });

	dimensions = MeasureString(surface, Name, Font("consolas", 7, { 255,255,255 }));
	surface->DrawText(world->Transform(Position - dimensions / 2).ToInt2(), Name, Font("consolas", float(7 * world->Scale), { 255,255,255 }, Selected ? WL_UNDERLINE : 0));

}

bool DnD::Player::ClickedOn(Double2 pos, double scale)
{
	Double2 Radius = { 10, 10 };
	double value = ((pos.X - (double)Position.X)*(pos.X - (double)Position.X)) / ((Radius.X)*(Radius.X)) + ((pos.Y - (double)Position.Y)*(pos.Y - (double)Position.Y)) / ((Radius.Y)*(Radius.Y));
	
	if (value < 1.0)
	{
		return true;
	}
	return false;
}

int DnD::Player::GetDataSize(Player *player)
{
	int size = 0;

	size += sizeof(Int2);
	size += sizeof(int);//stores size of Name
	size += (int)Name.size();

	return size;
}

int DnD::Player::GetData(char *data, Player *player)
{
	int offset = 0;

	*(Int2 *)(data + offset) = Position; offset += sizeof(Int2);
	*(int *)(data + offset) = (int)Name.size(); offset += sizeof(int);
	StringToChar(Name, data + offset); offset += (int)Name.size();

	return offset;
}

int DnD::Player::LoadFromData(char *data)
{
	int offset = 0;

	Position = *(Int2 *)(data + offset); offset += sizeof(Int2);
	int size = *(int *)(data + offset); offset += sizeof(int);
	Name = CharToString(data + offset, size); offset += size;
	
	return offset;
}

uint32_t DnD::HashColour(Colour colour, uint32_t hash)
{
	return hash ^ (colour.R | colour.G << 8 * 1 | colour.B << 8 * 2 | colour.A << 8 * 3);
	
}

uint32_t DnD::HashInt(uint32_t val, uint32_t hash)
{
	return hash ^ (val ^ (val << 1 | val >> 16));
}

uint32_t DnD::HashLong(uint64_t val, uint32_t hash)
{
	return HashInt((uint32_t)val) ^ HashInt((uint32_t)(val >> 32));
}

uint32_t DnD::HashString(std::string str, uint32_t hash)
{
	uint32_t buf = 0;
	for (int i = 0; i < str.size(); i++)
	{
		buf ^= str[i] << (i % 4);
	}
	return hash ^ buf;
}

uint32_t DnD::HashBool(bool val, uint32_t hash)
{
	return val ? ~hash : hash;
}

uint32_t DnD::HashInt2(Int2 val, uint32_t hash)
{
	return HashInt(val.X) ^ HashInt(val.Y);
}

uint32_t DnD::HashDouble2(Double2 val, uint32_t hash)
{
	return HashLong((uint64_t)val.X) ^ HashLong((uint64_t)val.Y);
}

DnD::WorldObject::WorldObject()
{
	static int count = 0;
	//std::stringstream ss;
	//ss << std::setw(5) << std::setfill('0') << count++;
	//Name = ss.str();
	Name = std::to_string(count++);
}

void DnD::WorldObject::Draw(Surface *surface, World *world, Player *activePlayer)
{

}

bool DnD::WorldObject::ClickedOn(Double2 pos, double scale, Surface *surface)
{
	return false;
}

bool DnD::WorldObject::ContainsPlayer(Player *player)
{
	if (player == 0) { return true; }
	for (int i = 0; i < PlayerList.size(); i++)
	{
		if (PlayerList[i] == player)
		{
			return true;
		}
	}
	return false;
}

bool DnD::WorldObject::ContainsOneOfPlayers(std::vector<Player *> players)
{
	for (int i = 0; i < players.size(); i++)
	{
		if (ContainsPlayer(players[i]))
		{
			return true;
		}
	}
	return false;
}

uint32_t DnD::WorldObject::CalculateHash()
{
	uint32_t hash = 0;

	hash = HashColour(ObjectColour, hash);
	hash = HashInt(Type, hash);
	hash = HashString(Name, hash);
	hash = HashBool(Visible, hash);

	return hash;
}

int DnD::WorldObject::GetDataSize(Player *player)
{
	int size = 0;
	size += sizeof(Colour);
	//size += sizeof(WorldObjectTypes);
	size += sizeof(int);//stores size of Name
	size += (int)Name.size();
	if (player == 0) 
	{
		size += sizeof(bool);

		size += sizeof(int);//size of playerlist
		for (int i = 0; i < PlayerList.size(); i++)
		{
			size += sizeof(int);//stores size of Name
			size += (int)PlayerList[i]->Name.size();
		}
	}
	return size;
}

int DnD::WorldObject::GetData(char *data, Player *player)
{
	int offset = 0;

	*(Colour *)(data + offset) = ObjectColour; offset += sizeof(Colour);
	//*(WorldObjectTypes *)(data + offset) = Type; offset += sizeof(WorldObjectTypes);
	*(int *)(data + offset) = (int)Name.size(); offset += sizeof(int);
	StringToChar(Name, data + offset); offset += (int)Name.size();
	if (player == 0)
	{
		*(bool *)(data + offset) = Visible; offset += sizeof(bool);
		
		*(int *)(data + offset) = (int)PlayerList.size(); offset += sizeof(int);
		for (int i = 0; i < PlayerList.size(); i++)
		{
			*(int *)(data + offset) = (int)PlayerList[i]->Name.size(); offset += sizeof(int);
			StringToChar(PlayerList[i]->Name, data + offset); offset += (int)PlayerList[i]->Name.size();
		}
	}

	return offset;
}

int DnD::WorldObject::LoadFromData(char *data, World *world, WorldObjectTypes type)
{
	int offset = 0;
	
	Type = type;

	ObjectColour = *(Colour *)(data + offset); offset += sizeof(Colour);
	//Type = *(WorldObjectTypes *)(data + offset); offset += sizeof(WorldObjectTypes);
	int size = *(int *)(data + offset); offset += sizeof(int);
	Name = CharToString(data + offset, size); offset += size;
	Visible = *(bool *)(data + offset); offset += sizeof(bool);

	size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		int nameSize = *(int *)(data + offset); offset += sizeof(int);
		std::string name = CharToString(data + offset, nameSize); offset += nameSize;
		PlayerList.push_back(world->FindPlayer(name));
	}

	return offset;
}


DnD::Lines::Lines()
{
	Type = LineObject;
}

/*DnD::Lines::Lines(const Lines &other)
{
	ObjectColour = other.ObjectColour;
	Type = other.Type;
	Selected = other.Selected;
	Name = other.Name;
	Visible = other.Visible;
	//Line Bounds;
	PlayerList = other.PlayerList;
}*/

void DnD::Lines::Draw(Surface *surface, World *world, Player *activePlayer)
{
	/*bool draw = (activePlayer == 0);
	if (activePlayer)
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (PlayerList[i] == activePlayer)
			{
				draw = true; i = (int)PlayerList.size();
			}
		}
	}*/
	if (ContainsPlayer(activePlayer)) { ObjectColour.A = 255; }
	else { ObjectColour.A = 150; }
	if (Visible)// && ContainsPlayer(activePlayer))//draw)
	{
		std::vector<Int2> points(LineList.size() + Closed);
		for (int i = 0; i < LineList.size(); i++)
		{
			points[i] = world->Transform(LineList[i]).ToInt2();
		}
		if (Closed) { points.back() = world->Transform(LineList[0]).ToInt2(); }
		surface->DrawLines(points, ObjectColour, Selected ? 3 : 0.5f);

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
	}
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

uint32_t DnD::Lines::CalculateHash()
{
	uint32_t hash = WorldObject::CalculateHash();

	for (int i = 0; i < LineList.size(); i++)
	{
		hash = HashInt2(LineList[i], hash);
	}

	return hash;
}

int DnD::Lines::GetDataSize(Player *player)
{
	int size = 0;
	size += WorldObject::GetDataSize(player);
	size += sizeof(bool);
	size += sizeof(int);//size of linelist
	size += (int)LineList.size() * sizeof(Int2);

	return size;
}

int DnD::Lines::GetData(char *data, Player *player)
{
	int offset = 0;

	offset += WorldObject::GetData(data + offset, player);

	*(bool *)(data + offset) = Closed; offset += sizeof(bool);
	*(int *)(data + offset) = (int)LineList.size(); offset += sizeof(int);
	for (int i = 0; i < LineList.size(); i++)
	{
		*(Int2 *)(data + offset) = LineList[i]; offset += sizeof(Int2);
	}

	return offset;
}

int DnD::Lines::LoadFromData(char *data, World *world, WorldObjectTypes type)
{
	int offset = 0;

	offset += WorldObject::LoadFromData(data + offset, world, type);

	Closed = *(bool *)(data + offset); offset += sizeof(bool);
	int size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		LineList.push_back(*(Int2 *)(data + offset)); offset += sizeof(Int2);
	}

	return offset;
}

DnD::Ellipse::Ellipse()
{
	Type = EllipseObject;
}

void DnD::Ellipse::Draw(Surface *surface, World *world, Player *activePlayer)
{
	/*bool draw = (activePlayer == 0);
	if (activePlayer)
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (PlayerList[i] == activePlayer)
			{
				draw = true; i = (int)PlayerList.size();
			}
		}
	}*/
	if (ContainsPlayer(activePlayer)) { ObjectColour.A = 255; }
	else { ObjectColour.A = 150; }
	if (Visible)// && ContainsPlayer(activePlayer))//draw)
	{
		Int2 pos = world->Transform(Pos).ToInt2();
		Int2 dimensions = (Double2(Radius) * world->Scale).ToInt2();
		surface->DrawEllipse(pos - dimensions, pos + dimensions, ObjectColour, Selected ? 3 : 0.5f);


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

uint32_t DnD::Ellipse::CalculateHash()
{
	uint32_t hash = WorldObject::CalculateHash();

	hash = HashInt2(Pos, hash);
	hash = HashInt2(Radius, hash);

	return hash;
}

int DnD::Ellipse::GetDataSize(Player *player)
{
	int size = 0;
	size += WorldObject::GetDataSize(player);
	size += sizeof(Int2);
	size += sizeof(Int2);

	return size;
}

int DnD::Ellipse::GetData(char *data, Player *player)
{
	int offset = 0;

	offset += WorldObject::GetData(data + offset, player);

	*(Int2 *)(data + offset) = Pos; offset += sizeof(Int2);
	*(Int2 *)(data + offset) = Radius; offset += sizeof(Int2);

	return offset;
}

int DnD::Ellipse::LoadFromData(char *data, World *world, WorldObjectTypes type)
{
	int offset = 0;

	offset += WorldObject::LoadFromData(data + offset, world, type);

	Pos = *(Int2 *)(data + offset); offset += sizeof(Int2);
	Radius = *(Int2 *)(data + offset); offset += sizeof(Int2);

	return offset;
}

DnD::Text::Text()
{
	Type = TextObject;
}

void DnD::Text::Draw(Surface *surface, World *world, Player *activePlayer)
{
	/*bool draw = (activePlayer == 0);
	if (activePlayer)
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (PlayerList[i] == activePlayer)
			{
				draw = true; i = (int)PlayerList.size();
			}
		}
	}*/
	if (ContainsPlayer(activePlayer)) { ObjectColour.A = 255; }
	else { ObjectColour.A = 150; }
	if (Visible )//&& ContainsPlayer(activePlayer))//draw)
	{
		surface->DrawText(world->Transform(Pos).ToInt2(), TextData, Font("consolas", float(float(FontSize) * world->Scale), ObjectColour, Selected ? WL_UNDERLINE : 0));

		if (Selected == SubSelected)
		{
			Int2 dimensions = GetSize(surface, world);//MeasureString(surface, TextData.size() ? TextData : "a", Font("consolas", float(float(FontSize) * world->Scale), ObjectColour, Selected ? WL_UNDERLINE : 0));
			surface->DrawRectangle({ world->Transform(Pos).ToInt2(), world->Transform(Pos).ToInt2() + dimensions }, ObjectColour);
		}
	}
}

bool DnD::Text::ClickedOn(Double2 pos, double scale, Surface *surface)
{
	Int2 dimensions = MeasureString(surface, TextData, Font("consolas", float(FontSize), ObjectColour));
	if (pos.X > Pos.X && pos.X < Pos.X + dimensions.X && pos.Y > Pos.Y && pos.Y < Pos.Y + dimensions.Y)
	{
		return true;
	}
	return false;
}

Int2 DnD::Text::GetSize(Surface *surface, World *world)
{
	return MeasureString(surface, TextData.size() ? TextData : "a", Font("consolas", float(float(FontSize) * world->Scale), ObjectColour, Selected ? WL_UNDERLINE : 0));
}

uint32_t DnD::Text::CalculateHash()
{
	uint32_t hash = WorldObject::CalculateHash();

	hash = HashInt2(Pos, hash);
	hash = HashString(TextData, hash);
	hash = HashInt(FontSize, hash);

	return hash;
}

int DnD::Text::GetDataSize(Player *player)
{
	int size = 0;
	size += WorldObject::GetDataSize(player);
	size += sizeof(Int2);
	size += sizeof(int);
	size += sizeof(int);//size of name
	size += (int)Name.size();
	size += sizeof(int);//size of textdata
	size += (int)TextData.size();

	return size;
}

int DnD::Text::GetData(char *data, Player *player)
{
	int offset = 0;

	offset += WorldObject::GetData(data + offset, player);

	*(Int2 *)(data + offset) = Pos; offset += sizeof(Int2);
	*(int *)(data + offset) = FontSize; offset += sizeof(int);

	*(int *)(data + offset) = (int)Name.size(); offset += sizeof(int);
	StringToChar(Name, data + offset); offset += (int)Name.size();

	*(int *)(data + offset) = (int)TextData.size(); offset += sizeof(int);
	StringToChar(TextData, data + offset); offset += (int)TextData.size();

	return offset;
}

int DnD::Text::LoadFromData(char *data, World *world, WorldObjectTypes type)
{
	int offset = 0;

	offset += WorldObject::LoadFromData(data + offset, world, type);

	Pos = *(Int2 *)(data + offset); offset += sizeof(Int2);
	FontSize = *(int *)(data + offset); offset += sizeof(int);

	int size = *(int *)(data + offset); offset += sizeof(int);
	Name = CharToString(data + offset, size); offset += size;

	size = *(int *)(data + offset); offset += sizeof(int);
	TextData = CharToString(data + offset, size); offset += size;

	return offset;
}

DnD::Character::Character()
{
	Type = CharacterObject;
}

void DnD::Character::Draw(Surface *surface, World *world, Player *activePlayer)
{
	if (ContainsPlayer(activePlayer)) { ObjectColour.A = 255; }
	else { ObjectColour.A = 150; }
	if (Visible)// && ContainsPlayer(activePlayer))//draw)
	{
		Int2 pos = world->Transform(Pos).ToInt2();
		Int2 dimensions = (Double2{ 10, 10 } * world->Scale).ToInt2();
		surface->DrawEllipse(pos - dimensions, pos + dimensions, ObjectColour, Selected ? 3 : 0.5f);


		if (Selected == SubSelected)
		{
			Rect rect = { pos - 3, pos + 3 };//center
			surface->DrawRectangle(rect, Colour(0, 0, 0));
			surface->DrawRectangle(rect, ObjectColour);
		}
	}
}

bool DnD::Character::ClickedOn(Double2 pos, double scale, Surface *surface)
{
	Double2 Radius = { 10, 10 };
	double value = ((pos.X - (double)Pos.X)*(pos.X - (double)Pos.X)) / ((Radius.X)*(Radius.X)) + ((pos.Y - (double)Pos.Y)*(pos.Y - (double)Pos.Y)) / ((Radius.Y)*(Radius.Y));

	if (value < 1.0)
	{
		return true;
	}
	return false;
}

uint32_t DnD::Character::CalculateHash()
{
	uint32_t hash = WorldObject::CalculateHash();

	hash = HashInt2(Pos, hash);

	return hash;
}

int DnD::Character::GetDataSize(Player *player)
{
	int size = 0;
	size += WorldObject::GetDataSize(player);
	size += sizeof(Int2);

	return size;
}

int DnD::Character::GetData(char *data, Player *player)
{
	int offset = 0;

	offset += WorldObject::GetData(data + offset, player);

	*(Int2 *)(data + offset) = Pos; offset += sizeof(Int2);

	return offset;
}

int DnD::Character::LoadFromData(char *data, World *world, WorldObjectTypes type)
{
	int offset = 0;

	offset += WorldObject::LoadFromData(data + offset, world, type);

	Pos = *(Int2 *)(data + offset); offset += sizeof(Int2);

	return offset;
}

DnD::Layer::Layer()
{
	static int count = 0;
	//std::stringstream ss;
	//ss << std::setw(5) << std::setfill('0') << count++;
	//Name = ss.str();
	Name = std::to_string(count++);
}

DnD::Layer::~Layer()
{
	for (int i = 0; i < ObjectList.size(); i++)
	{
		delete ObjectList[i]; ObjectList[i] = 0;
	}
	ObjectList.clear();
}

void DnD::Layer::Draw(Surface *surface, World *world, Player *activePlayer)
{
	/*bool draw = (activePlayer == 0);
	if (activePlayer)
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (PlayerList[i] == activePlayer)
			{
				draw = true; i = (int)PlayerList.size();
			}
		}
	}*/

	for (int i = 0; Visible && /*draw*/ /*ContainsPlayer(activePlayer) &&*/ i < ObjectList.size(); i++)
	{
		ObjectList[i]->Draw(surface, world, activePlayer);
	}
}

bool DnD::Layer::ContainsPlayer(Player *player)
{
	if (player == 0) { return true; }
	for (int i = 0; i < PlayerList.size(); i++)
	{
		if (PlayerList[i] == player)
		{
			return true;
		}
	}
	return false;
}

bool DnD::Layer::ContainsOneOfPlayers(std::vector<Player *> players)
{
	for (int i = 0; i < players.size(); i++)
	{
		if (ContainsPlayer(players[i]))
		{
			return true;
		}
	}
	return false;
}

void DnD::Layer::UpdatePlayers()
{
	for (int i = 0; i < ObjectList.size(); i++)
	{
		for (int p = 0; p < ObjectList[i]->PlayerList.size(); p++)
		{
			if (!ContainsPlayer(ObjectList[i]->PlayerList[p]))
			{
				PlayerList.push_back(ObjectList[i]->PlayerList[p]);
			}
		}
	}
}

uint32_t DnD::Layer::CalculateHash()
{
	uint32_t hash = 0;

	hash = HashString(Name, hash);
	hash = HashBool(Visible, hash);
	for (int i = 0; i < ObjectList.size(); i++)
	{
		hash = HashInt(ObjectList[i]->CalculateHash(), hash);
	}

	return hash;
}

int DnD::Layer::GetDataSize(Player *player)
{
	int size = 0;

	size += sizeof(int);//stores size of Name
	size += (int)Name.size();
	if (player == 0)
	{
		size += sizeof(bool);

		size += sizeof(int);//size of playerlist
		for (int i = 0; i < PlayerList.size(); i++)
		{
			size += sizeof(int);//stores size of Name
			size += (int)PlayerList[i]->Name.size();
		}
	}

	size += sizeof(int);//stores size of ObjectList
	for (int i = 0; i < ObjectList.size(); i++)
	{
		if (ObjectList[i]->ContainsPlayer(player))
		{
			size += sizeof(WorldObjectTypes);
			size += ObjectList[i]->GetDataSize(player);
		}
	}

	return size;
}

int DnD::Layer::GetData(char *data, Player *player)
{
	int offset = 0;

	*(int *)(data + offset) = (int)Name.size(); offset += sizeof(int);
	StringToChar(Name, data + offset); offset += (int)Name.size();
	if (player == 0)
	{
		*(bool *)(data + offset) = Visible; offset += sizeof(bool);

		*(int *)(data + offset) = (int)PlayerList.size(); offset += sizeof(int);
		for (int i = 0; i < PlayerList.size(); i++)
		{
			*(int *)(data + offset) = (int)PlayerList[i]->Name.size(); offset += sizeof(int);
			StringToChar(PlayerList[i]->Name, data + offset); offset += (int)PlayerList[i]->Name.size();
		}
	}

	int count = 0;
	for (int i = 0; i < ObjectList.size(); i++)
	{
		if (ObjectList[i]->ContainsPlayer(player))
		{
			count++;
		}
	}
	*(int *)(data + offset) = count; offset += sizeof(int);
	for (int i = 0; i < ObjectList.size(); i++)
	{
		if (ObjectList[i]->ContainsPlayer(player))
		{
			*(WorldObjectTypes *)(data + offset) = ObjectList[i]->Type; offset += sizeof(WorldObjectTypes);
			offset += ObjectList[i]->GetData(data + offset, player);
		}
	}

	return offset;
}

int DnD::Layer::LoadFromData(char *data, World *world)
{
	int offset = 0;
	
	int size = *(int *)(data + offset); offset += sizeof(int);
	Name = CharToString(data + offset, size); offset += size;
	Visible = *(bool *)(data + offset); offset += sizeof(bool);

	size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		int nameSize = *(int *)(data + offset); offset += sizeof(int);
		std::string name = CharToString(data + offset, nameSize); offset += nameSize;
		PlayerList.push_back(world->FindPlayer(name));
	}

	size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		WorldObjectTypes type = *(WorldObjectTypes *)(data + offset); offset += sizeof(WorldObjectTypes);
		WorldObject *worldObject = 0;
		if (type == LineObject) { worldObject = new Lines(); }
		else if (type == EllipseObject) { worldObject = new Ellipse(); }
		else if (type == TextObject) { worldObject = new Text(); }
		
		offset += worldObject->LoadFromData(data + offset, world, type);
		ObjectList.push_back(worldObject);
	}

	return offset;
}

DnD::Environment::Environment()
{
	static int count = 0;
	//std::stringstream ss;
	//ss << std::setw(5) << std::setfill('0') << count++;
	//Name = ss.str();
	Name = std::to_string(count++);
}

DnD::Environment::~Environment()
{
	for (int i = 0; i < LayerList.size(); i++)
	{
		delete LayerList[i]; LayerList[i] = 0;
	}
	LayerList.clear();
}

void DnD::Environment::Draw(Surface *surface, World *world, Player *activePlayer)
{
	/*bool draw = (activePlayer == 0);
	if (activePlayer)
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (PlayerList[i] == activePlayer)
			{
				draw = true; i = (int)PlayerList.size();
			}
		}
	}*/

	for (int i = 0; Visible && /*draw*/ /*ContainsPlayer(activePlayer) &&*/ i < LayerList.size(); i++)
	{
		LayerList[i]->Draw(surface, world, activePlayer);
	}
}

bool DnD::Environment::ContainsPlayer(Player *player)
{
	if (player == 0) { return true; }
	for (int i = 0; i < PlayerList.size(); i++)
	{
		if (PlayerList[i] == player)
		{
			return true;
		}
	}
	return false;
}

bool DnD::Environment::ContainsOneOfPlayers(std::vector<Player *> players)
{
	for (int i = 0; i < players.size(); i++)
	{
		if (ContainsPlayer(players[i]))
		{
			return true;
		}
	}
	return false;
}

void DnD::Environment::UpdatePlayers()
{
	for (int i = 0; i < LayerList.size(); i++)
	{
		//LayerList[i]->UpdatePlayers();
		for (int p = 0; p < LayerList[i]->PlayerList.size(); p++)
		{
			if (!ContainsPlayer(LayerList[i]->PlayerList[p]))
			{
				PlayerList.push_back(LayerList[i]->PlayerList[p]);
			}
		}
	}
}

uint32_t DnD::Environment::CalculateHash()
{
	uint32_t hash = 0;

	hash = HashString(Name, hash);
	hash = HashBool(Visible, hash);
	for (int i = 0; i < LayerList.size(); i++)
	{
		hash = HashInt(LayerList[i]->CalculateHash(), hash);
	}

	return hash;
}

int DnD::Environment::GetDataSize(Player *player)
{
	int size = 0;

	size += sizeof(int);//stores size of Name
	size += (int)Name.size();
	if (player == 0)
	{
		size += sizeof(bool);

		size += sizeof(int);//size of playerlist
		for (int i = 0; i < PlayerList.size(); i++)
		{
			size += sizeof(int);//stores size of Name
			size += (int)PlayerList[i]->Name.size();
		}
	}

	size += sizeof(int);//stores size of LayerList
	for (int i = 0; i < LayerList.size(); i++)
	{
		if (LayerList[i]->ContainsPlayer(player))
		{
			size += LayerList[i]->GetDataSize(player);
		}
	}

	return size;
}

int DnD::Environment::GetData(char *data, Player *player)
{
	int offset = 0;

	*(int *)(data + offset) = (int)Name.size(); offset += sizeof(int);
	StringToChar(Name, data + offset); offset += (int)Name.size();
	if (player == 0)
	{
		*(bool *)(data + offset) = Visible; offset += sizeof(bool);

		*(int *)(data + offset) = (int)PlayerList.size(); offset += sizeof(int);
		for (int i = 0; i < PlayerList.size(); i++)
		{
			*(int *)(data + offset) = (int)PlayerList[i]->Name.size(); offset += sizeof(int);
			StringToChar(PlayerList[i]->Name, data + offset); offset += (int)PlayerList[i]->Name.size();
		}
	}

	int count = 0;
	for (int i = 0; i < LayerList.size(); i++)
	{
		if (LayerList[i]->ContainsPlayer(player))
		{
			count++;
		}
	}
	*(int *)(data + offset) = count; offset += sizeof(int);
	for (int i = 0; i < LayerList.size(); i++)
	{
		if (LayerList[i]->ContainsPlayer(player))
		{
			offset += LayerList[i]->GetData(data + offset, player);
		}
	}

	return offset;
}

int DnD::Environment::LoadFromData(char *data, World *world)
{
	int offset = 0;

	int size = *(int *)(data + offset); offset += sizeof(int);
	Name = CharToString(data + offset, size); offset += size;
	Visible = *(bool *)(data + offset); offset += sizeof(bool);

	size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		int nameSize = *(int *)(data + offset); offset += sizeof(int);
		std::string name = CharToString(data + offset, nameSize); offset += nameSize;
		PlayerList.push_back(world->FindPlayer(name));
	}

	size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		Layer *layer = new Layer();
		offset += layer->LoadFromData(data + offset, world);
		LayerList.push_back(layer);
	}

	return offset;
}


DnD::World::World(Window *parentWindow, DnDProgram *program)
{
	ParentWindow = parentWindow;
	Program = program;

	//PlayerList.push_back(new Player());
	//PlayerList[0]->Name = "All";
	//PlayerList[0]->Connected = true;

	EnvironmentList.push_back(new Environment());
	//EnvironmentList[0]->PlayerList.push_back(PlayerList[0]);
	EnvironmentList[0]->LayerList.push_back(new Layer());
	//EnvironmentList[0]->LayerList[0]->PlayerList.push_back(PlayerList[0]);

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
	ClearObjects();
}

void DnD::World::ClearObjects()
{
	for (int i = 0; i < PlayerList.size(); i++)
	{
		delete PlayerList[i]; PlayerList[i] = 0;
	}

	for (int i = 0; i < EnvironmentList.size(); i++)
	{
		delete EnvironmentList[i]; EnvironmentList[i] = 0;
	}
	EnvironmentList.clear();

	PlayerList.clear();
}

void DnD::World::Draw(Environment *activeEnvironment, Player *activePlayer)
{
	//for (int i = 0; i < EnvironmentList.size(); i++)
	//{
	//	EnvironmentList[i]->Draw(ParentWindow->WindowSurface, this);
	//}
	if (activeEnvironment) { activeEnvironment->Draw(ParentWindow->WindowSurface, this, activePlayer); }
	for (int i = 0; i < PlayerList.size(); i++)
	{
		PlayerList[i]->Draw(ParentWindow->WindowSurface, this);
	}

	if (DrawGrid)
	{
		Int2 begin;
		Int2 end;
		

		begin = ReverseTransform({ 0,0 }).ToInt2() - 2;
		end = ReverseTransform({ 800,800 }).ToInt2() + 2;
		if (Scale >= 4)
		{
			ParentWindow->WindowSurface->ClearPath();
			for (int i = begin.X; i <= end.X; i++)
			{
				ParentWindow->WindowSurface->StartFigure();
				ParentWindow->WindowSurface->AddLine(Transform(Int2{ i,begin.Y }).ToInt2(), Transform(Int2{ i,end.Y }).ToInt2());
			}
			for (int i = begin.Y; i <= end.Y; i++)
			{
				ParentWindow->WindowSurface->StartFigure();
				ParentWindow->WindowSurface->AddLine(Transform(Int2{ begin.X,i }).ToInt2(), Transform(Int2{ end.X,i }).ToInt2());
			}
			ParentWindow->WindowSurface->DrawPath(Colour(200-30, 200-30, 255-30, 255 / 4));
		}

		ParentWindow->WindowSurface->ClearPath();
		begin = Int2{ (int)Floor(ReverseTransform({ 0,0 }).X, GridSize), (int)Floor(ReverseTransform({ 0,0 }).Y, GridSize) } - (2 * GridSize);
		end = Int2{ (int)Ceil(ReverseTransform({ 800,800 }).X, GridSize), (int)Floor(ReverseTransform({ 800,800 }).Y, GridSize) } + (2 * GridSize);
		if (GridSize && ((long)end.X - (long)begin.X) / GridSize <= 400)
		{
			for (int i = begin.X; i <= end.X; i += GridSize)
			{
				ParentWindow->WindowSurface->StartFigure();
				ParentWindow->WindowSurface->AddLine(Transform(Int2{ i,begin.Y }).ToInt2(), Transform(Int2{ i,end.Y }).ToInt2());
			}
			for (int i = begin.Y; i <= end.Y; i += GridSize)
			{
				ParentWindow->WindowSurface->StartFigure();
				ParentWindow->WindowSurface->AddLine(Transform(Int2{ begin.X,i }).ToInt2(), Transform(Int2{ end.X,i }).ToInt2());
			}
		}

		ParentWindow->WindowSurface->DrawPath(Colour(200, 200, 255, 255 / 4));
	}
}

DnD::Player *DnD::World::FindPlayer(std::string name)
{
	for (int i = 0; i < PlayerList.size(); i++)
	{
		if (PlayerList[i]->Name == name)
		{
			return PlayerList[i];
		}
	}
	return 0;
}

DnD::Player *DnD::World::ConnectPlayer(std::string name)
{
	DnD::Player *player = FindPlayer(name);
	if (player) { player->Connected = true; }
	return player;
}

void DnD::World::AddPlayer(DnD::Player *player)
{
	PlayerList.push_back(player);
}

void DnD::World::RemovePlayer(DnD::Player *player)
{
	if (player->Name != "All")
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (PlayerList[i] == player)
			{
				PlayerList.erase(PlayerList.begin() + i);
				i = (int)PlayerList.size();
			}
		}
	}
}

DnD::Player *DnD::World::RemovePlayer(std::string name)
{
	DnD::Player *player = 0;
	if (name != "All")
	{
		for (int i = 0; i < PlayerList.size(); i++)
		{
			if (PlayerList[i]->Name == name)
			{
				player = PlayerList[i];
				PlayerList.erase(PlayerList.begin() + i);
				i = (int)PlayerList.size();
			}
		}
	}
	return player;
}

Double2 DnD::World::Transform(Double2 point)
{
	return{ (point.X - ViewPos.X) * Scale + 400, (point.Y - ViewPos.Y) * Scale + 400 };
}

Double2 DnD::World::ReverseTransform(Double2 point)
{
	return{ (point.X - 400) / Scale + ViewPos.X, (point.Y - 400) / Scale + ViewPos.Y };
}

void DnD::World::DeselectPlayers()
{
	for (int i = 0; i < PlayerList.size(); i++)
	{
		PlayerList[i]->Selected = Unselected;
	}
}

void DnD::World::DeselectEnvironments()
{
	for (int e = 0; e < EnvironmentList.size(); e++)
	{
		EnvironmentList[e]->Selected = Unselected;
	}
}

void DnD::World::DeselectLayers()
{
	for (int e = 0; e < EnvironmentList.size(); e++)
	{
		for (int l = 0; l < EnvironmentList[e]->LayerList.size(); l++)
		{
			EnvironmentList[e]->LayerList[l]->Selected = Unselected;
		}
	}
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

int DnD::World::GetDataSize(Player *player)
{
	int size = 0;

	
	size += sizeof(int);//size of playerlist
	for (int i = 1; i < PlayerList.size(); i++)
	{
		size += PlayerList[i]->GetDataSize(player);
	}

	size += sizeof(int);//stores size of EnvironmentList
	for (int i = 0; i < EnvironmentList.size(); i++)
	{
		size += EnvironmentList[i]->GetDataSize(player);
	}

	return size;
}

int DnD::World::GetData(char *data, Player *player)
{
	int offset = 0;

	
	*(int *)(data + offset) = (int)PlayerList.size() - 1; offset += sizeof(int);
	for (int i = 1; i < PlayerList.size(); i++)
	{
		offset += PlayerList[i]->GetData(data + offset, player);
	}

	*(int *)(data + offset) = (int)EnvironmentList.size(); offset += sizeof(int);
	for (int i = 0; i < EnvironmentList.size(); i++)
	{
		offset += EnvironmentList[i]->GetData(data + offset, player);
	}

	return offset;
}

void DnD::World::LoadFromData(char *data)
{
	int offset = 0;

	ClearObjects();
	
	{
		Player *player = new Player();
		player->Name = "All"; player->Connected = true;
		PlayerList.push_back(player);
	}

	int size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		Player *player = new Player();
		offset += player->LoadFromData(data + offset);
		PlayerList.push_back(player);
	}

	size = *(int *)(data + offset); offset += sizeof(int);
	for (int i = 0; i < size; i++)
	{
		Environment *environment = new Environment();
		offset += environment->LoadFromData(data + offset, this);
		EnvironmentList.push_back(environment);
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




