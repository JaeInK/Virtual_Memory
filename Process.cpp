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

	//make commandNum and commandArray
	string line;
	ifstream inFile(codeName);
	getline(inFile, line);
	stringstream SS(line);
	SS>>commandNum;
	cout<<"Making"<<codeName<<"COMMANDNUM"<<commandNum<<endl;
	if(commandNum<100)
	{
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
		cout<<"COMMANDARRAY"<<commandArray[i][0]<<commandArray[i][1]<<endl;
	}
	}
}

int Process::getCommandNum()
{
}

string Process::getCodeName()
{
	return codeName;
}

int Process::getPid()
{
	return pid;
}

int Process::getCpuCycle()
{
	return cpuCycle;
}

void Process::addCpuCycle(int i)
{
	cpuCycle += i;
}

void Process::reduceCpuCycle()
{
	cpuCycle--;
}

void Process::setPid(int _pid)
{
	pid = _pid;
}
