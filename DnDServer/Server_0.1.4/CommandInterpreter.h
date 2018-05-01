
class ParseCommand;
class CommandInterpretor;

#pragma once

//#include <thread>
//#include <iostream>
#include "DnDProgram.h"
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
	DnD::DnDProgram *Program;
	ParseCommand CommandParser;

	CommandInterpretor(DnD::DnDProgram *program);

	void Interpret(std::string command);

	//Commands
	void Exit(std::vector<std::string> parameters);
	void Help(std::vector<std::string> parameters);
	void Name(std::vector<std::string> parameters);
	void Redraw(std::vector<std::string> parameters);
	void Save(std::vector<std::string> parameters);
	void Load(std::vector<std::string> parameters);
	void Net(std::vector<std::string> parameters); //start stop restart
	void Remove(std::vector<std::string> parameters);
	void Sync(std::vector<std::string> parameters);
	void Spawn(std::vector<std::string> parameters);
	void Center(std::vector<std::string> parameters);
};
