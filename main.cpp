#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <sstream>
#include <deque>
#include <algorithm>

#include "Process.h"

using namespace std;

int EventNum, timeQuantum, vmSize, pmSize, pageSize, feedFreq, feedSize;
int vmPage, pmPage;
int cycle=0;

deque<int> buddySlice;
deque<int> getBuddyMemory(deque<int> BuddySlice);
void memoryAllocation(int PageNum, Process &RunningProcess);
void memoryAccess(int AllocationId, Process &RunningProcess);
void memoryRelease(int AllocationId, Process &RunningProcess, bool lru);
deque< deque<int> > allocatedFrame;
deque<Process> LRUProcess;
deque<int> LRUAllocationID;

deque<Process> SleepList;
deque<Process> IOWaitList;

deque< deque< deque<int> > > pageTable;

FILE *me;

int main( int argc, const char* argv[] )
{
	bool terminate=false;
	char charline[100];
	string line;
	string line_array[3];
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
	buddySlice.push_back(0);
	buddySlice.push_back(pmPage);
	deque<int> allo;
	for(int i=0; i<pmPage; i++)//if there is no page allocated in frame. it will be [-1,-1]
	{
		allo.push_back(-1);
		allo.push_back(-1);
		allocatedFrame.push_back(allo);
	}

	ifstream INfile("input");
	getline(INfile, line);
	int processNumber = 0;
	for(int k=0; k<EventNum;k++)
	{
		getline(INfile, line);
		istringstream iss(line);
		string word;
		for(int i=0;i<3;i++)
		{
			iss>>word;
			line_array[i]=word;
		}
		if(line_array[1]!="INPUT")
		{
			processNumber++;
		}
	}
	vector<FILE *> files;

	//making files array for pid.txt and Process' pageTable
	for(int k=0; k<processNumber; k++)
	{
		stringstream kk;
		kk<<k;
		FILE *tmpFile = fopen(kk.str().append(".txt").c_str(), "w");
		files.push_back(tmpFile);

		deque<int> _page;
		deque< deque<int> > _Table;
		for(int i=0; i<vmPage; i++)
		{
			_page.push_back(-1);//AllocationId
			_page.push_back(0);//ValidBit
			_Table.push_back(_page);
		}
		pageTable.push_back(_Table);
	}

	FILE *fw = fopen("scheduler.txt", "w");
	FILE *sy = fopen("system.txt", "w");
	me = fopen("memory.txt", "w");
	ifstream infile("input");
	getline(infile, line);
	line="";
	
	while(!terminate)
	{
		//line is empty means that inputs' instruction is processed so you need to read next instruction
		if(line.empty())
		{
			getline(infile, line);
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
			for(int i=0; i<runQueue.size(); i++)
			{
				runQueue[i].cpuCycle += feedSize;
			}
			if(running)
			{
				runningProcess.cpuCycle += feedSize;
			}
			for(int i=0; i<SleepList.size(); i++)
			{
				SleepList[i].cpuCycle += feedSize;
			}
			for(int i=0; i<IOWaitList.size(); i++)
			{
				IOWaitList[i].cpuCycle += feedSize;
			}
		}

		//check SleepList
		for(int i=0; i<SleepList.size(); i++)
		{
			SleepList[i].sleepTime++;
		}
		for(int i=0; i<SleepList.size(); i++)
		{
			if(SleepList[i].sleepTime == SleepList[i].sleepLimit)
			{
				runQueue.push_back(SleepList[i]);
				SleepList.erase(SleepList.begin()+i);
			}
		}

	 	//check if this cycle is included in inputs' instruction
		stringstream ss;
		ss<<cycle;
		if(line_array[0]==ss.str())
		{
			if(line_array[1]=="INPUT")//IO Call
			{
				for(int i=0; i<IOWaitList.size(); i++)
				{
					stringstream pp;
					pp<<IOWaitList[i].pid;
					if(pp.str() == line_array[2])
					{
						runQueue.push_back(IOWaitList[i]);
						IOWaitList.erase(IOWaitList.begin()+i);
					}
				}
			}
			else //create Process
			{
				Process proc(feedSize, line_array[1]);
				proc.pid = processId;
				processId++;
				runQueue.push_back(proc);
			}
			line="";
		}

	 	//get a process from run_queue if there is no running process
		if(runQueue.size()!=0 && !running)
		{
			runningProcess =  runQueue.front();
			runQueue.pop_front();
			int emptyProcess=0;
			while(runningProcess.cpuCycle==0)//if the picked process doesn't have remaining cpu cycle
			{
				runQueue.push_back(runningProcess);
				emptyProcess++;
				if(emptyProcess == runQueue.size())//when All of remaining process don't have cpu cycle
				{//feed cpuCycle
					for(int i=0; i<runQueue.size(); i++)
					{
						runQueue[i].cpuCycle += feedSize;
					}
					if(running)
					{
						runningProcess.cpuCycle += feedSize;
					}
					for(int i=0; i<SleepList.size(); i++)
					{
						SleepList[i].cpuCycle += feedSize;
					}
					for(int i=0; i<IOWaitList.size(); i++)
					{
						IOWaitList[i].cpuCycle += feedSize;
					}
					feedLimit=0;
				}
				runningProcess = runQueue.front();
				runQueue.pop_front();	
			}			
			running = true;
			fprintf(fw, "%d\t%d\t%s\n", cycle, runningProcess.pid, runningProcess.codeName.c_str());
		}

		/////////////////Print system.txt
		fprintf(sy, "%d Cycle: ", cycle);
		if(running)
		{
			vector<int> instruction = runningProcess.commandArray[runningProcess.currentLine];
			fprintf(sy, "Process#%d running code %s line %d(op %d, arg %d)\n", runningProcess.pid, runningProcess.codeName.c_str(), runningProcess.currentLine+1, instruction[0], instruction[1]);
		}	
		else
		{
    	fprintf(sy, "\n");
		}
		fprintf(sy, "RunQueue: ");
		if(runQueue.size()==0)
		{
		    fprintf(sy, "Empty\n");
		}
		else
		{
		    for(int i=0; i<runQueue.size(); i++)
				{
		      fprintf(sy, "%d(%s) ", runQueue[i].pid, runQueue[i].codeName.c_str());
				}
				fprintf(sy, "\n");
		}
		fprintf(sy, "SleepList: ");
		if(SleepList.size()==0)
		{
    	fprintf(sy, "Empty\n");
		}
		else
		{
    	for(int i=0; i<SleepList.size(); i++)
			{
        fprintf(sy, "%d(%s) ", SleepList[i].pid, SleepList[i].codeName.c_str());
			}
			fprintf(sy, "\n");
		}
		fprintf(sy, "IOWait List: ");
		if(IOWaitList.size()==0)
		{
    	fprintf(sy, "Empty\n");
		}
		else
		{
    	for(int i=0; i<IOWaitList.size(); i++)
			{
        fprintf(sy, "%d(%s) ", IOWaitList[i].pid, IOWaitList[i].codeName.c_str());
			}
			fprintf(sy, "\n");
		}
		fprintf(sy, "|");
		for(int i=0; i<allocatedFrame.size(); i++) 
		{
    	char sep = ' ';
			for(int j=1; j<buddySlice.size(); j++)
			{
				if(i==buddySlice[j]-1)
				{
					sep='|';
				}
			}
    	if(allocatedFrame[i][0]==-1) 
			{
        fprintf(sy, "---%c", sep);
    	} 
			else 
			{
        fprintf(sy, "%d#%d%c", allocatedFrame[i][0], allocatedFrame[i][1], sep);
    	}
		}
		fprintf(sy, "\n");
		fprintf(sy, "LRU:");
		for(int i=0; i<LRUProcess.size(); i++)
		{
		  fprintf(sy, " (%d:%d)", LRUProcess[i].pid, LRUAllocationID[i]);
		}
		fprintf(sy, "\n");
		fprintf(sy, "\n");
		/////////////////Print system.txt Finished

		//Run Process
		if(running)
		{
			vector<int> instruction = runningProcess.commandArray[runningProcess.currentLine];
			runningProcess.currentLine++;
			runningProcess.cpuCycle--;
			if(instruction[0]==0)
			{
				memoryAllocation(instruction[1], runningProcess);
				runningProcess.allocatedNum++;
				timeLimit++;
			}
			else if(instruction[0]==1)
			{
				memoryAccess(instruction[1], runningProcess);
				timeLimit++;
			}
			else if(instruction[0]==2)
			{
				bool lru=false;
				memoryRelease(instruction[1], runningProcess, lru);
				timeLimit++;
			}
			else if(instruction[0]==3)
			{
				timeLimit++;
			}
			else if(instruction[0]==4)//sleep
			{
				if(runningProcess.currentLine != runningProcess.commandArray.size())
				{
					runningProcess.sleepTime = 0;
					runningProcess.sleepLimit = instruction[1];
					SleepList.push_back(runningProcess);
				}
				running=false;
				timeLimit = 0;
			}
			else if(instruction[0]==5)//IOWait
			{
				if(runningProcess.currentLine != runningProcess.commandArray.size())
				{
					IOWaitList.push_back(runningProcess);
				}
				running=false;
				timeLimit = 0;
			}

			////////Print Pid.txt
			fprintf(files[runningProcess.pid], "%d Cycle#Instruction op %d arg %d\n", cycle, instruction[0], instruction[1]);
			fprintf(files[runningProcess.pid], "AllocID");
			for(int i=0; i<pageTable[runningProcess.pid].size();i++) 
			{
			    if(i%4==0)
					{
			      fprintf(files[runningProcess.pid], "|");
					}
			    if(pageTable[runningProcess.pid][i][0] != -1)
					{
			      fprintf(files[runningProcess.pid], "%d", pageTable[runningProcess.pid][i][0]);
					}
			    else
			      fprintf(files[runningProcess.pid], "-");
			}
			fprintf(files[runningProcess.pid], "|\n");
			fprintf(files[runningProcess.pid], "Valid  ");
			for(int i=0; i<pageTable[runningProcess.pid].size();i++) 
			{
			    if(i%4==0)
					{
			      fprintf(files[runningProcess.pid], "|");
					}
			    fprintf(files[runningProcess.pid], "%d", pageTable[runningProcess.pid][i][1]);
			}
			fprintf(files[runningProcess.pid], "|\n");
			fprintf(files[runningProcess.pid], "\n");
			////////Print Pid.txt Finished

			//unable to keep running
			if((runningProcess.cpuCycle==0 || timeLimit==timeQuantum)&& instruction[0]!=4 && instruction[0]!=5)
			{
			 	running = false;
			 	timeLimit = 0;
				runQueue.push_back(runningProcess);
			}
			//End of process
			if(runningProcess.currentLine == runningProcess.commandArray.size())
			{
				//Memory Release Allocated process
				vector<int> Alloc_list;
				for(int k=0; k<allocatedFrame.size();k++)
				{
					if(allocatedFrame[k][0]==runningProcess.pid)
					{
						if(Alloc_list.size()!=0)
						{
							if(Alloc_list.back()!=allocatedFrame[k][1])
							{
								Alloc_list.push_back(allocatedFrame[k][1]);
							}
						}
						else
						{
							Alloc_list.push_back(allocatedFrame[k][1]);
						}
					}
				}
				sort(Alloc_list.begin(), Alloc_list.end());
				for(int k=0; k<Alloc_list.size();k++)
				{
					bool lru=true;
					memoryRelease(Alloc_list[k],runningProcess, lru);
				}
				//Release LRU
				for(int i=0; i<LRUProcess.size();i++)
				{
					if(LRUProcess[i].pid == runningProcess.pid)
					{
						LRUProcess.erase(LRUProcess.begin()+i);
						LRUAllocationID.erase(LRUAllocationID.begin()+i);
					}
				}
				running =false;
				timeLimit = 0;
			}
		}
		cycle++;
		feedLimit++;

		//Terminate when every lists are empty and there is no more instructions
		if(!running && runQueue.size()==0 &&SleepList.size()==0 && IOWaitList.size()==0 && line.empty())
		{
			terminate=true;
		}
	}
	
	return 0;
}

