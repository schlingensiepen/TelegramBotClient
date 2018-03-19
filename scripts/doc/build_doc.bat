rd /s /q "C:\Work\TelegramBotClient\trunk\docs"
rd /s /q "C:\temp\doxout"
md "C:\temp\doxout"
pause
"C:\tools\Doxygen\doxygen.exe" "C:\Work\TelegramBotClient\trunk\scripts\doc\TelegramBot"
xcopy /s /e "C:/temp/doxout/html" "C:\Work\TelegramBotClient\trunk\docs"
pause
cd "C:/temp/doxout/latex"
call make.bat
copy refman.pdf C:\Work\TelegramBotClient\trunk\docs\TelegramBotClient.pdf
pause
python %~dp0\fixPrefixes.py "C:\Work\TelegramBotClient\trunk\docs"
pause