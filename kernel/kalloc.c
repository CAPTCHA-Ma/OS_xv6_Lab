// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct
{

  struct spinlock lock;
  uint16 rfcnt[RFSZ];

} krfcnt;

static int pa2idx(uint64 pa)
{

  return (pa - KERNBASE) / PGSIZE;

}

void krfadd(uint64 pa)
{

  acquire(&krfcnt.lock);
  ++krfcnt.rfcnt[pa2idx(pa)];
  release(&krfcnt.lock);

}

void krfminus(uint64 pa)
{

  acquire(&krfcnt.lock);

  if(krfcnt.rfcnt[pa2idx(pa)] < 1) panic("krfminus");
  --krfcnt.rfcnt[pa2idx(pa)];

  release(&krfcnt.lock);

}

int krfget(uint64 pa)
{

  int cnt;

  acquire(&krfcnt.lock);
  cnt = krfcnt.rfcnt[pa2idx(pa)];
  release(&krfcnt.lock);
  return cnt;

}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&krfcnt.lock, "krfcnt");

  freerange(end, (void*)PHYSTOP);

}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {

    acquire(&krfcnt.lock);
    krfcnt.rfcnt[pa2idx((uint64)p)] = 1;
    release(&krfcnt.lock);
    kfree(p);
    
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  int idx;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  idx = pa2idx((uint64)pa);

  acquire(&krfcnt.lock);

  if (krfcnt.rfcnt[idx] < 1) panic("kfree rfcnt");
  --krfcnt.rfcnt[idx];
  if (krfcnt.rfcnt[idx] > 0)
  { 

    release(&krfcnt.lock);
    return;

  }

  release(&krfcnt.lock);

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
  {

    memset((char*)r, 5, PGSIZE); // fill with junk

    acquire(&krfcnt.lock);

    if(krfcnt.rfcnt[pa2idx((uint64)r)] != 0) panic("kalloc rfcnt");
    krfcnt.rfcnt[pa2idx((uint64)r)] = 1;

    release(&krfcnt.lock);

  }

  return (void*)r;
}
