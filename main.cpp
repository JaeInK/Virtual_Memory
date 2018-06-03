#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include "Process.h"
#include <vector>

using namespace std;

int main( int argc, const char* argv[] )
{
	FILE *fp;
	int EventNum, timeQuantum, vmSize, pmSize, pageSize, feedFreq, feedSize;
	bool terminate=false;
	char charline[100];
	string line;
	string line_array[3];
	int cycle=0;
	vector<Process> runQueue;
	bool running=false;
	Process *runningProcess;
	int timeLimit=0;
	int processId=0;

	if(fp = fopen("input","r"))
	{
		fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &EventNum, &timeQuantum, &vmSize, &pmSize, &pageSize, &feedFreq, &feedSize);
		while(getchar() != '\n');
		FILE *fw = fopen("scheduler.txt", "w");
	
		while(!terminate)
		{
			//line is empty means that inputs' instruction is processed so you need to read next instruction
			if(line.empty())
			{
				cout<<"a";
				if(fgets(charline,100,fp))
				{
					cout<<"b";
            		char *p;
            		if(p=strchr(charline, '\n'))
					{
               			*p = 0;
           			} 
					else 
					{   
						scanf("%*[^\n]");scanf("%*c");//clear upto newline
            			
					}
					line = string(charline);
					cout<<line;	
				}
				//split line with blank using istringstream
				istringstream iss(line);
				string word;
				for(int i=0;i<3;i++)
				{
					iss>>word;
					line_array[i]=word;
				}
			}

		// 	//check if this cycle is included in inputs' instruction
			stringstream ss;
			ss<<cycle;
			if(line_array[0]==ss.str())
			{
				if(line_array[1]=="INPUT")
				{
				}
				else
				{
					cout<<line_array[0];
					cout<<line_array[1];
					Process proc(feedSize, line_array[1]);
					runQueue.push_back(proc);
				}

				line="";
			}

		// 	//get a process from run_queue if there is no running process
			if(runQueue.size()!=0 && !running)
			{
				Process pop =  runQueue[0];
				runQueue.erase(runQueue.begin());					
				runningProcess = &pop;
				running = true;
				//if picked one doesnt have cpu cycle anmore?????
				if(runningProcess->getPid()==-1)//this process is picked first time
				{
					runningProcess->setPid(processId);
					processId++;
				}
			}

		// 	//keep running if there is running process
			if(running)
			{
				runningProcess->reduceCpuCycle();
				timeLimit++;
				// fprintf(fw, "%d\t%d\t%s\n", cycle, runningProcess->getPid(), runningProcess->getCodeName().c_str());

				// //unable to keep running
				// if(runningProcess->getCpuCycle()==0 || timeLimit==timeQuantum)
				// {
				// 	running = false;
				// 	timeLimit = 0;
				// }
			}

			cycle++;
		}
		// fclose(fp);
		// fclose(fw);
	}

	return 0;
}
