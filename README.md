# Vinbero
<p align="center"><img src="img/vinbero.png" alt="Vinbero" height="150px"/><img src="img/vinbero-text.png" alt="Vinbero" height="150px"/></p>

## Intoduction
Vinbero is a modular server written in C, and its main goal is flexibility. Its core is very small; It just loads children modules, initializes them, calls callbacks on them and destroys them. Currently multithreading, tcp, epoll, http, lua module exist. So you can try it as a simple web application server with lua scripting. But if you add your own module it could be even used as a mailserver, chatserver or gameserver.

## Usage
```console
Usage: vinbero [OPTION]...
A Modular Server.
Options:
  -i --inline-config       Inline JSON-based config.
  -c --config-file         JSON-based config file.
  -f --logging-flag        Set logging level flag.
  -o --logging-option      Set logging option.
  -h --help                Print this help message.
```

## History
It is initially started as a hobby project by Byeonggon Lee at Jul, 2016.
There have been many architectural changes for two years.

## License
MPLv2

## Contribution guide
Any type of contribution is welcome!
Radical changes like function renaming or small changes like removing extra spacing is allowed too.
Please don't hesitate to fork and contribute, this project needs a lot of work to do.

### Naming convention
#### Modules
Module names are snake case, and should be start with vinbero_
##### examples:
```C
vinbero_mt_epoll_http
vinbero_mt
```

#### Interfaces
Interface names are snake case and should start with vinbero_interface and **interface part** must be **uppercase with underscore**.
##### examples:
```C
vinbero_interface_HTTP
```

#### Structs
Struct names are pascal case and start with module names or interface names.
##### examples: 
```C
struct vinbero_mt_epoll_http_Module;
struct vinbero_mt_epoll_http_ParserData;
```

#### Functions
Function names are camel case and start with struct names if it act like methods, or start with module names or interface names.
##### examples:
```C
int vinbero_tcp_mt_epoll_loadChildClModules(struct vinbero_common_ClModule* clModule);
int vinbero_interface_HTTP_onRequestStart(struct vinbero_common_ClModule* clModule);
```

#### Macros
Macro naming is same as C macros (uppercase with underscore). But it also starts with module names or interfaces names.
#### examples:
```C
#define VINBERO_INTERFACE_HTTP_DLSYM(interface, dlHandle, ret)
```

#### Variables
Struct variable names are same as function names. but local variables don't start with module names

#### Reserved namespaces
- vinbero_core
- vinbero_common
- vinbero_interface
- vinbero_global
- vinbero_static
- vinbero_local

### Versioning
This project follows **semantic versioning**.
https://semver.org

### Branching
This project and all sub-projects are going to follow this branching model after vinbero v0.1.0 release.
**https://nvie.com/posts/a-successful-git-branching-model**
