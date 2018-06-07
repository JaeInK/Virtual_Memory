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

deque<int> buddySlice;
deque<int> getBuddyMemory(deque<int> BuddySlice);
void memoryAllocation(int PageNum, Process RunningProcess);
void memoryAccess(int AllocationId, Process RunningProcess);
void memoryRelease(int AllocationId, Process RunningProcess);
deque< deque<int> > allocatedFrame;
deque< deque<int> > LRU;

deque<Process> SleepList;
deque<Process> IOWaitList;

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
	buddySlice.push_back(0);
	buddySlice.push_back(pmPage);
	deque<int> allo;
	for(int i=0; i<pmPage; i++)//if there is no page allocated in frame. it will be [-1,-1]
	{
		allo.push_back(-1);
		allo.push_back(-1);
		allocatedFrame.push_back(allo);
	}

	FILE *fw = fopen("scheduler.txt", "w");
	FILE *sy = fopen("system.txt", "w");
	FILE *me = fopen("memory.txt", "w");
	ifstream infile("input");
	getline(infile, line);
	line="";
	
	while(!terminate)
	{
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
			//실행중인거 어떻게 주지? sleep 이랑 IO일때는 흠 runningProcess 안바꿔줬는데 
			//여기서 슬립이랑 IO도 줘야하고 런큐도 못돌릴때도 다 줘야해ㅣ
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
		stringstream ss, pp;
		ss<<cycle;
		if(line_array[0]==ss.str())
		{
			if(line_array[1]=="INPUT")//IO Call
			{
				for(int i=0; i<IOWaitList.size(); i++)
				{
					pp<<IOWaitList[i].pid;
					if(pp.str() == line_array[2])
					{
						runQueue.push_back(IOWaitList[i]);
						IOWaitList.erase(IOWaitList.begin()+i);
					}
				}
			}
			else //Produce Process
			{
				Process proc(feedSize, line_array[1], vmPage);
				proc.pid = processId;
				processId++;
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
			fprintf(fw, "%d\t%d\t%s\n", cycle, runningProcess.pid, runningProcess.getCodeName().c_str());
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
		for(int i=0; i<LRU.size(); i++)
		{
		  fprintf(sy, " (%d:%d)", LRU[i][0], LRU[i][1]);
		}
		fprintf(sy, "\n");
		fprintf(sy, "\n");
		/////////////////Print system.txt Finished

		//Run Process
		if(running)
		{
			vector<int> instruction = runningProcess.commandArray[runningProcess.currentLine];
			if(instruction[0]==0)
			{
				memoryAllocation(instruction[1], runningProcess);
			}
			else if(instruction[0]==1)
			{
				memoryAccess(instruction[1], runningProcess);
			}
			else if(instruction[0]==2)
			{
				memoryRelease(instruction[1], runningProcess);
			}
			else if(instruction[0]==3)
			{
				//nothing?
			}
			else if(instruction[0]==4)//sleep
			{
				runningProcess.sleepTime = 0;
				runningProcess.sleepLimit = instruction[1];
				SleepList.push_back(runningProcess);
				running=false;
				//다음번에 러닝에서 픽이 확실히 안되게 해야한다.
			}
			else if(instruction[0]==5)//IOWait
			{
				IOWaitList.push_back(runningProcess);
				running=false;
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

		if(cycle==30)//terminate 조건은 모든 러닝. 그리고 리스트 들이 비어있고. 또한 인스트럭션이 안남아 있을때!
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

void memoryAllocation(int pageNum, Process RunningProcess)
{
	//계산할때는 페이지로 나눠서 계산하고 이제 write할때는 다시 곱해서 곗ㅑ
	deque<int> buddyMemory = getBuddyMemory(buddySlice);
	cout<<"BUDDY"<<endl;
	for(int i=0; i<buddySlice.size();i++)
	{
		cout<<buddySlice[i];
	}
	cout<<endl;
	for(int i=0; i<buddyMemory.size();i++)
	{
		cout<<buddyMemory[i];
	}
	cout<<"END"<<endl;
	int smallestMemory=pmSize+1;//맨처음 슬라이스 아예 없을때에도 괜찮은지 확인 필요 맨처음 슬라이스 일때도 들어갈수
	// //있는건지 확인해야 하는데...
	int smallestIndex=-1;
	
	// //smallestIndex=-1이라는 건 들어갈 자리가 없다는 거다. LRU 이용해야해.
	// //줄일 필요 없이 그대로 넣으면?
	while(smallestMemory>=2*pageNum)//while 잘돌아가게 만들어야 하는데
	{//프린트로계속 중간중간 확	
		cout<<"AAAAAAAAAAAAa"<<endl;
		buddySlice.insert(buddySlice.begin()+smallestIndex+1, buddySlice[smallestIndex+1]-buddySlice[smallestIndex]);//여기 지금 마이너스.
		cout<<"BBBBBBBBBBB"<<endl;
		buddyMemory = getBuddyMemory(buddySlice);
		smallestMemory = pmSize;
		smallestIndex = 0;
	// 	for(int i=0; i<buddyMemory.size(); i++)
	// 	{
	// 		if(buddyMemory[i]<smallestMemory && buddyMemory[i]>pageNum && allocatedFrame[buddySlice[i]][0]==-1)//그리고 다른 페이지가 들어가 있으면 안된다. 추가로
	// 			//슬라이스 생겨도 차있는지 확인할수 있어야 한다.usage 덱을 하나 더 만들어?0,1 불 들어가 있는걸로?
	// 			//아니면 시작 프레임이 페이지에 저장되어 있으면 그걸로 확인하면 되나?
	// 		{
	// 			smallestMemory = buddyMemory[i];
	// 			smallestIndex=i;
	// 		}
	// 	}
	// 	//어느 인덱스에 넣어야 하지?
	// 	//만약 4배보다 같거나 크면?
	}

	// if(smallestIndex==-1)//there is no place to put pages
	// {
	// 	//LRU 이용 빼기
	// }// Line 6

	//그다음에 자리 할당. allocatedFrame까지 수정

	

	//다 정한 다음에야 페이지 테이블 정해야지.

	//만약 들어갈 공간 없으면 LRUdlsep..
	//if vmPage>pageNum
	//	Memory/2;
	//check memory;//deque에서 전에거랑 현재 인덱스 빼면 공간이네..이중에서 가장 좁은거 뽑자.
}

void memoryAccess(int AllocationId, Process RunningProcess)
{
	//LRU 갱신
	deque<int> tmp;
	tmp.push_back(RunningProcess.pid);
	tmp.push_back(AllocationId);
	LRU.push_back(tmp);
}

void memoryRelease(int AllocationId, Process RunningProcess)
{
	int address;
	for(int i=0; i<pmPage; i++)
	{
		if(allocatedFrame[i][0] == RunningProcess.pid && allocatedFrame[i][1] == AllocationId)
		{
			address = i;
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
		allocatedFrame[i][0]=-1;
	}


	//Merge Buddy System
	int buddyAddress;
	int buddyLength;
	bool buddyFront;
	while(1)
	{
		if(address%(length*2)==0)
		{
			//만약 합칠수없으면 바로 브레이크 합칠수 있으면 합치고 index랑 length 바꾸기. 슬라이스도 같이 수정하기
			buddyAddress = address+length;
			buddyFront = false;
		}
		else if(address%(length*2)==length)
		{
			buddyAddress = address-length;
			buddyFront = true;
			//아니면 index를 여기서 바꿔도 괜찮나?
		}
		for(int i=0; i<buddySlice.size(); i++)
		{
			if(buddySlice[i]==buddyAddress)
			{
				buddyLength = buddySlice[i+1]-buddySlice[i];
				if(buddyLength==length)//able to merge
				{
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
				else//unable to merge
				{
					return;//return 괜찮나? while만 빠져나오는거 알면 좋은데
				}
			}
		}
	}
}
