#ifndef _TUCUBE_IO_H
#define _TUCUBE_IO_H
struct tucube_IO {
    void* object;
    int (*read)(void*, void*, int);
    int (*write)(void* void*, int);
    int close(void*);
};
#endif
