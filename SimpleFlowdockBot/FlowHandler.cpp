#include "FlowHandler.h"
#include "FlowAPILibrary.h"

#include "LinkFixer.h"
#include "ScreencastLink.h"
#include "PullRequestTitle.h"
#include "VSID.h"
#include <exception>
#include <stdexcept>

#ifndef WIN32
#include <string.h>//?? TODO: Find out why including this?
#include <unistd.h>//For usleep
#endif

#ifdef WIN32
#include <windows.h>//For Sleep
#endif

FlowHandler::FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword)
   : m_pFlowdock(NULL), m_strOrg(strOrg), m_strFlow(strFlow), m_strUsername(strUsername), m_strPassword(strPassword),
   m_SaysRemaining(10), m_bExit(false)
{
   FlowAPILibrary::instance().Create(&m_pFlowdock);

   //Important such that I don't see my messages as new messages :)
   if( !FlowAPILibrary::instance().SetDefaults(m_pFlowdock, m_strUsername, m_strPassword) )
      throw std::runtime_error("Unable to set defaults username/password");

   if( !FlowAPILibrary::instance().GetUserList(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword) )
      throw std::runtime_error("Failed to get user's list");

   FlowAPILibrary::instance().StartListening(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword);

   m_thread = pthread_self();
   int iRet = pthread_create( &m_thread, NULL, FlowHandler::HandleThread, (void*)this);
}

FlowHandler::~FlowHandler()
{
   m_bExit = true;
   pthread_join( m_thread, NULL);
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
   std::string strMessage = FlowAPILibrary::instance().Listen(m_pFlowdock, nThreadID);

   if( strMessage.empty() )
      return;

   if( m_SaysRemaining<= 0 )
      return;

   bool bSaidSomething = false;

   //if message see if anything to say
   {
      ScreencastLinkHandler sc;
      if( sc.HasSCLink(strMessage) )
      {
         std::vector<std::string> astrLinks = sc.SCLinksFromMessage(strMessage);
         for(std::vector<std::string>::size_type i=0; i<astrLinks.size(); i++)
         {
            std::string strImg = sc.GetImageURL(astrLinks[i]);
            FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strImg, "");
            bSaidSomething = true;
         }
      }
   }

   if( !bSaidSomething )
   {
      PullRequestTitleHandler pr;
      if( pr.HasPR(strMessage) )
      {
         std::vector<std::string> astrPRs = pr.PRsFromMessage(strMessage);
         for(std::vector<std::string>::size_type i=0; i<astrPRs.size(); i++)
         {
            std::string strTitle = pr.GetPRTitle(astrPRs[i]);
            FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strTitle, "PR-Title");
            bSaidSomething = true;
         }
      }
   }

   /*if( !bSaidSomething )
   {
      if( VSIDHandler::HasVSID(strMessage) )
      {
         VSIDHandler vsid;
         std::vector<int> arrVSIDs = vsid.VSIDsFromMessage(strMessage);
         for(std::vector<int>::size_type i=0; i<arrVSIDs.size(); i++ )
         {
            std::string strResponse = vsid.GetResponseForVSID(arrVSIDs[i]);

            if( strResponse.length() > 0 )
            {
               FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strResponse, "VSID");
               bSaidSomething = true;
            }
         }
      }
   }*/

   if( !bSaidSomething )
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

