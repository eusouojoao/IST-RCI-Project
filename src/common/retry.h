#ifndef RETRY_H
#define RETRY_H

int retry(void (*func)(), int max_attempts, ...);

#endif
