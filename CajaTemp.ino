#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include<EEPROM.h>
#include <WiFiClient.h>
#include<WiFi.h>


#include "DHT.h"
#define DHTpin 32
#define DHT_TIPO DHT11
DHT dht(DHTpin,DHT_TIPO);

 
Adafruit_ADS1115 ads(0x48);

  
const float FACTOR = 80.5; //30A/1V
const float multiplicador = 0.0625F;
float Suma_pot=0;
float Consumo_pot=0;
long dt = 0;
long t0=0;  // millis
unsigned long tx=0,ty=0;
unsigned long tiempo1,tiempo2;
void setup()
{
  Serial.begin(115200);
  ads.setGain(GAIN_TWO);        // ±2.048V  1 bit = 0.0625mV
  ads.begin();
  dht.begin();
  EEPROM.get(0,Suma_pot);
  t0=millis();

}
void printMeasure_i(String prefix, float value, String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 5);
 Serial.println(postfix);
}

 void printMeasure_v(String prefix, float value, String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 4);
 Serial.println(postfix);
}

void printMeasure_pVA(String prefix, float value, String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 5);
 Serial.println(postfix);
}

void printMeasure_Kwh(String prefix, float value, String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 9);
 Serial.println(postfix);
}
void printMeasure_W(String prefix, float value, String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 5);
 Serial.println(postfix);
}
void printMeasure_fp(String prefix, float value)
{
 Serial.print(prefix);
 Serial.print(value, 3);
 //Serial.println(postfix);
}
void printMeasure_angulo(String prefix, float value,String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 3);
 //Serial.println(postfix);
}
void printMeasure_reactiva(String prefix, float value,String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 3);
 Serial.println(postfix);
}
void printMeasure_temp(String prefix, float value,String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 2);
 Serial.println(postfix);
}
void printMeasure_hum(String prefix, float value,String postfix)
{
 Serial.print(prefix);
 Serial.print(value, 2);
 Serial.println(postfix);
}
void loop()
{
 
 float Irms = getCorriente();
 float Vrms = getVoltaje();
 float Pot_VA=getPotencia_VA(Irms,Vrms);
 float Pot_W=getPotencia_W();
 float Consumo=getConsumo(Pot_W);
 float fp=getfp(Pot_W,Pot_VA);
 float angulo= getangulo(fp);
 float Pot_VAR=getPot_reactiva(angulo,Pot_VA);

 float temperatura=getTemp();
 float humedad=getHumedad();

   Serial.println(" ");
 printMeasure_v("Vrms: ", Vrms ," V  ");
 printMeasure_i("Irms: ", Irms, " A ");
 printMeasure_fp("Factor de potencia: ",fp);
 Serial.println(" ");
 printMeasure_W("Potencia real: ",Pot_W," W ");
 printMeasure_pVA("Potencia aparente: ",Pot_VA," VA ");
 printMeasure_pVA("Potencia reactiva: ",Pot_VAR," VAR ");
 if (angulo==90){
  Serial.print("Angulo: --");
  }
  else{
 printMeasure_angulo("Angulo: ",angulo,"º ");
  }
 Serial.println(" ");
 printMeasure_Kwh("Consumo: ",Consumo," KW/h ");

 printMeasure_temp("Temperatura1: ",temperatura,"°C");
 printMeasure_hum("Humedad1: ",humedad,"% ");
 
 Serial.println(" ");
 EEPROM.put(0,Suma_pot);
}
 
float getCorriente()
{
 float voltage;
 float corriente;
 float sum = 0;
 long tiempo = millis();
 int counter = 0;
 
 while (millis() - tiempo < 2000)
 {
   voltage = ads.readADC_Differential_2_3() * multiplicador;
   corriente = voltage * FACTOR;
   corriente /= 1000.0;
 
   sum += sq(corriente);
   counter = counter + 1;
  }
 
 corriente = sqrt(sum / counter);
 return(corriente);
}

float getVoltaje()
{
 float voltaje;
 float Vrms;
 float sum = 0;
 long tiempo = millis();
 int counter = 0;
 
 while (millis() - tiempo < 2000)
 {
   voltaje = ads.readADC_Differential_0_1() * multiplicador;
   Vrms = voltaje * FACTOR;
   Vrms /= 1000.0;
 
   sum += sq(Vrms);
   counter = counter + 1;
  }
 
 Vrms = sqrt(sum / counter);
 return(Vrms);
}

float getPotencia_VA(float Irms,float Vrms){
 
    float Potencia=Vrms*Irms;

   return(Potencia);
  }
  
 float getConsumo(float Potencia){
  dt=millis()-t0;
   t0=millis();
   Suma_pot=Suma_pot+Potencia*(dt/1000);
   Consumo_pot= (Suma_pot)/3600000;
    EEPROM.put(0,Suma_pot);
   return(Consumo_pot);
  }

  float getPotencia_W(){
    float Voltaje=0,Voltaje1=0,Voltaje_v=0;
    float V=0;
    float Corriente=0,Corriente_I=0;
    float I=0;
    int Counter=0;
    float Potencia_real=0,Potencia_real1,Potencia_real2=0;
    unsigned long tiempo=millis();

    while(millis() - tiempo < 2000){
     


      float adc0=ads.readADC_Differential_0_1();

  Voltaje1 = (adc0 * 0.0625)/1000;
  Voltaje_v=Voltaje1*80.5;

  Serial.print("V: ");
  Serial.print(Voltaje_v);
  Serial.print(" ");

  delay(15.15);  
  
      float adc1=ads.readADC_Differential_2_3();
  //adc1 = ads.readADC_SingleEnded(1);

  //Voltaje = (adc1 * 0.0625)/1000;
  Corriente = (adc1 * 0.0625)/1000 ;
  Corriente_I=Corriente*80.5*(-1);
  Serial.print("I: "); 
  Serial.println(Corriente_I,4);

    
      Potencia_real1=Voltaje_v*Corriente_I;
      Potencia_real2+=Potencia_real1;
      Counter+=1;
     
   // for(int i;i<1000;i++){
     // Potencia_real1=V*i;
     // Potencia_real2+=Potencia_real1;
     // }
      }
      Potencia_real=Potencia_real2/Counter;
      if(Potencia_real<0){
        return(0);
        }else{
     return(Potencia_real); 
    }
  }

float getfp(float RealPower, float AparentPower){

  float factor= RealPower/AparentPower;
  if(factor>1){
    return(1);
    }
  if(factor<0 || RealPower==0 && AparentPower==0){
    return(0);
  }
   else{
    return (factor);
    } 
  }
  
float getangulo(float fp){
    float angulorad = acos(fp);
    float angulo = angulorad*180/3.14159265359;
    return (angulo);
  } 
  
float getPot_reactiva(float angulo,float aparent){
  float reactiva=aparent*sin(angulo*3.14159265359/180);
  return (reactiva);
  }  

float getTemp(){
  float temp=dht.readTemperature();
  if(isnan(temp)){
    return 0;
    }
 
  return temp;
  }  
float getHumedad(){
  float hum=dht.readHumidity();
  if(isnan(hum)){
    return 0;
    }
  
    return hum;
  }
