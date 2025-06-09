
// Original idea & coding by Yves Caze, Savoie Plongee, modified and supplemented by GoDive BRB, 2021
// Modified by Dominik Wiedmer 2021
// Version 1.1 4.10.2021

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <RunningAverage.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_SH110X.h> // Library for 1.3" AliExpress OLED display
#include <SPI.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


Adafruit_ADS1115 ads;    

float Vcalib = 0;        
float voltage = 0;       
float bruecke = 0;       

float gain = 0.03125;     
float calibMD62 = 661.26;   
float brueckeCalib = 10;   
float TempKomp = 0 ;       
unsigned long time;       

RunningAverage RA0(10);       
RunningAverage RA1(10);

int16_t adc0;
int16_t adc1;


void cal(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10,0);
  display.print("Calibrating");
  display.setCursor(10,10);
  display.print("O2 Sensor");
  display.setCursor(10,20);
  display.print("(Air 20.9% O2 )");
  display.display();
  int i = 0;
  float Vavg = 0;
  
  for(i = 1; i <10 or (abs (voltage - (Vavg / (i-1)))) > 0.005; i++) 
    {
      adc0 = ads.readADC_Differential_0_1();
      RA0.addValue(adc0);
      voltage = abs(RA0.getAverage()*gain); 
      Vavg = Vavg + voltage;
      delay(200);
    }
     
  
   display.clearDisplay();
   display.setCursor(10,20);
   display.print("Calibration OK");
 
   Vavg = Vavg / (i - 1);
   Vcalib = Vavg;

   display.setCursor(10,30);
   display.print("V cal. = ");
   display.print(Vcalib,2);
   display.print(" mV");
   display.display();  
   delay(2000);
   display.clearDisplay(); 

  }


void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000L);
  Serial.print(F("Test")); // Setup and COM check
  pinMode(1, INPUT_PULLUP); // D1 as input with internal pull up resistor, so high when button is not pressed.

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  display.begin(i2c_Address, true); // Address 0x3C default
 //display.setContrast (0); // dim display
 
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();


  uint16_t time = millis();
  time = millis() - time;
  delay(1000);


  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE,SH110X_BLACK);
  //display.setTextColor(0xFFFF, 0x0000);
  display.setCursor(20, 20);
  display.print("Trimix");
  display.setCursor(20, 40);
  display.print("Analyser");
  display.display();
  
  delay(2000);

  
  ads.setGain(GAIN_FOUR); // 4x gain 1 bit = 0.03125mV
  ads.begin();

 
 
  adc0 = ads.readADC_Differential_0_1();
  RA0.addValue(adc0);
  voltage = abs(RA0.getAverage()*gain);
  
  adc1 = ads.readADC_Differential_2_3();
  RA1.addValue(adc1);
  bruecke = RA1.getAverage()*gain;
 
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10,0);
  display.print("He Wheatstone");
  display.setCursor(10,10);
  display.print("V cell = ");
  display.print(voltage,2);
  display.print("  mV");
  display.setCursor(10,20);
  display.print("VBridge = ");
  display.print(bruecke,2);
  display.print(" mV");
  display.display();
  delay(2000);

