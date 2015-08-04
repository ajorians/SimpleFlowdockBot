#ifndef FLOWHANDLER_H
#define FLOWHANDLER_H

#include <pthread.h>
#include <string>
#include "FlowdockAPI.h"

#include "FlowRespondings.h"

class FlowHandler
{
public:
   FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nFlowRespondingsFlags = RESPONDINGS_ALL);
   ~FlowHandler();

   static void* HandleThread(void* ptr);
   void HandleMessages();

protected:
   FlowdockAPI m_pFlowdock;
   std::string m_strOrg;
   std::string m_strFlow;
   std::string m_strUsername;
   std::string m_strPassword;

   //This is a limitation so it isn't too chatty :)
   int m_SaysRemaining;

   bool m_bExit;

   int m_nFlowRespondingsFlags;

   pthread_t m_thread;
};

#endif
