#include "FlowMessage.h"

#include <algorithm>
#include <cassert>

FlowMessage::FlowMessage(int nMessageId, const std::string& strMessage, const std::string& strUserName, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags)
   : m_nMessageId( nMessageId ),
   m_strText( strMessage ),
   m_strUser( strUserName ),
   m_astrTags( astrAddedTags )
{
   //Shouldn't be any removed tags??
   assert(astrRemovedTags.empty());
}

FlowMessage::FlowMessage(int nMessageId, const std::string& strUserName)
   : m_nMessageId( nMessageId ),
   m_strUser( strUserName )
{

}

void FlowMessage::SetMessage(const std::string& strMessage)
{
   m_strText = strMessage;
}

std::string FlowMessage::GetMessage() const
{
   return m_strText;
}

void FlowMessage::AddTag(const std::string& strTag)
{
   m_astrTags.push_back(strTag);
}

void FlowMessage::RemoveTag(const std::string& strTag)
{
   std::vector<std::string>::iterator it = std::find(m_astrTags.begin(), m_astrTags.end(), strTag);
   if (it != m_astrTags.end())
   {
      m_astrTags.erase(it);
   }
}

void FlowMessage::AddEmoji(const std::string& strEmoji)
{
   m_mapEmojis[strEmoji] = m_mapEmojis[strEmoji] + 1;
}

void FlowMessage::RemoveEmoji(const std::string& strEmoji)
{
   m_mapEmojis[strEmoji] = m_mapEmojis[strEmoji] - 1;
}

void FlowMessage::AddComment(const std::string& strComment)
{
   m_astrComments.push_back(strComment);
}

int FlowMessage::GetId() const
{
   return m_nMessageId;
}

