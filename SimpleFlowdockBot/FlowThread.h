#pragma once

#include <vector>
#include <string>
#include "FlowMessage.h"

class FlowThread
{
public:
   FlowThread(int nThreadId);

   void AddMessage(const std::string& strMessage, int nMessageId, const std::string& strUserName, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags);
   void MessageEdit(int nMessageID, const std::string& strMessage, const std::string&strUserName);
   void AddTag(const std::string& strUserName, int nMessageId, const std::string& strTag);
   void RemoveTag(const std::string& strUserName, int nMessageId, const std::string& strTag);
   void AddComment(const std::string& strUserName, int nMessageId, const std::string& strComment);

   void AddEmoji(const std::string& strUserName, int nMessageId, const std::string& strEmoji);
   void RemoveEmoji(const std::string& strUserName, int nMessageId, const std::string& strEmoji);

   std::string GetMessage(int nMessageId) const;

   int GetId() const;

protected:
   FlowMessage& GetMessageOrCreate(int nMessageId, const std::string& strUserName);

protected:
   int m_nThreadId;
   std::vector<FlowMessage> m_arrMessages;
};
