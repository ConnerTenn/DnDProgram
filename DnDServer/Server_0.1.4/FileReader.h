

class FileReader;

#pragma once

//#include <thread>
//#include <iostream>
#include "DnDProgram.h"
#include "World.h"
#include "Parser.h"

class FileReader : public Parser
{
private:
	enum State
	{
		None,
		Player,
		Environment,
		//EnvironmentPlayer,
		Layer,
		//LayerPlayer,
		LineWorldObject,
		EllipseWorldObject,
		Object,
		//ObjectPlayer,
	} InterpretState = None;

public:
	std::string Buffer;
	std::string Identifier;
	int Length = 0;
	char *Buf;
	DnD::DnDProgram *Program;
	int ParseState = 0;
	int TargetCount = 0;
	int Count = 0;

public:
	Result(FileReader::*Func)(char current, char next);

	FileReader(DnD::DnDProgram *program);

	Result Call(char current, char next);
	Result Start(char next);
	Result End();

	Result ParseIdentifier(char current, char next);
	//Result ParseSize(char current, char next);
	Result ParseDelimeter(char current, char next);
	Result ParseValue(char current, char next);

	//Result ParseSize();
	Result Execute();
	Result Execute2();
	//Result ParseText(char current, char next);
};


void LoadConfig(DnD::DnDProgram *program);

