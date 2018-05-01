
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
		return Result::Error;
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


CommandInterpretor::CommandInterpretor(DnDProgram *program)
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
		else
		{
			Program->ConsoleWin.CommandLine->Write("WARNING::Unrecognized Command\n");
		}
	}
	else
	{
		Program->ConsoleWin.CommandLine->Write("WARNING::Unrecognized Command\n");
	}
}

//Commands
void CommandInterpretor::Exit(std::vector<std::string> parameter)
{
	//std::vector<std::string> *parameters = (std::vector<std::string> *)parameter;
	Program->ConsoleWin.Window->State.Quit = true;
}

