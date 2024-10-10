#!/bin/sh

mkdir build37
cd build37
"C:\Program Files\CMake\bin\cmake" .. -G "Visual Studio 15 Win64" -DPYTHON_EXECUTABLE:FILEPATH="C:/Program Files/Python37/python.exe"
