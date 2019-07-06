#include "FlowAPILibrary.h"
#include <cassert>

#ifdef __linux__ 
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#endif

FlowAPILibrary& FlowAPILibrary::instance()
{
   static FlowAPILibrary inst;
   return inst;
}

bool FlowAPILibrary::Create(FlowdockAPI* ppFlow)
{
   FlowdockCreateFunc CreateAPI = (FlowdockCreateFunc)m_lib.Resolve("FlowdockCreate");
   if( !CreateAPI )
      return false;

   CreateAPI(ppFlow, 0/*Not verbose*/);
   return true;
}

bool FlowAPILibrary::Destroy(FlowdockAPI* ppFlow)
{
   FlowdockFreeFunc FreeAPI = (FlowdockFreeFunc)m_lib.Resolve("FlowdockFree");
   if( !FreeAPI )
      return false;

   FreeAPI(ppFlow);
   return true;
}

bool FlowAPILibrary::SetDefaults(FlowdockAPI pFlow, const std::string& strUsername, const std::string& strPassword)
{
   FlowdockSetUsernamePasswordFunc SetDefaults = (FlowdockSetUsernamePasswordFunc)m_lib.Resolve("FlowdockSetUsernamePassword");
   if( !SetDefaults )
      return false;

   SetDefaults(pFlow, strUsername.c_str(), strPassword.c_str());
   return true;
}

bool FlowAPILibrary::GetUserList(FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword)
{
   FlowdockGetUsersFunc GetUsers = (FlowdockGetUsersFunc)m_lib.Resolve("FlowdockGetUsers");
   if( !GetUsers )
      return false;

   GetUsers(pFlow, strOrg.c_str(), strFlow.c_str(), strUsername.c_str(), strPassword.c_str());
   return true;
}

bool FlowAPILibrary::GetUserName(FlowdockAPI pFlow, int nUserId, std::string& strUserName)
{
   int nSizeOfNickName = 0;
   if (1 == FlowdockGetNicknameForUser(pFlow, nUserId, NULL, nSizeOfNickName))
   {
      char* pstrData = new char[nSizeOfNickName + 1];
      FlowdockGetNicknameForUser(pFlow, nUserId, pstrData, nSizeOfNickName);
      strUserName = std::string(pstrData);
      delete pstrData;
   }
   return true;
}

bool FlowAPILibrary::GetUserEMail(FlowdockAPI pFlow, int nUserId, std::string& strUserEMail)
{
   int nSizeOfEMail = 0;
   if (1 == FlowdockGetEMailForUser(pFlow, nUserId, NULL, nSizeOfEMail))
   {
      char* pstrData = new char[nSizeOfEMail + 1];
      FlowdockGetEMailForUser(pFlow, nUserId, pstrData, nSizeOfEMail);
      strUserEMail = std::string(pstrData);
      delete pstrData;
   }
   return true;
}

bool FlowAPILibrary::StartListening(FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword)
{
   FlowdockAddListenFlowFunc AddListenFlow = (FlowdockAddListenFlowFunc)m_lib.Resolve("FlowdockAddListenFlow");
   if( !AddListenFlow )
      return false;

   AddListenFlow(pFlow, strOrg.c_str(), strFlow.c_str());

   FlowdockStartListeningFunc StartListening = (FlowdockStartListeningFunc)m_lib.Resolve("FlowdockStartListening");
   if( !StartListening )
      return false;

   StartListening(pFlow, strUsername.c_str(), strPassword.c_str());
   return true;
}

bool FlowAPILibrary::Say(FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nThreadID, const std::string& strMessage, const std::string& strTags, const std::string& strName /*= "Build_bot"*/)
{
   FlowdockSayFunc Say = (FlowdockSayFunc)m_lib.Resolve("FlowdockSay");
   if( !Say )
      return false;

   Say(pFlow, strOrg.c_str(), strFlow.c_str(), strUsername.c_str(), strPassword.c_str(), strMessage.c_str(), nThreadID, strTags.c_str(), strName.c_str());
   return true;
}

bool FlowAPILibrary::Tag( FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nThreadID, const std::string& strTags )
{
   FlowdockTagFunc Tag = (FlowdockTagFunc)m_lib.Resolve( "FlowdockTag" );
   if ( !Tag )
      return false;

   Tag( pFlow, strOrg.c_str(), strFlow.c_str(), strUsername.c_str(), strPassword.c_str(), nThreadID, strTags.c_str() );
   return true;
}

bool FlowAPILibrary::AddEmojiReaction( FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nMessageID, const std::string& strEmojiReaction )
{
    FlowdockAddEmojiReactionFunc AddReaction = (FlowdockAddEmojiReactionFunc)m_lib.Resolve( "FlowdockAddEmojiReaction" );
    if ( !AddReaction )
        return false;

    AddReaction( pFlow, strOrg.c_str(), strFlow.c_str(), strUsername.c_str(), strPassword.c_str(), nMessageID, strEmojiReaction.c_str() );
    return true;
}

bool FlowAPILibrary::AddListen(FlowdockAPI pFlow, FlowMessageCallback cb, void* pUserData)
{
   FlowdockAddListenCallbackFunc AddListen = (FlowdockAddListenCallbackFunc)m_lib.Resolve("FlowdockAddListenCallback");
   if (!AddListen)
      return false;

   AddListen(pFlow, cb, pUserData);
   return true;
}

FlowAPILibrary::FlowAPILibrary()
{
   std::string strPath;
#ifdef WIN32
#ifdef _DEBUG
   strPath = "..\\..\\FlowdockAPI\\FlowdockAPI\\Debug\\FlowdockAPI-d.dll";
#else
   strPath = "..\\..\\FlowdockAPI\\FlowdockAPI\\Release\\FlowdockAPI.dll";
#endif
#else
   char path[PATH_MAX] = "/proc/self/exe";
   char dest[PATH_MAX];
   readlink(path, dest, PATH_MAX);
   std::string strEXE(dest);
   strEXE = strEXE.substr(0, strEXE.rfind('/'));//EXE folder
   strEXE = strEXE.substr(0, strEXE.rfind('/'));//Build folder

   strPath = strEXE + "/FlowdockAPI/FlowdockAPI/libFlowdockAPI.so";

#endif
   m_lib.SetLibrary(strPath);

   bool bOK = m_lib.Load();
   assert(bOK);

   //Load up functions
}

FlowAPILibrary::~FlowAPILibrary()
{

}
