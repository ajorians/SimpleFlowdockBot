#ifndef FlowdockBOT_PULLREQUESTTITLEHANDLER_H
#define FlowdockBOT_PULLREQUESTTITLEHANDLER_H

#include <curl/curl.h>
#include <string>
#include <vector>

class PullRequestTitleHandler
{
public:
	PullRequestTitleHandler();
	~PullRequestTitleHandler();

   static bool HasPR(const std::string& strMessage);
   static std::vector<std::string> PRsFromMessage(const std::string& strMessage);

   std::pair<std::string, std::string> GetPRTitleAndName(const std::string& strURL);

protected:
	static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream);

protected:
	CURL *m_pCurl;
	CURLcode m_resLast;
	curl_slist * m_pCookies;

	std::string m_strWrite;
};


#endif
