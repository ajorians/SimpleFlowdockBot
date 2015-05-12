#ifndef FlowdockBOT_WHOSINHANDLER_H
#define FlowdockBOT_WHOSINHANDLER_H

#include <string>
#include <vector>

class WhosIn
{
public:
   enum InOrOut
   {
      In,
      Out
   };

   static bool IsWhosInMessage(const std::string& strMessage);

   static std::string HandleMessage(const std::string& strMessage, const std::string& strUsername);

   static WhosIn& instance();

   WhosIn();

   void Load();
   void Persist();

   bool DoesTagExist(const std::string& strTag) const;
   bool AddTag(const std::string& strTag, bool silent);
   bool AddPerson(const std::string& strTag, const std::string& strPerson, InOrOut eIn, bool silent);
   std::string GetInOuts(const std::string& strTag) const;

private:
   struct Person
   {
      std::string strName;
      InOrOut eIn;
   };
   struct Tag
   {
      std::string m_strTag;
      std::vector<Person> m_aPersons;
   };

   std::vector<Tag> m_aTags;
};

#endif
