#include "EmojiReactionAdder.h"

#include "FlowAPILibrary.h"

#include <algorithm>
#include <utility>
#include <set>

typedef std::vector<std::string> Phrases;
typedef std::vector<std::string> Emojis;
typedef std::pair<Phrases, Emojis> EmojiMatch;
static const std::vector<EmojiMatch> g_matches =
        {
            EmojiMatch( Phrases{"birthday", "bday", "birth-day"}, Emojis{"cake", "gift", "balloon", "confetti_ball"}),
            EmojiMatch( Phrases{"haircut"}, Emojis{"haircut", "barber"}),
            EmojiMatch( Phrases{"tired", }, Emojis{"sleepy", "tired_face", "sleeping", "zzz"}),
            EmojiMatch( Phrases{"question", }, Emojis{"question"}),
            EmojiMatch( Phrases{"bank"}, Emojis{"atm", "heavy_dollar_sign", "dollar", "moneybag", "money_with_wings", "credit_card"}),
            EmojiMatch( Phrases{"dentist", "dental"}, Emojis{"open_mouth", "mask", "grimacing", "sweat_smile", "cold_sweat", "fearful"}),
            EmojiMatch( Phrases{"cedar point"}, Emojis{"roller_coaster", "ferris_wheel", "circus_tent"}),
            EmojiMatch( Phrases{"construction"}, Emojis{"construction", "construction_worker", "truck"}),
            EmojiMatch( Phrases{"raining"}, Emojis{"umbrella", "rainbow", "droplet", "cloud"}),
            EmojiMatch( Phrases{"family"}, Emojis{"family"}),
            EmojiMatch( Phrases{"jogging", "5K"}, Emojis{"runner", "running"}),
            EmojiMatch( Phrases{"dog", "puppy"}, Emojis{"dog", "paw_prints"}),
            EmojiMatch( Phrases{"cat", "kitten"}, Emojis{"cat", "smiley_cat", "cat2", "octocat"}),
            EmojiMatch( Phrases{"bathroom"}, Emojis{"wc", "restroom", "hankey", "toilet", "bath", "shower"}),
            EmojiMatch( Phrases{"shower"}, Emojis{"wc", "restroom", "hankey", "toilet", "bath", "shower"}),
            EmojiMatch( Phrases{"halloween", "pumpkin"}, Emojis{"jack_o_lantern", "ghost"}),
            EmojiMatch( Phrases{"battery"}, Emojis{"battery", "electric_plug"}),
            EmojiMatch( Phrases{"package"}, Emojis{"package", "money_with_wings"}),
            EmojiMatch( Phrases{"baby"}, Emojis{"baby_symbol"}),
            EmojiMatch( Phrases{"ship it"}, Emojis{"shipit"}),
            EmojiMatch( Phrases{"postoffice", "post office", "post-office"}, Emojis{"post_office", "envelope", "mailbox_with_mail"}),
            EmojiMatch( Phrases{"christmas", "xmas", "x-mas"}, Emojis{"santa", "gift", "christmas_tree", "package", "bell", "tada"}),
        };

namespace {
    bool IsPhraseSeparator(char ch)
    {
        return isspace(ch) || ispunct(ch);
    }
}

EmojiReactionAdder::EmojiReactionAdder(FlowdockAPI pFlowdock,
        const std::string& strOrg, const std::string& strFlow, const std::string& strUsername, const std::string& strPassword, int nEmojiReactionAmount)
        : m_pFlowdock( pFlowdock )
        , m_strOrg( strOrg )
        , m_strFlow( strFlow )
        , m_strUsername( strUsername )
        , m_strPassword( strPassword )
        , m_nEmojiReactionAmount( nEmojiReactionAmount )
{
    std::string strFileName = "EmojiRections.log";
    m_Out.open(strFileName.c_str(), std::ios::app);
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
            int nPos = -1;
            while( true )
            {
                const std::string& phrase = *itPhrases;
                nPos = lowerCaseMessage.find( phrase, nPos + 1);
                if ( nPos == std::string::npos )
                    break;

                //Does it have a space before or after (or any other punctuation)
                bool isPhrase = false;
                if( nPos == 0 ) {//Fist word
                    if( IsPhraseSeparator(lowerCaseMessage[nPos + phrase.length()]) )//If something after this
                    {
                        isPhrase = true;
                    }
                }
                else if( nPos + phrase.length() == lowerCaseMessage.length()) {//Last word
                    if( IsPhraseSeparator( lowerCaseMessage[nPos-1]))//If something before this
                    {
                        isPhrase = true;
                    }
                }
                else
                {
                    if( (nPos + phrase.length()) < lowerCaseMessage.length()
                    && IsPhraseSeparator(lowerCaseMessage[nPos-1])
                    && IsPhraseSeparator(lowerCaseMessage[nPos + phrase.length()]) )
                    {
                        isPhrase = true;
                    }
                }

                if( isPhrase )
                {
                    for( auto itEmojis = (*it).second.cbegin(); itEmojis != (*it).second.cend(); itEmojis++) {
                        emojiReactionsToAdd.insert( *itEmojis );
                    }
                }
            }
        }
    }

    if( emojiReactionsToAdd.size() > 0 )
    {
        int nEmojisAdded = 0;
        std::string logMessage = m_strFlow + ": ";
        for( auto it = emojiReactionsToAdd.cbegin(); it!= emojiReactionsToAdd.cend(); it++)
        {
            if( nEmojisAdded >= m_nEmojiReactionAmount)
                break;

            nEmojisAdded++;
            FlowAPILibrary::instance().AddEmojiReaction(m_pFlowdock, m_strOrg, m_strFlow, m_strUsername, m_strPassword, nMessageID, (*it).c_str() );
            logMessage += *it;
            logMessage += ", ";
        }
        m_Out << logMessage << std::endl;
    }
}

