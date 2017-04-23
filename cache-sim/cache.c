/*
* cache.c
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cache.h"
#include "main.h"

/* cache configuration parameters */
static int cache_split = 0;
static int cache_usize = DEFAULT_CACHE_SIZE;
static int cache_isize = DEFAULT_CACHE_SIZE;
static int cache_dsize = DEFAULT_CACHE_SIZE;
static int cache_block_size = DEFAULT_CACHE_BLOCK_SIZE;
static int words_per_block = DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int cache_assoc = DEFAULT_CACHE_ASSOC;
static int cache_writeback = DEFAULT_CACHE_WRITEBACK;
static int cache_writealloc = DEFAULT_CACHE_WRITEALLOC;

/* cache model data structures */
static Pcache icache;
static Pcache dcache;
static cache c1;
static cache c2;
static cache_stat cache_stat_inst;
static cache_stat cache_stat_data;

/************************************************************/
void set_cache_param(param, value)
		int param;
		int value;
{

	switch (param) {
		case CACHE_PARAM_BLOCK_SIZE:
			cache_block_size = value;
			words_per_block = value / WORD_SIZE;
			break;
		case CACHE_PARAM_USIZE:
			cache_split = FALSE;
			cache_usize = value;
			break;
		case CACHE_PARAM_ISIZE:
			cache_split = TRUE;
			cache_isize = value;
			break;
		case CACHE_PARAM_DSIZE:
			cache_split = TRUE;
			cache_dsize = value;
			break;
		case CACHE_PARAM_ASSOC:
			cache_assoc = value;
			break;
		case CACHE_PARAM_WRITEBACK:
			cache_writeback = TRUE;
			break;
		case CACHE_PARAM_WRITETHROUGH:
			cache_writeback = FALSE;
			break;
		case CACHE_PARAM_WRITEALLOC:
			cache_writealloc = TRUE;
			break;
		case CACHE_PARAM_NOWRITEALLOC:
			cache_writealloc = FALSE;
			break;
		default:
			printf("error set_cache_param: bad parameter value\n");
			exit(-1);
	}

}
/************************************************************/

/************************************************************/
void init_cache()
{
	int i;
	/* initialize the cache, and cache statistics data structures */
	if(cache_split==1){
		c1.size = cache_isize/4;
		c1.associativity = cache_assoc;
		c1.index_mask_offset = LOG2(cache_block_size);
		c1.n_sets = cache_isize/(cache_assoc*cache_block_size);
		c1.index_mask = (c1.n_sets-1) << LOG2(cache_block_size);

		c1.LRU_head = (Pcache_line *) calloc(1,sizeof(Pcache_line)*c1.n_sets);
		c1.LRU_tail = (Pcache_line *) calloc(1,sizeof(Pcache_line)*c1.n_sets);
		c1.set_contents = (int *) calloc(1, sizeof(int) * c1.n_sets);

		for (i=0;i<c1.n_sets;i++){
			c1.LRU_head[i] = NULL;
			c1.LRU_tail[i] = NULL;
			c1.set_contents[i] = 0;
		}

		c2.size = cache_dsize/4;
		c2.associativity = cache_assoc;
		c2.index_mask_offset = LOG2(cache_block_size);
		c2.n_sets = cache_dsize/(cache_assoc*cache_block_size);
		c2.index_mask = (c2.n_sets-1) << LOG2(cache_block_size);

		c2.LRU_head = (Pcache_line *) calloc(1,sizeof(Pcache_line)*c2.n_sets);
		c2.LRU_tail = (Pcache_line *) calloc(1,sizeof(Pcache_line)*c2.n_sets);
		c2.set_contents = (int *) calloc(1, sizeof(int) * c2.n_sets);

		for (i=0;i<c2.n_sets;i++){
			c2.LRU_head[i] = NULL;
			c2.LRU_tail[i] = NULL;
			c2.set_contents[i] = 0;
		}
	}
	else{

		c1.size = cache_usize/4;
		c1.associativity = cache_assoc;
		c1.index_mask_offset = LOG2(cache_block_size);
		c1.n_sets = cache_usize/(cache_assoc*cache_block_size);
		c1.index_mask = (c1.n_sets-1) << LOG2(cache_block_size);

		c1.LRU_head = (Pcache_line *) calloc(1,sizeof(Pcache_line)*c1.n_sets);  //maybe * after Pcache_line
		c1.LRU_tail = (Pcache_line *) calloc(1,sizeof(Pcache_line)*c1.n_sets);
		c1.set_contents = (int *) calloc(1, sizeof(int) * c1.n_sets);

		for (i=0;i<c1.n_sets;i++){
			c1.LRU_head[i] = NULL;
			c1.LRU_tail[i] = NULL;
			c1.set_contents[i] = 0;
		}


	}

}
/************************************************************/

