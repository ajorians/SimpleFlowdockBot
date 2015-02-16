#ifndef FlowdockBOT_LINKFIXERHANDLER_H
#define FlowdockBOT_LINKFIXERHANDLER_H

#include <vector>
#include <string>

class LinkFixerHandler
{
public:
   static std::vector<std::string> LinksFromMessage(const std::string& strMessage);
   static bool HasLinks(const std::string& strMessage);
   static std::string GetCorrectedLink(const std::string& strLink);
};


#endif
