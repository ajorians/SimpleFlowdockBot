#include <iostream>
#include <string>
#include <fstream>

#include "FlowdockBot.h"

using namespace std;

int main(int argc, char *argv[])
{
   cout << "Starting up FlowdockBot" << endl;

   FlowdockBot bot;
   bot.AddFlow("aj-org", "main", "ajorians@gmail.com", "1Smajjmd");
   while(true)
   {
      bot.HandleMessages();
   }

   cout << "Exiting FlowdockBot" << endl;

	return 0;
}

