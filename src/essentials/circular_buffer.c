#include "circular_buffer.h"

#include <string.h>

/**
 * @brief Initializes the circular buffer, setting read and write positions to 0 and
 * current_size to 0.
 *
 * @param cb: pointer to the circular_buffer_t structure
 */
void cb_init(circular_buffer_t *cb) {
  cb->read_pos = 0;
  cb->write_pos = 0;
  cb->current_size = 0;
}

/**
 * @brief Writes data to the buffer up to the specified length or until the buffer is
 * full.
 *
 * @param cb: pointer to the circular_buffer_t structure
 * @param data: pointer to the data to be written
 * @param len: length of the data to be written
 *
 * @return The number of bytes written.
 */
size_t cb_write(circular_buffer_t *cb, char *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (cb->current_size == CBUFFER_SIZE) {
      return i;
    }

    cb->data[cb->write_pos++] = data[i];
    cb->write_pos %= CBUFFER_SIZE;
    cb->current_size++;
  }

  return len;
}

/**
 * @brief Reads data from the buffer up to the specified length or until the buffer is
 * empty.
 *
 * @param cb: pointer to the circular_buffer_t structure
 * @param buffer: pointer to the buffer where the read data will be stored
 * @param len: maximum number of bytes to be read
 *
 * @return
 */
size_t cb_read_LF(circular_buffer_t *cb, char *buffer, size_t len) {
  size_t size_bck = cb->current_size, read_pos_bck = cb->read_pos;
  for (size_t i = 0; i < len; i++) {
    if (cb->current_size == 0) {
      cb->current_size = size_bck, cb->read_pos = read_pos_bck;
      return 0;
    }

    buffer[i] = cb->data[cb->read_pos++];
    cb->read_pos %= CBUFFER_SIZE;
    cb->current_size--;

    if (buffer[i] == '\n') {
      return 1;
    }
  }

  cb->current_size = size_bck, cb->read_pos = read_pos_bck;
  return 0;
}

/**
 * @brief Returns the number of available bytes in the buffer.
 *
 * @param cb: pointer to the circular_buffer_t structure
 *
 * @return The number of bytes available in the buffer.
 */
size_t cb_available(circular_buffer_t *cb) { return (CBUFFER_SIZE - cb->current_size); }

/**
 * @brief Flushes the buffer, setting read and write positions to 0, current_size to 0,
 * and clearing the data.
 *
 * @param cb: pointer to the circular_buffer_t structure
 */
void cb_flush(circular_buffer_t *cb) {
  cb->write_pos = 0;
  cb->read_pos = 0;
  cb->current_size = 0;
  memset(cb->data, 0, CBUFFER_SIZE);
}
