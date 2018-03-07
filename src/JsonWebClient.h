/**
    \file JsonWebClient.h
    \brief Header of a simple web client receiving json
           uses an underlying implementation of Client interface.
           It implements a pseudo background behavior by providing a loop()
           method that can be polled and calls callback on receiving valid data.

    Part of TelegramBotClient (https://github.com/schlingensiepen/TelegramBotClient)
    Jörn Schlingensiepen <joern@schlingensiepen.com>
*/

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


/**
   \class JwcProcessError
   @enum mapper::JwcProcessError

   \file JsonWebClient.h

   \brief JwcProcessError state = JwcProcessError::Ok;

   Enumeration to indicate internal process state of JsonWebClient.

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/

enum class JwcProcessError : int
{
  /** Everything Ok, no error */
  Ok = 0,
  /** Not found HTTP 200 Header --> Server Error */
  HttpErr    = -1,
  /** Message bigger than JWC_BUFF_SIZE adjust JWC_BUFF_SIZE to avoid this,
      beware ArduinoJSON still needs to fit to your device's memory */
  MsgTooBig  = -2,
  /** ArduinoJSON was not able to parse the message */
  MsgJsonErr = -3
};

/** Static list of JwcProcessError names */
static String JwcProcessErrorString[] = {"Ok", "HttpErr", "MsgTooBig", "MsgJsonErr"};

static String toString(JwcProcessError err)
{
  return JwcProcessErrorString[(-1) * (int) err];
}



// Inspired by PubSubClient by Nick O'Leary (http://knolleary.net)
#ifdef ESP8266
#include <functional>
#define JWC_CALLBACK_MESSAGE_SIGNATURE std::function<void(void*, JwcProcessError, JsonObject&)> callbackSuccess
#define JWC_CALLBACK_ERROR_SIGNATURE std::function<void(void*, JwcProcessError, Client*)> callbackError
#else
#define JWC_CALLBACK_MESSAGE_SIGNATURE void (*callbackSuccess)(void*, JwcProcessError, JsonObject&)
#define JWC_CALLBACK_ERROR_SIGNATURE void (*callbackError)(void*, JwcProcessError, Client*)
#endif

/**
   \class JwcClientState

   \file JsonWebClient.h

   \brief JwcClientState state = JwcClientState::Unconnected;

   Enumeration to indicate internal process state of JsonWebClient.

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/
enum class JwcClientState : int
{
  /** Client is not connected */
  Unconnected = 0,
  /** Client is connected but no command was sent. */
  Connected = 1,
  /** Client is waiting for response from server. */
  Waiting = 2,
  /** Client is processing headers. */
  Headers = 3,
  /*! Client is processing json from response */
  Json = 4
};

/** Static list of JwcClientState names */
static String JwcClientStateString[] = {"Unconnected", "Connected", "Waiting", "Headers", "Json"};

static String toString(JwcClientState state)
{
  return JwcClientStateString[(int) state];
}


/**
   \class JsonWebClient

   \file JsonWebClient.h

   \brief JSONWebClient (netClient, "www.example.com", 80,
              CallBackObject, callBackMessage, callBackError);

   This class implements a minimum http client to receive json data
   from a host. It uses an underlying implementation of Client interface
   and can be used with raw client or ssl client.

   \note Should only be used as a part of TelegramBotClient
   (https://github.com/schlingensiepen/TelegramBotClient)

   \author Jörn Schlingensiepen <joern@schlingensiepen.com>

*/
class JsonWebClient
{
  private:
    /** Current state of the client */
    JwcClientState State = JwcClientState::Unconnected;
    /** Client used to access the net (depends on hardware) */
    Client* NetClient;
    /** Host to connect to */
    String Host;
    /** Port to connect to */
    int Port;
    /** Content length stored during header processing */
    long ContentLength = JWC_BUFF_SIZE;
    /** Indicate if Http 200 Ok header was found */
    bool HttpStatusOk = false;
    /**
        \brief Reconnects to host

        \return Return nothing

        \details Reconnects to host, skips open connection
    */
    void reConnect();
    /** Object passed to the callbacks */
    void* CallBackObject;
    /** Callback called on receiving a message / valid json data */
    JWC_CALLBACK_MESSAGE_SIGNATURE;
    /** Callback called on error while receiving */
    JWC_CALLBACK_ERROR_SIGNATURE;
    /**
        \brief Process a header

        \return Returns true while headers found in underlying Client

        \details Read a header from NetClient and process it.
    */
    bool processHeader();
    /**
        \brief Process JSON

        \return Returns true on success

        \details Reads data from underlying Client and process it by ArduinoJSON
    */
    bool processJson();

  public:
    /**
      Constructor, initializing all members
      \param netClient a object implementing Client interface to access the network.
      Using a Client implementing ssl feature will result in https otherwise http.
      \param host Host to connect to
      \param port Port to connect to
      \param callBackObject Object passed to the callbacks, shall not be 0
      \param JWC_CALLBACK_MESSAGE_SIGNATURE
      Callback called on receiving a message / valid json data
      \param JWC_CALLBACK_ERROR_SIGNATURE
      Callback called on error while receiving
    */
    JsonWebClient (
      Client* netClient,
      String host,
      int port,
      void* callBackObject,
      JWC_CALLBACK_MESSAGE_SIGNATURE,
      JWC_CALLBACK_ERROR_SIGNATURE);
    /**
        \brief Executes a list of commands

        \param [in] commands[] list of commands
        \param [in] count of commands
        \return Return true on success

        \details Sends a list of commands to the server by
        calling println() for each command and flush() at the
        end of list. The commands shall follow the http protocol.
    */
    bool fire (String commands[], int count);
    /**
        \brief Current state of the client

        \return The current state as a JwcClientState

        \details Make the current state of the client public accessible.
    */
    JwcClientState state();
    /**
        \brief Method to poll client processing.

        \return True is an internal action was executed.

        \details Method to poll client processing,
           shall be called in each main loop()
    */
    bool loop();
    /**
        \brief Stops the client

        \return True

        \details Stops the underlying client connection and
          reset client state to JwcClientState::unconnected
    */
    bool stop();
};
#endif
