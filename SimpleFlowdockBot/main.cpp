#include <iostream>
#include <string>
#include <fstream>

#ifndef WIN32
#include <string.h>//?? TODO: Find out why including this?
#include <unistd.h>//For usleep
#endif

#ifdef WIN32
#include <windows.h>//For Sleep
#endif

#include "FlowdockBot.h"
#include "FlowRespondings.h"

using namespace std;

int main(int argc, char *argv[])
{
   cout << "Starting up FlowdockBot" << endl;

   FlowdockBot bot;

   int n = 0;
   while(true)
   {
      n++;
      n = n%10;
      cout << n;
#ifdef _WIN32
      Sleep(1000);//1 second
#else
      usleep(1000*1000);
#endif
   }

   cout << "Exiting FlowdockBot" << endl;

	return 0;
}

