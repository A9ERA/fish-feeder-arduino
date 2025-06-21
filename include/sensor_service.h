#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

void initAllSensors();
void readAndPrintAllSensors();
void controlSensor();

// New timer-based sensor service functions
void initSensorService();
void updateSensorService();
void setSensorPrintInterval(unsigned long intervalMs);

// Sensor service control functions
void startSensorService();
void stopSensorService();
bool isSensorServiceActive();

#endif 