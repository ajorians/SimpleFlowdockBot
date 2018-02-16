#pragma once

#include <string>
#include <vector>
#include <map>

class FlowMessage
{
public:
   FlowMessage(int nMessageId, const std::string& strMessage, const std::string& strUserName, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags);
   FlowMessage(int nMessageId, const std::string& strUserName);

   void SetMessage(const std::string& strMessage);
   std::string GetMessage() const;

   void AddTag(const std::string& strTag);
   void RemoveTag(const std::string& strTag);

   void AddEmoji(const std::string& strEmoji);
   void RemoveEmoji(const std::string& strEmoji);

   void AddComment(const std::string& strComment);

   int GetId() const;

protected:
   int m_nMessageId;
   std::string m_strText;
   std::string m_strUser;
   std::vector<std::string> m_astrComments;
   std::vector<std::string> m_astrTags;
   std::map<std::string, int> m_mapEmojis;
};

