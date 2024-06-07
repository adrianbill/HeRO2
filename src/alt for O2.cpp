#include <Arduino.h>
#include <Wire.h>
#include <ADS1X15.h>
#include <RunningAverage.h>

ADS1115 ads(0x48);

RunningAverage RA_O2_cal(20); //Initialize Running Average & Define running average pool size

void initialize_O2()
{
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  ads.setGain(16);

}

double calibrate_O2(double target_O2)
{

  for (int i = 0; i < 30; i++)
  {
    
    int16_t reading = ads.readADC_Differential_0_1();
   
    RA_O2_cal.addValue(reading);

    delay(300);
  }

  double millivolts = ads.toVoltage(RA_O2_cal.getAverage()) * 1000; //converts reading to voltage in mV

  Serial.print("Voltage: "); Serial.print(millivolts, 2); Serial.print("mV");
  Serial.print(" ±"); Serial.print(RA_O2_cal.getStandardDeviation(), 3); Serial.println("mV");

  return target_O2 / millivolts;
}

void setup()
{
  Serial.begin(115200);       //Start serial port (debugging purposes only)
  Wire.begin();

  initialize_O2();
}

void loop()
{
  
  double O2_calibration_factor = calibrate_O2(20.9);
  
  int16_t result = ads.readADC_Differential_0_1();

  double voltage = ads.toVoltage(result) * 1000; //converts reading to voltage in mV

  Serial.print("Voltage: "); Serial.print(voltage, 3); Serial.print("mV");
  Serial.print("  ||  O₂: "); Serial.print(voltage * O2_calibration_factor, 3); Serial.println("%");

  delay(1000);
}
