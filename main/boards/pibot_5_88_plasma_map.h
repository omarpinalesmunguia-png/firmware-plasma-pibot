/*
  pibot_5_88_plasma_map.h - PiBot V5.88 Ultra (ESP32-S3) configured as a plasma cutter

  Based on pibot_5_88_ultra_map.h, Copyright (c) 2026 @luc-github

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

  Machine: 3 axes (X, Y, Z) driven by external DM542 drivers, plasma torch with
  arc ok output, arc voltage via opto-isolated 52:1 divider -> MCP3221 (I2C),
  ohmic touch-off on the probe input, E-stop / feed hold / cycle start buttons.
  No X/Y limit switches - those opto inputs are reused for the buttons and arc ok.

  Electrical notes (from wiki.pibot.com, V5.88 Ultra pinout reference):
  - All limit/probe inputs are opto-isolated, NPN (sinking) type, with a 2.2 kOhm
    series resistor. Supply selected by the SW-VCC jumper (5V default).
    Wire buttons/contacts between the input and GND (dry contact).
  - VDD logic jumper selects 3.3V or 5V step/dir level - use 5V for DM542.
  - GPIO.7 (out1/Fwd) / GPIO.8 (out2/Rev) are optocoupler outputs, both transistor ends on the
    0-10V header: GPIO.7/GPIO.8 and "out Common" (isolated switch).
  - GPIO.4/GPIO.5 drive the MOSFET low-side outputs io.4-MOS-GND / io.5-MOS-GND (+ is Vmot-Main,
    500 mA max) and are also present as 5V logic on the PWM 5V header.
  - The RS485 header also has +5V and +3V3 pins (current rating not specified).
*/

#ifndef CONFIG_IDF_TARGET_ESP32S3
#error "This board has ESP32-S3 processor, select a corresponding build!"
#endif

#include "use_i2s_out.h"

#define BOARD_NAME "PiBot V5.88 Ultra - Plasma"
#define BOARD_URL "https://wiki.pibot.com/doku.php?id=pibot_cnc_laser_series:v588_ultra:pinout_reference:start"

// SPI (SD card / TMC)
#define SPI_MISO_PIN            GPIO_NUM_2
#define SPI_MOSI_PIN            GPIO_NUM_1
#define SPI_SCK_PIN             GPIO_NUM_21

// I2S shift registers (step/dir/enable)
#define I2S_OUT_BCK             GPIO_NUM_16
#define I2S_OUT_DATA            GPIO_NUM_17
#define I2S_OUT_WS              GPIO_NUM_18

// SD Card
#if SDCARD_ENABLE
#define SD_CS_PIN               GPIO_NUM_9
#endif

// I2C - OLED connector (GND, 3.3V, SCL, SDA). Used for the MCP3221 arc voltage ADC.
// The OLED (0x3C) can share the bus, the MCP3221 default address is 0x4D.
// NOTE: the driver does not enable the ESP32 internal pull-ups, the bus needs
//       external 4.7 kOhm pull-ups (most MCP3221/OLED breakout boards have them).
#define I2C_PORT                I2C_NUM_0
#define I2C_SDA                 GPIO_NUM_14
#define I2C_SCL                 GPIO_NUM_13
#define I2C_CLOCK               400000 // MCP3221 and SSD1306 both support 400 kHz, shortens blocking reads.

// Axes - X, Y and Z only
#define X_STEP_PIN              I2SO(2)
#define X_DIRECTION_PIN         I2SO(1)
#define X_ENABLE_PIN            I2SO(0)
#define MOTOR_CSX_PIN           I2SO(3)

#define Y_STEP_PIN              I2SO(5)
#define Y_DIRECTION_PIN         I2SO(4)
#define Y_ENABLE_PIN            I2SO(7)
#define MOTOR_CSY_PIN           I2SO(6)

#define Z_STEP_PIN              I2SO(10)
#define Z_DIRECTION_PIN         I2SO(9)
#define Z_ENABLE_PIN            I2SO(8)
#define MOTOR_CSZ_PIN           I2SO(11)

// Optional Z top limit switch (terminal GPIO.38). Leave hard limits ($21) and homing ($22) off
// if no switch is connected.
#define Z_LIMIT_PIN             GPIO_NUM_38 // Terminal GPIO.38 (lower row)

