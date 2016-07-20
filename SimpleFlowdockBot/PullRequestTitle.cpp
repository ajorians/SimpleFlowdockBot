#include "PullRequestTitle.h"
#include <sstream>
#include <algorithm>//For std::transform
#include <stdlib.h>
#include <cstring>

#include <iostream>
using namespace std;

namespace{
   int StringToInt(const std::string& str)
   {
      return atoi(str.c_str());
   }

   std::string IntToString(int nValue)
   {
      std::ostringstream oss;
      oss << nValue;

      return oss.str();
   }

   std::string ToLower(const std::string& strCommand)
   {
	   std::string strReturn(strCommand);
	   std::transform(strReturn.begin(), strReturn.end(), strReturn.begin(), ::tolower);
	   return strReturn;
   }

   void Replace(std::string& s, const std::string& strToReplace, const std::string& strToReplaceWith)
   {
      int nPos = 0;
      while(true)
      {
         nPos = s.find(strToReplace, nPos);
         if( nPos == std::string::npos )
            return;

         s.replace(nPos, strToReplace.length(), strToReplaceWith);
         nPos += strToReplaceWith.length();
      }
   }

   std::string UnEscapeXML(const std::string str)
   {
      std::string strReturn(str);
      Replace(strReturn, "&quot;", "\"");
      Replace(strReturn, "&amp;", "&");
      Replace(strReturn, "&lt;", "<");
      Replace(strReturn, "&gt;", ">");
      Replace(strReturn, "&apos;", "\'");
      return strReturn;
   }

   bool HasItem(const std::vector<std::string>& arrstr, const std::string& str)
   {
	   for(std::vector<std::string>::size_type i=0; i<arrstr.size(); i++)
		   if( arrstr[i] == str )
			   return true;

	   return false;
   }
}

PullRequestTitleHandler::PullRequestTitleHandler()
: m_pCurl(NULL), m_pCookies(NULL)
{
	//curl_global_init(CURL_GLOBAL_ALL);
	m_pCurl = curl_easy_init();

   curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, PullRequestTitleHandler::write_callback);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

   curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);

   curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(m_pCurl, CURLOPT_SSLENGINE_DEFAULT,1L);

   curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, "ajclient/0.0.1");
   curl_easy_setopt(m_pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
   std::string strUserPass = "ajorians:1Smajjmd";
   curl_easy_setopt(m_pCurl, CURLOPT_USERPWD, strUserPass.c_str());

	curl_easy_setopt(m_pCurl, CURLOPT_VERBOSE, 0L);
}

PullRequestTitleHandler::~PullRequestTitleHandler()
{
	curl_easy_cleanup(m_pCurl);
	//curl_global_cleanup();

	curl_slist_free_all(m_pCookies);
}

bool PullRequestTitleHandler::HasPR(const std::string& strMessage)
{
	return PullRequestTitleHandler::PRsFromMessage(strMessage).size() > 0;
}

std::vector<std::string> PullRequestTitleHandler::PRsFromMessage(const std::string& strMessage)
{
	std::vector<std::string> arrstrURLs;

	int nStart = 0;
	while(true)
	{
		int nStartShortID = strMessage.find("https://github.com/TechSmith/", nStart);
		if( nStartShortID == std::string::npos )
                {
                        //cout << "https://github.com/TechSmith in: " << strLower << endl;
			break;
                }

		nStart = nStartShortID + strlen("https://github.com/TechSmith/");

      std::string strRepo;
      int nEnd = nStart;
      while(nEnd < (int)strMessage.length() &&
         ((strMessage[nEnd] >= 'a' && strMessage[nEnd] <= 'z') || (strMessage[nEnd] >= 'A' && strMessage[nEnd] <= 'Z') || (strMessage[nEnd] >= '0' && strMessage[nEnd] <= '9') || strMessage[nEnd] == '-') )
      {
         strRepo += strMessage[nEnd];
         nEnd++;
      }

      if( nEnd >= (int)strMessage.length() || strMessage[nEnd] != '/' )
         continue;
      nEnd++;

      if( nEnd >= (int)strMessage.length() || strMessage[nEnd] != 'p' )//pull/
         continue;

      nEnd+=5;

      std::string strNumber;
      while(nEnd < (int)strMessage.length() &&
         (strMessage[nEnd] >= '0' && strMessage[nEnd] <= '9') )
      {
         strNumber += strMessage[nEnd];
         nEnd++;
      }

      if( strNumber.empty() )
         continue;

      std::string strURL = "https://api.github.com/repos/TechSmith/" + strRepo + "/pulls/" + strNumber;
      if( !HasItem(arrstrURLs, strURL) )
         arrstrURLs.push_back(strURL);
	}

	return arrstrURLs;
}

size_t PullRequestTitleHandler::write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	PullRequestTitleHandler* pHandler = (PullRequestTitleHandler*)stream;
	pHandler->m_strWrite.append((char*)ptr, nmemb);

	return nmemb;
}

std::pair<std::string, std::string> PullRequestTitleHandler::GetPRTitleAndName(const std::string& strURL)
{
   std::string strAddress = strURL;

   std::pair<std::string, std::string> pairRet;

   curl_easy_setopt(m_pCurl, CURLOPT_URL, strAddress.c_str());

   m_strWrite.clear();

   m_resLast = curl_easy_perform(m_pCurl);

   //Get the PR title
   {
      int nStart = m_strWrite.find("\"title\":");
      if( nStart == std::string::npos )
         return pairRet;

      nStart = nStart + strlen("\"title\":");

      nStart = m_strWrite.find("\"", nStart);
      if( nStart == std::string::npos )
         return pairRet;

      nStart += strlen("\"");

      int i = 0;
      for ( ; i < 1000; i++ )
      {
         if ( ( i + nStart ) >= m_strWrite.length() )
            break;

         if ( m_strWrite[i + nStart] == '"' )
         {
            if ( i > 0 && m_strWrite[i + nStart - 1] == '\\' )
               continue;

            //Reached end
            break;
         }
      }

      int nEnd = nStart + i;

      pairRet.first = m_strWrite.substr(nStart, nEnd-nStart);
   }

   //Get the Repo name:
   {
      int nStart = m_strWrite.find("\"name\":");
      if( nStart == std::string::npos )
         return pairRet;

      nStart = nStart + strlen("\"name\":");

      nStart = m_strWrite.find("\"", nStart);
      if( nStart == std::string::npos )
         return pairRet;

      nStart += strlen("\"");

      int nEnd = m_strWrite.find("\"", nStart);
      if( nEnd == std::string::npos || nEnd <= nStart )
         return pairRet;

      pairRet.second = m_strWrite.substr(nStart, nEnd-nStart);
   }

   return pairRet;
}

