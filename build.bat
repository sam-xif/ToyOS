@echo off
:: Set path to only include necessary commands to prevent bugs
set PATH=C:\MinGW\64\bin;C:\bash;C:\Program Files (x86)\NASM
mingw32-make -B %1