// Auxiliary inputs - opto-isolated terminals, Inputs [1] on the board diagram.
// Each terminal is 3 pins: GPIO.xx / GND / SW-VCC. Dry contacts go between GPIO.xx and GND.
// Lower row: GPIO.42 GPIO.41 GPIO.40 GPIO.39 GPIO.38   Upper row: GPIO.37 GPIO.36 GPIO.48 GPIO.47 GPIO.3
// NOTE: arc ok is placed last on purpose: the plasma plugin picks the highest free
//       aux input as default arc ok port.
#define AUXINPUT0_PIN           GPIO_NUM_47 // Terminal GPIO.47 (upper row) -> ohmic touch-off (probe)
#define AUXINPUT1_PIN           GPIO_NUM_42 // Terminal GPIO.42 (lower row) -> E-stop (mushroom, NC)
#define AUXINPUT2_PIN           GPIO_NUM_41 // Terminal GPIO.41 (lower row) -> Feed hold (red)
#define AUXINPUT3_PIN           GPIO_NUM_40 // Terminal GPIO.40 (lower row) -> Cycle start (green)
#define AUXINPUT4_PIN           GPIO_NUM_37 // Terminal GPIO.37 (upper row) -> spare
#define AUXINPUT5_PIN           GPIO_NUM_36 // Terminal GPIO.36 (upper row) -> spare
#define AUXINPUT6_PIN           GPIO_NUM_48 // Terminal GPIO.48 (upper row) -> spare
#define AUXINPUT7_PIN           GPIO_NUM_39 // Terminal GPIO.39 (lower row) -> Arc ok (from plasma CNC port)
// Terminal GPIO.3 (upper row, also on EXTRA IO [14], function set by jumper [10]) is not used.

// Auxiliary outputs
#define AUXOUTPUT0_PIN          GPIO_NUM_10 // EXTRA IO [14] GPIO.10
#define AUXOUTPUT1_PIN          GPIO_NUM_15 // EXTRA IO [14] GPIO.15
#define AUXOUTPUT2_PIN          GPIO_NUM_4  // MOSFET [6] io.4-MOS-GND - coolant mist  (M7). Also 5V logic on PWM 5V header [2].
#define AUXOUTPUT3_PIN          GPIO_NUM_5  // MOSFET [6] io.5-MOS-GND - coolant flood (M8). Also 5V logic on PWM 5V header [2].
#define AUXOUTPUT4_PIN          GPIO_NUM_7  // Outputs [2] 0-10V header: GPIO.7 (out1/Fwd) + out Common - TORCH ON
#define AUXOUTPUT5_PIN          GPIO_NUM_8  // Outputs [2] 0-10V header: GPIO.8 (out2/Rev) + out Common - spare
// GPIO.45, GPIO.46 (PWM 5V header) and GPIO.6 (0-10V) are not used in this configuration.

// Torch on/off = spindle enable (configure SPINDLE0_ENABLE as SPINDLE_ONOFF0 in my_machine.h).
// To use a MOSFET output instead change to AUXOUTPUT3_PIN (only if the SSR input accepts Vmot-Main).
#if DRIVER_SPINDLE_ENABLE & SPINDLE_ENA
#define SPINDLE_ENABLE_PIN      AUXOUTPUT4_PIN
#endif

// Coolant outputs - can be used for a water table pump, fume extraction, air etc.
#if COOLANT_ENABLE & COOLANT_MIST
#define COOLANT_MIST_PIN        AUXOUTPUT2_PIN
#endif
#if COOLANT_ENABLE & COOLANT_FLOOD
#define COOLANT_FLOOD_PIN       AUXOUTPUT3_PIN
#endif

// Control inputs
#if CONTROL_ENABLE & CONTROL_HALT
#define RESET_PIN               AUXINPUT1_PIN
#endif
#if CONTROL_ENABLE & CONTROL_FEED_HOLD
#define FEED_HOLD_PIN           AUXINPUT2_PIN
#endif
#if CONTROL_ENABLE & CONTROL_CYCLE_START
#define CYCLE_START_PIN         AUXINPUT3_PIN
#endif

// Probe (ohmic sensing). Use an isolated ohmic sensor module that presents a dry contact
// to this input, never connect the torch shield directly to the board.
#if PROBE_ENABLE
#define PROBE_PIN               AUXINPUT0_PIN
#endif

// UART1 (RS485, Modbus)
#define SERIAL1_PORT            1
#define UART1_RX_PIN            GPIO_NUM_11
#define UART1_TX_PIN            GPIO_NUM_12
#if MODBUS_ENABLE && (MODBUS_ENABLE & MODBUS_RTU_ENABLED)
#define MODBUS_RTU_STREAM 1
#endif

// UART2 (RJ12 - pendant, FNC expander)
#define SERIAL2_PORT
#define UART2_RX_PIN            GPIO_NUM_35
#define UART2_TX_PIN            GPIO_NUM_0
#if MPG_ENABLE && !defined(MPG_STREAM)
#define MPG_STREAM 2
#endif
#if FNC_EXPANDER_ENABLE && !defined(FNC_STREAM)
#define FNC_STREAM 2
#endif
