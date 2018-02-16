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
#include "FlowHandler.h"
#include "FlowRespondings.h"

using namespace std;

int main(int argc, char *argv[])
{
   cout << "Starting up FlowdockBot" << endl;
#if 1
   std::string strOrg, strFlow, strUser, strPassword;
   int nRespondings = RESPONDINGS_ALL;
   for (int i = 0; i < argc; i++)
   {
      std::string str = argv[i];
      if (str == "--org")
         strOrg = argv[i + 1];

      if (str == "--flow")
         strFlow = argv[i + 1];

      if (str == "--user")
         strUser = argv[i + 1];

      if (str == "--password")
         strPassword = argv[i + 1];

      if (str == "--responding")
         nRespondings = atoi(argv[i + 1]);
   }
   if (argc < 4 || strOrg.length() == 0 || strFlow.length() == 0 || strUser.length() == 0 || strPassword.length() == 0)
   {
      cout << "Usage: " << argv[0] << " --org aj-org --flow main --user ajorians@gmail.com --password abc123" << endl;
      return 0;
   }

   FlowHandler handler(strOrg, strFlow, strUser, strPassword, nRespondings);

   while (true)
   {
#ifdef _WIN32
      Sleep(1000);//1 second
#else
      usleep(1000 * 1000);
#endif
   }
#else

   FlowdockBot bot;
   bot.AddFlow("aj-org", "main", "ajorians@gmail.com", "1Smajjmd");

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
#endif

   cout << "Exiting FlowdockBot" << endl;

	return 0;
}

