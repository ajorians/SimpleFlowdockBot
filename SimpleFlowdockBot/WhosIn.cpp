#include "WhosIn.h"
#include <string>
#include <fstream>
#include "JSONLib/JSON.h"
using namespace std;

namespace {
   bool StartsWith(std::string strText, std::string str)
   {
      for(string::size_type i=0; i<strText.length(); i++)
         strText[i] = tolower(strText[i]);

      for(string::size_type i=0; i<str.length(); i++)
         str[i] = tolower(str[i]);

      if( strText.length() < str.length() )//Too short
         return false;
      
      if( strText.find(str) != 0 )//Not at beginning
         return false;

      return true;
   }

   bool Matches(std::string strText, std::string str)
   {
      for(string::size_type i=0; i<strText.length(); i++)
         strText[i] = tolower(strText[i]);

      for(string::size_type i=0; i<str.length(); i++)
         str[i] = tolower(str[i]);

      if( strText.length() < str.length() )//Too short
         return false;

      if( strText != str )//No match
         return false;

      return true;
   }

   std::string fileToString(std::string const& name) {
      std::ifstream in(name.c_str());
      return std::string(std::istreambuf_iterator<char>(in),
         std::istreambuf_iterator<char>());
   }
}

bool WhosIn::IsWhosInMessage(const std::string& strMessage)
{
   //Valid messages are:
   //WhosIn <TAG>
   //In <Tag>
   //Out <Tag>
   //Maybe <Tag>
   //Reset <Tag>
   if( StartsWith(strMessage, "WhosIn") )
   {
      return true;
   }
   else if( StartsWith(strMessage, "In") )
   {
      return true;
   }
   else if( StartsWith(strMessage, "Out") )
   {
      return true;
   }
   else if( StartsWith(strMessage, "Maybe") )
   {
      return true;
   }
   else if( StartsWith(strMessage, "Reset") )
   {
      return true;
   }

   return false;
}

std::string WhosIn::HandleMessage(const std::string& strMessage, const std::string& strUsername)
{
   if( !WhosIn::IsWhosInMessage(strMessage) )
      return std::string();

   if( StartsWith(strMessage, "WhosIn") )
   {
      int n = strMessage.find(" ");
      if( n != string::npos )
      {
         std::string strTag = strMessage.substr(n+1);
         if( WhosIn::instance().DoesTagExist(strTag) )
         {
            return WhosIn::instance().GetInOuts(strTag);
         }
         else if( WhosIn::instance().AddTag(strTag, false) ) {
            std::string strMessage = "Who's in for: \"" + strTag + "\"\n";
            strMessage += ">> Type: In " + strTag + "\n";
            strMessage += "   To be in! :smiley:\n";
            strMessage += ">> Type: Out " + strTag + "\n";
            strMessage += "   To be out :frowning:";

            return strMessage;
         }
      }
   }
   else if( StartsWith(strMessage, "In") )
   {
      int n = strMessage.find(" ");
      if( n != string::npos )
      {
         std::string strTag = strMessage.substr(n+1);
         if( WhosIn::instance().DoesTagExist(strTag) )
         {
            if( WhosIn::instance().AddPerson(strTag, strUsername, In, false) ) {
               return "You (" + strUsername + ") are added as In";
            }
            else {

            }
         }
      }
      return std::string();
   }
   else if( StartsWith(strMessage, "Out") )
   {
      int n = strMessage.find(" ");
      if( n != string::npos )
      {
         std::string strTag = strMessage.substr(n+1);
         if( WhosIn::instance().DoesTagExist(strTag) )
         {
            if( WhosIn::instance().AddPerson(strTag, strUsername, Out, false) ) {
               return "You (" + strUsername + ") are added as Out";
            }
            else {

            }
         }
      }
      return std::string();
   }
   else if( StartsWith(strMessage, "Maybe") )
   {
      //TODO
      return std::string();
   }
   else if( StartsWith(strMessage, "Reset") )
   {
      //TODO
      return std::string();
   }

   return std::string();
}

WhosIn& WhosIn::instance()
{
   static WhosIn inst;
   return inst;
}

WhosIn::WhosIn()
{
   Load();
}

