#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "segment_array.h"



inline static uint32_t _log2_floor(uint32_t num) {
	return 8 * sizeof(uint32_t) - __builtin_clz(num) - 1;
}


inline static uint32_t _segment_of_index(uint32_t index) {
	return _log2_floor((index >> SEGMENTS_TO_SKIP) | 1);
}


static int _min_seg_count(uint64_t size) {
	if(size == 0)
		return 1;
	return _segment_of_index(size - 1) + 1;
}


static uint64_t _segment_capacity(int segment) {
	if(segment == 0)
		segment = 1;
	return 1lu << (segment + SEGMENTS_TO_SKIP);
}


static int _add_segment(segment_array_t sa) {
	if(sa->segment_count == MAX_SEG_COUNT)
		return E_SA_ALREADY_MAX_SIZE;

	sa->segments[sa->segment_count] = calloc(sa->element_size, sa_capacity(sa));
	if(sa->segments[sa->segment_count] == NULL)
		return E_SA_OUT_OF_MEMORY;

	sa->segment_count++;

	return SA_SUCCESS;
}


static segment_array_t _bare_init_sa(size_t element_size) {
	segment_array_t new_sa = malloc(sizeof(__segment_array));
	if(new_sa == NULL)
		return NULL;

	new_sa->segment_count = 0;
	new_sa->size = 0;
	new_sa->element_size = element_size;

	return new_sa;
}


static void *_calculate_address(const segment_array_t sa, uint32_t index) {
	int segment = _segment_of_index(index);
	int iner_index = index - (-(segment > 0) & _segment_capacity(segment));
	return sa->segments[segment] + iner_index * sa->element_size;
}


void sa_delete(segment_array_t sa) {
	sa_delete2(sa, NULL);
}


void sa_delete2(segment_array_t sa, void (*delete_func)(void *)) {
	sa_clear2(sa, delete_func);
	free(sa->segments[0]);
	free(sa);
}


segment_array_t sa_create(size_t element_size) {
	segment_array_t new_sa = _bare_init_sa(element_size);
	if(new_sa == NULL)
		return NULL;

	if(_add_segment(new_sa)) {
		sa_delete(new_sa);
		return NULL;
	}

	return new_sa;
}


segment_array_t sa_dup(const segment_array_t old_sa) {
	segment_array_t new_sa = _bare_init_sa(old_sa->element_size);
	if(new_sa == NULL)
		return NULL;

	for(int i = 0; i < old_sa->segment_count; i++) {
		if(_add_segment(new_sa)) {
			sa_delete(new_sa);
			return NULL;
		}
		
		memcpy(new_sa->segments[i], old_sa->segments[i], old_sa->element_size * _segment_capacity(i));
	}

	new_sa->size = old_sa->size;

	return new_sa;
}


int sa_resize(segment_array_t sa, uint64_t new_size, bool force) {
	int new_segment_count = _min_seg_count(new_size);

	if(new_size >= sa->size) {
		uint64_t elements_to_fill = ((new_size <= sa_capacity(sa)) ? new_size : sa_capacity(sa)) - sa->size;
		memset(_calculate_address(sa, sa->size), 0, sa->element_size * elements_to_fill);

		for(int i = sa->segment_count; i < new_segment_count; i++) {
			if(_add_segment(sa) == E_SA_OUT_OF_MEMORY) {
				if(force) {
					sa->size = sa_capacity(sa);
					return SA_SUCCESS;
				}
				else {
					for(int j = sa->segment_count; j < i; j++) {
						free(sa->segments[j]);
						sa->segment_count--;
					}
					return E_SA_OUT_OF_MEMORY;
				}
			}
			memset(sa->segments[sa->segment_count-1], 0, _segment_capacity(sa->segment_count - 1));
		}
		sa->size = new_size;
	}
	else {
		if(!force)
			return E_SA_WILL_LOSE_ELEMENTS;

		for(int i = new_segment_count; i < sa->segment_count; i++)
			free(sa->segments[i]);
	
		sa->segment_count = new_segment_count;
		sa->size = new_size;
	}


	return SA_SUCCESS;
}


void sa_clear(segment_array_t sa) {
	sa_clear2(sa, NULL);
}


void sa_clear2(segment_array_t sa, void (*delete_func)(void *)) {
	if(delete_func != NULL)
		for(uint32_t i = 0; i < sa->size; i++)
			delete_func(_calculate_address(sa, i));

	for(int i = 1; i < sa->segment_count; i++)
		free(sa->segments[i]);

	sa->segment_count = 1;
	sa->size = 0;
}


void *sa_get(const segment_array_t sa, uint32_t index) {
	if(index >= sa->size)
		return NULL;

	return _calculate_address(sa, index);
}


int sa_new(segment_array_t sa, void **slot_p) {
	if(sa->size == sa_capacity(sa)) {
		int exit_code = _add_segment(sa);
		if(exit_code != SA_SUCCESS) {
			slot_p = NULL;
			return exit_code;
		}
	}

	*slot_p = _calculate_address(sa, sa->size++);

	return SA_SUCCESS;
}


int sa_push_back(segment_array_t sa, const void *new_element)  {
	void *slot;
	int exit_code = sa_new(sa, &slot);
	if(exit_code != SA_SUCCESS)
		return exit_code;

	memcpy(slot, new_element, sa->element_size);
	
	return SA_SUCCESS;
}


int sa_pop(segment_array_t sa) {
	if(sa_is_empty(sa))
		return E_SA_ALREADY_EMPTY;

	sa->size--;
	int last_segment = sa->segment_count - 1;
	if(sa->segment_count > 1 && sa->size <= _segment_capacity(last_segment)) {
		free(sa->segments[last_segment]);
		sa->segment_count--;
	}

	return SA_SUCCESS;
}


int sa_switch(segment_array_t sa, uint32_t index, const void *new_element, void *old_element) {
	void *slot = sa_get(sa, index);
	if(slot == NULL)
		return E_SA_INVALID_INDEX;

	if(old_element != NULL)
		memcpy(old_element, slot, sa->element_size);
	if(new_element != NULL)
		memcpy(slot, new_element, sa->element_size);

	return SA_SUCCESS;
}


uint64_t sa_size(const segment_array_t sa) {
	return sa->size;
}


uint64_t sa_capacity(const segment_array_t sa) {
	return _segment_capacity(sa->segment_count);
}


size_t sa_element_size(const segment_array_t sa) {
	return _segment_capacity(sa->segment_count);
}


bool sa_is_empty(const segment_array_t sa) {
	return sa->size == 0;
}


bool sa_is_full(const segment_array_t sa) {
	return sa->size == _segment_capacity(MAX_SEG_COUNT);
}
