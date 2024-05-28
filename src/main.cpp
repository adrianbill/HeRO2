// Lbraries
#include <Arduino.h>          // Basic Library
#include <math.h>             // Math Library
#include <Wire.h>             // I2C Library
#include <Adafruit_HTU21DF.h> // Temp/Hum sensor Library
#include <Adafruit_GFX.h>     // Graphic Library
#include <Adafruit_SSD1306.h> // Display Library
#include <Adafruit_ADS1X15.h> // ADC / Amplifier Library
#include <RunningAverage.h>   // Running Average Library

// Custom Headers
#include "oxygen.h"
#include "temp_hum.h"



// function declarations
double measure_duration();
double calculate_molar_mass(double, double, double, double);
double calculate_effective_gamma(double, double, double, double);
double calculate_speed_of_sound(double, double, double);

double calibrate_distance(int, double, double);
double speed_measurement(double);
double measure_helium(double, double, double, double);
void displayValues(double, double, double, double);
void serialdisplayValues(double, double, double, double, double, double);

void Display_Initialise();


// Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




// Ultrasonic Sensor Setup
const int trigPin = 27; // Single Trigger pin for both ultrasonic sensors
const int echoPin = 26; // Echo pin for each utrasonic sensors

// Constants
// Universal Gas constant, J/(mol.K)
const double R = 8.314462618;

// Atmospheric Pressure (kPa)
const double pres_atm = 101.325;

// Adiabatic Indices
const double gamma_He = 1.666;
const double gamma_O2 = 1.394;
const double gamma_N2 = 1.399;
const double gamma_H2O = 1.33;

// Molar Masses, kg/mol
const double M_He = 0.00400;
const double M_O2 = 0.03200;
const double M_N2 = 0.02802;
const double M_H2O = 0.01802;

// Oxygen Analysis setup
double O2_calibration; // Calibration value (%/mV)

// calibrated distance in m
double dist_cal = 0.04805;

// Running Average Setup
RunningAverage RA_He(10);
RunningAverage RA_dur(10);

void setup()
{
    // start serial connection
    Serial.begin(115200);
    delay(1000); // Delay to stabilize serial communication

    // establish pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // Initialise I2C
    Wire.begin();

    // Temp Sensor Initialise
    Temp_Initialise();

    // Display Initialise
    Display_Initialise();

    // O2 Initialise
    O2_Initialise();


    RA_He.clear();
    RA_dur.clear();

    O2_calibration = calibrate_oxygen();

    // Serial.print(" Cal: ");
    // Serial.println(O2_calibration);

    // for (int i = 0; i <= 4; i++)
    // {

    //     Serial.print("O₂ Calibration No. ");
    //     Serial.print(i);
    //     calibrate_oxygen();
    //     Serial.print(" Cal: ");
    //     Serial.println(O2_calibration);

    // }
}

