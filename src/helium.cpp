// Libraries
#include <Arduino.h>	    // Basic Library
#include <RunningAverage.h> // Running Average Library
#include <RunningMedian.h>  // Running Median Library
#include <math.h>	    // Math Library
#include <EEPROM.h>

// Custom Headers
#include "constants.h"
#include "environment.h" // environmenttal parameters: temperature, humidity, atmospheric pressure
#include "oxygen.h"      // oxygen calculations
#include "menu.h"

// Paired Header
#include "helium.h"

// Running Average Setup
RunningMedian RM_dur(3);
RunningMedian RM_speed(3);
RunningMedian RM_He(99);
RunningMedian RM_dist_calibration(51);

// unsigned long previousMillis = 0;

// helium initialization
int He_Initialise(void)
{
        // establish pins
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin0, INPUT);
        // pinMode(echoPin1, INPUT);
        
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
        return He_fraction * He_molar_mass + O2_fraction * O2_molar_mass + N2_fraction * N2_molar_mass + H2O_fraction * H2O_molar_mass;
}

// Function to calculate the effective adiabatic index (tmp=(γ−1)^-1
double calculate_adiabatic_index(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction)
{
        double He_tmp = (1 / (He_adiabatic_index - 1));
        double O2_tmp = (1 / (O2_adiabatic_index - 1));
        double N2_tmp = (1 / (N2_adiabatic_index - 1));
        double H2O_tmp = (1 / (H2O_adiabatic_index - 1));

        return (1 / (He_fraction * He_tmp + O2_fraction * O2_tmp + N2_fraction * N2_tmp + H2O_fraction * H2O_tmp)) + 1;
}

// Function to calculate the speed of sound
double calculate_speed_of_sound(double He_fraction, double O2_fraction, double N2_fraction, double H2O_fraction, double temperature)
{
        double mix_molar_mass = calculate_molar_mass(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
        double mix_adiabatic_index = calculate_adiabatic_index(He_fraction, O2_fraction, N2_fraction, H2O_fraction);

        return sqrt(mix_adiabatic_index * R_gas_constant * temperature / mix_molar_mass);
}

// Function to measure the sound travel time in seconds one way
double measure_duration(void)
{
        if (toggle_He) return 1;

        // RM_dur.clear();

        // while (!RM_dur.isFull()) {

                check_button_event();

                currentMillis = millis();
                deltaMillis = currentMillis - previousMillis;

                // if (deltaMillis > 500) {
                        // Clear the trigger pin
                        digitalWrite(trigPin, LOW);
                        delayMicroseconds(2);

                        // Send a 10 microsecond pulse to trigger the sensor
                        digitalWrite(trigPin, HIGH);
                        delayMicroseconds(10);
                        digitalWrite(trigPin, LOW);

                        double duration0 = pulseIn(echoPin0, HIGH);
                        RM_dur.add(duration0);
                        
                // }
                
                previousMillis = currentMillis;
        // }
                
        return RM_dur.getMedian() / 1000000.0;
}

// speed of sound measurement
double speed_measurement(void)
{
        RM_speed.add(EEPROM.readDouble(eeprom_dist_address) / measure_duration());

        return RM_speed.getMedian(); // Calculate the speeds in m/s
}

// Function to calibrate distance, returns distance in m
void calibrate_distance(double He_fraction)
{
        RM_dist_calibration.clear();

        while (!RM_dist_calibration.isFull()) {

                double O2_fraction = oxygen_measurement();
                double H2O_fraction = water_measurement();
                double N2_fraction = 1.0 - (He_fraction + O2_fraction + H2O_fraction);
                double speed_of_sound_calculated = calculate_speed_of_sound(He_fraction, O2_fraction, N2_fraction, H2O_fraction, temperature_measurement());
                double duration = measure_duration();

                RM_dist_calibration.add(speed_of_sound_calculated * duration);

        }

        distance_calibrated = RM_dist_calibration.getMedian();

        EEPROM.writeDouble(eeprom_dist_address, distance_calibrated);
        EEPROM.commit();

        RM_dist_calibration.clear();

        // Serial.println("Dist Calibrated");

        // Serial.print("Value: ");
        // Serial.print(distance_calibrated, 8);
        // Serial.print(" | eeprom: ");
        // Serial.println(EEPROM.readDouble(eeprom_dist_address), 8);
}

//  New function to trigger helium reading under development
double helium_measurement(double He_fraction, double O2_fraction, double H2O_fraction, double speed_of_sound_measured, double temperature)
{
        // currentMillis = millis();
        // deltaMillis = currentMillis - previousMillis;
        double gain = 0.005;
        double threshold = 0.09;
        // double error = 0;
        // double speed_of_sound_calculated;        

        double He_fraction_max = 1.0 - O2_fraction - H2O_fraction;
        double N2_fraction = 1.0 - O2_fraction - H2O_fraction - He_fraction;

        int reps = 0;

        double speed_of_sound_calculated = calculate_speed_of_sound(He_fraction, O2_fraction, N2_fraction, H2O_fraction, temperature);

        double error = speed_of_sound_measured - speed_of_sound_calculated;

        He_fraction = gain * error / abs(error) + He_fraction;

        deltaMillis = He_fraction * 100;

        if (He_fraction < 0) {
                He_fraction = 0.0;
                RM_He.add(He_fraction);
        } else if (He_fraction > He_fraction_max) {
                He_fraction = He_fraction_max;
                RM_He.add(He_fraction);
        } else if (abs(error) > threshold) RM_He.add(He_fraction);
        






        // do {                       
        //         reps = reps + 1;

        //         check_button_event();

        //         speed_of_sound_calculated = calculate_speed_of_sound(He_fraction, O2_fraction, N2_fraction, H2O_fraction, temperature);

        //         error = speed_of_sound_measured - speed_of_sound_calculated;

        //         He_fraction = gain * error + He_fraction;
                
        //         N2_fraction = 1.0 - O2_fraction - H2O_fraction - He_fraction;

        //         // He_fraction = He_fraction + 0.0001;
        //         // deltaMillis = He_fraction * 100;
        //         deltaMillis = reps * 1.0 ;

        //         if (He_fraction > He_fraction_max) {
        //                 He_fraction = He_fraction_max;
        //                 break;
        //         } 
        //         if (He_fraction < 0) {
        //                 He_fraction = 0.0;
        //                 break;
        //         }
        //         check_button_event();

        // } while (abs(error) > threshold);

        // RM_He.add(He_fraction);

        check_button_event();

        He_spd = speed_of_sound_calculated;
        He_error = error;
     


        // previousMillis = currentMillis;

        return RM_He.getMedianAverage(75);
}

double helium_stddev(void)
{
        return 0.0;
        // return RM_He.getStandardDeviation();
}