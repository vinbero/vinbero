FROM alpine:latest
MAINTAINER Byeonggon Lee (gonny952@gmail.com)

RUN apk update && apk add git cmake automake autoconf libtool make gcc musl-dev cmocka-dev jansson-dev uthash-dev

RUN git clone https://github.com/vinbero/libgenc
RUN git clone https://github.com/vinbero/libgaio
RUN git clone https://github.com/vinbero/libfastdl
RUN git clone https://github.com/vinbero/libvinbero_common
RUN git clone https://github.com/vinbero/vinbero-interfaces
RUN git clone https://github.com/vinbero/vinbero

RUN mkdir libgenc/build; cd libgenc/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..; make; make test; make install

RUN mkdir libgaio/build; cd libgaio/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..; make; make test; make install

RUN mkdir libfastdl/build; cd libfastdl/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..; make; make test; make install

RUN mkdir libvinbero_common/build; cd libvinbero_common/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..; make; make test; make install

RUN mkdir vinbero-interfaces/build; cd vinbero-interfaces/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install

RUN mkdir vinbero/build; cd vinbero/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..; make; make test; make install
