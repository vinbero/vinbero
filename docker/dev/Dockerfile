FROM alpine:latest
MAINTAINER Byeonggon Lee (gonny952@gmail.com)

RUN apk update && apk add git cmake automake autoconf libtool make gcc musl-dev cmocka-dev jansson-dev uthash-dev openssl openssl-dev
RUN mkdir -p /usr/src

RUN apk add tmux vim less grep curl musl-dbg gdb valgrind tree
RUN apk add tcl
COPY vimrc /etc/vim/vimrc
RUN git clone https://github.com/gonapps-org/gmcr /usr/src/gmcr
RUN git clone --depth 1 https://github.com/tcltk/tcllib /usr/src/tcllib
RUN cp /usr/src/gmcr/gmcr /usr/bin/gmcr
RUN cd /usr/src/tcllib; yes | ./installer.tcl
FROM alpine:latest
MAINTAINER Byeonggon Lee (gonny952@gmail.com)

RUN apk update && apk add git cmake automake autoconf libtool make gcc musl-dev cmocka-dev jansson-dev uthash-dev openssl openssl-dev
RUN mkdir -p /usr/src

RUN apk add tmux vim less grep curl musl-dbg gdb valgrind tree
RUN apk add tcl
COPY vimrc /etc/vim/vimrc
RUN git clone https://github.com/gonapps-org/gmcr /usr/src/gmcr
RUN git clone --depth 1 https://github.com/tcltk/tcllib /usr/src/tcllib
RUN cp /usr/src/gmcr/gmcr /usr/bin/gmcr
RUN cd /usr/src/tcllib; yes | ./installer.tcl

RUN git clone -b dev https://github.com/vinbero/libgenc /usr/src/libgenc
RUN git clone -b dev https://github.com/vinbero/libgaio /usr/src/libgaio
RUN git clone -b dev https://github.com/vinbero/libfastdl /usr/src/libfastdl
RUN git clone -b dev https://github.com/vinbero/libvinbero_com /usr/src/libvinbero_com
RUN git clone -b dev https://github.com/vinbero/vinbero-ifaces /usr/src/vinbero-ifaces
RUN git clone -b dev https://github.com/vinbero/vinbero /usr/src/vinbero

RUN mkdir /usr/src/libgenc/build; cd /usr/src/libgenc/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libgaio/build; cd /usr/src/libgaio/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libfastdl/build; cd /usr/src/libfastdl/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libvinbero_com/build; cd /usr/src/libvinbero_com/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/vinbero-ifaces/build; cd /usr/src/vinbero-ifaces/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/vinbero/build; cd /usr/src/vinbero/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install


