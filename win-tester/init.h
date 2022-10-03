#ifndef INIT_H_
#define INIT_H_

#include <QDebug>

#define N_STEPPERS 4
#define N_SENSORS 7
#define N_CONTROLLERS 4
#define N_FUNCTIONS (N_STEPPERS + N_SENSORS + N_CONTROLLERS)

/* General settings */
#define CONTROLLER_REFRESH_PERIOD 20 /* 50 Hertz */
#define CONTROLLER_INTEGRAL_ENABLE FALSE /* No integral term for PID */
#define CONTROLLER_FAST_MULTIPLIER 10.0f
#define SDADC_AVERAGE_FACTOR 32U
#define MODBUS_FRAME_LENGTH 128U /* Max frame size for modbus server */
#define USART_BAUDRATE 115200U /* Baud rate for serial communication */

/* Volumeter coefficients */
#define VOLUM_CTRL_KP 0.008f
#define VOLUM_CTRL_KD 0.2f
#define VOLUM_CLAMP_SPEED  300.0f
#define VOLUM_ERROR_MARGIN 3000.0f
#define VOLUM_ACCEL_FREE   10000.0f
#define VOLUM_ACCEL_LOCKED 3000.0f

/* Load frame coefficients */
#define LOAD_CTRL_KP 0.3f
#define LOAD_CTRL_KD 3.0f
#define LOAD_CLAMP_SPEED  2000.0f
#define LOAD_ERROR_MARGIN 5.0f
#define LOAD_ACCEL_FREE   10000.0f
#define LOAD_ACCEL_LOCKED 5000.0f

/* Shear device specific coefficients */
#define SHEAR_CTRL_KP 10.0f
#define SHEAR_CTRL_KD 30.0f


#define TRUE 1
#define FALSE 0

union Int2F {
    float f;
    unsigned int u32;
    int i32;
};

struct Calibration
{
  float slope;
  float intercept;
};

struct ModbusData {
    qint16 command;
    qint16 idata;
    struct Calibration caldata;
    union Int2F func[N_FUNCTIONS];
};

inline unsigned int millis(void) {
//    return __ticks;
    return 0;
}

inline unsigned int micros(void) {
    unsigned int ticks, counts;

    do {
//        ticks = __ticks;
        ticks = 1;
//        counts = SysTick->VAL;
        counts = 1;
    } while(ticks != 1);
//} while(ticks != __ticks);

//    return ticks * 1000U + ((SYSCLK_VALUE / 1000U) - counts) / (SYSCLK_VALUE / 1000000U);
    return ticks * 1000U + ((1 / 1000U) - counts) / (1 / 1000000U);
}

void Peripherals_Init(void);

#endif /* INIT_H_ */
