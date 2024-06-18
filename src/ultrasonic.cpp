// Libraries
#include <Arduino.h>        // Basic Library
#include <RunningAverage.h> // Running Average Library

// custom headers
#include "helium.h"
#include "constants.h"

// Paired Header
#include "ultrasonic.h"

// Ultrasonic Sensor Setup
const int trigPin = 19;  // Single Trigger pin for both ultrasonic sensors
const int echoPin0 = 18; // Echo pin for each ultrasonic sensors
const int echoPin1 = 17; // Echo pin for each ultrasonic sensors

double measure_duration();

// Running Average Setup
RunningAverage RA_dur(50);

// Initialise ultrasonic sensor & running average
int ultrasonic_Initialise()
{
    // establish pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin0, INPUT);
    pinMode(echoPin1, INPUT);

    RA_dur.clear();

    if (!measure_duration())
    {
        Serial.println("Failed to measure speed.");
        while (1)
            ;
    }

    return 1;
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

    double duration0 = pulseIn(echoPin0, HIGH);
    RA_dur.addValue(duration0);
    Serial.print("Dur 1: ");
    Serial.println(duration0);

    // delay(50);

    // Clear the trigger pin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Send a 10 microsecond pulse to trigger the sensor
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    double duration1 = pulseIn(echoPin1, HIGH);
    RA_dur.addValue(duration1);
    Serial.print(" | Dur 2: ");
    Serial.println(duration1);

    return RA_dur.getAverage() / 1000000;
}

// speed of sound measurement
double speed_measurement()
{
    double duration = measure_duration();
    return distance_calibrated / duration; // Calculate the speeds in m/s
}

// Function to calibrate distance
double calibrate_distance(int gas, double T, double H2O_fraction)
{
    double speed_of_sound_calculated, mix_molar_mass, mix_adiabatic_index;

    switch (gas)
    {
    case 1:
        // dry air
        mix_molar_mass = calculate_molar_mass(0, 0.209, (1 - 0.209), 0);
        mix_adiabatic_index = calculate_adiabatic_index(0, 0.209, (1 - 0.209), 0);
        speed_of_sound_calculated = calculate_speed_of_sound(mix_adiabatic_index, mix_molar_mass, T);
        break;
    case 2:
        // ambient air
        mix_molar_mass = calculate_molar_mass(0, 0.209, (1 - 0.209), H2O_fraction);
        mix_adiabatic_index = calculate_adiabatic_index(0, 0.209, (1 - 0.209), H2O_fraction);
        speed_of_sound_calculated = calculate_speed_of_sound(mix_adiabatic_index, mix_molar_mass, T);
        // speed_of_sound_calculated = 348.5;
        break;
    case 3:
        // Oxygen
        mix_molar_mass = calculate_molar_mass(0, 1, 0, 0);
        mix_adiabatic_index = calculate_adiabatic_index(0, 0.209, (1 - 0.209), 0);
        speed_of_sound_calculated = calculate_speed_of_sound(mix_adiabatic_index, mix_molar_mass, T);
        break;
    case 4:
        // Helium
        mix_molar_mass = calculate_molar_mass(1, 0, 0, 0);
        mix_adiabatic_index = calculate_adiabatic_index(0, 0.209, (1 - 0.209), 0);
        speed_of_sound_calculated = calculate_speed_of_sound(mix_adiabatic_index, mix_molar_mass, T);
        break;
    }

    double duration = measure_duration();

    double distance = speed_of_sound_calculated * duration;

    Serial.print("Speed Cal: ");
    Serial.print(speed_of_sound_calculated);

    Serial.print(" | Dur: ");
    Serial.print(duration * 1000000);
    Serial.print(" µs | Dist: ");
    Serial.print(distance * 1000);
    Serial.println(" mm");

    return distance;
}