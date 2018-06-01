#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

int main( int argc, const char* argv[] )
{
	FILE *fp;
	int EventNum, timeQuantum, vmSize, pmSize, pageSize, feedFreq, feedSize;
	char charline[100];
	bool terminate=false;
	string line="";

	if(fp = fopen("input","r"))
	{
		fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &EventNum, &timeQuantum, &vmSize, &pmSize, &pageSize, &feedFreq, &feedSize);
		
		
		while(!terminate)
		{
			if(line.empty())
			{
				fgets(charline, 100, fp);
				line = string(charline);
				line = line.substr(0, line.size()-1);
				fscanfo(line, 
			}

			//if line is used set it ""

		}
			
			
		while(fgets(charline, 100, fp))
		{
			string line(charline);
			line = line.substr(0, line.size()-1);
			cout<<line<<endl;
		}


		ㅌ
		fclose(fp);
	}

	return 0;
}
