#include "EmojiReactionAdder.h"

#include "FlowAPILibrary.h"

#include <algorithm>
#include <utility>
#include <set>

typedef std::vector<std::string> Phrases;
typedef std::pair<Phrases, std::string> EmojiMatch;
static const std::vector<EmojiMatch> g_matches =
        {
            EmojiMatch( Phrases{"birthday"}, "cake"),
            EmojiMatch( Phrases{"haircut"}, "haircut")
        };

EmojiReactionAdder::EmojiReactionAdder(FlowdockAPI pFlowdock,
        const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword)
        : m_pFlowdock( pFlowdock )
        , m_strOrg( strOrg )
        , m_strFlow( strFlow )
        , m_strUsername( strUsername )
        , m_strPassword( strPassword )
{

}

void EmojiReactionAdder::MessageSaid(const std::string& strMessage,
        int nUserID, int nThreadId, int nMessageID, const std::vector<std::string> astrAddedTags, const std::vector<std::string> astrRemovedTags )
{
    std::set<std::string> emojiReactionsToAdd;

    std::string lowerCaseMessage(strMessage);
    std::transform(lowerCaseMessage.begin(), lowerCaseMessage.end(), lowerCaseMessage.begin(), ::tolower);

    for( auto it = g_matches.cbegin(); it != g_matches.cend(); it++ )
    {
        const auto& phrases = (*it).first;
        for( auto itPhrases = phrases.cbegin(); itPhrases != phrases.cend(); itPhrases++ )
        {
            if( lowerCaseMessage.find(*itPhrases) != std::string::npos )
            {
                emojiReactionsToAdd.insert( (*it).second );
                break;
            }
        }
    }

    if( emojiReactionsToAdd.size() > 0 )
    {
        for( auto it = emojiReactionsToAdd.cbegin(); it!= emojiReactionsToAdd.cend(); it++)
        {
            FlowAPILibrary::instance().AddEmojiReaction(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nMessageID, (*it).c_str() );
        }
    }
}

