#include "FlowHandler.h"
#include "FlowAPILibrary.h"

#include "LinkFixer.h"
#include "ScreencastLink.h"
#include "PullRequestTitle.h"
#include "GitIssueTitle.h"
#include <exception>
#include <stdexcept>
#include <algorithm>

#ifndef WIN32
#include <string.h>//?? TODO: Find out why including this?
#include <unistd.h>//For usleep
#endif

#ifdef WIN32
#include <windows.h>//For Sleep
#endif

FlowHandler::FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nFlowRespondingsFlags /*= RESPONDINGS_ALL*/)
   : m_pFlowdock(NULL), m_strOrg(strOrg), m_strFlow(strFlow), m_strUsername(strUsername), m_strPassword(strPassword),
   m_SaysRemaining(40), m_bExit(false), m_nFlowRespondingsFlags(nFlowRespondingsFlags)
{
   FlowAPILibrary::instance().Create(&m_pFlowdock);

   //Important such that I don't see my messages as new messages :)
   if( !FlowAPILibrary::instance().SetDefaults(m_pFlowdock, m_strUsername, m_strPassword) )
      throw std::runtime_error("Unable to set defaults username/password");

   if( !FlowAPILibrary::instance().GetUserList(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword) )
      throw std::runtime_error("Failed to get user's list");

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
      pThis->HandleMessages();
#ifdef _WIN32
      Sleep(100);//1/10 second
#else
      usleep(100*1000);
#endif
   }

   return NULL;
}

void FlowHandler::HandleMessages()
{
   int nThreadID = 0;
   std::string strUserName;
   std::string strMessage = FlowAPILibrary::instance().Listen(m_pFlowdock, strUserName, nThreadID);

   if( strMessage.empty() )
      return;

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
               FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strImg, "");
               bSaidSomething = true;
            }
         }
      }
   }

   if( !bSaidSomething && (m_nFlowRespondingsFlags&PRTitles)==PRTitles )
   {
      PullRequestTitleHandler pr;
      if( pr.HasPR(strMessage) )
      {
         std::vector<std::string> astrPRs = pr.PRsFromMessage(strMessage);
         for(std::vector<std::string>::size_type i=0; i<astrPRs.size(); i++)
         {
            std::pair<std::string, std::string> pairTitleAndRepo
               = pr.GetPRTitleAndName(astrPRs[i]);

            std::string strMessage = "[" + pairTitleAndRepo.second + "]: " + pairTitleAndRepo.first;

            FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strMessage, "PR-Title");
            bSaidSomething = true;
         }
      }

      GitIssueTitleHandler issue;
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

            FlowAPILibrary::instance().Say( m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strMessage, "Issue-Title" );
            bSaidSomething = true;
         }
      }
   }

   if( !bSaidSomething && (m_nFlowRespondingsFlags&LinkFixing) == LinkFixing)
   {
      std::vector<std::string> arrstrLinks = LinkFixerHandler::LinksFromMessage(strMessage);
      for(std::vector<std::string>::size_type i=0; i<arrstrLinks.size(); i++)
      {
         std::string strCorrected = LinkFixerHandler::GetCorrectedLink(arrstrLinks[i]);
         if( strCorrected != arrstrLinks[i] )
         {
            FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strCorrected, "linkfixer");
            bSaidSomething = true;
         }
      }
   }

   if( bSaidSomething )
      m_SaysRemaining--;

}

