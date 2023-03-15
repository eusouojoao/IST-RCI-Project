#ifndef RETRY_H
#define RETRY_H

int retry(int (*func)(), int max_attempts, ...);

#endif
