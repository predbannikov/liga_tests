#ifndef SENSOR_H_
#define SENSOR_H_

#include <QDebug>
#include "init.h"
//#include "sdadc.h"
//#include "stm32f3xx.h"
//#include "comptypes.h"

struct Sensor
{
  struct SDADC *hSdadc;
  qint8 channel;
  qint8 id;

  struct Calibration calibration;
  float offset;
};

void Sensor_Init(struct Sensor *sensor);
void Sensor_Null(struct Sensor *sensor);
void Sensor_SetOffset(struct Sensor *sensor, float offset);

float Sensor_Read(struct Sensor *sensor);
unsigned int Sensor_ReadRaw(struct Sensor *sensor);

void Sensor_SetCalibration(struct Sensor *sensor, struct Calibration calibration);
void Sensor_SetCalibration2f(struct Sensor *sensor, float slope, float intercept);
struct Calibration Sensor_GetCalibration(struct Sensor *sensor);

#endif /* SENSOR_H_ */
