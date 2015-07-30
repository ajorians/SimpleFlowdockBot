#ifndef FlowdockBOT_TYJONESHANDLER_H
#define FlowdockBOT_TYJONESHANDLER_H

#include <string>
#include <vector>

class TyJones
{
public:
   static bool HasTYMessage(const std::string& strMessage);

   static std::string HandleMessage(const std::string& strMessage, const std::string& strUsername);
};

#endif