void loop()
{

   
    // To be measured
    // double x_O2 = 0.209; // Oxygen fraction
    // double T = 293.0; // Temperature in Kelvin
    double x_O2 = measure_oxygen(O2_calibration);
    double T = temperature_measurement();       // Measure the temperature
    double c_mea = speed_measurement(dist_cal); // Speed of sound in m/s
    double hum = humidity_measurement();

    double x_H2O = measure_water(T, hum, pres_atm);

    // calibrate_distance (2, T, x_H2O);

    double x_He = measure_helium(x_O2, x_H2O, c_mea, T);
    // double x_He = 0;

    // Display the O2, He fractions and Temperature
    displayValues(x_O2, x_He, T, hum);
    serialdisplayValues(c_mea, x_O2, x_He, x_H2O, hum, T);

    // measure_oxygen();
    delay(500);
    
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


// Function to calculate the molar mass of the mixture
double calculate_molar_mass(double x_He, double x_O2, double x_N2, double x_H2O)
{
    return x_He * M_He + x_O2 * M_O2 + x_N2 * M_N2 + x_H2O * M_H2O;
}

// Function to calculate the effective adiabatic index (G_i=(gamma_i−1)^-1
double calculate_effective_gamma(double x_He, double x_O2, double x_N2, double x_H2O)
{
    double G_He = 1 / (gamma_He - 1);
    double G_O2 = 1 / (gamma_O2 - 1);
    double G_N2 = 1 / (gamma_N2 - 1);
    double G_H2O = 1 / (gamma_H2O - 1);
    double G_mix = x_He * G_He + x_O2 * G_O2 + x_N2 * G_N2 + x_H2O * G_H2O;
    return (1 / G_mix) + 1;
}

// Function to calculate the speed of sound
double calculate_speed_of_sound(double gamma_mix, double M_mix, double T)
{
    return sqrt(gamma_mix * R * T / M_mix);
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

// speed of sound measurement
double speed_measurement(double distance)
{
    double duration = measure_duration();
    return distance / duration; // Calculate the speeds in m/s
}

// function to trigger helium reading
double measure_helium(double x_O2, double x_H2O, double c_mea, double T)
{

    double x_N2 = 1.0 - x_O2 - x_H2O; // Assume N2 makes up the rest
    double x_He = 0.0;
    double max_He = 1 - x_O2 - x_H2O;

    // Iterate to solve for He fraction

    for (int i = 0; i < 1000; i++)
    {
        double N2_mod = x_N2 - x_He;

        // Ensure He fraction stays within valid range
        if (N2_mod < 0)
        {
            x_He = x_N2; // Ensure the fraction does not go negative
            break;
        }
        else if (x_He > max_He)
        {
            x_He = max_He;
            break;
        }
        else if (x_He < 0)
        {
            x_He = 0;
            break;
        }

        double M_mix = calculate_molar_mass(x_He, x_O2, N2_mod, x_H2O);
        double gamma_mix = calculate_effective_gamma(x_He, x_O2, N2_mod, x_H2O);

        double c_calc = calculate_speed_of_sound(gamma_mix, M_mix, T);
        double error = c_mea - c_calc;

        if (abs(error) < 0.001)
        {
            break; // Converged to solution within tolerance
        }

        // Adjust He fraction for next iteration
        x_He += 0.0001 * error; // Simple proportional adjustment

        x_N2 = 1.0 - x_He - x_O2 - x_H2O;
    }

    return x_He;
}

void displayValues(double x_O2, double x_He, double T, double hum)
{
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("O2: ");
    display.print(x_O2 * 100, 2); // Display O2 fraction in percentage
    display.println("%");

    display.setCursor(0, 22);
    display.print("He: ");
    display.print(x_He * 100, 2); // Display He fraction in percentage
    display.println("%");

    display.setTextSize(1);
    display.setCursor(0, 44);
    display.print("T: ");
    display.print(T - 273.15, 1); // Display temperature in Celsius
    display.print("C  Hu: ");
    display.print(hum * 100, 1); // Display temperature in Celsius
    display.println("%");

    display.display();
}

void serialdisplayValues(double c_mea, double x_O2, double x_He, double x_H2O, double hum, double T)
{
    Serial.print("Speed: ");
    Serial.print(c_mea, 1); // Display measured speed in m/s
    Serial.print("m/s || ");

    Serial.print("O₂: ");
    Serial.print(x_O2 * 100, 2); // Display O2 fraction in percentage
    Serial.print("% || ");

    Serial.print("He: ");
    Serial.print(x_He * 100, 2); // Display He fraction in percentage
    Serial.print("% || ");

    Serial.print("H₂O: ");
    Serial.print(x_H2O * 100, 2); // Display H2O fraction in percentage
    Serial.print("% || ");

    Serial.print("Hum: ");
    Serial.print(hum * 100, 2); // Display Humidity fraction in percentage
    Serial.print("% || ");

    Serial.print("Temp: ");
    Serial.print(T - 273.15, 2); // Display temperature in Celsius
    Serial.println("°C");
}



void Display_Initialise()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
}

// Need to add functionality for O2, temp, buttons, display
