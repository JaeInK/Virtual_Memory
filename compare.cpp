#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
	ifstream a(argv[1]);
	ifstream b(argv[2]);
	int i=0;
	while (!a.eof())
	{
		string s1,s2;
		getline(a,s1);
		getline(b,s2);

		if(s1!=s2)
		{
			cout<<s1<<" "<<s2<<endl;
			cout<<i<<endl;
			return 0;
		}
		i++;
	}
}
