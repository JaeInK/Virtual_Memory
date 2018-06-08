#include "Process.h"
#include <fstream>
#include <sstream>

using namespace std;

Process::Process()
{
}

Process::Process(int _cpuCycle, string _codeName)
{
	cpuCycle = _cpuCycle;
	codeName = _codeName;
	currentLine = 0;
	pid = -1;
	allocatedNum = 0;

	//make commandNum and commandArray
	string line;
	ifstream inFile(codeName.c_str());
	getline(inFile, line);
	stringstream SS(line);
	SS>>commandNum;
	for(int i=0; i<commandNum; i++)
	{
		getline(inFile, line);
		stringstream ss(line);
		vector<int> tmp;
		int k;
		ss>>k;
		tmp.push_back(k);
		ss>>k;
		tmp.push_back(k);
		commandArray.push_back(tmp);
	}
}