cal();
 

   display.clearDisplay();
   display.setCursor(10,00);
   display.print("Claibrating");
   display.setCursor(10,20);
   display.print("Helium sensor...");
   display.display();
   delay(500);

      while(bruecke > 10){
         adc1 = ads.readADC_Differential_2_3();
         RA1.addValue(adc1);
         bruecke = RA1.getAverage()*gain;
         display.clearDisplay();
         display.setCursor(10,00);
         display.print("Claibrating");
         display.setCursor(10,20);
         display.print("Helium sensor...");
         display.setCursor(10,40);    
         display.print("V Bridge= ");
         display.print(bruecke,0);
         display.print(" mV  ");
         display.display(); 
         delay(50);
         }
         
   display.clearDisplay();
   display.setCursor(10,10);
   display.print(" Heliumsensor OK");
   display.display();           
   delay(2000);

   display.clearDisplay();
   display.setCursor(10,10);
   display.print(" System OK"); 
   display.display();         
   delay(2000);
   display.clearDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:

  int16_t adc0;
  int16_t adc1;
  adc0 = ads.readADC_Differential_0_1();
  adc1 = ads.readADC_Differential_2_3();
  
  time = millis();      //  MS

  RA0.addValue(adc0);
  voltage = abs(RA0.getAverage()*gain);

  RA1.addValue(adc1);
  bruecke = RA1.getAverage()*gain;

  float nitrox = 0;
  float helium = 0;
  
    nitrox = voltage * (20.9 / Vcalib);

    //display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    if (voltage > 1) {
       display.print("O2:");
       display.setCursor(18,0);
       display.print("      ");  
       display.setCursor(18,0);
       display.print(nitrox,1);  
       display.print(" %");
       display.display(); 
      }
      else if (voltage > 1 && helium < 80){
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(20,10);       
        display.print("O2 Sensor");
        display.setCursor(20,20);    
        display.print("missing/expired ");
        display.display();
        delay(2000);
        
        }
    display.setCursor(0,10);
    display.setTextSize(1);
    display.print("O2mV ");
    display.setCursor(30,10);
    display.print("   ");
    display.setCursor(30,10);
    display.print(voltage,2);
    display.setCursor(66,10);       
    display.print("HEmV ");
    display.setCursor(96,10);
    display.print("   ");
    display.setCursor(96,10);      
    display.print(bruecke,0);
    
    display.display(); 


    bruecke = bruecke - brueckeCalib;  
    
    if (time < 480000) {  TempKomp = 1 ; }   
    if (time < 360000) {  TempKomp = 2 ; }
    if (time < 300000) {  TempKomp = 3 ; }
    if (time < 270000) {  TempKomp = 4 ; }
    if (time < 240000) {  TempKomp = 5 ; }
    if (time < 210000) {  TempKomp = 6 ; }
    if (time < 180000) {  TempKomp = 7 ; }
    if (time < 165000) {  TempKomp = 8 ; }
    if (time < 150000) {  TempKomp = 9 ; }
    if (time < 120000) {  TempKomp = 10 ; }
    if (time < 105000) {  TempKomp = 11 ; }
    if (time < 90000) {  TempKomp = 12 ; }
    if (time < 80000) {  TempKomp = 13 ; }
    if (time < 70000) {  TempKomp = 14 ; }
    if (time < 60000) {  TempKomp = 15 ; }
    if (time < 50000) {  TempKomp = 16 ; }
    if (time < 40000) {  TempKomp = 17 ; }
    if (time < 30000) {  TempKomp = 18 ; }

    
    bruecke = bruecke - TempKomp;          
   //correct heVact in case of high O2 levels - empirical!
 
    if (nitrox>89) { bruecke = bruecke-24; }
    else if (nitrox>82) { bruecke = bruecke-22; }
    else if (nitrox>75) { bruecke = bruecke-20; }
    else if (nitrox>71) { bruecke = bruecke-18; }
    else if (nitrox>66) { bruecke = bruecke-16; }
    else if (nitrox>62) { bruecke = bruecke-15; }
    else if (nitrox>57) { bruecke = bruecke-14; }
    else if (nitrox>52) { bruecke = bruecke-14; }
    else if (nitrox>48) { bruecke = bruecke-12; }
    else if (nitrox>40) { bruecke = bruecke-8; }

    display.setCursor(66,0);
    display.setTextSize(1); 
    display.print("HE:");
    helium = 100 * bruecke / calibMD62;
    display.setCursor(84,0);
    display.print("      ");
    display.setCursor(84,0);
    if (helium > 50) {
      helium = helium * (1 + (helium - 50) * 0.4 / 100);
      }
    if (helium > 2) {
      display.print(helium,1); 
      display.print(" %");
      display.display(); 
      }
    else {
      helium = 0;
      display.print("  0");
      display.print(" %");
      display.display();  
      }
      
  
    if (helium > 0) {
      display.setCursor(10,25);
      display.setTextSize(2);
      display.print("       ");
      display.setCursor(10,25);
      display.print("Trimix ");
      display.setCursor(10,45);
      display.print(nitrox,0);
      display.print(" / ");
      display.print(helium,0);
      display.print(" ");
      display.setCursor(10,50);
      display.display();
      }
     else {
      display.setCursor(10,25);
      display.setTextSize(2);
      display.print("Nitrox ");
      display.setCursor(10,45);
      display.print("       ");
      display.setCursor(10,45);
      display.print(nitrox,0);
      display.display();
      }
       
// manual calibration when pressing button at D1

if(digitalRead(1) == LOW)
{
cal();
}

delay(100);
}
