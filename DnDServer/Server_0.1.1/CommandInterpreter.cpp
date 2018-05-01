
#include "CommandInterpreter.h"


ParseCommand::ParseCommand()
{
	//Func = &ParseMath::Start;
}

Result ParseCommand::Call(char current, char next)
{
	return (this->*Func)(current, next);
}

Result ParseCommand::Start(char next)
{
	if (EndOfString)
	{
		return Result::End;
	}
	else if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z'))
	{
		Func = &ParseCommand::Command;
		return Result::Continue;
	}

	return Result::Error;
}

Result ParseCommand::End()
{
	return Result::End;
}

Result ParseCommand::Command(char current, char next)
{
	Buffer += current;

	if (EndOfString)
	{
		ParseOut.push_back(Buffer);
		Buffer = "";

		return Result::End;
	}
	else if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z'))
	{
		Func = &ParseCommand::Command;
		return Result::Continue;
	}
	else if (next == ' ')
	{
		ParseOut.push_back(Buffer);
		Buffer = "";

		Func = &ParseCommand::Whitespace;
		return Result::Continue;
	}

	return Result::Error;
}

Result ParseCommand::Whitespace(char current, char next)
{
	if (EndOfString)
	{
		return Result::End;
	}
	else if (next == ' ')
	{
		Func = &ParseCommand::Whitespace;
		return Result::Continue;
	}
	else if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z'))
	{
		Func = &ParseCommand::Parameter;
		return Result::Continue;
	}

	return Result::Error;
}

Result ParseCommand::Parameter(char current, char next)
{
	Buffer += current;

	if (EndOfString)
	{
		ParseOut.push_back(Buffer);
		Buffer = "";

		return Result::End;
	}
	else if ((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z'))
	{
		Func = &ParseCommand::Parameter;
		return Result::Continue;
	}
	else if (next == ':')
	{
		ParseOut.push_back(Buffer);
		Buffer = "";

		Func = &ParseCommand::ParameterEnd;
		return Result::Continue;
	}

	return Result::Error;
}

Result ParseCommand::ParameterEnd(char current, char next)
{
	if (EndOfString)
	{
		return Result::Error;
	}
	else
	{
		Func = &ParseCommand::ParameterValue;
		return Result::Continue;
	}

	return Result::Error;
}

Result ParseCommand::ParameterValue(char current, char next)
{
	Buffer += current;

	if (EndOfString)
	{
		ParseOut.push_back(Buffer);
		Buffer = "";

		return Result::End;
	}
	else if (next == ' ')
	{
		ParseOut.push_back(Buffer);
		Buffer = "";

		Func = &ParseCommand::Whitespace;
		return Result::Continue;
	}
	else
	{
		Func = &ParseCommand::ParameterValue;
		return Result::Continue;
	}

	return Result::Error;
}


CommandInterpretor::CommandInterpretor(DnD::DnDProgram *program)
{
	Program = program;
}

void CommandInterpretor::Interpret(std::string command)
{
	Result result;
	std::vector<std::string> parsedCommand = CommandParser.Parse(command, result);

	if (result != Result::Error)
	{
		if (parsedCommand[0] == "exit") { parsedCommand.erase(parsedCommand.begin()); Exit(parsedCommand); }
		else if (parsedCommand[0] == "help") { parsedCommand.erase(parsedCommand.begin()); Help(parsedCommand); }
		else if (parsedCommand[0] == "name") { parsedCommand.erase(parsedCommand.begin()); Name(parsedCommand); }
		else if (parsedCommand[0] == "redraw") { parsedCommand.erase(parsedCommand.begin()); Redraw(parsedCommand); }
		else if (parsedCommand[0] == "save") { parsedCommand.erase(parsedCommand.begin()); Save(parsedCommand); }
		else if (parsedCommand[0] == "load") { parsedCommand.erase(parsedCommand.begin()); Load(parsedCommand); }
		else if (parsedCommand[0] == "net") { parsedCommand.erase(parsedCommand.begin()); Net(parsedCommand); }
		else if (parsedCommand[0] == "remove") { parsedCommand.erase(parsedCommand.begin()); Remove(parsedCommand); }
		else
		{
			Program->ConsoleWin.CommandLine->Write("COMMAND ERROR::Unrecognized Command\n");
		}
	}
	else
	{
		Program->ConsoleWin.CommandLine->Write("COMMAND ERROR::Parse Error\n");
	}
}

//Commands
void CommandInterpretor::Exit(std::vector<std::string> parameter)
{
	//std::vector<std::string> *parameters = (std::vector<std::string> *)parameter;
	Program->ConsoleWin.WindowPtr->State.Quit = true;
}

void CommandInterpretor::Help(std::vector<std::string> parameter)
{
	if (parameter.size() == 0)
	{
		Program->ConsoleWin.CommandLine->Write("Commands:\n\
		exit\n\
		help\n\n\
		name\n\
		redraw\n\
		save\n\
		load\n\
		net\n\
		remove\n");
	}
	else
	{
		Program->ConsoleWin.CommandLine->Write("COMMAND ERROR::Invalid Parameters\n");
	}
}

void CommandInterpretor::Name(std::vector<std::string> parameter)
{
	Result result;
	int count = 1;
	for (int i = 0; i < parameter.size(); i += 2)
	{
		if (parameter[i] == "c" || parameter[i] == "count")
		{
			count = stoi(parameter[i + 1]);
		}
		else
		{
			Program->ConsoleWin.CommandLine->Write("COMMAND ERROR::Parameter Error\n"); result = Result::Error;
		}
	}
	if (result != Result::Error)
	{
		for (int i = 0; i < count; i++)
		{
			std::string text(rand() % 6 + 4 + 1, '\n');
			for (int j = 0; j < text.size() - 1; j++)
			{
				text[j] = rand() % ('z' - 'a') + 'a';
			}
			Program->ConsoleWin.CommandLine->Write(text);
		}
	}
}

void CommandInterpretor::Redraw(std::vector<std::string> parameter)
{
	Program->GraphicWin.WindowPtr->AddMessage(MessageData({ WL_UPDATESURFACE, 0, 0 }), true);
}

void CommandInterpretor::Save(std::vector<std::string> parameter)
{
	if (parameter[0] == "f")
	{
		Program->Save(parameter[1]);
	}
	else
	{
		Program->ConsoleWin.CommandLine->Write("COMMAND ERROR::No File Specified\n");
	}
}

void CommandInterpretor::Load(std::vector<std::string> parameter)
{
	if (parameter[0] == "f")
	{
		Program->Load(parameter[0]);
	}
	else
	{
		Program->ConsoleWin.CommandLine->Write("COMMAND ERROR::No File Specified\n");
	}
}

void CommandInterpretor::Net(std::vector<std::string> parameter)
{
	for (int i = 0; i < parameter.size(); i+=2)
	{
		if (parameter[i] == "i")
		{
			Program->Network->NetIP = parameter[i+1];
		}
		else if (parameter[i] == "p")
		{
			Program->Network->NetPort = parameter[i+1];
		}
		else if (parameter[i] == "start")
		{
			Program->Network->Initialize();
		}
		else if (parameter[i] == "stop")
		{
			Program->Network->~NetworkManager();
		}
	}
	if (!parameter.size())
	{
		Program->ConsoleWin.CommandLine->Write("IP:" + Program->Network->NetIP + "\nPort:" + Program->Network->NetPort + "\nStatus:" + (Program->Network->Run ? "Up" : "Down") + "\n");
	}
}

void CommandInterpretor::Remove(std::vector<std::string> parameter)
{
	Program->GraphicWin.GameWorld->RemovePlayer(parameter[0]);
}





