#ifndef _PROCESS_H_
#define _PROCESS_H_

class Process
{
	private:
		int commandNum;
		int cpuCycle;
		string codeName;

	public:
		Process();
		Process(int _cpuCycle, string _codeName);
		void getCommandNum();
		void getCodeName();
};

#endif
