// Libraries
#include <Arduino.h>          // Basic Library
#include <RunningAverage.h>   // Running Average Library
#include <Adafruit_ADS1X15.h> // ADC / Amplifier Library

// custom headers
#include "speed_of_sound.h"
#include "constants.h"

// Paired Header
#include "ultrasonic.h"



// Ultrasonic Sensor Setup
const int trigPin = 27; // Single Trigger pin for both ultrasonic sensors
const int echoPin = 26; // Echo pin for each utrasonic sensors

// Running Average Setup
RunningAverage RA_dur(10);

// Initialise ultrasonic sensor & running average
void ultrasonic_Initialise()
{
    // establish pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    RA_dur.clear(); 
}

// Function to measure the sound travel time in seconds one way
double measure_duration()
{
    // Clear the trigger pin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Send a 10 microsecond pulse to trigger the sensor
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    double duration = pulseIn(echoPin, HIGH);
    RA_dur.addValue(duration);

    return RA_dur.getAverage() / 2000000;
}

// speed of sound measurement
double speed_measurement(double distance)
{
    double duration = measure_duration();
    return distance / duration; // Calculate the speeds in m/s
}

// Function to calibrate distance
double calibrate_distance(int gas, double T, double x_H2O)
{
    double c_calibrate, M_mix, gamma_mix;

    switch (gas)
    {
    case 1:
        // dry air
        M_mix = calculate_molar_mass(0, 0.209, (1 - 0.209), 0);
        gamma_mix = calculate_effective_gamma(0, 0.209, (1 - 0.209), 0);
        c_calibrate = calculate_speed_of_sound(gamma_mix, M_mix, T);
        break;
    case 2:
        // ambient air
        M_mix = calculate_molar_mass(0, 0.209, (1 - 0.209), x_H2O);
        gamma_mix = calculate_effective_gamma(0, 0.209, (1 - 0.209), x_H2O);
        c_calibrate = calculate_speed_of_sound(gamma_mix, M_mix, T);
        // c_calibrate = 348.5;
        break;
    case 3:
        // Oxygen
        M_mix = calculate_molar_mass(0, 1, 0, 0);
        gamma_mix = calculate_effective_gamma(0, 0.209, (1 - 0.209), 0);
        c_calibrate = calculate_speed_of_sound(gamma_mix, M_mix, T);
        break;
    case 4:
        // Helium
        M_mix = calculate_molar_mass(1, 0, 0, 0);
        gamma_mix = calculate_effective_gamma(0, 0.209, (1 - 0.209), 0);
        c_calibrate = calculate_speed_of_sound(gamma_mix, M_mix, T);
        break;
    }

    double duration = measure_duration();

    double distance = c_calibrate * duration;

    Serial.print("Speed Cal: ");
    Serial.print(c_calibrate);

    Serial.print(" | Dur: ");
    Serial.print(duration * 2000000);
    Serial.print(" µs | Dist: ");
    Serial.print(distance * 1000);
    Serial.println(" mm");

    return distance;
}