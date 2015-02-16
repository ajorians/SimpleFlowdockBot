#include "FlowHandler.h"
#include "FlowAPILibrary.h"

#include "LinkFixer.h"
#include "ScreencastLink.h"
#include "VSID.h"

FlowHandler::FlowHandler(const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword)
   : m_pFlowdock(NULL), m_strOrg(strOrg), m_strFlow(strFlow), m_strUsername(strUsername), m_strPassword(strPassword)
{
   FlowAPILibrary::instance().Create(&m_pFlowdock);
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

   //if message see if anything to say
   {
      std::vector<std::string> arrstrLinks = LinkFixerHandler::LinksFromMessage(strMessage);
      for(std::vector<std::string>::size_type i=0; i<arrstrLinks.size(); i++)
         FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, LinkFixerHandler::GetCorrectedLink(arrstrLinks[i]), "linkfixer");
   }

   {
      ScreencastLinkHandler sc;
      if( sc.HasSCLink(strMessage) )
      {
         std::vector<std::string> astrLinks = sc.SCLinksFromMessage(strMessage);
         for(std::vector<std::string>::size_type i=0; i<astrLinks.size(); i++)
         {
            std::string strImg = sc.GetImageURL(astrLinks[i]);
            FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strImg, "sc.com");
         }
      }
   }

   {
      if( VSIDHandler::HasVSID(strMessage) )
      {
         VSIDHandler vsid;
         std::vector<int> arrVSIDs = vsid.VSIDsFromMessage(strMessage);
         for(std::vector<int>::size_type i=0; i<arrVSIDs.size(); i++ )
         {
            std::string strResponse = vsid.GetResponseForVSID(arrVSIDs[i]);

            if( strResponse.length() > 0 )
               FlowAPILibrary::instance().Say(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nThreadID, strResponse, "VSID");
         }
      }
   }

}

