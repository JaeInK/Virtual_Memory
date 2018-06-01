class Process
{
	private:
		int commandNum;
		int cpuCycle;
	public:
		Process();
		Process(int _commandNum, int _cpuCycle);
		void getCommandNum();
}
