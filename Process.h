#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdio.h>
#include <iostream>
#include <string>

class Process
{
	private:
		int commandNum;
		int cpuCycle;
		std::string codeName;

	public:
		Process();
		Process(int _cpuCycle, std::string _codeName);
		int getCommandNum();
		int getCodeName();
		int getCpuCycle();
		void reduceCpuCycle();
};

#endif
