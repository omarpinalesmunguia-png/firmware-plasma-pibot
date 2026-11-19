/*
  Persistent reset diagnostics for the PiBot plasma controller.

  The active SD job marker lives in RTC no-init memory. It is updated in RAM
  only, so it does not wear flash. After a brownout, watchdog or panic reset,
  $PIBOTDIAG reports whether a TF job was active and its last parsed line.
*/

#include "pibot_diag.h"

#include <stdio.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_system.h"

#include "grbl/hal.h"
#include "grbl/system.h"

#define PIBOT_DIAG_MAGIC 0x50424447u /* PBDG */
#define PIBOT_DIAG_VERSION 1u
#define PIBOT_DIAG_FILENAME_LEN 48

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t generation;
    uint32_t job_active;
    uint32_t line;
    uint32_t position;
    uint32_t size;
    char filename[PIBOT_DIAG_FILENAME_LEN];
    uint32_t checksum;
} pibot_rtc_diag_t;

RTC_NOINIT_ATTR static pibot_rtc_diag_t rtc_diag;

static pibot_rtc_diag_t previous_diag;
static bool previous_valid = false;
static esp_reset_reason_t reset_reason = ESP_RST_UNKNOWN;

static uint32_t diag_checksum (const pibot_rtc_diag_t *diag)
{
    const uint8_t *data = (const uint8_t *)diag;
    size_t length = offsetof(pibot_rtc_diag_t, checksum);
    uint32_t hash = 2166136261u;

    while(length--) {
        hash ^= *data++;
        hash *= 16777619u;
    }

    return hash;
}

static bool diag_valid (const pibot_rtc_diag_t *diag)
{
    return diag->magic == PIBOT_DIAG_MAGIC &&
           diag->version == PIBOT_DIAG_VERSION &&
           diag->checksum == diag_checksum(diag);
}

static void diag_commit (void)
{
    rtc_diag.checksum = diag_checksum(&rtc_diag);
}

static const char *reset_reason_name (esp_reset_reason_t reason)
{
    switch(reason) {
        case ESP_RST_POWERON:  return "POWERON";
        case ESP_RST_EXT:      return "EXTERNAL";
        case ESP_RST_SW:       return "SOFTWARE";
        case ESP_RST_PANIC:    return "PANIC";
        case ESP_RST_INT_WDT:  return "INT_WATCHDOG";
        case ESP_RST_TASK_WDT: return "TASK_WATCHDOG";
        case ESP_RST_WDT:      return "WATCHDOG";
        case ESP_RST_DEEPSLEEP:return "DEEPSLEEP";
        case ESP_RST_BROWNOUT: return "BROWNOUT";
        case ESP_RST_SDIO:     return "SDIO";
        default:               return "UNKNOWN";
    }
}

void pibot_diag_capture_reset (void)
{
    reset_reason = esp_reset_reason();
    previous_valid = diag_valid(&rtc_diag);
    if(previous_valid)
        memcpy(&previous_diag, &rtc_diag, sizeof(previous_diag));
    else
        memset(&previous_diag, 0, sizeof(previous_diag));

    uint32_t generation = previous_valid ? previous_diag.generation + 1u : 1u;
    memset(&rtc_diag, 0, sizeof(rtc_diag));
    rtc_diag.magic = PIBOT_DIAG_MAGIC;
    rtc_diag.version = PIBOT_DIAG_VERSION;
    rtc_diag.generation = generation;
    diag_commit();
}

void pibot_diag_job_started (const char *filename, size_t size)
{
    rtc_diag.job_active = 1u;
    rtc_diag.line = 0u;
    rtc_diag.position = 0u;
    rtc_diag.size = size > UINT32_MAX ? UINT32_MAX : (uint32_t)size;
    if(filename) {
        const char *base = strrchr(filename, '/');
        snprintf(rtc_diag.filename, sizeof(rtc_diag.filename), "%s", base ? base + 1 : filename);
    } else
        rtc_diag.filename[0] = '\0';
    diag_commit();
}

void pibot_diag_job_progress (uint32_t line, size_t position)
{
    if(rtc_diag.job_active) {
        rtc_diag.line = line;
        rtc_diag.position = position > UINT32_MAX ? UINT32_MAX : (uint32_t)position;
        diag_commit();
    }
}

void pibot_diag_job_ended (void)
{
    rtc_diag.job_active = 0u;
    diag_commit();
}

static status_code_t report_diagnostics (sys_state_t state, char *args)
{
    (void)state;
    (void)args;

    char message[220];
    snprintf(message, sizeof(message),
             "[PIBOT_DIAG:RESET=%s;CODE=%u;TRACE=%s;PREV_JOB=%s;LINE=%lu;POS=%lu;SIZE=%lu;FILE=%s]\r\n",
             reset_reason_name(reset_reason), (unsigned)reset_reason,
             previous_valid ? "VALID" : "NONE",
             previous_valid && previous_diag.job_active ? "ACTIVE" : "INACTIVE",
             (unsigned long)(previous_valid ? previous_diag.line : 0u),
             (unsigned long)(previous_valid ? previous_diag.position : 0u),
             (unsigned long)(previous_valid ? previous_diag.size : 0u),
             previous_valid && previous_diag.filename[0] ? previous_diag.filename : "-");
    hal.stream.write(message);

    return Status_OK;
}

void pibot_diag_init (void)
{
    static const sys_command_t commands[] = {
        { "PIBOTDIAG", report_diagnostics, { .noargs = On, .allow_blocking = On }, { .str = "report PiBot reset and interrupted TF job diagnostics" } }
    };
    static sys_commands_t command_list = {
        .n_commands = sizeof(commands) / sizeof(sys_command_t),
        .commands = commands
    };

    system_register_commands(&command_list);
}
