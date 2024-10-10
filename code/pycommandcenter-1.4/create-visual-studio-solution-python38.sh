#!/bin/sh

mkdir build38
cd build38
"C:\Program Files\CMake\bin\cmake" .. -G "Visual Studio 15 Win64" -DPYTHON_EXECUTABLE:FILEPATH="C:/Program Files/Python38/python.exe"
