#include "Process.h"

using namespace std;

Process::Process()
{
}

Process::Process(int _cpuCycle, string _codeName)
{
	cpuCycle = _cpuCycle;
	codeName = _codeName;
}

int Process::getCommandNum()
{
}

int Process::getCodeName()
{
}

int Process::getCpuCycle()
{
	return cpuCycle;
}

void Process::reduceCpuCycle()
{
	cpuCycle--;
}

