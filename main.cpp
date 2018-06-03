#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
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

	if(fp = fopen("input","r"))
	{
		fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &EventNum, &timeQuantum, &vmSize, &pmSize, &pageSize, &feedFreq, &feedSize);
			
		while(!terminate)
		{
			cout<<"a";
			//line is empty means that inputs' instruction is processed so you need to read next instruction
			if(line.empty())
			{
				fgets(charline, 100, fp);
				line = string(charline);

				//split line with blank using istringstream
				istringstream iss(line);
				string word;
				for(int i=0;i<3;i++)
				{
					iss>>word;
					line_array[i]=word;
				}
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
					runQueue.push_back(proc);
				}

				line="";
			}

			//keep running if there is running process
			if(running)
			{
				runningProcess.cpuCylcle--;
				timeLimit++;

				//unable to keep running
				if(runningProcess.cpuCycle==0 || timeLimit==timeQuantum)
				{
					running = false;
					timeLimit = 0;
				}
			}

			//get a process from run_queue if there is no running process
			if(vector.size()!=0 && !running)
			{
				runningProcess = &poc
				running = true;
			}

			cycle++;
		}
		fclose(fp);
	}

	return 0;
}
