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

using namespace std;

int main(int argc, char *argv[])
{
   cout << "Starting up FlowdockBot" << endl;

   FlowdockBot bot;
   bot.AddFlow("aj-org",      "main",              "ajorians@gmail.com",      "1Smajjmd");
   bot.AddFlow("techsmith",   "camtasia-windows",  "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "cory-test",         "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "development",       "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "snagit-windows",    "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "ux-overhaul",       "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "c-p-i",             "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "shared-library",    "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "git-initiative",    "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "main",              "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "smartplayer",       "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "identity",          "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "uxoverhaul-camtasia-win","a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "screencast-pro-desktop","a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "macdevroom",         "a.orians@techsmith.com",  "1Smajjmd");
   bot.AddFlow("techsmith",   "camtasia-mac",       "a.orians@techsmith.com",  "1Smajjmd");

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

