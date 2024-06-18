// Libraries
#include <Arduino.h>        // Basic Library
#include <RunningAverage.h> // Running Average Library

// custom headers
#include "helium.h"
#include "constants.h"
#include "environment.h" // environmenttal parameters: temperature, humidity, atmospheric pressure

// Paired Header
#include "ultrasonic.h"

// Ultrasonic Sensor Setup
const int trigPin = 19;  // Single Trigger pin for both ultrasonic sensors
const int echoPin0 = 18; // Echo pin for each ultrasonic sensors
const int echoPin1 = 17; // Echo pin for each ultrasonic sensors

double measure_duration();

// Running Average Setup
RunningAverage RA_dur(100);

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

    // Clear the trigger pin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Send a 10 microsecond pulse to trigger the sensor
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    double duration1 = pulseIn(echoPin1, HIGH);
    RA_dur.addValue(duration1);

    return RA_dur.getAverage() / 1000000;
}

// speed of sound measurement
double speed_measurement()
{
    double duration = measure_duration();
    return distance_calibrated / duration; // Calculate the speeds in m/s
}

// Function to calibrate distance, returns distance in m
void calibrate_distance(double He_fraction, double O2_fraction, double H2O_fraction)
{
    double N2_fraction = 1.0 - (He_fraction + O2_fraction + H2O_fraction);

    double speed_of_sound_calculated = calculate_speed_of_sound(He_fraction, O2_fraction, N2_fraction, H2O_fraction, temperature_measurement());

    double duration = measure_duration();

    distance_calibrated = speed_of_sound_calculated * duration;

    Serial.println("Dist Calibrated");
}