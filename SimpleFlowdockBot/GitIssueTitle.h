#ifndef FlowdockBOT_GITISSUETITLEHANDLER_H
#define FlowdockBOT_GITISSUETITLEHANDLER_H

#include <curl/curl.h>
#include <string>
#include <vector>

class GitIssueTitleHandler
{
public:
   GitIssueTitleHandler();
	~GitIssueTitleHandler();

   static bool HasIssue(const std::string& strMessage);
   static std::vector<std::string> IssuesFromMessage(const std::string& strMessage);

   std::pair<std::string, std::vector<std::string> > GetIssueTitleAndLabels(const std::string& strURL);

protected:
	static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream);

protected:
	CURL *m_pCurl;
	CURLcode m_resLast;
	curl_slist * m_pCookies;

	std::string m_strWrite;
};


#endif
