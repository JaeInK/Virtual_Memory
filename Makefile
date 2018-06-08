simulator: Process.h main.cpp Process.cpp
	g++ -o simulator main.cpp Process.cpp

clean:
	rm simulator
