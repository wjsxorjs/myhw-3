#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>

#include "run.h"
#include "util.h"

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define BS 8 /*(4*2)*/

void *base = 0;

p_meta find_meta(p_meta *last, size_t size) {
  p_meta index = base;
  p_meta result = base;

  switch(fit_flag){
    case FIRST_FIT:
    {

   //FIRST FIT CODE
while(index && (index->free && index->size >= size)){

*last = index;
index = index->next;	
}
result = index;
}
    break;

    case BEST_FIT:
    {
      //BEST_FIT CODE
	size_t  s = (index->size - size);
	while(index && (index->free && index->size >= size)){
	s = (index->size - size);
	if(s<result->size - size){
	result = index;
	}
	*last = index;
	index = index->next;
	}
	

    }
    break;

    case WORST_FIT:
    {
      //WORST_FIT CODE

        size_t  s = (index->size - size);
        while(index && (index->free && index->size >= size)){
        s = (index->size - size);
        if(s>result->size - size){
        result = index;
        }
        *last = index;
        index = index->next;
        }
	
    }

    break;

  }
  return result;
}

void *m_malloc(size_t size){
p_meta p, last;
size_t siz3;
siz3 = align4(size);
if(base){
last = base;
p = find_meta(&last,siz3);
if(p){
if((p->size - siz3) >= (BS+4)){
split_it(p,siz3);
}
p->free =0;
}
else{
p=extend_it(last,siz3);
if(!p){return(NULL);}
}
}
else{
p= extend_it(NULL,siz3);
if(!p){
return NULL;
}
base = p;
}

return(p->data);
}

void m_free(void *ptr) {
p_meta p;
if(valid_addr(ptr)){
p = get_meta(ptr);
p->free =1;
if(p->prev && p->prev->free){
p = fusion(p->prev);}
if(p->next){
fusion(p);
}
else{
if(p->prev){
p->prev->next = NULL;
}
else{
base = 0;
}
brk(p);
}
}
}

void *m_realloc(void* ptr, size_t size)
{
size_t s;
p_meta p, new;
void *newp;
if(!p){return(malloc(size));}
if(valid_addr(ptr)){
s = align4(size);
p = get_meta(ptr);
if(p->size >= s){
	if(p->size -s >= (BS + 4)){
	split_it(p,s);
		}
}
else{
if(p->next && p->next->free && (p->size + BS + p->next->size) >= s){
fusion(p);
if(p->size - s >= (BS +4)){
split_it(p,s);
}
}
else{
newp = malloc(s);
if(!newp){return NULL;}
new = get_meta(newp);
free(p);
return(newp);
}
}
return ptr;
}

return NULL;

}

void split_it(p_meta p, size_t s){
p_meta new;
new =p->data +s;
new->size = p->size -s - BS;
new->next = p->next;
new->free = 1;
p->size = s;
p->next = new;
}

p_meta extend_it(p_meta last, size_t s){
p_meta p;
p = sbrk(0);
if(sbrk(BS +s) == (void*)-1)
{return NULL;}
p->size = s;
p->next=NULL;
if(last){
last->next = p;
}
p->free =0;

return (p);
}


p_meta fusion(p_meta p){
if(p->next && p->next->free){
p->size += BS + p->next->size;
p->next = p->next->next;
if(p->next){
p->next->prev = p;
}

}
return(p);
}

p_meta get_meta(void *v){
char *tmp;
tmp = v;
return(v = tmp -= BS);
}

int valid_addr(void *v){
if(base){
if(v>base && v<sbrk(0)){
return(v == (get_meta(v))->ptr);
}
}
return (0);
}

void copy_meta(p_meta src, p_meta dst){
int *sdata, *ddata;
size_t i;
sdata = src->ptr;
ddata = dst->ptr;
for(i=0; i*4<src->size && i*4<src->size; i++){
ddata[i] = sdata[i];
}
}
