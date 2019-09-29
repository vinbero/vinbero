

FROM alpine:latest
MAINTAINER Byeonggon Lee (gonny952@gmail.com)

RUN apk update && apk add git cmake automake autoconf libtool make gcc musl-dev cmocka-dev jansson-dev openssl openssl-dev clang yaml-dev
RUN mkdir -p /usr/src

RUN git clone -j8 --recurse-submodules -b master https://github.com/gonapps-org/libgenc /usr/src/libgenc
RUN git clone -j8 --recurse-submodules -b master https://github.com/gonapps-org/libgaio /usr/src/libgaio
RUN git clone -j8 --recurse-submodules -b master https://github.com/gonapps-org/libfastdl /usr/src/libfastdl
RUN git clone -j8 --recurse-submodules -b master https://github.com/vinbero/libvinbero_com /usr/src/libvinbero_com
RUN git clone -j8 --recurse-submodules -b master https://github.com/vinbero/vinbero-ifaces /usr/src/vinbero-ifaces
RUN git clone -j8 --recurse-submodules -b master https://github.com/vinbero/vinbero /usr/src/vinbero

RUN mkdir /usr/src/libgenc/build; cd /usr/src/libgenc/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ..; make; make test; make install
RUN mkdir /usr/src/libgaio/build; cd /usr/src/libgaio/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ..; make; make test; make install
RUN mkdir /usr/src/libfastdl/build; cd /usr/src/libfastdl/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ..; make; make test; make install
RUN mkdir /usr/src/libvinbero_com/build; cd /usr/src/libvinbero_com/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ..; make; make test; make install
RUN mkdir /usr/src/vinbero-ifaces/build; cd /usr/src/vinbero-ifaces/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ..; make; make test; make install
RUN mkdir /usr/src/vinbero/build; cd /usr/src/vinbero/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Release ..; make; make test; make install

