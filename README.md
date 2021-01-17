# FlexibleComputerLanguage

## Introduction

This project is based of Flexible Computer Language i.e. Query Language used for log analysis. This was then augmented to suite Tracified's requirements. Thus Tracified Grammar was born. 

## Prerequisites
 - CMake and Make will be required to build this project.
 - Current version version is tested and stable on Linux environment, Windows enviroment has various compatibility issues.
 - An IDE of your choice, we do recommend the use of CLion.

## Getting started
 - Clone of this project.
 - Set the environment variables in a .env file. Environment variables currently include file names for the Named Pipes
 - Run the followin commands.

    ```> cmake .```

    ```> make```
 - These commands will build the binary FlexibleComputerLanguage1. Go ahead and run that.

    ```> ./FlexibibleComputerLanguage1```

## Project strucutre

## Diving deep
TBD

## Third party libraries used
 - rapidJSON
 - dotenv.h
 - easylogging++
 - json.hpp (deprecated)

## Containerization
Docker has build used to containerize the application. The Dockerfile has been written. One thing to keep in mind is the build image is more than 100MB.

## DevOps
 - Jenkinsfile is present. The process is self explantory once you go through it.
 - Ansible files have been written for staging and production environments. This enables Jenkins to auto deploy code to those environments.

## Note

 - For the logger to work use:
  - Add the following to CMakeLists ```SET(CMAKE_CXX_FLAGS -pthread)```