#ifndef FLOWHANDLER_H
#define FLOWHANDLER_H

#ifdef USE_PTHREADS
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#else
#include <thread>
#endif
#include <string>
#include <fstream>
#include "FlowdockAPI.h"

#include "FlowRespondings.h"
#include "FlowThread.h"
#include "EmojiReactionAdder.h"

class FlowHandler
{
public:
   FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, const std::string& strGithubToken, int nFlowRespondingsFlags = RESPONDINGS_ALL);
   ~FlowHandler();

protected:

   static void* HandleThread(void* ptr);
   static void Listen_Callback(FlowdockMessage message, void* pUserData);
   void HandleMessages(const std::string& strMessage, int nUserID, int nThreadId, int nMessageID, const std::vector<std::string> astrAddedTags, const std::vector<std::string> astrRemovedTags);
   void HandleMessageEdit(const std::string& strMessage, int nUserId, int nThreadId, int nMessageID);
   void HandleTag(int nUserID, int nThreadId, int nMessageId, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags);
   void HandleComment(int nUserId, int nThreadId, int nMessageId, const std::string& strComment);
   void HandleEmoji(int nUserId, int nThreadId, int nMessageId, const std::string& strEmoji, bool bAdded);

   void AddEmojiReaction(const std::string& strMessage, int nUserID, int nThreadId, int nMessageID, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags);

   FlowThread* GetFlowThread(int nThreadId);
   FlowThread* CreateFlowThread(int nThreadId);

   std::string GetUserNameWithRetry(int nUserId);

   void PrintInformation(const std::string& strText);

protected:
   FlowdockAPI m_pFlowdock;
   std::string m_strOrg;
   std::string m_strFlow;
   std::string m_strUsername;
   std::string m_strPassword;
   std::string m_strGithubToken;

   std::ofstream m_Out;

   //This is a limitation so it isn't too chatty :)
   int m_SaysRemaining;

   bool m_bExit;

   int m_nFlowRespondingsFlags;

   std::vector<FlowThread> m_arrFlowThreads;

   EmojiReactionAdder m_emojiReactionAdder;

#ifdef USE_PTHREADS
   pthread_t m_thread;
#else
   std::thread m_thread;
#endif
};

#endif
