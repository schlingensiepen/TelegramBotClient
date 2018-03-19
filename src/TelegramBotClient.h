/**
    \file TelegramBotClient.h
    \brief Header of a simple client sending and receiving message
           via Telegram's Bot API.
           Uses one or two underlying objects implementing the Client interface.
           It implements a pseudo background behavior by providing a loop()
           method that can be polled and calls callback on receiving valid data.

    Part of TelegramBotClient (https://github.com/schlingensiepen/TelegramBotClient)
    Jörn Schlingensiepen <joern@schlingensiepen.com>
*/
#pragma once
#ifndef TelegramBotClient_h
#define TelegramBotClient_h

#include "TBCDebug.h"
#include "Arduino.h"
#include <Client.h>
#include <ArduinoJson.h>
#include "JsonWebClient.h"

#define TELEGRAMHOST F("api.telegram.org")
#define TELEGRAMPORT 443
#define POLLINGTIMEOUT 600
#define USERAGENTSTRING F("telegrambotclient /0.1")

// Inspired by PubSubClient by Nick O'Leary (http://knolleary.net)
#ifdef ESP8266
#include <functional>
#define TBC_CALLBACK_RECEIVE_SIGNATURE std::function<void(TelegramProcessError, JwcProcessError, Message*)> callbackReceive
#define TBC_CALLBACK_ERROR_SIGNATURE std::function<void(TelegramProcessError, JwcProcessError)> callbackError
#else
#define TBC_CALLBACK_RECEIVE_SIGNATURE void (*callbackReceive)(TelegramProcessError, JwcProcessError, Message*)
#define TBC_CALLBACK_ERROR_SIGNATURE void (*callbackError)(TelegramProcessError, JwcProcessError)
#endif

#ifndef uint
#define uint unsigned int
#endif

/**
   \class TelegramProcessError
   @enum mapper::TelegramProcessError

   \file TelegramBotClient.h

   \brief TelegramProcessError state = TelegramProcessError::Ok;

   Enumeration to indicate error or success of processing by TelegramBotClient.

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/
enum class TelegramProcessError : int
{
  /** Everything Ok, no error */
  Ok = 0,
  /** JSONWebClient host returns error while polling */
  JcwPollErr = -1,
  /** JSONWebClient host returns error while posting */
  JcwPostErr = -2,
  /** Telegram host returns error while polling */
  RetPollErr = -3,
  /** Telegram host returns error while posting */
  RetPostErr = -4
};

/** Static list of JwcClientState names */
static String TelegramProcessErrorString[] = {"Ok", "JcwPollErr", "JcwPostErr", "RetPollErr", "RetPostErr"};

static String toString(TelegramProcessError err)
{
  return TelegramProcessErrorString[(-1) * (int) err];
}

/**
   \struct Message

   \file TelegramBotClient.h

   \brief Telegram Message

   Struct to store elements of a Telegram Message
   (https://core.telegram.org/bots/api#message)
   and the update_id provided by each callback
   (https://core.telegram.org/bots/api#getting-updates)

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/
struct Message {
  /** update_id
      The update‘s unique identifier. Update identifiers start from a certain
      positive number and increase sequentially. This ID becomes especially handy
      if you’re using Webhooks, since it allows you to ignore repeated updates or
      to restore the correct update sequence, should they get out of order.
      If there are no new updates for at least a week, then identifier of the next
      update will be chosen randomly instead of sequentially.*/
  long UpdateId;
  /** message_id : message_id
      Unique message identifier inside this chat */
  long MessageId;
  /** from_id : from/id
      Unique identifier for this user or bot */
  long FromId;
  /** from_is_bot: from/is_bot
      True, if this user is a bot */
  bool FromIsBot;
  /** from_first_name: from/first_name
      User‘s or bot’s first name
  */
  String FromFirstName;
  /** from_last_name: from/last_name
      Optional. User‘s or bot’s last name */
  String FromLastName;
  /** from_language_code: from/language_code
      Optional. IETF language tag of the user's language
  */
  String FromLanguageCode;
  /** chat_id: chat/id
      
      \Note Used to identify chat while posting a message
      Unique identifier for this chat. This number may be greater
      than 32 bits and some programming languages may have difficulty/silent
      defects in interpreting it. But it is smaller than 52 bits, so a signed
      64 bit integer or double-precision float type are safe for storing this identifier.*/
  long ChatId;
  /** chat_first_name: chat/first_name
      Optional. First name of the other party in a private chat */
  String ChatFirstName;
  /** chat_last_name: chat/last_name
      Optional. Last name of the other party in a private chat*/
  String ChatLastName;
  /** chat_type: chat/type
      Type of chat, can be either “private”, “group”, “supergroup” or “channel” */
  String ChatType;
  /** text: text
      Optional. For text messages, the actual UTF-8 text of the message,
      0-4096 characters.*/
  String Text;
  /** date: date
      Date the message was sent in Unix time
  */
  long Date;
};

