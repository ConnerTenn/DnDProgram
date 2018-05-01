
enum Result;
class Parser;

#pragma once
//#include <iostream>
#include <string>
#include <vector>

#define chrstr(s) std::string(1, s)

enum Result
{
	End,
	Error,
	Continue
};

class Parser
{
protected:
	std::string String;
	int Index = 0;

public:
	std::vector<std::string> ParseOut;
	bool EndOfString = false;

public:

	std::vector<std::string> Parse(std::string in, Result &result)
	{
		ParseOut.clear();
		EndOfString = false;
		Index = 0;
		String = in;

		result = Start(String[0]);
		while (result == Result::Continue)
		{
			result = Call(String[Index], String[Index + 1]);
			if (result != Result::Error)
			{
				Index++;
				if (Index + 1 >= String.length())
				{
					EndOfString = true;
				}
				else
				{
					EndOfString = false;
				}


				if (Index >= String.length())
				{
					result = Result::End;
				}
			}
		}

		if (result == Result::End)
		{
			result = End();
		}

		/*if (result == Error)
		{
			//std::cout << "Parse Error\n";
			//throw (0);
		}*/

		//result == Result::End ? true : false;

		return ParseOut;
	}

	std::vector<std::string> Parse(std::string in)
	{
		Result result;
		String = "";
		ParseOut.clear();
		EndOfString = false;
		return Parse(in, result);
	}

	virtual Result Call(char current, char next)
	{
		return Result::End;
	}

	virtual Result Start(char next)
	{
		return Result::End;
	}

	virtual Result End()
	{
		return Result::End;
	}
};
