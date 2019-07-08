#include "FlowHandler.h"
#include "FlowAPILibrary.h"

#include "LinkFixer.h"
#include "ScreencastLink.h"
#include "PullRequestTitle.h"
#include "GitIssueTitle.h"
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cassert>

#ifndef WIN32
#include <string.h>//?? TODO: Find out why including this?
#include <unistd.h>//For usleep
#endif

#ifdef WIN32
#include <windows.h>//For Sleep
#ifdef GetMessage
#undef GetMessage
#endif
#endif

FlowHandler::FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, const std::string& strGithubToken, int nFlowRespondingsFlags /*= RESPONDINGS_ALL*/)
   : m_pFlowdock(NULL), m_strOrg(strOrg), m_strFlow(strFlow), m_strUsername(strUsername), m_strPassword(strPassword), m_strGithubToken( strGithubToken ),
   m_SaysRemaining(40), m_bExit(false), m_nFlowRespondingsFlags(nFlowRespondingsFlags), m_emojiReactionAdder(&m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword)
{
   FlowAPILibrary::instance().Create(&m_pFlowdock);

   std::string strFileName = strFlow + ".log";
   m_Out.open(strFileName.c_str(), std::ios::app);

   //Important such that I don't see my messages as new messages :)
   if( !FlowAPILibrary::instance().SetDefaults(m_pFlowdock, m_strUsername, m_strPassword) )
      throw std::runtime_error("Unable to set defaults username/password");

   if( !FlowAPILibrary::instance().GetUserList(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword) )
      throw std::runtime_error("Failed to get user's list");

   FlowAPILibrary::instance().AddListen(m_pFlowdock, Listen_Callback, this);

   FlowAPILibrary::instance().StartListening(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword);

#ifdef USE_PTHREADS
   m_thread = pthread_self();
   int iRet = pthread_create( &m_thread, NULL, FlowHandler::HandleThread, (void*)this);
#else
   std::thread t(FlowHandler::HandleThread, (void*)this);
   m_thread = std::move(t);
#endif
}

FlowHandler::~FlowHandler()
{
   m_bExit = true;
#ifdef USE_PTHREADS
   pthread_join( m_thread, NULL);
#else
   m_thread.join();
#endif
   FlowAPILibrary::instance().Destroy(&m_pFlowdock);
}

void* FlowHandler::HandleThread(void* ptr)
{
   FlowHandler* pThis = (FlowHandler*)ptr;
   while(!pThis->m_bExit)
   {
#ifdef _WIN32
      Sleep(100);//1/10 second
#else
      usleep(100*1000);
#endif
   }

   return NULL;
}

void FlowHandler::Listen_Callback(FlowdockMessage message, void* pUserData)
{
   FlowHandler* pThis = (FlowHandler*)pUserData;
   std::string strMessage(message.Message);//Or emoji
   std::vector<std::string> astrAddedTags;
   std::vector<std::string> astrRemovedTags;

   for (int i = 0; i < message.nAddedTags; i++)
   {
      std::string s(message.AddedTags[i]);
      astrAddedTags.push_back(s);
   }

   for (int i = 0; i < message.nRemovedTags; i++)
   {
      std::string s(message.RemovedTags[i]);
      astrRemovedTags.push_back(s);
   }

   switch (message.eEvent)
   {
   case Message:
      pThis->HandleMessages(strMessage, message.nUserId, message.nThreadId, message.nMessageId, astrAddedTags, astrRemovedTags);
      break;
   case MessageEdit:
      pThis->HandleMessageEdit(strMessage, message.nUserId, message.nThreadId, message.nMessageId);
      break;
   case Tag_Change:
      pThis->HandleTag(message.nUserId, message.nThreadId, message.nMessageId, astrAddedTags, astrRemovedTags);
      break;
   case Comment:
      pThis->HandleComment(message.nUserId, message.nThreadId, message.nMessageId, strMessage);
      break;
   case Emoji_Reaction:
      pThis->HandleEmoji(message.nUserId, message.nThreadId, message.nMessageId, strMessage, message.bAdded);
      break;
   }
}