/**
   \struct TBCKeyBoardRow

   \file TelegramBotClient.h

   \brief Row in a keyboard

   Struct to store elements of a Telegram key board

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/
struct TBCKeyBoardRow
{
  uint Count;
  String* Buttons;
};

/**
   \class TBCKeyBoard

   \file TelegramBotClient.h

   \brief Class to represent a keyboard used in Telegram chat

   This class represents a keyboard that can be displayed in
   a Telegram chat. Keyboards can be assembled by Rows including
   buttons. To add a row to a keyboard use push().

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/
class TBCKeyBoard
{
  private:

    uint Count;             /*< Maximum number of rows in the keyboard */
    uint Counter;           /*< Number of rows already stored in keyboard */
    TBCKeyBoardRow* Rows;   /*< Array of rows in the keyboard */
    /** Requests clients to hide the keyboard as soon as it's been used. 
     *  The keyboard will still be available, but clients will automatically 
     *  display the usual letter-keyboard in the chat – the user can press a 
     *  special button in the input field to see the custom keyboard again. 
     *  
     *  Defaults to false. 
     *  
     *  https://core.telegram.org/bots/api#replykeyboardmarkup
     */
    bool OneTime = false;
    /** Requests clients to resize the keyboard vertically for optimal fit 
     *  (e.g., make the keyboard smaller if there are just two rows of buttons). 
     *  Defaults to false, in which case the custom keyboard is always of the same 
     *  height as the app's standard keyboard.
     *  
     *  Defaults to false. 
     *  
     *  https://core.telegram.org/bots/api#replykeyboardmarkup
     */
    bool Resize = false;
  public:
    /**
        \brief Constructor
        \details Constructor, initializing all members
        \param count The number of rows in keyboard.
        \param oneTime value for OneTime
        \param resize value for Resize
    */
  TBCKeyBoard (uint count, bool oneTime = false, bool resize = false);
    /**
        \brief Destructor
        \details Destructor
    */
    ~TBCKeyBoard ();
    /**
     *  \brief Adds a row to the keyboard
     *  
     *  \param [in] count Number of buttons passend in buttons
     *  \param [in] buttons Button to be displayed in this row
     *  \return The keyboard itself
     *  
     *  \details Adds a row to the keyboard containing buttons
     *  displaying the string passed in buttons[]
     */
    TBCKeyBoard& push(uint count, const String buttons[]);
    /**
     *  \brief Gets a button text
     *  
     *  \param [in] row Index of row to fetch button text from
     *  \param [in] col Index of column to fetch button text from
     *  \return button text
     *  
     *  \details Gets the text of a button in given row and column
     */
    const String get(const uint row, const uint col);
    /**
     *  \brief Length of row
     *  
     *  \param [in] row Index of row to get length
     *  \return return length of row
     *  
     *  \details Gets the length of the row at the given index
     *  The length of a row is the number of buttons in this row.
     */
    const int length (const uint row);
    /**
     *  \brief Length of keyboard
     *  
     *  \return return length of keyboard
     *  
     *  \details Gets the length of the keyboard
     *  The length of a keyboard is the number of rows in this keyboard.
     */
    const int length ();

    /**
     *  \brief Gets value of OneTime
     *  
     *  \return Value of OneTime
     *  
     *  \details See OneTime, this methods makes it read only.
     */
    const bool getOneTime() {
      return OneTime;
    }
    /**
     *  \brief Gets value of Resize
     *  
     *  \return Value of Resize
     *  
     *  \details See Resize, this methods makes it read only.
     */
    const bool getResize() {
      return Resize;
    }

};

