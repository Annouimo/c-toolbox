#include "ctb_vector.h"
#include "ctb_deque.h"
#include "ctb_heap.h"
#include <assert.h>
typedef struct {int score, time;} Record;
CTB_VECTOR_DEFINE(Records,Record)
CTB_DEQUE_DEFINE(Queue,Record)
CTB_HEAP_DEFINE(Heap,Record)
static int compare(const Record *a,const Record *b){return a->score!=b->score?(a->score>b->score)-(a->score<b->score):(a->time>b->time)-(a->time<b->time);}
static int raw_compare(const void *a,const void *b){return compare(a,b);}
int main(void){Records v=Records_new();Records_push(&v,(Record){2,1});Records_push(&v,(Record){1,2});Records_sort_by(&v,compare);assert(Records_get(&v,0).score==1);Records_free(&v);Queue q=Queue_new();Queue_push_back(&q,(Record){3,4});assert(Queue_pop_front(&q).time==4);Queue_free(&q);Heap h=Heap_new(raw_compare);Heap_push(&h,(Record){5,6});assert(Heap_pop(&h).score==5);Heap_free(&h);}
