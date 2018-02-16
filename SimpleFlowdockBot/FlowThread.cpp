#include "FlowThread.h"

FlowThread::FlowThread(int nThreadId)
   : m_nThreadId(nThreadId)
{

}

void FlowThread::AddMessage(const std::string& strMessage, int nMessageId, const std::string& strUserName, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags)
{
   FlowMessage message(nMessageId, strMessage, strUserName, astrAddedTags, astrRemovedTags);
   m_arrMessages.push_back(message);
}

void FlowThread::MessageEdit(int nMessageID, const std::string& strMessage, const std::string&strUserName)
{
   FlowMessage& message = GetMessageOrCreate(nMessageID, strUserName);
   message.SetMessage(strMessage);
}

void FlowThread::AddTag(const std::string& strUserName, int nMessageId, const std::string& strTag)
{
   FlowMessage& message = GetMessageOrCreate(nMessageId, strUserName);
   message.AddTag(strTag);
}

void FlowThread::RemoveTag(const std::string& strUserName, int nMessageId, const std::string& strTag)
{
   FlowMessage& message = GetMessageOrCreate(nMessageId, strUserName);
   message.RemoveTag(strTag);
}

void FlowThread::AddComment(const std::string& strUserName, int nMessageId, const std::string& strComment)
{
   FlowMessage& message = GetMessageOrCreate(nMessageId, strUserName);
   message.AddComment(strComment);
}

void FlowThread::AddEmoji(const std::string& strUserName, int nMessageId, const std::string& strEmoji)
{
   FlowMessage& message = GetMessageOrCreate(nMessageId, strUserName);
   message.AddEmoji(strEmoji);
}

void FlowThread::RemoveEmoji(const std::string& strUserName, int nMessageId, const std::string& strEmoji)
{
   FlowMessage& message = GetMessageOrCreate(nMessageId, strUserName);
   message.RemoveEmoji(strEmoji);
}

std::string FlowThread::GetMessage(int nMessageId) const
{
   for (int i = 0; i < m_arrMessages.size(); i++)
   {
      if (m_arrMessages[i].GetId() == nMessageId)
      {
         return m_arrMessages[i].GetMessage();
      }
   }

   return std::string();
}

int FlowThread::GetId() const
{
   return m_nThreadId;
}

FlowMessage& FlowThread::GetMessageOrCreate(int nMessageId, const std::string& strUserName)
{
   for (int i = 0; i < m_arrMessages.size(); i++)
   {
      if (m_arrMessages[i].GetId() == nMessageId)
      {
         return m_arrMessages[i];
      }
   }

   FlowMessage message(nMessageId, strUserName);
   m_arrMessages.push_back(message);
   return m_arrMessages.back();
}
