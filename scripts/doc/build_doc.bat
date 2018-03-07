rd /s /q "C:\Work\TelegramBotClient\trunk\doc"
rd /s /q "C:\temp\doxout"
md "C:\temp\doxout"
"C:\tools\Doxygen\doxygen.exe" "C:\Work\TelegramBotClient\trunk\scripts\doc\TelegramBot"
xcopy /s /e "C:/temp/doxout/html" "C:\Work\TelegramBotClient\trunk\doc"
pause