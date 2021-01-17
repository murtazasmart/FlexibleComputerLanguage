# FROM gcc:4.9 AS base
# FROM frolvlad/alpine-gxx
# FROM alpine:3.7
FROM ubuntu:18.04 AS build

RUN apt-get update && apt-get install -y build-essential

RUN apt-get install -y cmake gcc g++ git libsasl2-dev libssl-dev libsnappy-dev make pkg-config tar wget python3

RUN apt-get upgrade -y

# Installing monogocxx,mongoc is first installed and then followed by mongocxx installation
# DCMAKE_INSTALL_PREFIX sets the installation path. Should be reflected in CMakeLists file accordingly
RUN cd ~ \
    && wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.2/mongo-c-driver-1.17.2.tar.gz \
    && tar -xzvf mongo-c-driver-1.17.2.tar.gz \
    && cd mongo-c-driver-1.17.2 \
    && mkdir cmake-build \
    && cd cmake-build \
    && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
    && make \
    && make install \
    && cd ../build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && cd ~ \
    && rm mongo-c-driver-1.17.2.tar.gz \
    && rm -rf mongo-c-driver-1.17.2 \
    && git clone https://github.com/mongodb/mongo-cxx-driver.git \
    && cd mongo-cxx-driver \
    && git checkout r3.6.2 \
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

# Install runtime dependencies,these are needed in addition to the build dependencies 
RUN apt-get update && apt-get install -y libsasl2-dev libssl-dev libssl1.0.0 libsnappy-dev && ls -l && export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig"

CMD [ "./FlexibleComputerLanguage1" ]
