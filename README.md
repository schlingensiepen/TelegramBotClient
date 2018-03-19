# TelegramBotClient
This library provides a client for accessing Telegram's Bot API
(https://core.telegram.org/bots/api). 
It uses a pseudo-background mechanism inspired by Nick O'Leary's
PubSubClient (https://github.com/knolleary/pubsubclient)
using long polls via https against Telegram's Bot API.
Client is set up with the security token provided by BotFather
and some callbacks. Thus it shall be polled by calling `.loop()` 
in each main `loop()`.

## Documentation
Full documentation of API will can be found on gitHub: 

https://schlingensiepen.github.io/TelegramBotClient/

https://schlingensiepen.github.io/TelegramBotClient/TelegramBotClient.pdf

## Limitations
- The client uses underlying SSL-Client objects which may consume a lot of
memory, thus a mode using one underlying client is implemented that skips
running receive polls to post messages (No message lost!).
- The Library uses ArduinoJSON (https://arduinojson.org/) that use to
keep the full message in a memory block while parsing it. The block size is
set by `#define JWC_BUFF_SIZE` in `JsonWebClient.h`.
- The current version does not provide custom KeyBoards in Telegram (see road map).

## Compatible Hardware
The Library uses the client interface assuming a SSL implementation. Thus it shall
work with all plattforms supported by `WiFiClientSecure.h` for ESP or
WiFi101 library by Arduino. Please open an issue in gitHub on problems with specific
platforms.

## Road Map
- 0.5.0 Support Telegram custom keyboards
- 0.6.0 Full Documentation in src, source examples
- 0.7.0 Check for memory leaks
- 0.8.0 Test other plattforms
- 1.0.0 Tested Version feature complete
- 1.x.x Adding other media types

## License
This code is released under the MIT License