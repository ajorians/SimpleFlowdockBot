#ifndef FlowdockBOT_VSIDHANDLER_H
#define FlowdockBOT_VSIDHANDLER_H

#include <curl/curl.h>
#include <vector>

class VSIDHandler
{
public:
	VSIDHandler();
	~VSIDHandler();

   static std::vector<int> VSIDsFromMessage(const std::string& strMessage);
   static bool HasVSID(const std::string& strMessage);

   std::string GetResponseForVSID(int nID);

protected:
	static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream);

protected:
   bool GetInfoOutOfResponse(std::string& strTitle, std::string& strAssignedTo, std::string& strCurrentStatus, std::string& strSeverity, std::string& strPriority) const;

protected:
	CURL *m_pCurl;
	CURLcode m_resLast;
	curl_slist * m_pCookies;

	std::string m_strWrite;
};


#endif
