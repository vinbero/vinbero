
FROM golang:alpine as installer
RUN apk update && apk add git
RUN wget https://github.com/isacikgoz/gitbatch/releases/download/v0.4.1/gitbatch_0.4.1_linux_amd64.tar.gz -O /gitbatch.tar.gz
RUN tar -xvzf /gitbatch.tar.gz -C /
RUN wget 'https://bin.equinox.io/c/4VmDzA7iaHb/ngrok-stable-linux-amd64.zip' -O /ngrok.zip
RUN unzip /ngrok.zip -d /
RUN go get -u github.com/git-chglog/git-chglog/cmd/git-chglog
    

FROM alpine:latest
MAINTAINER Byeonggon Lee (gonny952@gmail.com)

RUN apk update && apk add git cmake automake autoconf libtool make gcc musl-dev cmocka-dev jansson-dev openssl openssl-dev clang yaml-dev
RUN mkdir -p /usr/src
COPY --from=installer /gitbatch /usr/bin/gitbatch
COPY --from=installer /ngrok /usr/bin/ngrok
COPY --from=installer /go/bin/git-chglog /usr/bin/git-chglog
RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories; apk update
RUN apk add tmux vim less grep curl musl-dbg gdb valgrind tree zsh procps flex-dev g++ boost-dev boost-program_options lua5.3-dev nlohmann-json uncrustify
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"
COPY vimrc /etc/vim/vimrc
COPY .zshrc /root/.zshrc
COPY vinbero-dev /usr/bin/vinbero-dev
RUN chmod +x /usr/bin/vinbero-dev
RUN git config --global pager.branch false
RUN git clone -j8 --recurse-submodules https://github.com/gonapps-org/gmcr /usr/src/gmcr
RUN mkdir /usr/src/gmcr/build
RUN cd /usr/src/gmcr/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..; make; make install
    

RUN git clone -j8 --recurse-submodules -b dev https://github.com/gonapps-org/libgenc /usr/src/libgenc
RUN git clone -j8 --recurse-submodules -b dev https://github.com/gonapps-org/libgaio /usr/src/libgaio
RUN git clone -j8 --recurse-submodules -b dev https://github.com/gonapps-org/libfastdl /usr/src/libfastdl
RUN git clone -j8 --recurse-submodules -b dev https://github.com/vinbero/libvinbero_com /usr/src/libvinbero_com
RUN git clone -j8 --recurse-submodules -b dev https://github.com/vinbero/vinbero-ifaces /usr/src/vinbero-ifaces
RUN git clone -j8 --recurse-submodules -b dev https://github.com/vinbero/vinbero /usr/src/vinbero

RUN mkdir /usr/src/libgenc/build; cd /usr/src/libgenc/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libgaio/build; cd /usr/src/libgaio/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libfastdl/build; cd /usr/src/libfastdl/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/libvinbero_com/build; cd /usr/src/libvinbero_com/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/vinbero-ifaces/build; cd /usr/src/vinbero-ifaces/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install
RUN mkdir /usr/src/vinbero/build; cd /usr/src/vinbero/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_BUILD_TYPE=Debug ..; make; make test; make install

