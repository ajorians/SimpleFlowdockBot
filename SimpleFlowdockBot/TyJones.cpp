#include "TyJones.h"
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

namespace {
   vector<string> split(const char *str, char c = ' ')
   {
      vector<string> result;

      do
      {
         const char *begin = str;

         while(*str != c && *str)
            str++;

         result.push_back(string(begin, str));
      } while (0 != *str++);

      return result;
   }

   std::string ToLower(const std::string& strCommand)
   {
      std::string strReturn(strCommand);
      std::transform(strReturn.begin(), strReturn.end(), strReturn.begin(), ::tolower);
      return strReturn;
   }
}

bool TyJones::HasTYMessage(const std::string& strMessage)
{
   std::vector<std::string> pieces = split(ToLower(strMessage).c_str());

   for(std::vector<std::string>::size_type i=0; i<pieces.size(); i++)
   {
      if( pieces[i] == "ty" )
         return true;
   }

   return false;
}

std::string TyJones::HandleMessage(const std::string& strMessage, const std::string& strUsername)
{
   if( !TyJones::HasTYMessage(strMessage) )
      return std::string();

   int n = rand()%3;
   switch(n)
   {
   default:
   case 0:
      return "Ty Jones sure is awesome!! :smiley:";
   case 1:
      return "You are welcome! :smiley:";
   case 2:
      return "That's me! :smiley:";
   }

   return std::string();
}

