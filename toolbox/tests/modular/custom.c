#include "ctb/vector/custom.h"
#include "ctb/matrix/custom.h"
#include <assert.h>
typedef struct {int score, time;} Record;
CTB_VECTOR_DEFINE(Records,Record)
CTB_MATRIX_DEFINE(RecordMatrix,Record)
static int compare(const Record *a,const Record *b){return a->score!=b->score?(a->score>b->score)-(a->score<b->score):(a->time>b->time)-(a->time<b->time);}
int main(void){Records v=Records_new();Records_push(&v,(Record){2,1});Records_push(&v,(Record){1,2});Records_sort_by(&v,compare);assert(Records_get(&v,0).score==1);Records_free(&v);RecordMatrix m=RecordMatrix_new(2,3);assert(RecordMatrix_get(&m,1,2).score==0);RecordMatrix_set(&m,1,2,(Record){5,6});RecordMatrix t=RecordMatrix_transpose(&m);assert(RecordMatrix_get(&t,2,1).time==6);RecordMatrix_free(&t);RecordMatrix_free(&m);}
