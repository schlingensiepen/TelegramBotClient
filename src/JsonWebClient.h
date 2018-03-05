#pragma once
#ifndef JsonWebClient_h
#define JsonWebClient_h

#include "TBCDebug.h"
#include "Arduino.h"
#include <Client.h>
#include <ArduinoJson.h>

#ifndef JWC_BUFF_SIZE
#ifdef ESP8266
#define JWC_BUFF_SIZE 1000
#else
#define JWC_BUFF_SIZE 10000
#endif
#endif

enum class JwcProcessError : int
{
  Ok = 0,
  HttpErr    = -1,    // Not found HTTP 200 Header --> Server Error
  MsgTooBig  = -2,    // Message bigger than JWC_BUFF_SIZE adjust JWC_BUFF_SIZE to avoid this, beware ArduinoJSON still needs to fit to your device's memory
  MsgJsonErr = -3     // ArduinoJSON was not able to parse the message
};



// Inspired by PubSubClient by Nick O'Leary (http://knolleary.net)
#ifdef ESP8266
#include <functional>
#define JWC_CALLBACK_MESSAGE_SIGNATURE std::function<void(void*, JwcProcessError, JsonObject&)> callbackSuccess
#define JWC_CALLBACK_ERROR_SIGNATURE std::function<void(void*, JwcProcessError, Client*)> callbackError
#else
#define JWC_CALLBACK_MESSAGE_SIGNATURE void (*callbackSuccess)(void*, JwcProcessError, JsonObject&)
#define JWC_CALLBACK_ERROR_SIGNATURE void (*callbackError)(void*, JwcProcessError, Client*)
#endif

enum class JwcClientState : int
{
  unconnected = 0,
  connected = 1,
  waiting = 2,
  headers = 3,
  json = 4
};

class JsonWebClient
{
  private:
    JwcClientState State = JwcClientState::unconnected;  
    Client* NetClient;
    String Host;
    int Port;
    long ContentLength = JWC_BUFF_SIZE;
    bool HttpStatusOk = false;
    void reConnect();
    void* CallBackObject;
    JWC_CALLBACK_MESSAGE_SIGNATURE;
    JWC_CALLBACK_ERROR_SIGNATURE;
    bool processHeader();
    bool processJson();
    
  public:
    JsonWebClient (
      Client* netClient, 
      String host, 
      int port,
      void* callBackObject,
      JWC_CALLBACK_MESSAGE_SIGNATURE, 
      JWC_CALLBACK_ERROR_SIGNATURE);
      
      bool fire (String commands[], int count);
      JwcClientState state();
      bool loop();
      bool stop();
};
#endif
