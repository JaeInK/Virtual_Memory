#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <deque>

class Process
{
	private:
		int cpuCycle;

	public:
		Process();
		Process(int _cpuCycle, std::string _codeName, int _vmPage);
		int pid;
		std::string codeName;
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

		int allocatedNum;
		std::deque< std::deque<int> > pageTable;

		int sleepTime;
		int sleepLimit;
};

#endif
