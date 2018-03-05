#include "TelegramBotClient.h"

TelegramBotClient::TelegramBotClient (
  String token,
  Client& sslPollClient,
  Client& sslPostClient,
  TBC_CALLBACK_RECEIVE_SIGNATURE,
  TBC_CALLBACK_ERROR_SIGNATURE
)
{
  DOUT ("New TelegramBotClient");
  this->Parallel = (sslPostClient != sslPollClient);
  this->SslPollClient = new JsonWebClient(
    &sslPollClient, TELEGRAMHOST, TELEGRAMPORT, this,
    callbackPollSuccess, callbackPollError);
  this->SslPostClient = new JsonWebClient(
    &sslPostClient, TELEGRAMHOST, TELEGRAMPORT, this,
    callbackPostSuccess, callbackPostError);
  this->Token = String(token);
  DOUTKV ("Token", this->Token);
  this->setCallbacks(
    callbackReceive,
    callbackError);
}
TelegramBotClient::~TelegramBotClient()
{
  delete( SslPollClient );
  delete( SslPostClient );
}

void TelegramBotClient::setCallbacks (
  TBC_CALLBACK_RECEIVE_SIGNATURE,
  TBC_CALLBACK_ERROR_SIGNATURE)
{
  DOUT ("setCallbacks");
  this->callbackReceive = callbackReceive;
  this->callbackError = callbackError;
}

void TelegramBotClient::begin(
  TBC_CALLBACK_RECEIVE_SIGNATURE,
  TBC_CALLBACK_ERROR_SIGNATURE)
{
  setCallbacks(
    callbackReceive,
    callbackError);
}


bool TelegramBotClient::loop()
{
  SslPollClient->loop();
  SslPostClient->loop();

  if (
    SslPollClient->state() == JwcClientState::unconnected
    &&
    ( SslPostClient-> state() == JwcClientState::unconnected
      || Parallel
    ))
  {
    startPolling();
    return true;
  }
  return false;
}

void TelegramBotClient::startPolling()
{
  DOUT("startPolling");
  String httpCommands[] =
  {
    "GET /bot"
    + String(Token)
    + "/getUpdates?limit=1&offset="
    + String(LastUpdateId)
    + "&timeout="
    + String(POLLINGTIMEOUT)
    + " HTTP/1.1",

    "User-Agent: " + String(USERAGENTSTRING),

    "Host: " + String(TELEGRAMHOST),

    "Accept: */*",

    "" // indicate end of headers with empty line (http)
  };
  SslPollClient->fire (httpCommands, 5);
}

String toString(const char* tmp)
{
  if (tmp == 0) return String();
  return String(tmp);
}

