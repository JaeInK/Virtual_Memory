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
		int pid;
		std::string codeName;

	public:
		Process();
		Process(int _cpuCycle, std::string _codeName, int _pid);
		int getCommandNum();
		std::string getCodeName();
		int getCpuCycle();
		int getPid();
		void reduceCpuCycle();
		void setPid(int _pid);
};

#endif
