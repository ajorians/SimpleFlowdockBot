#ifndef FLOWDOCKBOT_H
#define FLOWDOCKBOT_H

#include <string>
#include <vector>

class FlowHandler;

class FlowdockBot
{
public:
   ~FlowdockBot();

   void AddFlow(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword);
   void HandleMessages();

protected:
   std::vector<FlowHandler*> m_apHandlers;
};

#endif
