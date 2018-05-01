
#include "FileReader.h"

FileReader::FileReader(DnD::DnDProgram *program)
{
	Program = program;
}

Result FileReader::Call(char current, char next)
{
	try
	{
		return (this->*Func)(current, next);
	}
	catch (...)
	{
		return Result::Error;
	}
}

Result FileReader::Start(char next)
{
	if (EndOfString)
	{
		return Result::End;
	}
	else if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z'))
	{
		Func = &FileReader::ParseIdentifier;
		return Result::Continue;
	}

	return Result::Error;
}

Result FileReader::End()
{
	return Result::End;
}

Result FileReader::ParseIdentifier(char current, char next)
{
	Buffer += current;
	ParseState = 0;

	if (current == ' ' || current == '\n' || current == '\t' || current == '\r')
	{
		Buffer.pop_back();
	}

	if (EndOfString)
	{
		if (Buffer == "EOF")
		{
			return Result::End;
		}
		return Result::Error;
	}
	else if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z'))
	{
		Func = &FileReader::ParseIdentifier;
		return Result::Continue;
	}
	else if (next == ' ' || next == '\n' || next == '\t' || next == '\r')
	{
		Func = &FileReader::ParseIdentifier;
		return Result::Continue;
	}
	else if (next == ':')
	{
		Identifier = Buffer;
		Buffer.clear();

		Func = &FileReader::ParseDelimeter;
		return Result::Continue;
	}

	return Result::Error;
}

Result FileReader::ParseDelimeter(char current, char next)
{
	if (EndOfString)
	{
		return Result::Error;
	}
	if (ParseState == 0)
	{
		ParseState = 1;
		Buf = new char[sizeof(int)];
		TargetCount = sizeof(int); Count = 0;
		Func = &FileReader::ParseValue;
	}
	else if (ParseState == 1)
	{
		ParseState = 2;
		Length = *(int *)Buf;
		delete[] Buf; Buf = 0;
		if (Length == 0) { Buf = 0; } else { Buf = new char[Length]; }
		TargetCount = Length; Count = 0;
		Func = &FileReader::ParseValue;
	}

	return Result::Continue;
}

Result FileReader::ParseValue(char current, char next)
{
	Func = &FileReader::ParseValue;

	if (EndOfString)
	{
		if (Count >= TargetCount - 1)// || TargetCount == 0)
		{
			return Result::End;
		}
		else
		{
			return Result::Error;
		}
	}

	if (Buf) { Buf[Count] = current; }

	if (Count >= TargetCount - 1)
	{
		if (ParseState == 1)
		{
			//ParseSize();
			Func = &FileReader::ParseDelimeter;
		}
		else if (ParseState == 2)
		{
			if (Execute() == Result::Error) { if (Buf) { delete[] Buf; Buf = 0; } return Result::Error; }
			if (Buf) { delete[] Buf; Buf = 0; } Buffer.clear();
			Func = &FileReader::ParseIdentifier;
		}

	}

	Count++;
	return Result::Continue;
}

/*Result FileReader::ParseSize()
{

}*/
template<typename T>
T GetEnd(std::vector<T> &list)
{
	return list[list.size() - 1];
}


