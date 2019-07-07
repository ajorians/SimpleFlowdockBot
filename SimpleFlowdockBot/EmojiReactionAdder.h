#ifndef EMOJIREACTIONADDER_H
#define EMOJIREACTIONADDER_H

#include "FlowdockAPI.h"

#include <string>
#include <vector>

class EmojiReactionAdder
{
public:
    EmojiReactionAdder(FlowdockAPI pFlowdock, const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword);

    void MessageSaid(const std::string& strMessage, int nUserID, int nThreadId, int nMessageID, const std::vector<std::string> astrAddedTags, const std::vector<std::string> astrRemovedTags );

protected:
    FlowdockAPI m_pFlowdock;
    std::string m_strOrg;
    std::string m_strFlow;
    std::string m_strUsername;
    std::string m_strPassword;
};

#endif