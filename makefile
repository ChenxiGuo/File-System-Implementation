myfsmake:  myApp.c functions.cpp disk.c
	g++ -std=c++11 myApp.c functions.cpp functions.h disk.c disk.h -o myfs
