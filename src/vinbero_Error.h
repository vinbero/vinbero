#ifndef _VINBERO_ERROR_H
#define _VINBERO_ERROR_H

#include <errno.h>

#define VINBERO_EUNKNOWN -1000
#define VINBERO_E2BIG -E2BIG
#define VINBERO_EACCES -EACCES
#define VINBERO_EADDRINUSE -EADDRINUSE
#define VINBERO_EADDRNOTAVAIL -EADDRNOTAVAIL
#define VINBERO_EAFNOSUPPORT -AFNOSUPPORT
#define VINBERO_EAGAIN -EAGAIN
#define VINBERO_EALREADY -EALREADY
#define VINBERO_EBADE -EBADE
#define VINBERO_EBADF -EBADF
#define VINBERO_EBADFD -EBADFD
#define VINBERO_EBADMSG -EBADMSG
#define VINBERO_EBADR -EBADR
#define VINBERO_EBADRQC -EBADRQC
#define VINBERO_EBADSLT -EBADSLT
#define VINBERO_EBUSY -EBUSY
#define VINBERO_ECANCELED -ECANCELED
#define VINBERO_ECHILD -ECHILD
#define VINBERO_ECHRNG -ECHRNG
#define VINBERO_ECOMM -ECOMM
#define VINBERO_ECONNABORTED -ECONNABORTED
#define VINBERO_ECONNREFUSED -ECONNREFUSED
#define VINBERO_ECONNRESET -ECONNRESET
#define VINBERO_EDEADLK -EDEADLK
#define VINBERO_EDEADLOCK -EDEADLOCK
#define VINBERO_EDESTADDRREQ -EDESTADDRREQ
#define VINBERO_EDOM -EDOM
#define VINBERO_EDQUOT -EDQUOT
#define VINBERO_EEXIST -EEXIST
#define VINBERO_EFAULT -EFAULT
#define VINBERO_EFBIG -EFBIG
#define VINBERO_EHOSTDOWN -EHOSTDOWN
#define VINBERO_EHOSTUNREACH -EHOSTUNREACH
#define VINBERO_EHWPOISON -EHWPOISON
#define VINBERO_EIDRM -EIDRM
#define VINBERO_EILSEQ -EILSEQ
#define VINBERO_EINPROGRESS -EINPROGRESS
#define VINBERO_EINTER -EINTR
#define VINBERO_EINVAL -EINVAL
#define VINBERO_EIO -EIO
#define VINBERO_EISCONN -EISCONN
#define VINBERO_EISDIR -EISDIR
#define VINBERO_EISNAM -EISNAM
#define VINBERO_EKEYEXPIRED -EKEYEXPIRED
#define VINBERO_EKEYREJECTED -EKEYREJECTED
#define VINBERO_EKEYREVOKED -EKEYREVOKED
#define VINBERO_EL2HLT -EL2HLT
#define VINBERO_EL2NSYNC -EL2NSYNC
#define VINBERO_EL3HLT -EL3HLT
#define VINBERO_EL3RST -EL3RST
#define VINBERO_ELIBACC -ELIBACC
#define VINBERO_ELIBBAD -ELIBBAD
#define VINBERO_ELIBMAX -ELIBMAX
#define VINBERO_ELIBSCN -ELIBSCN
#define VINBERO_ELIBEXEC -ELIBEXEC
#define VINBERO_ELNRANGE -ELNRANGE
#define VINBERO_ELOOP -ELOOP
#define VINBERO_EMEDIUMTYPE -EMEDIUMTYPE
#define VINBERO_EMFILE -EMFILE
#define VINBERO_EMLINK -EMLINK
#define VINBERO_EMSGSIZE -EMSGSIZE
#define VINBERO_EMULTIHOP -EMULTIHOP
#define VINBERO_ENAMETOOLONG -ENAMETOOLONG
#define VINBERO_ENETDOWN -ENETDOWN
#define VINBERO_ENETRESET -ENETRESET
#define VINBERO_ENETUNREACH -ENETUNREACH
#define VINBERO_ENFILE -ENFILE
#define VINBERO_ENOANO -ENOANO 
#define VINBERO_ENOBUFS -ENOBUFS 
#define VINBERO_ENODATA -ENODATA
#define VINBERO_ENODEV -ENODEV
#define VINBERO_ENOENT -ENOENT
#define VINBERO_ENOEXEC -ENOEXEC
#define VINBERO_ENOKEY -ENOKEY
#define VINBERO_ENOLCK -ENOLCK
#define VINBERO_ENOLINK -ENOLINK
#define VINBERO_ENOMEDIUM -ENOMEDIUM
#define VINBERO_ENOMEM -ENOMEM
#define VINBERO_ENOMSG -ENOMSG
#define VINBERO_ENONET -ENONET
#define VINBERO_ENOPKG -ENOPKG
#define VINBERO_ENOPROTOOPT -ENOPROTOOPT
#define VINBERO_ENOSPC -ENOSPC
#define VINBERO_ENOSR -ENOSR
#define VINBERO_ENOSTR -ENOSTR
#define VINBERO_ENOSYS -ENOSYS
#define VINBERO_ENOTBLK -ENOTBLK
#define VINBERO_ENOTCONN -ENOTCONN
#define VINBERO_ENOTDIR -ENOTDIR
#define VINBERO_ENOTEMPTY -ENOTEMPTY
#define VINBERO_ENOTRECOVERABLE -ENOTRECOVERABLE
#define VINBERO_ENOTSOCK -ENOTSOCK
#define VINBERO_ENOTSUP -ENOTSUP
#define VINBERO_ENOTTY -ENOTTY
#define VINBERO_ENOTUNIQ -ENOTUNIQ 
#define VINBERO_ENXIO -ENXIO
#define VINBERO_EOPNOTSUPP -EOPNOTSUPP
#define VINBERO_EOVERFLOW -EOVERFLOW
#define VINBERO_EOWNERDEAD -EOWNERDEAD
#define VINBERO_EPERM -EPERM
#define VINBERO_EPFNOSUPPORT -EPFNOSUPPORT
#define VINBERO_EPIPE -EPIPE
#define VINBERO_EPROTO -EPROTO
#define VINBERO_EPROTONOSUPPORT -EPROTONOSUPPORT
#define VINBERO_EPROTOTYPE -EPROTOTYPE
#define VINBERO_ERANGE -ERANGE
#define VINBERO_EREMCHG -EREMCHG
#define VINBERO_EREMOTE -EREMOTE
#define VINBERO_EREMOTEIO -EREMOTEIO
#define VINBERO_ERESTART -ERESTART
#define VINBERO_ERFKILL -ERFKILL
#define VINBERO_EROFS -EROFS
#define VINBERO_ESHUTDOWN -ESHUTDOWN 
#define VINBERO_ESPIPE -ESPIPE
#define VINBERO_ESOCKTNOSUPPORT -ESOCKTNOSUPPORT
#define VINBERO_ESRCH -ESRCH
#define VINBERO_ESTALE -ESTALE
#define VINBERO_ESTRPIPE -ESTRPIPE
#define VINBERO_ETIME -ETIME
#define VINBERO_ETIMEDOUT -ETIMEDOUT
#define VINBERO_ETOOMANYREFS -ETOOMANYREFS
#define VINBERO_ETXTBSY -ETXTBSY
#define VINBERO_EUCLEAN -EUCLEAN
#define VINBERO_EUNATCH -EUNATCH
#define VINBERO_EUSERS -EUSERS
#define VINBERO_EWOULDBLOCK -EWOULDBLOCK 
#define VINBERO_EXDEV -EXDEV
#define VINBERO_EXFULL -EXFULL

#endif
