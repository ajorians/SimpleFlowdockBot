#ifndef FLOWHANDLER_H
#define FLOWHANDLER_H

#include <string>
#include "FlowdockAPI.h"

class FlowHandler
{
public:
   FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword);
   ~FlowHandler();

   void HandleMessages();

protected:
   FlowdockAPI m_pFlowdock;
   std::string m_strOrg;
   std::string m_strFlow;
   std::string m_strUsername;
   std::string m_strPassword;
};

#endif
