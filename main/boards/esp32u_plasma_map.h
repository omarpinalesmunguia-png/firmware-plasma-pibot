/*
  esp32u_plasma_map.h - ESP32-WROOM-32U (4 MB flash) plasma cutter, direct GPIO, no SD card

  Part of grblHAL

  grblHAL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  grblHAL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with grblHAL. If not, see <http://www.gnu.org/licenses/>.

  Machine: X, Y (two motors with two drivers sharing the Y step/dir signals), Z.
  Same plasma setup as the PiBot build: torch on/off (M3/M5), arc ok, arc voltage via
  MCP3221 (I2C), floating head on the probe input, optional ohmic sensing switched by a relay
  (M64/M65 P1), E-stop / feed hold / cycle start, limit switches X, Y and Z top.
  G-code is stored in internal flash (LittleFS mounted as root), uploaded by FTP and run with $F=.

  Electrical notes:
  - ESP32 I/O is 3.3 V. Step/dir/enable go through a 5 V buffer (74HCT245) to the drivers,
    the buffer also isolates the boot strapping pins (2, 12, 15) from the driver side.
    For the second Y driver use a separate buffer channel fed from the same GPIO.
  - GPIO 34-39 are input only and have NO internal pull-up: use external 10 kOhm to 3.3 V
    (opto-isolated input modules normally provide it).
  - Strapping pins: GPIO2 (Z dir), GPIO15 (enable, high at boot = drivers disabled on most
    drivers), GPIO5 (ohmic input), GPIO12 (spare output, must be LOW at boot - do not pull up,
    or burn the flash voltage eFuse to 3.3 V). GPIO14 outputs a PWM burst at boot: Y dir only.
  - GPIO4 (torch) has no boot activity, still add a pull-down on the relay/SSR input.
  - Do not use the internal ADC (ADC2 does not work with WiFi): arc voltage is read by the MCP3221.
*/

#if !CONFIG_IDF_TARGET_ESP32
#error "BOARD_ESP32U_PLASMA is for the classic ESP32 (ESP32-WROOM-32U), select target esp32!"
#endif

#if N_ABC_MOTORS > 0
#error "BOARD_ESP32U_PLASMA: axis configuration is not supported!"
#endif

#if SDCARD_ENABLE
#error "BOARD_ESP32U_PLASMA has no SD card, G-code is stored in internal flash (LITTLEFS_ENABLE 2)."
#endif

#define BOARD_NAME "ESP32-WROOM-32U - Plasma"

// Step pulse outputs (RMT)
#define X_STEP_PIN              GPIO_NUM_26
#define Y_STEP_PIN              GPIO_NUM_25 // drives both Y drivers through the buffer
#define Z_STEP_PIN              GPIO_NUM_33

// Direction outputs
#define X_DIRECTION_PIN         GPIO_NUM_27
#define Y_DIRECTION_PIN         GPIO_NUM_14 // reverse the second Y motor by swapping one coil pair (A+ <-> A-)
#define Z_DIRECTION_PIN         GPIO_NUM_2

// Stepper enable (shared)
#define STEPPERS_ENABLE_PIN     GPIO_NUM_15

// Limit switches, one per axis. Z is the top (retract) switch. Leave $21/$22 off until wired.
#define X_LIMIT_PIN             GPIO_NUM_16
#define Y_LIMIT_PIN             GPIO_NUM_17
#define Z_LIMIT_PIN             GPIO_NUM_39 // input only, external pull-up

// Auxiliary outputs. Order matters: claimed outputs (torch) are last so that the free ones
// keep the same M62-M65 port numbers as the PiBot build (P1 = ohmic relay).
#define AUXOUTPUT0_PIN          GPIO_NUM_12 // spare (P0) - strapping pin, must stay low at boot
#define AUXOUTPUT1_PIN          GPIO_NUM_23 // ohmic sensing relay (P1), M64 P1 / M65 P1
#define AUXOUTPUT2_PIN          GPIO_NUM_18 // spare (P2) - reserved for Y2 step if auto-squaring is added
#define AUXOUTPUT3_PIN          GPIO_NUM_19 // spare (P3) - reserved for Y2 dir if auto-squaring is added
#define AUXOUTPUT4_PIN          GPIO_NUM_4  // TORCH ON (spindle enable, M3/M5)

#if DRIVER_SPINDLE_ENABLE & SPINDLE_ENA
#define SPINDLE_ENABLE_PIN      AUXOUTPUT4_PIN
#endif

// No coolant outputs on this board.
#undef COOLANT_ENABLE
#define COOLANT_ENABLE 0

// Auxiliary inputs.
// NOTE: arc ok is placed last on purpose: the plasma plugin picks the highest free
//       aux input as default arc ok port.
#define AUXINPUT0_PIN           GPIO_NUM_32 // floating head switch (probe)
#define AUXINPUT1_PIN           GPIO_NUM_34 // E-stop (mushroom, NC) - input only, external pull-up
#define AUXINPUT2_PIN           GPIO_NUM_35 // Feed hold - input only, external pull-up
#define AUXINPUT3_PIN           GPIO_NUM_36 // Cycle start - input only, external pull-up
#define AUXINPUT4_PIN           GPIO_NUM_5  // spare: dedicated ohmic sense input (future)
#define AUXINPUT5_PIN           GPIO_NUM_13 // Arc ok (from plasma CNC port, via opto)

#if CONTROL_ENABLE & CONTROL_HALT
#define RESET_PIN               AUXINPUT1_PIN
#endif
#if CONTROL_ENABLE & CONTROL_FEED_HOLD
#define FEED_HOLD_PIN           AUXINPUT2_PIN
#endif
#if CONTROL_ENABLE & CONTROL_CYCLE_START
#define CYCLE_START_PIN         AUXINPUT3_PIN
#endif

// Probe: floating head switch. The ohmic sensor is switched onto this same input by the
// relay on P1 (as done by PlasmaPibotLimpio). Use an isolated ohmic module presenting a dry contact.
#if PROBE_ENABLE
#define PROBE_PIN               AUXINPUT0_PIN
#endif

// I2C for the MCP3221 arc voltage ADC (default address 0x4D). Needs 4.7 kOhm pull-ups.
#define I2C_PORT                I2C_NUM_0
#define I2C_SDA                 GPIO_NUM_21
#define I2C_SCL                 GPIO_NUM_22
#define I2C_CLOCK               400000
