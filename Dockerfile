# FROM gcc:4.9 AS base
# FROM frolvlad/alpine-gxx
# FROM alpine:3.7
FROM ubuntu:16.04 AS build

RUN apt-get update && apt-get install -y build-essential

RUN apt-get install -y cmake gcc git libsasl2-dev libsasl2-2 libssl-dev libsnappy-dev make pkg-config tar wget libmongoc-dev libbson-dev

RUN apt-get upgrade -y

RUN cd ~ \
    && wget https://github.com/mongodb/mongo-c-driver/releases/download/1.14.0/mongo-c-driver-1.14.0.tar.gz \
    && tar -xzvf mongo-c-driver-1.14.0.tar.gz \
    && cd mongo-c-driver-1.14.0 \
    && mkdir cmake-build \
    && cd cmake-build \
    && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
    && make \
    && make install \
    && cd ../build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && cd ~ \
    && rm mongo-c-driver-1.14.0.tar.gz \
    && rm -rf mongo-c-driver-1.14.0 \
    && git clone https://github.com/mongodb/mongo-cxx-driver.git \
    && cd mongo-cxx-driver \
    && git checkout r3.4.0 \
    && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make EP_mnmlstc_core \
    && make \
    && make install \
    && cd ~ \
    && rm -rf mongo-cxx-driver \
    && cd /usr/local

COPY . ./FlexibleComputerLanguage

WORKDIR ./FlexibleComputerLanguage

RUN rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake

RUN tail -n 10 CMakeLists.txt

RUN cmake -version

RUN cmake .

RUN make

FROM ubuntu:18.04
# FROM alpine:3.7

COPY --from=build ./FlexibleComputerLanguage/FlexibleComputerLanguage1 ./myapp/FlexibleComputerLanguage1

COPY --from=build ./FlexibleComputerLanguage/Defs.txt ./myapp/Defs.txt

COPY --from=build /usr/local/ /usr/local/

WORKDIR ./myapp

# RUN apk upgrade

# RUN apk add --update \
#     libc6-compat \
#     libstdc++
    # g++

RUN apt-get update && apt-get install -y libsasl2-dev libssl-dev libssl1.0.0 libsnappy-dev && ls -l && export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig"

CMD [ "./FlexibleComputerLanguage1" ]
