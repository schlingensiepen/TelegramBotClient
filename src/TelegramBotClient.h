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

enum class TelegramProcessError : int
{
  Ok = 0,
  JcwPollErr = -1,   // JSONWebClient host returns error while polling
  JcwPostErr = -2,   // JSONWebClient host returns error while posting  
  RetPollErr = -3,   // Telegram host returns error while polling
  RetPostErr = -4    // Telegram host returns error while posting  
};

struct Message{
  long update_id;
  long message_id;
  long from_id;
  bool from_is_bot;
  String from_first_name;
  String from_last_name;
  String from_language_code; 
  long chat_id;
  String chat_first_name;
  String chat_last_name;
  String chat_type;
  String text;
  long date;
};

struct TBCKeyBoardRow
{
  int Count;
  String Buttons[];
};

struct TBCKeyBoard
{
  int Count;
  TBCKeyBoardRow Rows[];
};


class TelegramBotClient
{
    private:
    long LastUpdateId = 0;
    String Token;
    bool Parallel = false;
    JsonWebClient* SslPollClient;
    JsonWebClient* SslPostClient;

    void startPolling();
    void startPosting(String Message);

    TBC_CALLBACK_RECEIVE_SIGNATURE;
    TBC_CALLBACK_ERROR_SIGNATURE;
    
  public:
    TelegramBotClient (
      String token, 
      Client& sslPollClient, 
      Client& sslPostClient,
      TBC_CALLBACK_RECEIVE_SIGNATURE,
      TBC_CALLBACK_ERROR_SIGNATURE
    );
    TelegramBotClient (
      String token, 
      Client& sslPollClient, 
      Client& sslPostClient)
      :TelegramBotClient (token, sslPollClient, sslPostClient, 0,0){};

    TelegramBotClient (
      String token, 
      Client& sslPollClient)
      :TelegramBotClient (token, sslPollClient, sslPollClient,0,0){};
    
    ~TelegramBotClient();

    void begin(
      TBC_CALLBACK_RECEIVE_SIGNATURE,
      TBC_CALLBACK_ERROR_SIGNATURE);
      
    void setCallbacks(
      TBC_CALLBACK_RECEIVE_SIGNATURE,
      TBC_CALLBACK_ERROR_SIGNATURE);

    bool loop();
    void postMessage(long chatId, String text);

    void pollSuccess(JwcProcessError err, JsonObject& json);
    void pollError(JwcProcessError err, Client* client);
    void postSuccess(JwcProcessError err, JsonObject& json);
    void postError(JwcProcessError err, Client* client);
    
    static void callbackPollSuccess (void* obj, JwcProcessError err, JsonObject& json)
      { if (obj == 0) return; TelegramBotClient* botClient = (TelegramBotClient*)obj; botClient->pollSuccess(err,json);}
    static void callbackPollError(void* obj, JwcProcessError err, Client* client)
      { if (obj == 0) return; TelegramBotClient* botClient = (TelegramBotClient*)obj; botClient->pollError(err, client); }      
    static void callbackPostSuccess (void* obj, JwcProcessError err, JsonObject& json)
      { if (obj == 0) return; TelegramBotClient* botClient = (TelegramBotClient*)obj; botClient->postSuccess(err,json); }
    static void callbackPostError(void* obj, JwcProcessError err, Client* client)
      { if (obj == 0) return; TelegramBotClient* botClient = (TelegramBotClient*)obj; botClient->postError(err, client); }
};

#endif



