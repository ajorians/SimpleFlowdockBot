#ifndef FLOWAPILIBRARY_H
#define FLOWAPILIBRARY_H

#include <string>
#include "Library.h"
#include "FlowdockAPI.h"

class FlowAPILibrary
{
public:
   static FlowAPILibrary& instance();

   bool Create(FlowdockAPI* ppFlow);
   bool Destroy(FlowdockAPI* ppFlow);
   bool SetDefaults(FlowdockAPI pFlow, const std::string& strUsername, const std::string& strPassword);
   bool GetUserList(FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword);
   bool StartListening(FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword);
   bool Say(FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nThreadID, const std::string& strMessage, const std::string& strTags, const std::string& strName = "Build_bot");
   bool Tag( FlowdockAPI pFlow, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nThreadID, const std::string& strTags );
   std::string Listen(FlowdockAPI pFlow, std::string& strUser, int& nThreadID);

protected:
   RLibrary m_lib;

private:
   FlowAPILibrary();
   ~FlowAPILibrary();
};

#endif
