#include "types.h"
#include "stat.h"
#include <stdio.h>
#include "param.h"
#include <unistd.h>
#include <string.h>

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
    struct {
        union header* ptr;
        uint size;
    } s;
    Align x;
};

typedef union header Header;

static Header base;
static Header* freep;

void free(void* ap)
{
    Header *bp, *p;

    bp = (Header*)ap - 1;
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;
    if (bp + bp->s.size == p->s.ptr) {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr;
    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else
        p->s.ptr = bp;
    freep = p;
}

static Header* morecore(uint nu)
{
    char* p;
    Header* hp;

    if (nu < 4096)
        nu = 4096;
    p = sbrk(nu * sizeof(Header));
    if (p == (char*)-1)
        return 0;
    hp = (Header*)p;
    hp->s.size = nu;
    free((void*)(hp + 1));
    return freep;
}

void* malloc(size_t nbytes)
{
    Header *p, *prevp;
    uint nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = freep) == 0) {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {
            if (p->s.size == nunits)
                prevp->s.ptr = p->s.ptr;
            else {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void*)(p + 1);
        }
        if (p == freep)
            if ((p = morecore(nunits)) == 0)
                return 0;
    }
}

void* calloc(size_t nmemb, size_t size)
{
    size_t total_size;
    void* ptr;

    if (nmemb == 0 || size == 0) {
        return malloc(0);
    }

    total_size = nmemb * size;
    if (size != 0 && total_size / size != nmemb) {
        return 0;
    }

    ptr = malloc(total_size);
    if (ptr == 0) {
        return 0;
    }

    memset(ptr, 0, total_size);

    return ptr;
}

void* realloc(void* ptr, size_t size)
{
    Header* hp;
    size_t old_data_size;
    void* new_ptr;
    size_t copy_size;

    if (ptr == 0) {
        return malloc(size);
    }

    if (size == 0) {
        free(ptr);
        return 0;
    }

    hp = (Header*)ptr - 1;
    old_data_size = (hp->s.size - 1) * sizeof(Header);

    new_ptr = malloc(size);
    if (new_ptr == 0) {
        return 0;
    }

    copy_size = (old_data_size < size) ? old_data_size : size;

    memmove(new_ptr, ptr, copy_size);

    free(ptr);

    return new_ptr;
}
