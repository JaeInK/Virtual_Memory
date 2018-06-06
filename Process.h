#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

class Process
{
	private:
		int cpuCycle;
		int pid;
		std::string codeName;

	public:
		Process();
		Process(int _cpuCycle, std::string _codeName);
		int commandNum;
		int currentLine;
		std::vector< std::vector<int> > commandArray;
		int getCommandNum();
		std::string getCodeName();
		int getCpuCycle();
		int getPid();
		void reduceCpuCycle();
		void addCpuCycle(int i);
		void setPid(int _pid);
		int getCurrentLine();
		void plusCurrentLine();
};

#endif
