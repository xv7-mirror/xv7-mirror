#ifndef _SYS_UTSNAME_
#define _SYS_UTSNAME_
struct utsname {
    char sysname[16];
    char nodename[16];
    char release[16];
    char version[16];
    char machine[16];
};

#define MACHINE_ARCH "x86"

#endif
