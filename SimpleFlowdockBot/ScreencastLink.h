#ifndef FlowdockBOT_SCHANDLER_H
#define FlowdockBOT_SCHANDLER_H

#include <curl/curl.h>
#include <vector>

class ScreencastLinkHandler
{
public:
	ScreencastLinkHandler();
	~ScreencastLinkHandler();

   static bool HasSCLink(const std::string& strMessage);
   static std::vector<std::string> SCLinksFromMessage(const std::string& strMessage);

   std::string GetImageURL(const std::string& strURL);

protected:
	static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream);

protected:
	CURL *m_pCurl;
	CURLcode m_resLast;
	curl_slist * m_pCookies;

	std::string m_strWrite;
};


#endif
