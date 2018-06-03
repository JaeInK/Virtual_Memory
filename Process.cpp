#include "Process.h"

using namespace std;

Process::Process()
{
}

Process::Process(int _cpuCycle, string _codeName, int _pid)
{
	cpuCycle = _cpuCycle;
	codeName = _codeName;
	pid = _pid;
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

void Process::reduceCpuCycle()
{
	cpuCycle--;
}

void Process::setPid(int _pid)
{
	pid = _pid;
}
