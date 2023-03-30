
/******************************************************************************
 *
 * File: circular_buffer.h
 *
 * Description: This header file contains the declarations of functions for
 *              managing a circular buffer. It includes functions for
 *              initializing, writing to, reading from, and flushing the
 *              circular buffer.
 *
 *****************************************************************************/
#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "../essentials/struct.h"

void cb_init(circular_buffer_t *cb);
size_t cb_write(circular_buffer_t *cb, char *data, size_t len);
size_t cb_read_LF(circular_buffer_t *cb, char *buffer, size_t len);
size_t cb_available(circular_buffer_t *cb);
void cb_flush(circular_buffer_t *cb);

#endif // CIRCULAR_BUFFER_H
