#include "LinkFixer.h"
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <algorithm>

//#include <iostream>
//using namespace std;

namespace{
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

   std::string seps(std::string& s) {
       if (!s.size()) return "";
       std::stringstream ss;
       ss << s[0];
       for (std::string::size_type i = 1; i < s.size(); i++) {
           ss << '|' << s[i];
       }
       return ss.str();
   }

   void Tokenize(std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ")
   {
       seps(str);

       // Skip delimiters at beginning.
       std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
       // Find first "non-delimiter".
       std::string::size_type pos = str.find_first_of(delimiters, lastPos);

       while (std::string::npos != pos || std::string::npos != lastPos)
       {
           // Found a token, add it to the vector.
           tokens.push_back(str.substr(lastPos, pos - lastPos));
           // Skip delimiters.  Note the "not_of"
           lastPos = str.find_first_not_of(delimiters, pos);
           // Find next "non-delimiter"
           pos = str.find_first_of(delimiters, lastPos);
       }
   }
}

std::vector<std::string> LinkFixerHandler::LinksFromMessage(const std::string& strMessage)
{
   //cout << "strMessage: " << strMessage << endl;
   std::vector<std::string> arrLinks;

   std::string strCopy = strMessage;

   Tokenize(strCopy, arrLinks);

   for(std::string::size_type i=arrLinks.size(); i-->0;)
   {
      std::string str = arrLinks[i];
      //cout << "Possible link: " << str << endl;
      bool bLink = false;
      if( str.length() > 7 && str.substr(0, 7) == "http://" )
         bLink = true;
      if( str.length() > 8 && str.substr(0, 8) == "https://" )
         bLink = true;

      if( !bLink )
      {
         //cout << "Not a link: " << i << endl;
         arrLinks.erase(arrLinks.begin()+i);
      }
   }

   return arrLinks;
}

bool LinkFixerHandler::HasLinks(const std::string& strMessage)
{
   return LinksFromMessage(strMessage).size() > 0;
}

std::string LinkFixerHandler::GetCorrectedLink(const std::string& strLink)
{
   std::string strFixed = strLink;
   Replace(strFixed, "{", "%7B");
   Replace(strFixed, "}", "%7D");

   return strFixed;
}


