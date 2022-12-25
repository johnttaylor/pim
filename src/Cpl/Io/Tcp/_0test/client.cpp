/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "client.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Itc/PeriodicScheduler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/Tcp/InputOutput.h"

#define SECT_   "_0test"

using namespace Cpl::Io::Tcp;


#define TEST_STRING1    "Hi there.  My name is bob"
#define TEST_STRING2    "Well, Prince, so Genoa and Lucca are now just family estates of the Buonapartes. But I warn you, if you don't tell me that this means war, if you still try to defend the infamies and horrors perpetrated by that Antichrist- I really believe he is Antichrist- I will have nothing more to do with you and you are no longer my friend, no longer my 'faithful slave,' as you call yourself! But how do you do? I see I have frightened you- sit down and tell me all the news.'It was in July, 1805, and the speaker was the well-known Anna Pavlovna Scherer, maid of honor and favorite of the Empress Marya Fedorovna. With these words she greeted Prince Vasili Kuragin, a man of high rank and importance, who was the first to arrive at her reception. Anna Pavlovna had had a cough for some days. She was, as she said, suffering from la grippe; grippe being then a new word in St. Petersburg, used only by the elite.All her invitations without exception, written in French, and delivered by a scarlet-liveried footman that morning, ran as follows:'If you have nothing better to do, Count [or Prince], and if the prospect of spending an evening with a poor invalid is not too terrible, I shall be very charmed to see you tonight between 7 and 10- Annette Scherer.''Heavens! what a virulent attack!' replied the prince, not in the least disconcerted by this reception. He had just entered, wearing an embroidered court uniform, knee breeches, and shoes, and had stars on his breast and a serene expression on his flat face. He spoke in that refined French in which our grandfathers not only spoke but thought, and with the gentle, patronizing intonation natural to a man of importance who had grown old in society and at court. He went up to Anna Pavlovna, kissed her hand, presenting to her his bald, scented, and shining head, and complacently seated himself on the sofa.'First of all, dear friend, tell me how you are. Set your friend's mind at rest,' said he without altering his tone, beneath the politeness and affected sympathy of which indifference and even irony could be discerned.'Can one be well while suffering morally? Can one be calm in times like these if one has any feeling?' said Anna Pavlovna. 'You are staying the whole evening, I hope?''And the fete at the English ambassador's? Today is Wednesday. I must put in an appearance there,' said the prince. 'My daughter is coming for me to take me there.''I thought today's fete had been canceled. I confess all these festivities and fireworks are becoming wearisome.''If they had known that you wished it, the entertainment would have been put off,' said the prince, who, like a wound-up clock, by force of habit said things he did not even wish to be believed.'Don't tease! Well, and what has been decided about Novosiltsev's dispatch? You know everything.''What can one say about it?' replied the prince in a cold, listless tone. 'What has been decided? They have decided that Buonaparte has burnt his boats, and I believe that we are ready to burn ours.'Prince Vasili always spoke languidly, like an actor repeating a stale part. Anna Pavlovna Scherer on the contrary, despite her forty years, overflowed with animation and impulsiveness. To be an enthusiast had become her social vocation and, sometimes even when she did not feel like it, she became enthusiastic in order not to disappoint the expectations of those who knew her. The subdued smile which, though it did not suit her faded features, always played round her lips expressed, as in a spoiled child, a continual consciousness of her charming defect, which she neither wished, nor could, nor considered it necessary, to correct.In the midst of a conversation on political matters Anna Pavlovna burst out:'Oh, don't speak to me of Austria. Perhaps I don't understand things, but Austria never has wished, and does not wish, for war. She is betraying us! Russia alone must save Europe. Our gracious sovereign recognizes his high vocation and will be true to it. That is the one thing I have faith in! Our good and wonderful sovereign has to perform the noblest role on earth, and he is so virtuous and noble that God will not forsake him. He will fulfill his vocation and crush the hydra of revolution, which has become more terrible than ever in the person of this murderer and villain! We alone must avenge the blood of the just one.... Whom, I ask you, can we rely on?... England with her commercial spirit will not and cannot understand the Emperor Alexander's loftiness of soul. She has refused to evacuate Malta. She wanted to find, and still seeks, some secret motive in our actions. What answer did Novosiltsev get? None. The English have not understood and cannot understand the self-abnegation of our Emperor who wants nothing for himself, but only desires the good of mankind. And what have they promised? Nothing! And what little they have promised they will not perform! Prussia has always declared that Buonaparte is invincible, and that all Europe is powerless before him.... And I don't believe a word that Hardenburg says, or Haugwitz either. This famous Prussian neutrality is just a trap. I have faith only in God and the lofty destiny of our adored monarch. He will save Europe!'She suddenly paused, smiling at her own impetuosity.'I think,' said the prince with a smile, 'that if you had been sent instead of our dear Wintzingerode you would have captured the King of Prussia's consent by assault. You are so eloquent. Will you give me a cup of tea?''In a moment. A propos,' she added, becoming calm again, 'I am expecting two very interesting men tonight, le Vicomte de Mortemart, who is connected with the Montmorencys through the Rohans, one of the best French families. He is one of the genuine emigres, the good ones. And also the Abbe Morio. Do you know that profound thinker? He has been received by the Emperor. Had you heard?''I shall be delighted to meet them,' said the prince. 'But tell me,' he added with studied carelessness as if it had only just occurred to him, though the question he was about to ask was the chief motive of his visit, 'is it true that the Dowager Empress wants Baron Funke to be appointed first secretary at Vienna? The baron by all accounts is a poor creature.'Prince Vasili wished to obtain this post for his son, but others were trying through the Dowager Empress Marya Fedorovna to secure it for the baron.Anna Pavlovna almost closed her eyes to indicate that neither she nor anyone else had a right to criticize what the Empress desired or was pleased with.'Baron Funke has been recommended to the Dowager Empress by her sister,' was all she said, in a dry and mournful tone.As she named the Empress, Anna Pavlovna's face suddenly assumed an expression of profound and sincere devotion and respect mingled with sadness, and this occurred every time she mentioned her illustrious patroness. She added that Her Majesty had deigned to show Baron Funke beaucoup d'estime, and again her face clouded over with sadness.The prince was silent and looked indifferent. But, with the womanly and courtierlike quickness and tact habitual to her, Anna Pavlovna wished both to rebuke him (for daring to speak he had done of a man recommended to the Empress) and at the same time to console him, so she said:'Now about your family. Do you know that since your daughter came out everyone has been enraptured by her? They say she is amazingly beautiful.'The prince bowed to signify his respect and gratitude.'I often think,' she continued after a short pause, drawing nearer to the prince and smiling amiably at him as if to show that political and social topics were ended and the time had come for intimate conversation- 'I often think how unfairly sometimes the joys of life are distributed. Why has fate given you two such splendid children? I don't speak of Anatole, your youngest. I don't like him,' she added in a tone admitting of no rejoinder and raising her eyebrows. 'Two such charming children. And really you appreciate them less than anyone, and so you don't deserve to have them.'And she smiled her ecstatic smile.'I can't help it,' said the prince. 'Lavater would have said I lack the bump of paternity.''Don't joke; I mean to have a serious talk with you. Do you know I am dissatisfied with your younger son? Between ourselves' (and her face assumed its melancholy expression), 'he was mentioned at Her Majesty's and you were pitied....'The prince answered nothing, but she looked at him significantly, awaiting a reply. He frowned.'What would you have me do?' he said at last. 'You know I did all a father could for their education, and they have both turned out fools. Hippolyte is at least a quiet fool, but Anatole is an active one. That is the only difference between them.' He said this smiling in a way more natural and animated than usual, so that the wrinkles round his mouth very clearly revealed something unexpectedly coarse and unpleasant.'And why are children born to such men as you? If you were not a father there would be nothing I could reproach you with,' said Anna Pavlovna, looking up pensively.'I am your faithful slave and to you alone I can confess that my children are the bane of my life. It is the cross I have to bear. That is how I explain it to myself. It can't be helped!'He said no more, but expressed his resignation to cruel fate by a gesture. Anna Pavlovna meditated.'Have you never thought of marrying your prodigal son Anatole?' she asked. 'They say old maids have a mania for matchmaking, and though I don't feel that weakness in myself as yet,I know a little person who is very unhappy with her father. She is a relation of yours, Princess Mary Bolkonskaya.'Prince Vasili did not reply, though, with the quickness of memory and perception befitting a man of the world, he indicated by a movement of the head that he was considering this information.'Do you know,' he said at last, evidently unable to check the sad current of his thoughts, 'that Anatole is costing me forty thousand rubles a year? And,' he went on after a pause, 'what will it be in five years, if he goes on like this?' Presently he added: 'That's what we fathers have to put up with.... Is this princess of yours rich?''Her father is very rich and stingy. He lives in the country. He is the well-known Prince Bolkonski who had to retire from the army under the late Emperor, and was nicknamed 'the King of Prussia.' He is very clever but eccentric, and a bore. The poor girl is very unhappy. She has a brother; I think you know him, he married Lise Meinen lately. He is an aide-de-camp of Kutuzov's and will be here tonight.''Listen, dear Annette,' said the prince, suddenly taking Anna Pavlovna's hand and for some reason drawing it downwards. 'Arrange that affair for me and I shall always be your most devoted slave- slafe wigh an f, as a village elder of mine writes in his reports. She is rich and of good family and that's all I want.'And with the familiarity and easy grace peculiar to him, he raised the maid of honor's hand to his lips, kissed it, and swung it to and fro as he lay back in his armchair, looking in another direction.'Attendez,' said Anna Pavlovna, reflecting, 'I'll speak to Lise, young Bolkonski's wife, this very evening, and perhaps the thing can be arranged. It shall be on your family's behalf that I'll start my apprenticeship as old maid."


