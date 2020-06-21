#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <OneWire.h>
#include <DallasTemperature.h>

float temperatureC;
float mV_TDS;
float tdsValue;
float t_objetivo = 22; //????
float t_histeresis = 5; 

float temperatureC_ant;
float mV_TDS_ant;
float tdsValue_ant;


// GPIO where the DS18B20 is connected to
const int oneWireBus = 3;    
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

void setup_TDS_DS18B20(){
        // Start the DS18B20 water temperature sensor
        sensors.begin();
        // The ADC input range (or gain) can be changed via the following
        // functions, but be careful never to exceed VDD +0.3V max, or to
        // exceed the upper and lower limits if you adjust the input range!
        // Setting these values incorrectly may destroy your ADC!
        //                                                                ADS1015  ADS1115
        //                                                                -------  -------
        // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
           ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
        // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
        // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
        // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
        // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
        ads.begin();
}


  /////////////////////////////////////////TDS

 /*
  * https://github.com/DFRobot/GravityTDS
  * void GravityTDS::update()
{
  this->analogValue = analogRead(this->pin);
  this->voltage = this->analogValue/this->adcRange*this->aref;
  this->ecValue=(133.42*this->voltage*this->voltage*this->voltage - 255.86*this->voltage*this->voltage + 857.39*this->voltage)*this->kValue;
  this->ecValue25  =  this->ecValue / (1.0+0.02*(this->temperature-25.0));  //temperature compensation
  this->tdsValue = ecValue25 * TdsFactor;
  if(cmdSerialDataAvailable() > 0)
        {
            ecCalibration(cmdParse());  // if received serial cmd from the serial monitor, enter into the calibration mode
        }
}
  * /
  */
void Gravity_TDS(){
      float kValue = 1.0;
      float TdsFactor = 0.5; 
      float V_TDS=mV_TDS/1000.0;
      float ecValue=(133.42*V_TDS*V_TDS*V_TDS - 255.86*V_TDS*V_TDS + 857.39*V_TDS)*kValue;
      float ecValue25  =  ecValue / (1.0+0.02*(temperatureC-25.0));
      tdsValue_ant=tdsValue;
      tdsValue = ecValue25 * TdsFactor;
}

void TDS_DS18B20(){
   //temperatura de DS18B20 water temperature
  sensors.requestTemperatures(); 
  temperatureC_ant=temperatureC;
  temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  //Serial.print(temperatureC);
  //Serial.println("ºC");
  //Serial.print(temperatureF);
  //Serial.println("ºF");
  
  //float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */
  float multiplier = 0.125F;  // ganancia 1
  int16_t results = ads.readADC_Differential_0_1();  
 // Serial.print("Differential: "); Serial.print(results); Serial.print("("); Serial.print(results * multiplier); Serial.println("mV)");
  mV_TDS_ant=mV_TDS;
  mV_TDS=results * multiplier;
  Gravity_TDS(); //para convertir los voltios en valor
}

void activarRelTemp(int e1, int e2){
  estados[4]=e1;
  estados[5]=e2;
  //Serial.println("estados e1");Serial.println(e1);
  //Serial.println("estados e2");Serial.println(e2);
  e1=!e1; //en Nodem van al revés
  e2=!e2;
    digitalWrite(Relay_D0_calentador,e1);
    digitalWrite(Relay_D4_ventilador,e2);
  }
  
void controltemperatura(){
  //Serial.println("conf.temp_mode");Serial.println(conf.temp_mode);
  if (conf.temp_mode==0){activarRelTemp(0, 0);return;}
  if (conf.temp_mode==1){activarRelTemp(1, 1);return;}
  //if (conf.temp_mode==2){} //auto
  if (conf.temp_mode==3){return;} //manual
 
  if (temperatureC > conf.temp_max){
    activarRelTemp(0, 1);
    }
    if (temperatureC < conf.temp_min){
     activarRelTemp(1, 0);
    }
    if (temperatureC > conf.temp_min && temperatureC < conf.temp_max ){
      activarRelTemp(0, 0);
    }
}
