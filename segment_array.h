#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define SEGMENTS_TO_SKIP 2
#define MAX_SEG_COUNT (32 - SEGMENTS_TO_SKIP)

enum {
	SA_SUCCESS,
	E_SA_OUT_OF_MEMORY,
	E_SA_ALREADY_MAX_SIZE,
	E_SA_WILL_LOSE_ELEMENTS,
	E_SA_INVALID_INDEX,
	E_SA_ALREADY_EMPTY
};


typedef struct {
	void *segments[MAX_SEG_COUNT];
	int segment_count;
	uint64_t size;  // number of slots/elements in use;
	size_t element_size;  // size in bytes of the elements
} __segment_array;

typedef __segment_array *segment_array_t;


/* Deletes the segment array. */
void sa_delete(segment_array_t sa);
/* Deletes the segment array and calls delete_func on all elements.
*
*  If delete_func is NULL, it is the same as sa_delete.
*  The delete_func must not try to free the pointer it receives.
*/
void sa_delete2(segment_array_t sa, void (*delete_func)(void *));
/* Creates a new segment array with INIT_SEG_COUNT segments.
*
*  @param element_size should be the size in byte of the elements intended for storage.
*    sizeof should be used.
*/
segment_array_t sa_create(size_t element_size);
/* Makes a shallow copy of a segment array. */
segment_array_t sa_dup(const segment_array_t old_sa);

/* Changes the capacity of a segment_array to the smallest power of two bigger or equal to new_capacity.
*
*  When downsizing, if the current elements can't fit in the new capacity,
*  it will throw the error WILL_LOSE_ELEMENTS and the segment array will remain unchanged.
*  Unless force is true, in that case, elements will be lost.
*
*  When upsizing, if it runs out of memory, OUT_OF_MEMORY is returned and the segment array will remain unchanged.
*  The memory for all new elements is set to 0.
*  If force is true, the segments already added will remain, the resizing will be have done.
*  The segment array will be in a normal state, but not with the total size requested.
*
*  When force is true, SUCCESS is always returned.
*/
int sa_resize(segment_array_t sa, uint64_t new_capacity, bool force);
/* Clears the segment array. It will retain only INIT_SEG_COUNT segments. */
void sa_clear(segment_array_t sa);
/* Clears the segment array and calls delete_func on all elements.
*
*  If delete_func is NULL, it is the same as sa_clear.
*  The delete_func must not try to free the pointer it receives.
*/
void sa_clear2(segment_array_t sa, void (*delete_func)(void *));

/* Returns a pointer to the element stored in the given index.
*
*  NULL is returned if index is bigger or equal to the size of the segment array.
*/
void *sa_get(const segment_array_t sa, uint32_t index);
/* Creates space for a new element at the end of the segment array.
*
*  slot_p will be set to point to the new element. The memory of the new element isn't
*  set to anything.
*  If something goes wrong, slot_p will be set to NULL.
*/
int sa_new(segment_array_t sa, void **slot_p);
/* Addes a shallow copy of an element to the end of the segment array. */
int sa_push_back(segment_array_t sa, const void *new_element) ;
/* Remove the last element of the segment array.
*
*  If the segment array is empty, ALREADY_EMPTY is returned.
*  If it is possible, unused memory of the segment array will be released.
*/
int sa_pop(segment_array_t sa);
/* Replaces the value of a given element with a new one, but provides the old value.
*
*  INVALID_INDEX is returned if index is bigger or equal to the size of the segment array.
*
*  If new_element is NULL, the old element is left unchagned.
*  This can be used to put a copy of an element in a buffer of appropriate size.
*
*  If old_element is NULL, the old value isn't returned.
*  old_element must point to a buffer large enough to hold an element.
*/
int sa_switch(segment_array_t sa, uint32_t index, const void *new_element, void *old_element);

/* Returns the size of the segment array. (the number of elements) */
uint64_t sa_size(const segment_array_t sa);
/* Returns the capacity of the segment array. */
uint64_t sa_capacity(const segment_array_t sa);
/* Returns the size of the elements of the segment array in bytes. */
size_t sa_element_size(const segment_array_t sa);
/* Returns true if the segment array is empty. */
bool sa_is_empty(const segment_array_t sa);
/* Returns true if the segment array is full, can't grow any more. */
bool sa_is_full(const segment_array_t sa);
