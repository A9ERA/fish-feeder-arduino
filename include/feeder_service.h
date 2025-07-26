#ifndef FEEDER_SERVICE_H
#define FEEDER_SERVICE_H

// Feeder service control functions
void initFeederService();
void startFeederSequence(int feedAmount, int blowerDuration);
void stopFeederSequence();

#endif // FEEDER_SERVICE_H 