#define STATE_WAITING_INCOMING_DATA     0
#define STATE_DELAYING                  1
#define STATE_SENDING_LONG_STRING       2

#ifndef OPTION_DELAY_INTERVAL_MS
#define OPTION_DELAY_INTERVAL_MS       1000
#endif

class ConnectorClient : public AsyncConnector::Client
{
public:
    bool                      m_connected;
    unsigned long             m_timeMark;
    int                       m_state;
    bool                      m_shortString;
    int                       m_offset;
    int                       m_maxLenLongString;
    int                       m_inOffset;

public:
    ConnectorClient()
        : m_connected( false )
        , m_offset( 0 )
        , m_maxLenLongString( strlen( TEST_STRING2 ) )
    {
    }

public:
    bool newConnection( Cpl::Io::Descriptor newFd )  noexcept
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Connection ESTABLISHED") );
        activate( newFd );
        m_connected   = true;
        m_state       = STATE_WAITING_INCOMING_DATA;
        m_shortString = true;
        sendData();
        return true;
    }

    void sendData()
    {
        int  bytesWritten;
        if ( m_shortString )
        {
            if ( write( TEST_STRING1, strlen( TEST_STRING1 ), bytesWritten ) )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: Bytes OUT: %d", bytesWritten) );
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: WRITE FAILED") );
                m_connected = false;
            }
        }
        else
        {
            if ( write( TEST_STRING2 + m_offset, m_maxLenLongString - m_offset, bytesWritten ) )
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: Bytes OUT: %d, offset=%d", bytesWritten, m_offset) );
                m_offset += bytesWritten;
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: WRITE FAILED") );
                m_connected = false;
            }
        }
    }

    void connectionFailed( Error_T errorCode ) noexcept
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Connection FAILED: error=%X", errorCode) );
    }

