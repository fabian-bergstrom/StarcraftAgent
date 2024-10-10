#!/bin/sh

mkdir build
cd build
"C:\Program Files\CMake\bin\cmake" .. -G "Visual Studio 15 Win64" -DPYTHON_EXECUTABLE:FILEPATH="C:/Program Files/Python39/python.exe"
