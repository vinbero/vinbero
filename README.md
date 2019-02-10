<!--
+++
title = "Vinbero"
description = "The Modular Server"
+++
-->
# Vinbero

[![GitHub release](http://img.shields.io/github/release/vinbero/vinbero.svg)](https://github.com/vinbero/vinbero/releases)
[![Build Status](https://travis-ci.org/vinbero/vinbero.svg?branch=master)](https://travis-ci.org/vinbero/vinbero)
[![license](http://img.shields.io/github/license/vinbero/vinbero.svg)](https://raw.githubusercontent.com/vinbero/vinbero/master/LICENSE)
[![Docker Stars](http://img.shields.io/docker/stars/vinbero/vinbero.svg)](https://hub.docker.com/r/vinbero/vinbero)
[![Docker Pulls](http://img.shields.io/docker/pulls/vinbero/vinbero.svg)](https://hub.docker.com/r/vinbero/vinbero)
[![codecov](https://codecov.io/gh/vinbero/vinbero/branch/master/graph/badge.svg)](https://codecov.io/gh/vinbero/vinbero)

<p align="center"><img src="https://raw.githubusercontent.com/vinbero/vinbero/master/img/vinbero.png" alt="Vinbero" height="175px"/><img src="https://raw.githubusercontent.com/vinbero/vinbero/master/img/vinbero-text.png" alt="Vinbero" height="150px"/></p>

## Intoduction
Vinbero is a modular server written in C, and its main goal is flexibility. Its core is very small; It just loads children modules, initializes them, calls callbacks on them and destroys them. Currently multithreading, tcp, epoll, tls, http, lua modules exist. So you can try it as a simple web application server with lua scripting. But if you add your own module it could be even used as a mailserver, chatserver or gameserver.

## Example
```console
docker run -it -d --name vinbero -p 8080:80 vinbero/vinbero_mt_http_lua
curl localhost:8080
```
```console
docker run -it -d --name vinbero -p 8080:80 vinbero/vinbero-blog
#default id: 'admin', default password: 'password'
ngrok http 8080
```

## Warning
**Currently this software is under heavy development, so there will be bugs or the architecture can change.
Writing new modules is not encouraged yet.**

## Usage
```console
Usage: vinbero [OPTION]...
A Modular Server.
Options:
  -i --inline-config       Inline JSON-based config.
  -c --config-file         JSON-based config file.
  -f --logging-flag        Set logging level flag.
  -o --logging-option      Set logging option.
  -v --version             Print version info.
  -h --help                Print this help message.
```

<a href="https://asciinema.org/a/188477" target="_blank"><img src="https://asciinema.org/a/188477.png" width="100%"/></a>

### Config file example
```JSON
{
    "core": {
        "config": {"vinbero.setUid": 1001},
        "next": ["vinbero_tcp"]
    },
    "vinbero_tcp": {
        "paths": ["/usr/local/lib/vinbero_tcp.so", "/usr/lib/vinbero/vinbero_tcp.so"],
        "config": {"vinbero_tcp.port": 80, "vinbero_tcp.reuseAddress": true},
        "next": ["vinbero_mt"]
    }, 
    "vinbero_mt": {
        "paths": ["/usr/local/lib/vinbero_mt.so", "/usr/lib/vinbero/vinbero_mt.so"],
        "config": {"vinbero_mt.workerCount": 4},
        "next": ["vinbero_strm_mt_epoll"]
    },
    "vinbero_strm_mt_epoll": {
        "paths": ["/usr/local/lib/vinbero_strm_mt_epoll.so", "/usr/lib/vinbero/vinbero_strm_mt_epoll.so"],
        "config": {"vinbero_strm_mt_epoll.clientTimeoutSeconds": 3},
        "next": ["vinbero_mt_epoll_http"]
    },
    "vinbero_mt_epoll_http": {
        "paths": ["/usr/local/lib/vinbero_mt_epoll_http.so", "/usr/lib/vinbero/vinbero_mt_epoll_http.so"],
        "config": {},
        "next": ["vinbero_mt_http_lua"]
    },
    "vinbero_mt_http_lua": {
        "paths": ["/usr/local/lib/vinbero_mt_http_lua.so", "/usr/lib/vinbero/vinbero_mt_http_lua.so"],
        "config": {
            "vinbero_mt_http_lua.scriptFile": "/srv/app.lua",
            "vinbero_mt_http_lua.scriptArg": {}
        },
        "next": []
    }
}
```

### Logging flags and Logging options
Logging flags and logging options are integer bitmasks:

- FLAG_TRACE: 1
- FLAG_DEBUG: 2
- FLAG_INFO: 4
- FLAG_WARN: 8
- FLAG_ERROR: 16
- FLAG_FATAL: 32
- **default logging flag is 62**
- OPTION_COLOR: 1
- OPTION_SOURCE: 2
- **defualt logging option is 1**

### Core module config options
- vinbero.setUid (***int***) : Change uid after module initialization.
- vinbero.setGid (***int***) : Change gid after module initialization.

## History
It is initially started as a hobby project by Byeonggon Lee at Jul, 2016.
There have been many architectural changes for two years.

## License
MPL-2.0

## Contribution guide
Any type of contribution is welcome!
Radical changes like function renaming or small changes like removing extra spacing is allowed too.
Please don't hesitate to fork and contribute, this project needs a lot of work to do.

### Language
Use English on your commit message so everyone can understand

### Naming convention
#### Modules
Module names are snake case, and should be start with vinbero_

##### Example
```C
vinbero_mt_epoll_http
vinbero_mt
```

#### Interfaces
Interface names are snake case and should start with vinbero_iface and **interface part** must be **uppercase with underscore**.

##### Example
```C
vinbero_iface_HTTP
```

#### Structs
Struct names are pascal case and start with module names or interface names.
##### Example
```C
struct vinbero_mt_epoll_http_Module;
struct vinbero_mt_epoll_http_ParserData;
```

#### Functions
Function names are camel case and start with struct names if it act like methods, or start with module names or interface names.
##### Example
```C
int vinbero_strm_mt_epoll_loadChildClModules(struct vinbero_com_ClModule* clModule);
int vinbero_iface_HTTP_onRequestStart(struct vinbero_com_ClModule* clModule);
```

#### Macros
Macro naming is same as C macros (uppercase with underscore). But it also starts with module names or interfaces names.
#### Example
```C
#define VINBERO_INTERFACE_HTTP_DLSYM(interface, dlHandle, ret)
```

#### Variables
Struct variable names are same as function names. but local variables don't start with module names

#### Reserved namespaces
- vinbero_core
- vinbero_com
- vinbero_iface
- vinbero_global
- vinbero_static
- vinbero_local

### Versioning
This project follows **[semantic versioning](https://semver.org)**

### Branching
This project and all sub-projects are going to follow **[this branching model](https://nvie.com/posts/a-successful-git-branching-model)** after vinbero v0.1.0 release.
- master: Should always be executed without a bug.
- dev: Development branch, can contain a bug
- feature: When creating a new feature. this branch will be merged into dev branch
- release: When creating a new release. this branch will be merged into master branch
- hotfix: When a bug is found on master branch and you need to fix it fast, create this branch and merge it into master branch
- bugfix: When a bug is found on dev branch, create this branch and merge it into dev branch

### AUTHORS file
To update AUTHORS file, you have to run cmake or make to update AUTHORS file based on commits from origin/master

### Example
1. To start developing from the environment where all official modules are installed, run a docker container like this:
```console
docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it -d --name vinbero vinbero/vinbero_mt_http_lua:dev
docker exec -it vinbero /bin/sh
```
2. Inside the container clone a forked repository, start from a branch you want to improve e.g., dev, feature, release, hotfix, bugfix
```console
cd /
rm -rf vinbero
git clone -b dev https://github.com/YOU/vinbero
```
3. Checkout to a new branch
```console
git checkout -b feature-something or git checkout
```
4. Edit sources, run cmake, commit and push
5. Make a pull request