void TelegramBotClient::pollSuccess(JwcProcessError err, JsonObject& payload)
{
  DOUT("pollSuccess");
  if (!payload["ok"])
  {
    DOUT("Skip message, Server error");
    if (callbackError != 0)
      callbackError(TelegramProcessError::RetPollErr, err);
    return;
  }
  Message* msg = new Message();
  msg->update_id = payload["result"][0]["update_id"];
  DOUTKV("update_id", msg->update_id);
  LastUpdateId = msg->update_id + 1;
  msg->message_id = payload["result"][0]["message"]["message_id"];
  DOUTKV("message_id", msg->message_id);
  msg->from_id = payload["result"][0]["message"]["from"]["id"];
  DOUTKV("from_id", msg->from_id);
  msg->from_is_bot = payload["result"][0]["message"]["from"]["is_bot"];
  DOUTKV("from_is_bot", msg->from_is_bot);
  msg->from_first_name = toString(payload["result"][0]["message"]["from"]["first_name"]);
  DOUTKV("from_first_name", msg->from_first_name);
  msg->from_last_name = toString(payload["result"][0]["message"]["from"]["last_name"]);
  DOUTKV("from_last_name", msg->from_last_name);
  msg->from_language_code = toString(payload["result"][0]["message"]["from"]["language_code"]);
  DOUTKV("from_language_code", msg->from_language_code);
  msg->chat_id = payload["result"][0]["message"]["chat"]["id"];
  DOUTKV("chat_id", msg->chat_id);
  msg->chat_first_name = toString(payload["result"][0]["message"]["chat"]["first_name"]);
  DOUTKV("chat_first_name", msg->chat_first_name);
  msg->chat_last_name = toString(payload["result"][0]["message"]["chat"]["last_name"]);
  DOUTKV("chat_last_name", msg->chat_last_name);
  msg->chat_type = toString(payload["result"][0]["message"]["chat"]["type"]);
  DOUTKV("chat_type", msg->chat_type);
  msg->text = toString(payload["result"][0]["message"]["text"]);
  DOUTKV("text", msg->text);
  msg->date = payload["result"][0]["message"]["date"];
  DOUTKV("date", msg->date);
  if (msg->from_id == 0 || msg->chat_id == 0 || msg->text.length() == 0)
  {
    // no message, just the timeout from server
    DOUT("Timout by server");
  }
  else
  {
    if (callbackReceive != 0)
    {
      callbackReceive(TelegramProcessError::Ok, err, msg);
    }
  }
  delete (msg);
}
void TelegramBotClient::pollError(JwcProcessError err, Client* client)
{
  DOUT("pollError");
  switch (err)
  {
    case JwcProcessError::HttpErr: {
        if (callbackError != 0) callbackError(TelegramProcessError::JcwPollErr, err); break;
      }
    case JwcProcessError::MsgTooBig: {
        if (callbackError != 0) callbackError(TelegramProcessError::JcwPollErr, err);

        // This should find the update_id in JSON:
        // {"ok":true,"result":[{"update_id":512650849, ...
        String token = "";
        do
        {
          token = client->readStringUntil(',');
          DOUTKV ("Token", token);
        } while (token.indexOf("update_id") <= 0);
        token = token.substring(token.lastIndexOf(":") + 1);
        DOUTKV ("Token", token);
        LastUpdateId = token.toInt() + 1;
        DOUTKV ("LastUpdateId", LastUpdateId);
        break;
      }
    case JwcProcessError::MsgJsonErr: {
        if (callbackError != 0) callbackError(TelegramProcessError::JcwPollErr, err);
        //TODO: Try to get LastUpdateId somehow
        LastUpdateId++;
        break;
      }
  }
}

void TelegramBotClient::startPosting(String msg) {
  if (!Parallel) SslPollClient->stop();

  String httpCommands[] =
  {
    "POST /bot"
    + String(Token)
    + "/sendMessage"
    + " HTTP/1.1",

    "Host: " + String(TELEGRAMHOST),

    "User-Agent: " + String(USERAGENTSTRING),

    "Content-Type: application/json",

    "Connection: close",

    "Content-Length: " + String (msg.length()),

    "", // indicate end of headers by empty line --> http

    msg
  };
  SslPostClient->fire(httpCommands, 8);
}


void TelegramBotClient::postMessage(long chatId, String text)
{
  if (chatId == 0) {
    DOUT("Chat not defined.");
    return;
  }

  DOUT("postMessage");
  DOUTKV("chatId", chatId);
  DOUTKV("text", text);

  DynamicJsonBuffer jsonBuffer (JWC_BUFF_SIZE);
  JsonObject& obj = jsonBuffer.createObject();
  obj["chat_id"] = chatId;
  obj["text"] = text;

  String payload;
  obj.printTo(payload);
  startPosting(payload);

}

void TelegramBotClient::postSuccess(JwcProcessError err, JsonObject& json)
{
  DOUT("postSuccess");
  json.printTo(Serial);
}
void TelegramBotClient::postError(JwcProcessError err, Client* client)
{
  DOUT("postError");
  while (client->available() > 0)
  {
    String line = client->readStringUntil('\n');
    DOUTKV("line", line);
  }
}


