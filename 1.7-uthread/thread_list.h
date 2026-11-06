#include "uthread_impl.h"
#include <stdbool.h>

void ll_add(uthread *tcb);


void ll_remove(uthread *tcb);

bool ll_find(uthread *needle);

/** Get the number of threads
 * O(1) complexity
 */
int ll_length();

uthread *ll_choose_thread();
