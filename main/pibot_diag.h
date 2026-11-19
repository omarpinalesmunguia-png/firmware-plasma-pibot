#ifndef PIBOT_DIAG_H
#define PIBOT_DIAG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void pibot_diag_capture_reset (void);
void pibot_diag_init (void);
void pibot_diag_job_started (const char *filename, size_t size);
void pibot_diag_job_progress (uint32_t line, size_t position);
void pibot_diag_job_ended (void);

#endif
