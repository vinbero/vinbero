# Vinbero
<p align="center"><img src="vinbero.png" alt="Vinbero" height="150px"/><img src="vinbero-text.png" alt="Vinbero" height="150px"/></p>

## Intoduction
Vinbero is a modular server written in C, and its main goal is flexibility. Its core is very small; It just loads children modules, initializes them, calls callbacks on them and destroys them. Currently multithreading, tcp, epoll, http, lua module exist. So you can try it as a simple web application server with lua scripting. But if you add your own module it could be even used as a mailserver, chatserver or game server.

## Usage
```console
vinbero -c config.json
```
## History
It is initially started as a hobby project by @gonapps at Jul, 2016.
There have been many architectural changes for two years.

## License
MPLv2
## Contribution guide
Any type of contribution is welcome!
Radical changes like function renaming, small changes like removing extra spacing is allowed too.
Please don't hesitate to fork and contribute, this project needs a lot of work to do.

This project follows **semantic versioning**.
https://semver.org

This project and all sub-projects are going to follow this branching model after vinbero v0.1.0 release.
**https://nvie.com/posts/a-successful-git-branching-model**
