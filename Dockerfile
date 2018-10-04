# FROM gcc:4.9 AS base
# FROM frolvlad/alpine-gxx
# FROM alpine:3.7
FROM ubuntu:16.04

RUN apt-get update && apt-get install -y build-essential

RUN apt-get install -y cmake

COPY . ./FlexibleComputerLanguage

WORKDIR ./FlexibleComputerLanguage

RUN cmake .

RUN cmake -version

RUN make

CMD [ "./FlexibleComputerLanguage1" ]