void WhosIn::Load()
{
   JSON* pJSON = JSON::Parse(fileToString("whosin.txt"));
   if( pJSON )
   {
      JSON* pRoot = pJSON->Child("Tags");
      if( pRoot )
      {
         JSONArray arrTags = pRoot->AsArray();
         for(JSONArray::size_type i=0; i<arrTags.size(); i++)
         {
            JSONObjects Tag = arrTags[i]->AsObject();
            AddTag(Tag["Tag"]->AsString(), true);

            JSONArray arrPersons = Tag["Persons"]->AsArray();
            for(JSONArray::size_type j=0; j<arrPersons.size(); j++)
            {
               JSONObjects Person = arrPersons[j]->AsObject();
               int n = (int)Person["In"]->AsNumber();
               AddPerson(Tag["Tag"]->AsString(), Person["Name"]->AsString(), (InOrOut)n, true);
            }
         }
      }
   }
}

void WhosIn::Persist()
{
   JSONArray arrTags;
   for(std::vector<WhosIn::Tag>::size_type i=0; i<m_aTags.size(); i++)
   {
      JSONObjects Tag;
      Tag["Tag"] = new JSON(m_aTags[i].m_strTag);
      JSONArray arrPersons;
      for(std::vector<WhosIn::Person>::size_type j=0; j<m_aTags[i].m_aPersons.size(); j++)
      {
         JSONObjects objs;
         objs["Name"] = new JSON(m_aTags[i].m_aPersons[j].strName);
         objs["In"] = new JSON(m_aTags[i].m_aPersons[j].eIn);

         arrPersons.push_back(new JSON(objs));
      }
      Tag["Persons"] = new JSON(arrPersons);
      arrTags.push_back(new JSON(Tag));
   }

   JSONObjects Tags;
   Tags["Tags"] = new JSON(arrTags);

   JSON json(Tags);

   fstream fs("whosin.txt", ios::out);
   std::string str = json.Stringify();
   fs.write(str.c_str(), str.length());
}

bool WhosIn::DoesTagExist(const std::string& strTag) const
{
   for(std::vector<WhosIn::Tag>::size_type i=0; i<m_aTags.size(); i++)
   {
      if( Matches(m_aTags[i].m_strTag, strTag) )
         return true;
   }
   return false;
}

bool WhosIn::AddTag(const std::string& strTag, bool silent)
{
   if( DoesTagExist(strTag) )
      return false;

   WhosIn::Tag tag;
   tag.m_strTag = strTag;
   m_aTags.push_back(tag);

   if( !silent )
      Persist();
   return true;
}

bool WhosIn::AddPerson(const std::string& strTag, const std::string& strPerson, InOrOut eIn, bool silent)
{
   if( !DoesTagExist(strTag) )
      return false;

   for(std::vector<WhosIn::Tag>::size_type i=0; i<m_aTags.size(); i++)
   {
      if( Matches(m_aTags[i].m_strTag, strTag) )
      {
         WhosIn::Person p;
         p.strName = strPerson;
         p.eIn = eIn;

         m_aTags[i].m_aPersons.push_back(p);

         if( !silent )
            Persist();
         return true;
      }
   }

   return false;
}

std::string WhosIn::GetInOuts(const std::string& strTag) const
{
   if( !DoesTagExist(strTag) )
      return false;

   for(std::vector<WhosIn::Tag>::size_type i=0; i<m_aTags.size(); i++)
   {
      if( Matches(m_aTags[i].m_strTag, strTag) )
      {
         std::string strMessage;
         strMessage = "People in for \"" + m_aTags[i].m_strTag + "\":\n";
         for(std::vector<WhosIn::Person>::size_type j=0; j<m_aTags[i].m_aPersons.size(); j++)
         {
            strMessage += m_aTags[i].m_aPersons[j].strName;
            strMessage += ": ";
            if( m_aTags[i].m_aPersons[j].eIn == In )
            {
               strMessage += "In";
            }
            else if( m_aTags[i].m_aPersons[j].eIn == Out )
            {
               strMessage += "Out";
            }
            strMessage += "\n";
         }
         if( m_aTags[i].m_aPersons.size() == 0 )
         {
            strMessage += "Nobody yet :frowning:";
         }

         return strMessage;
      }
   }

   return std::string();
}

