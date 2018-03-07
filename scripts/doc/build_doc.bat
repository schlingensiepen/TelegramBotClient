rd /s /q "C:\Work\TelegramBotClient\trunk\docs"
rd /s /q "C:\temp\doxout"
md "C:\temp\doxout"
"C:\tools\Doxygen\doxygen.exe" "C:\Work\TelegramBotClient\trunk\scripts\doc\TelegramBot"
xcopy /s /e "C:/temp/doxout/html" "C:\Work\TelegramBotClient\trunk\docs"
pause