# FROM gcc:4.9 AS base
# FROM frolvlad/alpine-gxx
# FROM alpine:3.7
FROM ubuntu:16.04 AS build

RUN apt-get update && apt-get install -y build-essential

RUN apt-get install -y cmake

COPY . ./FlexibleComputerLanguage

WORKDIR ./FlexibleComputerLanguage

RUN rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake

RUN cmake -version

RUN cmake .

RUN make

FROM ubuntu:18.04
# FROM alpine:3.7

COPY --from=build ./FlexibleComputerLanguage/FlexibleComputerLanguage1 ./myapp/FlexibleComputerLanguage1

COPY --from=build ./FlexibleComputerLanguage/Defs.txt ./myapp/Defs.txt

WORKDIR ./myapp

# RUN apk upgrade

# RUN apk add --update \
#     libc6-compat \
#     libstdc++
    # g++

RUN ls

CMD [ "./FlexibleComputerLanguage1" ]