void FlowHandler::HandleMessages(const std::string& strMessage, int nUserID, int nThreadID, int nMessageID, const std::vector<std::string> astrAddedTags, const std::vector<std::string> astrRemovedTags)
{
   if( strMessage.empty() )
      return;

   std::string strEMail;
   FlowAPILibrary::instance().GetUserEMail(m_pFlowdock, nUserID, strEMail);

   std::string strUserName = GetUserNameWithRetry(nUserID);

   FlowThread* pFlowThread = GetFlowThread(nThreadID);
   if (!pFlowThread)
   {
      pFlowThread = CreateFlowThread(nThreadID);
   }
   pFlowThread->AddMessage(strMessage, nMessageID, strUserName, astrAddedTags, astrRemovedTags);

   PrintInformation( std::string("Message from: ") + strUserName + std::string( " said \"" ) + strMessage + "\"" );

    AddEmojiReaction( strMessage, nUserID, nThreadID, nMessageID, astrAddedTags, astrRemovedTags);

   //Early return if seeing my message or ReviewBot
   if (strEMail == m_strUsername)
      return;

   std::string lowerCaseUserName(strUserName);
   std::transform(lowerCaseUserName.begin(), lowerCaseUserName.end(), lowerCaseUserName.begin(), ::tolower);
   if (strUserName == "a.orians" || strUserName == "reviewbot")
      return;

   if (nUserID == 311366) {//This is ReviewBot.  Gonna try this :)
      return;
   }

   if( m_SaysRemaining<= 0 )
      return;

   std::string lowerCaseMessage(strMessage);
   std::transform(lowerCaseMessage.begin(), lowerCaseMessage.end(), lowerCaseMessage.begin(), ::tolower);
   bool bPossibleReviewBot = lowerCaseMessage.find("reviewbot") != std::string::npos;
   if (bPossibleReviewBot)
      return;

   bool bSaidSomething = false;

   //if message see if anything to say
   {
      if ((m_nFlowRespondingsFlags & SCImages) == SCImages)
      {
         ScreencastLinkHandler sc;
         if (sc.HasSCLink(strMessage))
         {
            std::vector<std::string> astrLinks = sc.SCLinksFromMessage(strMessage);
            for (std::vector<std::string>::size_type i = 0; i < astrLinks.size(); i++)
            {
               std::string strImg = sc.GetImageURL(astrLinks[i]);
               FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nMessageID, strImg, "");
               bSaidSomething = true;
            }
         }
      }
   }

   if( !bSaidSomething && (m_nFlowRespondingsFlags&PRTitles)==PRTitles )
   {
      PullRequestTitleHandler pr( m_strGithubToken);
      if( pr.HasPR(strMessage) )
      {
         std::vector<std::string> astrPRs = pr.PRsFromMessage(strMessage);
         for(std::vector<std::string>::size_type i=0; i<astrPRs.size(); i++)
         {
            std::pair<std::string, std::string> pairTitleAndRepo
               = pr.GetPRTitleAndName(astrPRs[i]);

            std::string strMessage = "[" + pairTitleAndRepo.second + "]: " + pairTitleAndRepo.first;

            FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nMessageID, strMessage, "PR-Title");
            bSaidSomething = true;
         }
      }

      GitIssueTitleHandler issue( m_strGithubToken );
      if ( issue.HasIssue( strMessage ) )
      {
         std::vector<std::string> astrIssues = issue.IssuesFromMessage( strMessage );
         for ( std::vector<std::string>::size_type i = 0; i < astrIssues.size(); i++ )
         {
            std::pair<std::string, std::vector<std::string> > pairTitleAndLabels
               = issue.GetIssueTitleAndLabels( astrIssues[i] );

            std::string strMessage = pairTitleAndLabels.first;
            if ( false && pairTitleAndLabels.second.size() > 0 )
            {
               strMessage += "\n";
               for ( int i = 0; i < pairTitleAndLabels.second.size(); i++ )
               {
                  strMessage += "* " + pairTitleAndLabels.second[i];
                  if ( i != pairTitleAndLabels.second.size() - 1 )
                     strMessage += ", \n";
               }
               strMessage += "\n";
            }

            FlowAPILibrary::instance().Say( m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nMessageID, strMessage, "Issue-Title" );
            bSaidSomething = true;
         }
      }
   }

   if (!bSaidSomething && strMessage == "Hi Bots")
   {
      FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nMessageID, "Hi!  I'm build_bot! :smiley:", "greeting");
      bSaidSomething = true;
   }

   if( !bSaidSomething && (m_nFlowRespondingsFlags&LinkFixing) == LinkFixing)
   {
      std::vector<std::string> arrstrLinks = LinkFixerHandler::LinksFromMessage(strMessage);
      for(std::vector<std::string>::size_type i=0; i<arrstrLinks.size(); i++)
      {
         std::string strCorrected = LinkFixerHandler::GetCorrectedLink(arrstrLinks[i]);
         if( strCorrected != arrstrLinks[i] )
         {
            FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nMessageID, strCorrected, "linkfixer");
            bSaidSomething = true;
         }
      }
   }

   if( bSaidSomething )
      m_SaysRemaining--;
}

