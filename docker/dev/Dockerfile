FROM golang:alpine as installer
RUN apk update && apk add git
RUN wget https://github.com/isacikgoz/gitbatch/releases/download/v0.4.1/gitbatch_0.4.1_linux_amd64.tar.gz -O /gitbatch.tar.gz
RUN tar -xvzf /gitbatch.tar.gz -C /
RUN wget 'https://bin.equinox.io/c/4VmDzA7iaHb/ngrok-stable-linux-amd64.zip' -O /ngrok.zip
RUN unzip /ngrok.zip -d /

FROM alpine:latest
MAINTAINER Byeonggon Lee (gonny952@gmail.com)

COPY --from=installer /gitbatch /usr/bin/gitbatch
COPY --from=installer /ngrok /usr/bin/ngrok

RUN apk update && apk add git cmake automake autoconf libtool make gcc musl-dev cmocka-dev jansson-dev openssl openssl-dev procps
RUN mkdir -p /usr/src

RUN apk add tmux vim less grep curl musl-dbg gdb valgrind tree zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"
COPY vimrc /etc/vim/vimrc
COPY .zshrc /root/.zshrc
RUN git config --global pager.branch false

RUN git clone -b dev --recurse-submodules -j8 https://github.com/gonapps-org/libgenc /usr/src/libgenc
RUN git clone -b dev --recurse-submodules -j8 https://github.com/gonapps-org/libgaio /usr/src/libgaio
RUN git clone -b dev --recurse-submodules -j8 https://github.com/gonapps-org/libfastdl /usr/src/libfastdl
RUN git clone -b dev --recurse-submodules -j8 https://github.com/vinbero/libvinbero_com /usr/src/libvinbero_com
RUN git clone -b dev --recurse-submodules -j8 https://github.com/vinbero/vinbero-ifaces /usr/src/vinbero-ifaces
RUN git clone -b dev --recurse-submodules -j8 https://github.com/vinbero/vinbero /usr/src/vinbero

RUN mkdir /usr/src/libgenc/build; cd /usr/src/libgenc/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libgaio/build; cd /usr/src/libgaio/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libfastdl/build; cd /usr/src/libfastdl/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libvinbero_com/build; cd /usr/src/libvinbero_com/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/vinbero-ifaces/build; cd /usr/src/vinbero-ifaces/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/vinbero/build; cd /usr/src/vinbero/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install


