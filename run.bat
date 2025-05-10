@echo off
g++ -std=c++11 src/main.cpp src/vocabulary.cpp -o vocab_app.exe -Iinclude
vocab_app.exe
pause