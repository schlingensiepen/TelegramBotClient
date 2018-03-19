/**
    ESP_EchoBotKeyboard
    Example show how to receive and send messages via Telegram's Bot API.
    It also demonstrates how to use a custom keyboard shown to the user
  
    Part of TelegramBotClient (https://github.com/schlingensiepen/TelegramBotClient)    
    Jörn Schlingensiepen <joern@schlingensiepen.com>

    Client's API:   https://schlingensiepen.github.io/TelegramBotClient/
    Telegram's API: https://core.telegram.org/bots/api
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <TelegramBotClient.h>

// Instantiate Wifi connection credentials
const char* ssid     = "digitalisierung";
const char* password = "cloudification";

// Instantiate Telegram Bot secure token
// This is provided by BotFather
const String botToken = "YOUR BOT TOKEN";

// Instantiate the ssl client used to communicate with Telegram's web API
WiFiClientSecure sslPollClient;

// Instantiate the client with secure token and client
TelegramBotClient client(
      botToken, 
      sslPollClient);
      
// Instantiate a keybord with 3 rows
TBCKeyBoard board(3);

// Function called on receiving a message
void onReceive (TelegramProcessError tbcErr, JwcProcessError jwcErr, Message* msg)
{      
    Serial.println("onReceive");
    Serial.print("tbcErr"); Serial.print((int)tbcErr); Serial.print(":"); Serial.println(toString(tbcErr));
    Serial.print("jwcErr"); Serial.print((int)jwcErr); Serial.print(":"); Serial.println(toString(jwcErr));
  
    Serial.print("UpdateId: "); Serial.println(msg->UpdateId);      
    Serial.print("MessageId: "); Serial.println(msg->MessageId);
    Serial.print("FromId: "); Serial.println(msg->FromId);
    Serial.print("FromIsBot: "); Serial.println(msg->FromIsBot);
    Serial.print("FromFirstName: "); Serial.println(msg->FromFirstName);
    Serial.print("FromLastName: "); Serial.println(msg->FromLastName);
    Serial.print("FromLanguageCode: "); Serial.println(msg->FromLanguageCode); 
    Serial.print("ChatId: "); Serial.println(msg->ChatId);
    Serial.print("ChatFirstName: "); Serial.println(msg->ChatFirstName);
    Serial.print("ChatLastName: "); Serial.println(msg->ChatLastName);
    Serial.print("ChatType: "); Serial.println(msg->ChatType);
    Serial.print("Text: "); Serial.println(msg->Text);
    Serial.print("Date: "); Serial.println(msg->Date);

    // Sending the text of received message back to the same chat
    // and add the custom keyboard to the message
    // chat is identified by an id stored in the ChatId attribute of msg    
    client.postMessage(msg->ChatId, msg->Text, board);
    
}

// Function called if an error occures
void onError (TelegramProcessError tbcErr, JwcProcessError jwcErr)
{
  Serial.println("onError");
  Serial.print("tbcErr"); Serial.print((int)tbcErr); Serial.print(":"); Serial.println(toString(tbcErr));
  Serial.print("jwcErr"); Serial.print((int)jwcErr); Serial.print(":"); Serial.println(toString(jwcErr));
}

// Setup WiFi connection using credential defined at begin of file
void setupWiFi()
{
  Serial.println();
  Serial.printf("Try to connect to network %s ",ssid);
  Serial.println();
    
  WiFi.begin(ssid, password);
  Serial.print(".");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("OK");
  Serial.print("IP address .: ");
  Serial.println(WiFi.localIP());

  Serial.print("Strength ...: ");
  Serial.println(WiFi.RSSI());
  Serial.println();   
}

// Setup
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  setupWiFi();

  // Adding the 3 rows to the keyboard
  String row1[] = {"A1", "A2"};
  String row2[] = {"B1", "B2" , "B3", "B4"};
  String row3[] = {"C1", "C2" , "C3"};

  // push() always returns the keyboard, so pushes can be chained 
  board
    .push(2, row1)
    .push(4, row2)
    .push(3, row3);
  
  // Sets the functions implemented above as so called callback functions,
  // thus the client will call this function on receiving data or on error.
  client.begin(      
      onReceive,
      onError);    
}

// Loop
void loop() {
  // To process receiving data this method has to be called each main loop()
  client.loop();
}
