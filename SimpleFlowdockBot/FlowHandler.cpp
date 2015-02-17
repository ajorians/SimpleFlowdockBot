#include "FlowHandler.h"
#include "FlowAPILibrary.h"

#include "LinkFixer.h"
#include "ScreencastLink.h"
#include "VSID.h"
#include <exception>

FlowHandler::FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword)
   : m_pFlowdock(NULL), m_strOrg(strOrg), m_strFlow(strFlow), m_strUsername(strUsername), m_strPassword(strPassword),
   m_SaysRemaining(10)
{
   FlowAPILibrary::instance().Create(&m_pFlowdock);

   //Important such that I don't see my messages as new messages :)
   if( !FlowAPILibrary::instance().SetDefaults(m_pFlowdock, m_strUsername, m_strPassword) )
      throw std::runtime_error("Unable to set defaults username/password");

   if( !FlowAPILibrary::instance().GetUserList(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword) )
      throw std::runtime_error("Failed to get user's list");

   FlowAPILibrary::instance().StartListening(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword);
}

FlowHandler::~FlowHandler()
{
   FlowAPILibrary::instance().Destroy(&m_pFlowdock);
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
   }

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

