#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "../common/struct.h"
#include <sys/types.h>

void cb_init(circular_buffer_t *cb);
size_t cb_write(circular_buffer_t *cb, char *data, size_t len);
size_t cb_read(circular_buffer_t *cb, char *buffer, size_t len);
size_t cb_available(circular_buffer_t *cb);
void cb_flush(circular_buffer_t *cb);

#endif // !CIRCULAR_BUFFER_H
