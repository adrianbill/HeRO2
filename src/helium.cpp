// Libraries
#include <Arduino.h>	    // Basic Library
#include <RunningAverage.h> // Running Average Library
#include <RunningMedian.h>  // Running Median Library
#include <math.h>	    // Math Library
#include <NewPing.h>        // US Sensor Library   
#include <EEPROM.h>

// Custom Headers
#include "constants.h"
#include "environment.h" // environmenttal parameters: temperature, humidity, atmospheric pressure
#include "oxygen.h"      // oxygen calculations
#include "menu.h"

// Paired Header
#include "helium.h"

// Running Average Setup
RunningAverage RA_dur(1);
RunningAverage RA_dur_raw(1);
RunningMedian RM_dur(1);
RunningMedian RM_speed(11);
RunningMedian RM_He(11);
RunningMedian RM_dist_calibration(101);

NewPing sonar(trigPin, echoPin0, 200);

// helium initialization
int He_Initialise(void)
{
        // establish pins
        // pinMode(trigPin, OUTPUT);
        // pinMode(echoPin0, INPUT);
        // pinMode(echoPin1, INPUT);
        
        RA_dur.clear();
        RA_dur_raw.clear();
        RM_dur.clear();
        RM_He.clear();
        RM_speed.clear();

        // if (!measure_duration()) {
        //         Serial.println("Failed to measure speed.");
        //         while (1)
        //                 ;
        // }

        return 1;
}

// Function to calculate the molar mass of the mixture
double calculate_molar_mass(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction)
{
        check_button_event();
        return He_fraction * He_molar_mass + O2_fraction * O2_molar_mass + N2_fraction * N2_molar_mass + H2O_fraction * H2O_molar_mass;
}

// Function to calculate the effective adiabatic index (tmp=(γ−1)^-1
double calculate_adiabatic_index(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction)
{
        double He_tmp = (1 / (He_adiabatic_index - 1));
        double O2_tmp = (1 / (O2_adiabatic_index - 1));
        double N2_tmp = (1 / (N2_adiabatic_index - 1));
        double H2O_tmp = (1 / (H2O_adiabatic_index - 1));

        check_button_event();

        return (1 / (He_fraction * He_tmp + O2_fraction * O2_tmp + N2_fraction * N2_tmp + H2O_fraction * H2O_tmp)) + 1;
}

// Function to calculate the speed of sound
double calculate_speed_of_sound(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction, double temperature)
{
        double mix_molar_mass = calculate_molar_mass(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
        double mix_adiabatic_index = calculate_adiabatic_index(He_fraction, O2_fraction, N2_fraction, H2O_fraction);

        check_button_event();

        return sqrt(mix_adiabatic_index * R_gas_constant * temperature / mix_molar_mass);
}

// Function to measure the sound travel time in seconds
double measure_duration(void)
{
        if (toggle_He) return 1;

        check_button_event();

        // currentMillis = millis();
        deltaMillis_dur = millis() - previousMillis_dur;
        previousMillis_dur = millis();

        if (deltaMillis_dur > ping_delay) RA_dur.add(sonar.ping() / 1000000.0);
        
        return RA_dur.getValue(0);

        // ----------------------------------------------------

        // if (deltaMillis_dur > 50) RM_dur.add(sonar.ping());
        
        // return RM_dur.getMedian() / 1000000.0;

        // ----------------------------------------------------

        // // Clear the trigger pin
        // digitalWrite(trigPin, LOW);
        // delayMicroseconds(2);

        // // Send a 10 microsecond pulse to trigger the sensor
        // digitalWrite(trigPin, HIGH);
        // delayMicroseconds(10);
        // digitalWrite(trigPin, LOW);

        // double duration0 = pulseIn(echoPin0, HIGH);
        // // RM_dur.add(duration0);
                              
        // return duration0 / 1000000.0;

        // return RM_dur.getMedian() / 1000000.0;
}

// Function to measure the sound travel time in microseconds
double measure_duration_raw(void)
{
        check_button_event();

        // currentMillis = millis();
        deltaMillis_dur_raw = millis() - previousMillis_dur_raw;
        previousMillis_dur_raw = millis();

        if (deltaMillis_dur_raw > ping_delay) RA_dur_raw.add(sonar.ping());

        return RA_dur_raw.getValue(0);
}

// speed of sound measurement
double speed_measurement(void)
{      
        // RM_speed.clear();

        // while (!RM_speed.isFull()) {
        //         double duration = measure_duration();
        //         if (duration != 0) RM_speed.add(distance_calibration / duration);
        //         check_button_event();
        // }

        double duration = measure_duration();

        if (duration > 0) RM_speed.add(distance_calibration / duration); 

        // while (!RM_speed.isFull()) RM_speed.add(EEPROM.readDouble(eeprom_dist_address) / (measure_duration())); // Calculate the speeds in m/s

        double speed = RM_speed.getMedianAverage(5);

        // // return RM_speed.getMedianAverage(5);
        
        // RM_speed.add(distance_calibration / measure_duration()); // Calculate the speeds in m/s
        
        // RM_speed.clear();
        
        check_button_event();

        return speed;
}

// Function to calibrate distance, returns distance in m
void calibrate_distance(double He_fraction)
{
        RM_dist_calibration.clear();

        while (!RM_dist_calibration.isFull()) {

                double O2_fraction = oxygen_measurement();
                double H2O_fraction = water_measurement();
                double temperature_K = temperature_measurement();
                double N2_fraction = 1.0 - (He_fraction + O2_fraction + H2O_fraction);
                double speed_of_sound_calculated = calculate_speed_of_sound(He_fraction, O2_fraction, N2_fraction, H2O_fraction, temperature_K);
                // double duration = measure_duration() / 1000000.0;

                RM_dist_calibration.add(speed_of_sound_calculated * measure_duration());

                check_button_event();
        }

        distance_calibration = RM_dist_calibration.getMedianAverage(15);

        EEPROM.writeDouble(eeprom_dist_address, distance_calibration);

        EEPROM.commit();

        RM_dist_calibration.clear();

        check_button_event();

        // Serial.println("Dist Calibrated");

        // Serial.print("Value: ");
        // Serial.print(distance_calibrated, 8);
        // Serial.print(" | eeprom: ");
        // Serial.println(EEPROM.readDouble(eeprom_dist_address), 8);
}

//  New function to trigger helium reading under development
double helium_measurement(double He_fraction, double O2_fraction, double H2O_fraction, double speed_of_sound_measured, double temperature)
{     
        double gain = 1.0;
        double threshold = 0.01;      

        double He_fraction_max = 1.0 - O2_fraction - H2O_fraction;
        double N2_fraction = 1.0 - O2_fraction - H2O_fraction - He_fraction;

        double speed_of_sound_calculated = 0;
        double error = 0;

        check_button_event();

        speed_of_sound_calculated = calculate_speed_of_sound(He_fraction, O2_fraction, N2_fraction, H2O_fraction, temperature);
        error = (speed_of_sound_measured - speed_of_sound_calculated) / speed_of_sound_calculated;
        He_fraction = He_fraction + (gain * error); 

        if (He_fraction < 0) RM_He.add(0);
        else if (He_fraction > He_fraction_max) RM_He.add(He_fraction_max);
        else RM_He.add(He_fraction);

        He_spd = speed_of_sound_calculated;
        He_error = error;

        check_button_event();

        return RM_He.getMedianAverage(5);
}

double helium_stddev(void)
{
        return 0.0;
        // return RM_He.getStandardDeviation();
}