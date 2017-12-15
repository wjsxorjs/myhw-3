#ifndef _M_RUN_H_
#define _M_RUN_H_

#include <sys/types.h>

//Medata data block size
#define META_SIZE (sizeof(struct metadata) - 4)

typedef struct metadata *p_meta;

//Metadata
struct metadata
{
  size_t size;
  p_meta next;
  p_meta prev;
  int free;
  void *ptr;
  // A pointer tto the data section
  char data[1];
};

extern void *base;

void* m_malloc(size_t size);
void m_free(void* ptr);
void* m_realloc(void* ptr, size_t size);
void split_it(p_meta p, size_t s);
p_meta extend_it(p_meta last, size_t s);
p_meta find_meta(p_meta *last, size_t size);
p_meta fusion(p_meta p);
p_meta get_meta(void *v);
int valid_addr(void *v);
void copy_meta(p_meta src, p_meta dst);



#endif
