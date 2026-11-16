/*
  my_machine_map.h - PiBot V5.88 Ultra para cortadora de plasma

  Mapa minimo basado en el mapa oficial pibot_5_88_ultra_map.h. Solo expone
  los tres ejes y las entradas/salidas verificadas que requiere la maquina.
*/

#ifndef CONFIG_IDF_TARGET_ESP32S3
#error "PiBot V5.88 Ultra requiere seleccionar el objetivo ESP32-S3"
#endif

#include "use_i2s_out.h"

#define BOARD_NAME "PiBot V5.88 Ultra - Plasma base limpia"

// SPI de la tarjeta. La SD permanece deshabilitada en la Etapa 2.
#define SPI_MISO_PIN            GPIO_NUM_2
#define SPI_MOSI_PIN            GPIO_NUM_1
#define SPI_SCK_PIN             GPIO_NUM_21

// Expansor I2S que genera los pasos de los motores.
#define I2S_OUT_BCK             GPIO_NUM_16
#define I2S_OUT_DATA            GPIO_NUM_17
#define I2S_OUT_WS              GPIO_NUM_18

#if SDCARD_ENABLE
#define SD_CS_PIN               GPIO_NUM_9
#endif

// Bus reservado para el MCP3221. Solo se activa en la etapa de voltaje.
#if I2C_ENABLE
#define I2C_PORT                I2C_NUM_0
#define I2C_SDA                 GPIO_NUM_14
#define I2C_SCL                 GPIO_NUM_13
#define I2C_CLOCK               100000
#endif

// Ejes X, Y y Z. No se habilitan motores A/B/C ni ejes esclavos.
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
#define Z_LIMIT_PIN             GPIO_NUM_38

// Entradas optoaisladas verificadas en la configuracion anterior.
#define AUXINPUT0_PIN           GPIO_NUM_47 // Sonda / cabezal flotante
#define AUXINPUT1_PIN           GPIO_NUM_42 // Paro de emergencia
#define AUXINPUT2_PIN           GPIO_NUM_41 // Feed hold
#define AUXINPUT3_PIN           GPIO_NUM_40 // Cycle start
#define AUXINPUT4_PIN           GPIO_NUM_37 // Reserva; conserva numeracion anterior
#define AUXINPUT5_PIN           GPIO_NUM_36 // Reserva; conserva numeracion anterior
#define AUXINPUT6_PIN           GPIO_NUM_48 // Reserva; conserva numeracion anterior
#define AUXINPUT7_PIN           GPIO_NUM_39 // Arc OK; conserva el puerto logico 7

// Salida digital de antorcha. Sin PWM y sin salida de direccion.
#define AUXOUTPUT0_PIN          GPIO_NUM_7

#if DRIVER_SPINDLE_ENABLE & SPINDLE_ENA
#define SPINDLE_ENABLE_PIN      AUXOUTPUT0_PIN
#endif

#if CONTROL_ENABLE & CONTROL_HALT
#define RESET_PIN               AUXINPUT1_PIN
#endif
#if CONTROL_ENABLE & CONTROL_FEED_HOLD
#define FEED_HOLD_PIN           AUXINPUT2_PIN
#endif
#if CONTROL_ENABLE & CONTROL_CYCLE_START
#define CYCLE_START_PIN         AUXINPUT3_PIN
#endif

#if PROBE_ENABLE
#define PROBE_PIN               AUXINPUT0_PIN
#endif

// UART1 y UART2 quedan sin consumidores en la Etapa 2, pero se conservan sus
// pines oficiales para evitar reasignaciones accidentales futuras.
#define SERIAL1_PORT            1
#define UART1_RX_PIN            GPIO_NUM_11
#define UART1_TX_PIN            GPIO_NUM_12

#define SERIAL2_PORT
#define UART2_RX_PIN            GPIO_NUM_35
#define UART2_TX_PIN            GPIO_NUM_0
