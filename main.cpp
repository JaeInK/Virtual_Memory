#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

int main( int argc, const char* argv[] )
{
	FILE *fp;
	int EventNum, timeQuantum, vmSize, pmSize, pageSize, feedFreq, feedSize;
	char charline[100];

	if(fp = fopen("input","r"))
	{
		fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n", &EventNum, &timeQuantum, &vmSize, &pmSize, &pageSize, &feedFreq, &feedSize);
		
		while(fgets(charline, 100, fp))
		{
			cout<<charline[];
			string line(charline);
			cout<<line;
		}

		fclose(fp);
	}
	//ifstream openFile("input");
	//if(openFile.is_open())
	//{
	//	string line;
	//	while(getline(openFile, line))
	//	{
	//		cout<<line<<endl;
	//	}
	//	openFile.close();
	//}

	return 0;
}