//with BuddySlice that indicates partition address, get BuddyMemory Size
deque<int> getBuddyMemory(deque<int> BuddySlice)
{
	deque<int> buddyMemory;
	for(int i=1; i<BuddySlice.size(); i++)
	{
		buddyMemory.push_back(BuddySlice[i]-BuddySlice[i-1]);
	}
	return buddyMemory;
}

void memoryAllocation(int pageNum, Process &RunningProcess)
{
	deque<int> buddyMemory = getBuddyMemory(buddySlice);
	int smallestMemory=pmSize+1;
	int smallestIndex=-1; // if this index remains -1 that means there is not place to allocate
	
	while(smallestIndex==-1)//it will keep Releasing Memory by LRU if there is no place to allocate.
	{
		int loop=1;
		while(smallestMemory>=2*pageNum)//keep slicing Memory into half when it's too big
		{
			buddyMemory = getBuddyMemory(buddySlice);
			if(loop!=1)
			{
				buddySlice.insert(buddySlice.begin()+smallestIndex+1, (buddySlice[smallestIndex+1]+buddySlice[smallestIndex])/2);//여기 지금 마이너스.;
				buddyMemory = getBuddyMemory(buddySlice);
				smallestMemory = pmSize;
				smallestIndex = 0;
			}
			for(int i=0; i<buddyMemory.size(); i++)
			{
				//check smallestMemory which is not used by other page
				if(buddyMemory[i]<smallestMemory && buddyMemory[i]>pageNum && allocatedFrame[buddySlice[i]][0]==-1)
				{
					smallestMemory = buddyMemory[i];
					smallestIndex=i;
				}
			}
			if(smallestIndex==-1)
			{
				break;
			}
			loop++;
		}
		if(smallestIndex==-1)//Releasing LRU's front process Memory
		{
			Process popProc = LRUProcess.front();
			LRUProcess.pop_front();
			int popAlloc = LRUAllocationID.front();
			LRUAllocationID.pop_front();
			bool lru=true;
			memoryRelease(popAlloc, popProc,lru);
		}
	}
	
	//After finding the place to allocate, change status(allocatedFrame, pageTable)
	for(int i=buddySlice[smallestIndex]; i<buddySlice[smallestIndex+1]; i++)
	{
		allocatedFrame[i][0] = RunningProcess.pid;
		allocatedFrame[i][1] = RunningProcess.allocatedNum;
	}
	//Update Virtual Memory Status
	bool ter =true;
	for(int i=0; i<pageTable[RunningProcess.pid].size();i++)
	{
		if(pageTable[RunningProcess.pid][i][0]==RunningProcess.allocatedNum)
		{
			pageTable[RunningProcess.pid][i][1]=1;
			ter = false;
		}
	}
	int num=0;
	for(int i=0; i<pageTable[RunningProcess.pid].size();i++)
	{
		if(pageTable[RunningProcess.pid][i][0]==-1)
		{
			if(ter)
			{
				pageTable[RunningProcess.pid][i][0] = RunningProcess.allocatedNum;
				pageTable[RunningProcess.pid][i][1] = 1;
			}
			num++;
		}
		if(num==pageNum)
		{
			break;
		}
	}

	//After Allocating Memory update LRU
	LRUProcess.push_back(RunningProcess);
	LRUAllocationID.push_back(RunningProcess.allocatedNum);

	fprintf(me, "%d\t%d\t%d\t%d\n", cycle, RunningProcess.pid, RunningProcess.allocatedNum, 0);
}

