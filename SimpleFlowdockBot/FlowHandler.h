#ifndef FLOWHANDLER_H
#define FLOWHANDLER_H

#ifdef USE_PTHREADS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#else
#include <thread>
#endif
#include <string>
#include "FlowdockAPI.h"

#include "FlowRespondings.h"

class FlowHandler
{
public:
   FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nFlowRespondingsFlags = RESPONDINGS_ALL);
   ~FlowHandler();

protected:

   static void* HandleThread(void* ptr);
   static void Listen_Callback(FlowMessage message, void* pUserData);
   void HandleMessages(const std::string& strMessage, int nUserID, int nThreadId);

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

#ifdef USE_PTHREADS
   pthread_t m_thread;
#else
   std::thread m_thread;
#endif
};

#endif
