#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

/*
 * Define __dead and __pure, like in OpenBSD.
 * These are really not needed, you can just do a
 * __noreturn in your program. But we strive for
 * copy-and-paste from BSD levels of source compatability.
 */
#if defined(__GNUC__) || defined(__clang__)
#define __dead __attribute__((__noreturn__))
#define __pure __attribute__((__const__))
#else
#define __dead
#define __pure
#endif

#endif