void memoryAccess(int AllocationId, Process &RunningProcess)
{
	//check if the accessing process exists
	bool exist = false;
	for(int i=0; i<allocatedFrame.size(); i++)
	{
		if(allocatedFrame[i][0]==RunningProcess.pid && allocatedFrame[i][1]==AllocationId)
		{
			exist=true;
			break;
		}
	}
	//if it doesn't, we should allocate memory again.
	if(!exist)
	{
		int Num=0;
		for(int j=0; j<pageTable[RunningProcess.pid].size();j++)
		{
			if(pageTable[RunningProcess.pid][j][0]==AllocationId)
			{
				Num++;
			}
		}
		int tmp = RunningProcess.allocatedNum;
		RunningProcess.allocatedNum = AllocationId;
		memoryAllocation(Num, RunningProcess);
		RunningProcess.allocatedNum = tmp;
	}
	//LRU Update
	for(int i=0; i<LRUProcess.size();i++)
	{
		if(LRUProcess[i].pid == RunningProcess.pid)
		{
			if(LRUAllocationID[i] == AllocationId)
			{
				LRUProcess.erase(LRUProcess.begin()+i);
				LRUAllocationID.erase(LRUAllocationID.begin()+i);
			}
		}
	}
	LRUAllocationID.push_back(AllocationId);
	LRUProcess.push_back(RunningProcess);

	fprintf(me, "%d\t%d\t%d\t%d\n", cycle, RunningProcess.pid, AllocationId, 1);
}

