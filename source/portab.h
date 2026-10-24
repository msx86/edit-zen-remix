/* ------------- portab.h ----------- */
#ifndef PORTAB
#define PORTAB

#include <stdint.h>

typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;

#ifdef __WATCOMC__

#include <string.h>
#include <direct.h>
#include <dos.h>
#include <i86.h>

#define REGS REGS_386_WRAPPER
union REGS_386_WRAPPER {
    struct DWORDREGS d;
    struct WORDREGS  x;
    struct WORDREGS  w;
    struct BYTEREGS  h;
};

#undef int86
#undef int86x

static __inline int safe_int86(int intno, const union REGS *in, union REGS *out)
{
    union _REGS r;
    memset(&r, 0, sizeof(r));
    if (in) {
        r.w.ax = in->w.ax;
        r.w.bx = in->w.bx;
        r.w.cx = in->w.cx;
        r.w.dx = in->w.dx;
        r.w.si = in->w.si;
        r.w.di = in->w.di;
    }
    int386(intno, (const void *)&r, (void *)&r);
    if (out) {
        memset(out, 0, sizeof(*out));
        out->w.ax = r.w.ax;
        out->w.bx = r.w.bx;
        out->w.cx = r.w.cx;
        out->w.dx = r.w.dx;
        out->w.si = r.w.si;
        out->w.di = r.w.di;
        out->d.cflag = r.w.cflag;
    }
    return r.w.ax;
}

static __inline int safe_int86x(int intno, const union REGS *in, union REGS *out, struct SREGS *s)
{
    union _REGS r;
    struct _SREGS sr;
    memset(&r, 0, sizeof(r));
    segread((void *)&sr);
    if (in) {
        r.w.ax = in->w.ax;
        r.w.bx = in->w.bx;
        r.w.cx = in->w.cx;
        r.w.dx = in->w.dx;
        r.w.si = in->w.si;
        r.w.di = in->w.di;
    }
    if (s) {
        if (s->ds) sr.ds = s->ds;
        if (s->es) sr.es = s->es;
    }
    int386x(intno, (const void *)&r, (void *)&r, (void *)&sr);
    if (s) {
        s->ds = sr.ds;
        s->es = sr.es;
    }
    if (out) {
        memset(out, 0, sizeof(*out));
        out->w.ax = r.w.ax;
        out->w.bx = r.w.bx;
        out->w.cx = r.w.cx;
        out->w.dx = r.w.dx;
        out->w.si = r.w.si;
        out->w.di = r.w.di;
        out->d.cflag = r.w.cflag;
    }
    return r.w.ax;
}

#define int86(i, in, out) safe_int86(i, (const union REGS *)(in), (union REGS *)(out))
#define int86x(i, in, out, s) safe_int86x(i, (const union REGS *)(in), (union REGS *)(out), s)

unsigned short dpmi_seg_to_sel(unsigned short seg);
#pragma aux dpmi_seg_to_sel = \
    "mov ax, 0002h" \
    "int 31h" \
    parm [bx] \
    value [ax] \
    modify [ax];

struct dpmi_free_mem_info {
    unsigned int largest_block_bytes;
    unsigned int max_unlocked_pages;
    unsigned int max_locked_pages;
    unsigned int linear_address_space_pages;
    unsigned int total_unlocked_pages;
    unsigned int total_free_pages;
    unsigned int total_physical_pages;
    unsigned int free_linear_space_pages;
    unsigned int page_file_pages;
    unsigned int reserved[3];
};

static __inline unsigned long get_free_dpmi_memory(void)
{
    struct dpmi_free_mem_info info;
    union REGS r;
    struct SREGS s;
    memset(&info, 0, sizeof(info));
    memset(&r, 0, sizeof(r));
    memset(&s, 0, sizeof(s));
    segread(&s);
    s.es = s.ds;
    r.d.edi = (unsigned int)&info;
    r.w.ax = 0x0500;
    int386x(0x31, (const void *)&r, (void *)&r, (void *)&s);
    if ((r.d.cflag & 1) == 0) {
        if (info.largest_block_bytes > 0 && info.largest_block_bytes != 0xFFFFFFFF)
            return (unsigned long)info.largest_block_bytes;
        if (info.total_free_pages > 0 && info.total_free_pages != 0xFFFFFFFF)
            return (unsigned long)info.total_free_pages * 4096UL;
    }
    return 0;
}

#undef MK_FP
#define MK_FP(seg,ofs) _MK_FP(dpmi_seg_to_sel((unsigned short)(seg)),(ofs))

#define pokeb(seg, ofs, b) (*(unsigned char far *)_MK_FP(dpmi_seg_to_sel(seg), ofs) = (unsigned char)(b))
#define poke(seg, ofs, w) (*(unsigned short far *)_MK_FP(dpmi_seg_to_sel(seg), ofs) = (unsigned short)(w))
#define pokew poke
#define pokel(seg, ofs, l) (*(unsigned long far *)_MK_FP(dpmi_seg_to_sel(seg), ofs) = (unsigned long)(l))
#define peekb(seg, ofs) (*(unsigned char far *)_MK_FP(dpmi_seg_to_sel(seg), ofs))
#define peek(seg, ofs) (*(unsigned short far *)_MK_FP(dpmi_seg_to_sel(seg), ofs))
#define peekw peek
#define peekl(seg, ofs) (*(unsigned long far *)_MK_FP(dpmi_seg_to_sel(seg), ofs))

#define MAXDRIVE 3
#define MAXPATH  80 
#define MAXDIR   66
#define MAXFILE  9
#define MAXEXT   5

#define WILDCARDS 0x01
#define EXTENSION 0x02
#define FILENAME  0x04
#define DIRECTORY 0x08
#define DRIVE     0x10

void disable(void);
#pragma aux disable = "cli" modify exact [];
void enable(void);
#pragma aux enable = "sti" modify exact [];
void halt(void);
#pragma aux halt = "hlt" modify exact [];

void cpu_hlt(void);
#pragma aux cpu_hlt = \
    "pushf" \
    "sti" \
    "hlt" \
    "popf";

void setcbrk(char check);
#pragma aux setcbrk = \
    "mov ah, 33h" \
    "mov al, 01h" \
    "int 21h" \
    parm [dl] \
    modify [ax];

#define getdisk()   ( _getdrive() - 1 )
#define setdisk(i)  ( _dos_setdrive((i) + 1, NULL) )
#define setvect(n, h)  ( _dos_setvect(n, h) )
#define fnsplit(f, d, n, nm, e)  ( _splitpath(f, d, n, nm, e) )
#define fnmerge(spath, drive, dir, name, ext)  ( _makepath(spath, drive, dir, name, ext) )
#define CFlag(regs) ( (regs).d.cflag )

typedef struct fblock { struct find_t ff; } FBLOCK;
#define FindFirst(path,attr,blk) ( _dos_findfirst(path, attr, &((blk).ff)) )
#define FindNext(blk) ( _dos_findnext(&((blk).ff)) )
#define NameOf(blk) ((blk).ff.name)
#define AttribOf(blk) ((blk).ff.attrib)

#define OLDCRITERR 1

#else

#ifndef MAXPATH
#define MAXPATH 1024
#endif

#endif /* __WATCOMC__ */

#endif /* PORTAB */