public:
    void consumeInput()
    {
        if ( m_connected )
        {
            if ( m_state == STATE_SENDING_LONG_STRING )
            {
                sendData();
                m_state = STATE_WAITING_INCOMING_DATA;
            }

            if ( m_state == STATE_WAITING_INCOMING_DATA )
            {
                int  bytesRead;
                char inBuf[128];
                if ( read( inBuf, sizeof( inBuf ), bytesRead ) )
                {
                    if ( bytesRead > 0 )
                    {
                        CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: Bytes  IN: %d. %.*s", bytesRead, bytesRead, inBuf) );
                        if ( m_shortString )
                        {
                            if ( memcmp( inBuf, TEST_STRING1, strlen( TEST_STRING1 ) ) != 0 )
                            {
                                CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Expected: [%s]", TEST_STRING1) );
                                close();
                                m_connected = false;
                            }
                            else
                            {
                                m_state        = STATE_SENDING_LONG_STRING;
                                m_shortString  = false;
                                m_offset       = 0;
                                m_inOffset     = 0;
                            }
                        }
                        else
                        {
                            if ( memcmp( inBuf, TEST_STRING2 + m_inOffset, bytesRead ) != 0 )
                            {
                                CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Expected: [%.*s]", bytesRead, TEST_STRING2 + m_inOffset) );
                                close();
                                m_connected = false;
                            }
                            else
                            {
                                m_inOffset += bytesRead;
                                if ( m_inOffset == m_maxLenLongString )
                                {
                                    m_timeMark    = Cpl::System::ElapsedTime::milliseconds();
                                    m_state       = STATE_DELAYING;
                                    m_shortString = true;
                                }
                                else if ( m_inOffset == m_offset )
                                {
                                    m_state = STATE_SENDING_LONG_STRING;
                                }
                            }
                        }
                    }
                }
                else
                {
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: READ FAILED") );
                    m_connected = false;
                }
            }

            if ( m_state == STATE_DELAYING )
            {
                if ( Cpl::System::ElapsedTime::expiredMilliseconds( m_timeMark, OPTION_DELAY_INTERVAL_MS ) )
                {
                    m_state       = STATE_WAITING_INCOMING_DATA;
                    m_shortString = true;
                    sendData();
                }
            }
        }
    }
};

//////////////////////////////////////////////
static const char*                   remoteHostAddr_;
static int                           portNum_;
static Cpl::Io::Tcp::AsyncConnector* connectorPtr_;
static ConnectorClient               connectorClient_;


static void tcpScan_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    connectorPtr_->poll();
    connectorClient_.consumeInput();
}

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS // Create an empty set of intervals - actual intervals not used for the test
};

static void beginThreadProcessing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    connectorPtr_->establish( connectorClient_, remoteHostAddr_, portNum_ );
}
static void endThreadProcssing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    connectorPtr_->terminate();
}


static Cpl::Itc::PeriodicScheduler scheduler_( intervals_,
                                               beginThreadProcessing, endThreadProcssing,
                                               nullptr,
                                               Cpl::System::ElapsedTime::precision,
                                               tcpScan_ );


////////////////////////////////////////////////////////////////////////////////

int runTest( Cpl::Io::Tcp::AsyncConnector& connector, const char* remoteHostAddr, int portNum )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enter TCP test: Echo Client") );

    // Cache the UUTs
    portNum_        = portNum;
    connectorPtr_   = &connector;
    remoteHostAddr_ = remoteHostAddr;

    // Create thread for the test to run in
    Cpl::System::Thread::create( scheduler_, "TEST" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Scheduling Enabled!") );

    // The test assumes that enableScheduling() never returns -->so make sure we don't return
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }
}


