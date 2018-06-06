#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <deque>

#include "Process.h"

using namespace std;

int EventNum, timeQuantum, vmSize, pmSize, pageSize, feedFreq, feedSize;
int vmPage, pmPage;

void memoryAllocation(int PageNum, Process RunningProcess);
void memoryAccess(Process RunningProcess);
void memoryRelease();
deque<int> buddySlice;

int main( int argc, const char* argv[] )
{
	bool terminate=false;
	char charline[100];
	string line;
	string line_array[3];
	int cycle=0;
	deque<Process> runQueue;
	bool running=false;
	Process runningProcess;
	int timeLimit=0;
	int processId=0;
	int feedLimit=0;

	FILE *fp;
	fp = fopen("input","r");
	fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &EventNum, &timeQuantum, &vmSize, &pmSize, &pageSize, &feedFreq, &feedSize);
	vmPage = vmSize/pageSize;
	pmPage = pmSize/pageSize;
	fclose(fp);

	FILE *fw = fopen("scheduler.txt", "w");
	ifstream infile("input");
	getline(infile, line);
	line="";
	
	while(!terminate)
	{
		cout<<"CYCLE"<<cycle<<endl;
		//line is empty means that inputs' instruction is processed so you need to read next instruction
		if(line.empty())
		{
			getline(infile, line);
			cout<<"LINE"<<line<<endl;	
			cout.flush();
			//split line with blank using istringstream
			istringstream iss(line);
			string word;
			for(int i=0;i<3;i++)
			{
				iss>>word;
				line_array[i]=word;
			}
		}
		
		//feed cpuCycle at feed Freuquency
		if(feedLimit==feedFreq)
		{
			cout<<"FEED TIME!"<<endl;
			for(int i=0; i<runQueue.size(); i++)
			{
				runQueue[i].addCpuCycle(feedSize);
			}
			feedLimit=0;
		}

	 	//check if this cycle is included in inputs' instruction
		stringstream ss;
		ss<<cycle;
		if(line_array[0]==ss.str())
		{
			if(line_array[1]=="INPUT")
			{
			}
			else
			{
				Process proc(feedSize, line_array[1]);
				cout<<"PUSH CODENAME"<<line_array[1]<<endl;
				runQueue.push_back(proc);
			}
			line="";
		}

	 	//get a process from run_queue if there is no running process
		if(runQueue.size()!=0 && !running)
		{
			runningProcess =  runQueue.front();
			runQueue.pop_front();
			if(runningProcess.getPid()==-1)//first time to run
			{
				runningProcess.setPid(processId);
				processId++;
			}
			int emptyProcess=0;
			while(runningProcess.getCpuCycle()==0)//if the picked process doesn't have remaining cpu cycle
			{
				runQueue.push_back(runningProcess);
				emptyProcess++;
				if(emptyProcess == runQueue.size())//when All of remaining process don't have cpu cycle
				{
					for(int i=0; i<runQueue.size(); i++)
					{
						runQueue[i].addCpuCycle(feedSize);
					}
					feedLimit=0;
				}
				runningProcess = runQueue.front();
				runQueue.pop_front();
			}
			cout<<"POP PROCESS"<<runningProcess.getCodeName()<<endl;				
			running = true;
			fprintf(fw, "%d\t%d\t%s\n", cycle, runningProcess.getPid(), runningProcess.getCodeName().c_str());
		}

		//keep running if there is running process
		if(running)
		{
			cout<<"RUNNING CODE"<<runningProcess.getCodeName()<<"\n"<<endl;
			vector<int> instruction = runningProcess.commandArray[runningProcess.currentLine];
			if(instruction[0]==0)
			{
				memoryAllocation(instruction[1], runningProcess);
			}
			else if(instruction[0]==1)
			{
				memoryAccess(runningProcess);
			}
			else if(instruction[0]==2)
			{
				memoryRelease();
			}
			else if(instruction[0]==3)
			{
				//nothing?
			}
			else if(instruction[0]==4)//sleep
			{
			}
			else if(instruction[0]==5)//IOWait
			{
			}
			timeLimit++;
			runningProcess.currentLine++;
			//what if currentLine == commandNum-1? 언제 터미네이트
			//unable to keep running
			if(runningProcess.getCpuCycle()==0 || timeLimit==timeQuantum)
			{
				cout<<"CANT KEEP RUNNING"<<runningProcess.getCodeName()<<endl;
			 	running = false;
			 	timeLimit = 0;
				runQueue.push_back(runningProcess);
			}
		}
		cycle++;//이게 여기 있는게 맞을까 안으로 넣어야 하지 않을까?
		feedLimit++;

		if(cycle==15)
		{
			terminate=true;
		}
	}
	
	return 0;
}

void memoryAllocation(int pageNum, Process RunningProcess)
{
	//계산할때는 페이지로 나눠서 계산하고 이제 write할때는 다시 곱해서 곗ㅑ
	//deque<int> buddyMemory;
	//int start=0;
	//if(buddySlice.size()>0)
	//{
	//	buddyMemory.push
	//	for(int i=0; i<buddySlice.size(); i++)
	//	{
	//		if(i=0)
	//		{
	//			buddyMemory.push(buddySlice[0]);
	//		}
	//		else if(i=buddySlice.size()-1)
	//		{
	//			buddyMemory.push(pmPage-buddySlice[i]);
	//		}
	//		buddyMemory.push(buddyslice[i] - buddyslice[i-1])
	//	}
	//}
	//if vmPage>pageNum
	//	Memory/2;
	//check memory;//deque에서 전에거랑 현재 인덱스 빼면 공간이네..이중에서 가장 좁은거 뽑자.
}

void memoryAccess(Process RunningProcess)
{
	//LRU 갱신
}

void memoryRelease()
{
}
