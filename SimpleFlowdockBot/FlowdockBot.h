#ifndef FLOWDOCKBOT_H
#define FLOWDOCKBOT_H

#include <string>
#include <vector>

#include "FlowRespondings.h"

class FlowHandler;

class FlowdockBot
{
public:
   ~FlowdockBot();

   void AddFlow(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nReplyingFlags = RESPONDINGS_ALL);

protected:
   std::vector<FlowHandler*> m_apHandlers;
};

#endif