Result FileReader::Execute()
{
	DnD::World *gameWorld = Program->GraphicWin.GameWorld;
	Program->ConsoleWin.CommandLine->Write(Identifier + "\n");

	if (Identifier == "Version")
	{
		if (ProgramVersion != CharToString(Buf, Length))
		{
			Program->ConsoleWin.CommandLine->Write("WARNING::Loading file from different version\n");
		}
	}
	if (Identifier == "ViewPos")
	{
		gameWorld->ViewPos = *(Int2 *)Buf;
	}
	if (Identifier == "Scale")
	{
		gameWorld->Scale = *(double *)Buf;
	}
	if (Identifier == "DrawGrid")
	{
		gameWorld->DrawGrid = *(bool *)Buf;
	}
	if (Identifier == "GridSize")
	{
		gameWorld->GridSize = *(int *)Buf;
	}
	if (Identifier == "SelectedEnvironmentIndex")
	{
		Program->GraphicWin.SelectedEnvironmentIndex = *(int *)Buf;
	}
	if (Identifier == "SelectedLayerIndex")
	{
		Program->GraphicWin.SelectedLayerIndex = *(int *)Buf;
	}
	
	if (Identifier == "Player")
	{
		switch (InterpretState)
		{
		case None:
			InterpretState = Player;
			gameWorld->PlayerList.push_back(new DnD::Player());
			break;
		case Environment:
			for (int i = 0; i < gameWorld->PlayerList.size(); i++)
			{
				if (gameWorld->PlayerList[i]->Name == CharToString(Buf, Length))
				{
					GetEnd(gameWorld->EnvironmentList)->PlayerList.push_back(gameWorld->PlayerList[i]);
				}
			}
			break;
		case Layer:
			for (int i = 0; i < gameWorld->PlayerList.size(); i++)
			{
				if (gameWorld->PlayerList[i]->Name == CharToString(Buf, Length))
				{
					GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->PlayerList.push_back(gameWorld->PlayerList[i]);
				}
			}
			break;
		case Object:
			for (int i = 0; i < gameWorld->PlayerList.size(); i++)
			{
				if (gameWorld->PlayerList[i]->Name == CharToString(Buf, Length))
				{
					GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList)->PlayerList.push_back(gameWorld->PlayerList[i]);
				}
			}
			break;
		default:
			return Result::Error;
		}
	}
	if (Identifier == "Environment")
	{
		InterpretState = Environment;
		gameWorld->EnvironmentList.push_back(new DnD::Environment());
	}
	if (Identifier == "Layer" && InterpretState >= Environment)
	{
		InterpretState = Layer;
		GetEnd(gameWorld->EnvironmentList)->LayerList.push_back(new DnD::Layer());
	}
	if (Identifier == "WorldObject" && InterpretState >= Layer)
	{
		//InterpretState = WorldObject;
		switch (*(DnD::WorldObjectTypes *)Buf)
		{
		case DnD::WorldObjectTypes::LineObject:
			InterpretState = Object;
			GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList.push_back(new DnD::Lines());
			break;
		case DnD::WorldObjectTypes::EllipseObject:
			InterpretState = Object;
			GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList.push_back(new DnD::Ellipse());
			break;
		case DnD::WorldObjectTypes::TextObject:
			InterpretState = Object;
			GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList.push_back(new DnD::Text());
			break;
		}
		//GetEnd(gameWorld->EnvironmentList)->LayerList.push_back(new DnD::Layer());
	}

	if (Identifier == "Name")
	{
		switch (InterpretState)
		{
		case Player:
			GetEnd(gameWorld->PlayerList)->Name = CharToString(Buf, Length);
			break;
		case Environment:
			GetEnd(gameWorld->EnvironmentList)->Name = CharToString(Buf, Length);
			break;
		case Layer:
			GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->Name = CharToString(Buf, Length);
			break;
		case Object:
			GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList)->Name = CharToString(Buf, Length);
			break;
		default:
			return Result::Error;
		}
	}
	if (Identifier == "Position")
	{
		switch (InterpretState)
		{
		case Player:
			GetEnd(gameWorld->PlayerList)->Position = *(Int2 *)Buf;
			break;
		default:
			return Result::Error;
		}
	}
	if (Identifier == "Visible")
	{
		switch (InterpretState)
		{
		case Player:
			GetEnd(gameWorld->PlayerList)->Visible = *(bool *)Buf;
			break;
		case Environment:
			GetEnd(gameWorld->EnvironmentList)->Visible = *(bool *)Buf;
			break;
		case Layer:
			GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->Visible = *(bool *)Buf;
			break;
		case Object:
			GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList)->Visible = *(bool *)Buf;
			break;
		default:
			return Result::Error;
		}
	}
	if (Identifier == "ObjectColour" && InterpretState == Object)
	{
		GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList)->ObjectColour = *(Colour *)Buf;
	}
	if (Identifier == "Data" && InterpretState == Object && GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList)->Type == DnD::WorldObjectTypes::LineObject)
	{
		for (int i = 0; i < Length / sizeof(Int2); i++)
		{
			((DnD::Lines *)GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList))->LineList.push_back(*(Int2 *)(Buf + i*sizeof(Int2)));
		}
	}
	if (InterpretState == Object && GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList)->Type == DnD::WorldObjectTypes::EllipseObject)
	{
		if (Identifier == "Pos")
		{
			((DnD::Ellipse *)GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList))->Pos = *(Int2 *)Buf;
		}
		if (Identifier == "Radius")
		{
			((DnD::Ellipse *)GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList))->Radius = *(Int2 *)Buf;
		}
	}
	if (InterpretState == Object && GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList)->Type == DnD::WorldObjectTypes::TextObject)
	{
		if (Identifier == "Pos")
		{
			((DnD::Text *)GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList))->Pos = *(Int2 *)Buf;
		}
		if (Identifier == "TextData")
		{
			((DnD::Text *)GetEnd(GetEnd(GetEnd(gameWorld->EnvironmentList)->LayerList)->ObjectList))->TextData = CharToString(Buf, Length);
		}
	}

	return Result::Continue;
}



void LoadConfig(DnD::DnDProgram *program)
{
	std::fstream file;
	//file.open(name, std::ios::trunc | std::ios::app);
	file.open("config.txt", std::ios::in);

	std::string line;
	std::string identifier;
	std::string argument;

	while (std::getline(file, line))
	{
		int space = (int)line.find(' ');
		identifier = line.substr(0, space);
		argument = line.substr(space + 1, line.length() - space);

		if (identifier == "IP")
		{
			program->Network->NetIP = argument;
		}
		else if (identifier == "Port")
		{
			program->Network->NetPort = argument;
		}

	}

	file.close();
}