void FlowHandler::HandleMessageEdit(const std::string& strMessage, int nUserId, int nThreadId, int nMessageID)
{
   //Let's see if we can find that thread
   std::string strUserName = GetUserNameWithRetry(nUserId);

   PrintInformation( std::string("Edited message by: ") + strUserName );

   //Let's see if we can find that thread
   FlowThread* pFlowThread = GetFlowThread(nThreadId);
   if (!pFlowThread)
   {
      pFlowThread = CreateFlowThread(nThreadId);
   }

   std::string strOriginalMessage = pFlowThread->GetMessage(nMessageID);
   if (!strOriginalMessage.empty())
   {
      PrintInformation( std::string("Original text: \"") + strOriginalMessage + std::string( "\"" ) );
   }

   pFlowThread->MessageEdit(nMessageID, strMessage, strUserName);

   PrintInformation( std::string( "New text: \"") + strMessage + std::string( "\"" ) );
}

void FlowHandler::HandleTag(int nUserID, int nThreadId, int nMessageId, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags)
{
   std::string strUserName = GetUserNameWithRetry(nUserID);

   //Let's see if we can find that thread
   FlowThread* pFlowThread = GetFlowThread(nThreadId);
   if (!pFlowThread)
   {
      pFlowThread = CreateFlowThread(nThreadId);
   }

   for (int i = 0; i < astrAddedTags.size(); i++)
   {
      PrintInformation( std::string( "Tag added by: " ) + strUserName + std::string( " which is: \"" ) + astrAddedTags[i] + std::string( "\"" ) );
      pFlowThread->AddTag(strUserName, nMessageId, astrAddedTags[i]);
   }
   for (int i = 0; i < astrRemovedTags.size(); i++)
   {
      PrintInformation( std::string( "Tag removed by: ") + strUserName + std::string( " which is: \"" ) + astrRemovedTags[i] + std::string("\"" ) );
      pFlowThread->RemoveTag(strUserName, nMessageId, astrRemovedTags[i]);
   }
}

void FlowHandler::HandleComment(int nUserId, int nThreadId, int nMessageId, const std::string& strComment)
{
   std::string strUserName = GetUserNameWithRetry(nUserId);
}

void FlowHandler::HandleEmoji(int nUserId, int nThreadId, int nMessageId, const std::string& strEmoji, bool bAdded)
{
   std::string strUserName = GetUserNameWithRetry(nUserId);

   //Let's see if we can find that thread
   FlowThread* pFlowThread = GetFlowThread(nThreadId);
   if (!pFlowThread)
   {
      pFlowThread = CreateFlowThread(nThreadId);
   }

   if (bAdded)
   {
      PrintInformation( std::string( "Emoji added by: " ) + strUserName + std::string( " which is: \"" ) + strEmoji + std::string( "\"" ) );
      pFlowThread->AddEmoji(strUserName, nMessageId, strEmoji);
   }
   else
   {
      PrintInformation( std::string( "Emoji removed by: " ) + strUserName + std::string( " which is: \"" ) + strEmoji + std::string( "\"" ) );
      pFlowThread->RemoveEmoji(strUserName, nMessageId, strEmoji);
   }
}

void FlowHandler::AddEmojiReaction(const std::string& strMessage, int nUserID, int nThreadId, int nMessageID, const std::vector<std::string>& astrAddedTags, const std::vector<std::string>& astrRemovedTags)
{
    m_emojiReactionAdder.MessageSaid(strMessage, nUserID, nThreadId, nMessageID, astrAddedTags, astrRemovedTags);
}

FlowThread* FlowHandler::GetFlowThread(int nThreadId)
{
   for (int i = 0; i < m_arrFlowThreads.size(); i++)
   {
      if (m_arrFlowThreads[i].GetId() == nThreadId)
      {
         return &m_arrFlowThreads[i];
      }
   }

   return NULL;
}

FlowThread* FlowHandler::CreateFlowThread(int nThreadId)
{
   assert(GetFlowThread(nThreadId) == NULL);//Should have called GetFlowThread instead

   FlowThread flowThread(nThreadId);
   m_arrFlowThreads.push_back(flowThread);
   return GetFlowThread(nThreadId);
}

std::string FlowHandler::GetUserNameWithRetry(int nUserId)
{
   std::string strUserName;
   FlowAPILibrary::instance().GetUserName(m_pFlowdock, nUserId, strUserName);

   //If no username let's see if we can get it
   if (strUserName.empty())
   {
      FlowAPILibrary::instance().GetUserList(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword);
      FlowAPILibrary::instance().GetUserName(m_pFlowdock, nUserId, strUserName);
   }

   return strUserName;
}

void FlowHandler::PrintInformation(const std::string &strText)
{
   m_Out << strText << std::endl;
   std::cout << strText << std::endl;
}