/************************************************************/
void perform_access(addr, access_type)
		unsigned addr, access_type;
{

	/* handle an access to the cache */
	cache* to_access;

	if(cache_split==1){
		if(access_type == TRACE_INST_LOAD ){
			to_access = &c1;
		}
		else{
			to_access = &c2;
		}
	}
	else{
		to_access = &c1;
	}
	unsigned int ind = (addr & to_access->index_mask) >> to_access->index_mask_offset;
	unsigned int tag = addr >> (to_access->index_mask_offset + LOG2(to_access->n_sets));


	if(access_type == TRACE_INST_LOAD ){
		cache_stat_inst.accesses += 1;
	}
	else{
		cache_stat_data.accesses += 1;
	}
	Pcache_line toInsert;
	if(to_access->LRU_head[ind]== NULL){

		toInsert = (Pcache_line) calloc(1,sizeof(cache_line));
		toInsert->tag = tag;
		toInsert->dirty = FALSE;

		if(access_type == TRACE_INST_LOAD ){
			cache_stat_inst.misses += 1;
			cache_stat_inst.demand_fetches += cache_block_size/4;
		}
		else if (access_type == TRACE_DATA_LOAD) {
			cache_stat_data.misses += 1;
			cache_stat_data.demand_fetches += cache_block_size/4;
		}
		else { //DATA STORE
			cache_stat_data.misses += 1;
			if(cache_writealloc == TRUE) {
				cache_stat_data.demand_fetches += cache_block_size/4;
				if(cache_writeback == TRUE) {
					toInsert->dirty = TRUE;
				}
				else {
					//if write through, then copy back one word
					cache_stat_data.copies_back++;
				}
			}
			else{
				cache_stat_data.copies_back++;
			}

		}

		if(cache_writealloc == TRUE || access_type != TRACE_DATA_STORE) {
			to_access->set_contents[ind] = 1;
			insert(&(to_access->LRU_head[ind]),&(to_access->LRU_tail[ind]),toInsert);
		}

	}

	else{
		Pcache_line c_line;
		int found_flag = 0;
		for(c_line = to_access->LRU_head[ind]; c_line != to_access->LRU_tail[ind]->LRU_next; c_line = c_line->LRU_next) {
			if(c_line->tag == tag) {
				found_flag = 1;
				break;
			}
		}
		if(found_flag == 0){
			toInsert = (Pcache_line) calloc(1,sizeof(cache_line));
			toInsert->tag = tag;
			toInsert->dirty = FALSE;

			toInsert->LRU_next = NULL;
			toInsert->LRU_prev = NULL;

			if(access_type == TRACE_INST_LOAD ){
				cache_stat_inst.misses += 1;
				cache_stat_inst.demand_fetches += cache_block_size/4;
			}
			else if(access_type == TRACE_DATA_LOAD) {
				cache_stat_data.misses += 1;
				cache_stat_data.demand_fetches += cache_block_size/4;
			}
			else{//TRACE_DATA_STORE
				cache_stat_data.misses += 1;
				if(cache_writealloc == TRUE) {
					cache_stat_data.demand_fetches += cache_block_size/4;
					if(cache_writeback == TRUE) {
						toInsert->dirty = TRUE;
					}
					else {
						//if write through, then copy back one word
						cache_stat_data.copies_back++;
					}
				}
				else{
					cache_stat_data.copies_back++;
				}
			}

			if(to_access->set_contents[ind]<to_access->associativity){

				if(cache_writealloc == TRUE || access_type != TRACE_DATA_STORE) {
					insert(&(to_access->LRU_head[ind]),&(to_access->LRU_tail[ind]),toInsert);
					to_access->set_contents[ind] += 1;
				}

			}
			else{

				if(access_type == TRACE_INST_LOAD ){
					cache_stat_inst.replacements += 1;
				}
				else{
					if(cache_writealloc == TRUE  || access_type != TRACE_DATA_STORE) {
						cache_stat_data.replacements += 1;
					}
				}
				if(cache_writealloc == TRUE || access_type != TRACE_DATA_STORE) {

					if(to_access->LRU_tail[ind]->dirty == TRUE && (access_type == TRACE_DATA_LOAD || access_type == TRACE_DATA_STORE)) {
						cache_stat_data.copies_back += cache_block_size/4;
					}
					else if(to_access->LRU_tail[ind]->dirty == TRUE && access_type == TRACE_INST_LOAD) {
						cache_stat_inst.copies_back += cache_block_size/4;
					}

					delete(&(to_access->LRU_head[ind]),&(to_access->LRU_tail[ind]),to_access->LRU_tail[ind]);
					insert(&(to_access->LRU_head[ind]),&(to_access->LRU_tail[ind]),toInsert);
					to_access->set_contents[ind] += 1;
				}
			}
		}

		else{ // Cache Hit
			Pcache_line current_line = c_line;
			delete(&(to_access->LRU_head[ind]),&(to_access->LRU_tail[ind]),c_line);
			insert(&(to_access->LRU_head[ind]),&(to_access->LRU_tail[ind]),current_line);
			if(access_type == TRACE_DATA_STORE){
				if(cache_writeback == TRUE){
					current_line->dirty = 1;
				}
				else{
					cache_stat_data.copies_back++;
				}
			}
		}

	}

}
/************************************************************/