/**
   \class TelegramBotClient

   \file TelegramBotClient.h

   \brief Telegram Bot Client

   Client to access Telegram's Bot API

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/
class TelegramBotClient
{
  private:
    /** Id of last update, used to generate a call returning only
        messages more recent than the last received.
    */
    long LastUpdateId = 0;
    /** Secure Token provided by BotFather */
    String Token;
    /** Indicates if the client uses two underlying client objects
        allowing posting while keeping the poll call open in parallel.
    */
    bool Parallel = false;
    /** Underlying client for polling.*/
    JsonWebClient* SslPollClient;
    /** Underlying client for posting. In case of parallel mode it uses the
        same Client object than SslPollClient
    */
    JsonWebClient* SslPostClient;

    /**
        \brief Starts polling

        \return Nothing

        \details Starts the polling by open a http long call
    */
    void startPolling();
    /**
        \brief Starts posting a message

        \param [in] The Message to post as json string
        \return Nothing

        \details Start the posting of a message by
        open a http post call
    */
    void startPosting(String Message);
    /** Callback called on receiving a message */
    TBC_CALLBACK_RECEIVE_SIGNATURE;
    /** Callback called on error */
    TBC_CALLBACK_ERROR_SIGNATURE;
  public:
    /**
        \brief Constructor
        \details Constructor, initializing all members including callbacks
        using different clients for posting and polling
        \param token secure token for your bot provided by BotFather.
        \param sslPollClient SSL client used for polling messages from remote server
        \param sslPostClient SSL client used for posting messages to remote server
        \param TBC_CALLBACK_RECEIVE_SIGNATURE
        Callback called on receiving a message
        \param TBC_CALLBACK_ERROR_SIGNATURE
        Callback called on error while receiving
    */
    TelegramBotClient (
      String token,
      Client& sslPollClient,
      Client& sslPostClient,
      TBC_CALLBACK_RECEIVE_SIGNATURE,
      TBC_CALLBACK_ERROR_SIGNATURE
    );
    /**
        \brief Constructor
        \details Constructor, initializing only members no callbacks
          using different clients for posting and polling
        \param token secure token for your bot provided by BotFather.
        \param sslPollClient SSL client used for polling messages from remote server
        \param sslPostClient SSL client used for posting messages to remote server
    */
    TelegramBotClient (
      String token,
      Client& sslPollClient,
      Client& sslPostClient)
      : TelegramBotClient (token, sslPollClient, sslPostClient, 0, 0) {};
    /**
        \brief Constructor
        \details Constructor, initializing only members no callbacks
                 using the same client for posting and polling
        \param token secure token for your bot provided by BotFather.
        \param sslPollClient SSL client used for polling messages from remote server
        \param sslPostClient SSL client used for posting messages to remote server
    */
    TelegramBotClient (
      String token,
      Client& sslPollClient)
      : TelegramBotClient (token, sslPollClient, sslPollClient, 0, 0) {};
    /**
        \brief Destructor

        \details Destructor
    */
    ~TelegramBotClient();
    /**
        \brief Alias for setCallbacks following Arduino convention

        \param [in] TBC_CALLBACK_RECEIVE_SIGNATURE
        Callback called on receiving a message
        \param [in] TBC_CALLBACK_ERROR_SIGNATURE
        Callback called on error while receiving
        \return Nothing

        \details Alias for setCallbacks following Arduino convention
        sets callbacks
    */
    void begin(
      TBC_CALLBACK_RECEIVE_SIGNATURE,
      TBC_CALLBACK_ERROR_SIGNATURE);
    /**
        \brief Sets callbacks

        \param [in] TBC_CALLBACK_RECEIVE_SIGNATURE
        Callback called on receiving a message
        \param [in] TBC_CALLBACK_ERROR_SIGNATURE
        Callback called on error while receiving
        \return Nothing

        \details sets callbacks for receiving message and error handling
    */
    void setCallbacks(
      TBC_CALLBACK_RECEIVE_SIGNATURE,
      TBC_CALLBACK_ERROR_SIGNATURE);

    /**
        \brief Handles client background tasks

        \return Return true is an action was needed and performed

        \details Handles client background tasks, shall be calles in every main loop()
    */
    bool loop();
    /**
        \brief Post a message

        \param [in] chatId Id of the chat the message shall be sent to.
        \param [in] text Text of the message
        \param [in] keyBoard Optional. Keyboard to be send with this message.
        \return Nothing

        \details Post a message to a given chat. 
        (Only text messages and custom keyboards are supported, yet.)
    */
    void postMessage(long chatId, String text, TBCKeyBoard& keyBoard);
    /**
        \brief Post a message

        \param [in] chatId Id of the chat the message shall be sent to.
        \param [in] text Text of the message
        \return Nothing

        \details Post a message to a given chat. 
        (Only text messages and custom keyboards are supported, yet.)
    */

    void postMessage(long chatId, String text) {TBCKeyBoard keyBoard(0);
      postMessage(chatId, text, keyBoard);
    }
    /**
        \brief Callback called by JSONWebClient

        \param [in] err Error Code from JwcProcessError
        \param [in] json JsonObject generated by ArduinoJSON
        \return Nothing

        \details This is an internal method called by underlying JSONWebClient

        \note Do not call this method.
    */
    void pollSuccess(JwcProcessError err, JsonObject& json);
    /**
        \brief Callback called by JSONWebClient

        \param [in] err Error Code from JwcProcessError
        \param [in] client Client that causes the problem.
        \return Nothing

        \details This is an internal method called by underlying JSONWebClient

        \note Do not call this method.
    */
    void pollError(JwcProcessError err, Client* client);
    /**
        \brief Callback called by JSONWebClient

        \param [in] err Error Code from JwcProcessError
        \param [in] json JsonObject generated by ArduinoJSON
        \return Nothing

        \details This is an internal method called by underlying JSONWebClient

        \note Do not call this method.
    */
    void postSuccess(JwcProcessError err, JsonObject& json);
    /**
        \brief Callback called by JSONWebClient

        \param [in] err Error Code from JwcProcessError
        \param [in] client Client that causes the problem.
        \return Nothing

        \details This is an internal method called by underlying JSONWebClient

        \note Do not call this method.
    */
    void postError(JwcProcessError err, Client* client);

    static void callbackPollSuccess (void* obj, JwcProcessError err, JsonObject& json)
    {
      if (obj == 0) return;
      TelegramBotClient* botClient = (TelegramBotClient*)obj;
      botClient->pollSuccess(err, json);
    }
    static void callbackPollError(void* obj, JwcProcessError err, Client* client)
    {
      if (obj == 0) return;
      TelegramBotClient* botClient = (TelegramBotClient*)obj;
      botClient->pollError(err, client);
    }
    static void callbackPostSuccess (void* obj, JwcProcessError err, JsonObject& json)
    {
      if (obj == 0) return;
      TelegramBotClient* botClient = (TelegramBotClient*)obj;
      botClient->postSuccess(err, json);
    }
    static void callbackPostError(void* obj, JwcProcessError err, Client* client)
    {
      if (obj == 0) return;
      TelegramBotClient* botClient = (TelegramBotClient*)obj;
      botClient->postError(err, client);
    }
};

#endif