void memoryRelease(int AllocationId, Process &RunningProcess, bool lru)
{
	//first check the process' address and length.
	int address=-1;
	for(int i=0; i<pmPage; i++)
	{
		if(allocatedFrame[i][0] == RunningProcess.pid && allocatedFrame[i][1] == AllocationId)
		{
			address = i;
			break;
		}
	}
	int length;
	for(int i=0; i<buddySlice.size(); i++)
	{
		if(buddySlice[i]==address)
		{
			length = buddySlice[i+1]-buddySlice[i];
		}
	}

	//Release Physical Memory
	for(int i=address; i<address+length; i++)
	{
		allocatedFrame[i][0]=-1;
		allocatedFrame[i][1]=-1;
	}

	//Release Virtual Memory
	for(int i=0; i<pageTable[RunningProcess.pid].size();i++)
	{
		if(pageTable[RunningProcess.pid][i][0]==AllocationId)
		{
			if(!lru)
			{
				pageTable[RunningProcess.pid][i][0]=-1;
			}
			pageTable[RunningProcess.pid][i][1]=0;
		}
	}

	//Release LRU
	for(int i=0; i<LRUProcess.size();i++)
	{
		if(LRUProcess[i].pid == RunningProcess.pid)
		{
			if(LRUAllocationID[i] == AllocationId)
			{
				LRUProcess.erase(LRUProcess.begin()+i);
				LRUAllocationID.erase(LRUAllocationID.begin()+i);
			}
		}
	}

	//Merge Buddy System
	int buddyAddress;
	int buddyLength;
	bool buddyFront;
	while(1) //keep merging when it is possible
	{
		if(address%(length*2)==0)
		{
			buddyAddress = address+length;
			buddyFront = false;
		}
		else if(address%(length*2)==length)
		{
			buddyAddress = address-length;
			buddyFront = true;//buddy is front of the picked process
		}

		if(allocatedFrame[buddyAddress][0]!=-1)//unable to merge because it is currently filled with pages
		{
			break;
		}
		for(int i=0; i<buddySlice.size(); i++)
		{
			if(buddySlice[i]==buddyAddress)
			{
				buddyLength = buddySlice[i+1]-buddySlice[i];
				if(buddyLength==length)//able to merge
				{
					//update address and length to check if it can be merged more
					if(buddyFront)
					{
						buddySlice.erase(buddySlice.begin()+i+1);
						address = buddyAddress;
						length = 2*length;
					}
					else
					{
						buddySlice.erase(buddySlice.begin()+i);
						address = address;
						length = 2*length;
					}
				}
				else//unable to merge because it is not the same length
				{
					fprintf(me, "%d\t%d\t%d\t%d\n", cycle, RunningProcess.pid, AllocationId, 2);
					return;
				}
			}
		}
		if(buddySlice.size()==2)//it is merged as one!! nothing more.
		{
			fprintf(me, "%d\t%d\t%d\t%d\n", cycle, RunningProcess.pid, AllocationId, 2);
			return;
		}
	}
	fprintf(me, "%d\t%d\t%d\t%d\n", cycle, RunningProcess.pid, AllocationId, 2);
}