/************************************************************/
void flush()
{
	/* flush the cache */
	int i;
	//dump_cache();
	for(i = 0; i < c1.n_sets; i++) {
		Pcache_line c_line;
		if(c1.LRU_head[i] != NULL) {
			for(c_line = c1.LRU_head[i]; c_line != c1.LRU_tail[i]->LRU_next; c_line = c_line->LRU_next) {
				if(c_line != NULL && c_line->dirty == TRUE) {
					cache_stat_inst.copies_back += cache_block_size/4;
				}
			}
		}
	}

	for(i = 0; i < c2.n_sets; i++) {
		Pcache_line c_line;
		if(c2.LRU_head[i] != NULL) {
			for(c_line = c2.LRU_head[i]; c_line != c2.LRU_tail[i]->LRU_next; c_line = c_line->LRU_next) {
				if(c_line != NULL && c_line->dirty == TRUE) {
					cache_stat_data.copies_back += cache_block_size/4;
				}
			}
		}
	}


}
/************************************************************/

/************************************************************/
void delete(head, tail, item)
		Pcache_line *head, *tail;
		Pcache_line item;
{
	if (item->LRU_prev) {
		item->LRU_prev->LRU_next = item->LRU_next;
	} else {
		/* item at head */
		*head = item->LRU_next;
	}

	if (item->LRU_next) {
		item->LRU_next->LRU_prev = item->LRU_prev;
	} else {
		/* item at tail */
		*tail = item->LRU_prev;
	}
}
/************************************************************/

/************************************************************/
/* inserts at the head of the list */
void insert(head, tail, item)
		Pcache_line *head, *tail;
		Pcache_line item;
{
	item->LRU_next = *head;
	item->LRU_prev = (Pcache_line)NULL;

	if (item->LRU_next)
		item->LRU_next->LRU_prev = item;
	else
		*tail = item;

	*head = item;
}
/************************************************************/

/************************************************************/
void dump_settings()
{
	printf("*** CACHE SETTINGS ***\n");
	if (cache_split) {
		printf("  Split I- D-cache\n");
		printf("  I-cache size: \t%d\n", cache_isize);
		printf("  D-cache size: \t%d\n", cache_dsize);
	} else {
		printf("  Unified I- D-cache\n");
		printf("  Size: \t%d\n", cache_usize);
	}
	printf("  Associativity: \t%d\n", cache_assoc);
	printf("  Block size: \t%d\n", cache_block_size);
	printf("  Write policy: \t%s\n",
		   cache_writeback ? "WRITE BACK" : "WRITE THROUGH");
	printf("  Allocation policy: \t%s\n",
		   cache_writealloc ? "WRITE ALLOCATE" : "WRITE NO ALLOCATE");
}
/************************************************************/

/************************************************************/
void print_stats()
{
	printf("\n*** CACHE STATISTICS ***\n");

	printf(" INSTRUCTIONS\n");
	printf("  accesses:  %d\n", cache_stat_inst.accesses);
	printf("  misses:    %d\n", cache_stat_inst.misses);
	if (!cache_stat_inst.accesses)
		printf("  miss rate: 0 (0)\n");
	else
		printf("  miss rate: %2.4f%% (hit rate %2.4f%%)\n",
			   100*(float)cache_stat_inst.misses / (float)cache_stat_inst.accesses,100*(1.0 - (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses));
	printf("  replace:   %d\n", cache_stat_inst.replacements);

	printf(" DATA\n");
	printf("  accesses:  %d\n", cache_stat_data.accesses);
	printf("  misses:    %d\n", cache_stat_data.misses);
	if (!cache_stat_data.accesses)
		printf("  miss rate: 0 (0)\n");
	else
		printf("  miss rate: %2.4f%% (hit rate %2.4f%%)\n",
			   100*(float)cache_stat_data.misses / (float)cache_stat_data.accesses,
			   100*(1.0 - (float)cache_stat_data.misses / (float)cache_stat_data.accesses));
	printf("  replace:   %d\n", cache_stat_data.replacements);

	printf(" TRAFFIC (in words)\n");
	printf("  demand fetch:  %d\n", cache_stat_inst.demand_fetches +
									cache_stat_data.demand_fetches);
	printf("  copies back:   %d\n", cache_stat_inst.copies_back +
									cache_stat_data.copies_back);
}
/************************************************************/
void dump_cache() {
	int i;
	Pcache_line line;
	for(i = 0; i < c1.n_sets; i++) {
		for(line = c1.LRU_head[i]; line != c1.LRU_tail[i]; line = line->LRU_next) {
			printf("%d ", line->tag);
		}
		printf("\n");
	}
	printf("num_sets = %d\n", i);
}
