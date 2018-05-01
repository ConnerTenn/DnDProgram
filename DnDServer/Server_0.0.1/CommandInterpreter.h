
class ParseCommand;
class CommandInterpretor;

#pragma once

//#include <thread>
//#include <iostream>
#include  "DnDProgram.h"
#include "Parser.h"

class ParseCommand : public Parser
{

public:
	std::string Buffer;

public:
	Result(ParseCommand::*Func)(char current, char next);

	ParseCommand();

	Result Call(char current, char next);
	Result Start(char next);
	Result End();

	Result Command(char current, char next);
	Result Whitespace(char current, char next);
	Result Parameter(char current, char next);
	Result ParameterEnd(char current, char next);
	Result ParameterValue(char current, char next);
};

class CommandInterpretor
{
public:
	DnDProgram *Program;
	ParseCommand CommandParser;

	CommandInterpretor(DnDProgram *program);

	void Interpret(std::string command);

	//Commands
	void Exit(std::vector<std::string> parameters);
	void Help(std::vector<std::string> parameters);

	
};