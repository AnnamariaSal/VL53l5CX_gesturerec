#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX
//#include <Adafruit_TinyUSB.h>

int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output

SparkFun_VL53L5CX sensor_L5CX_1;
int sensorAddress1 = 0x44; //New address of unit without a wire. Valid: 0x08 <= address <= 0x77
int sensorlpn1 = D9; //GPIO that is connected to the Reset pin on sensor 1
VL53L5CX_ResultsData measurementData1;

SparkFun_VL53L5CX sensor_L5CX_2;
//sensorAddress2 = 0x52 cioè quello di Default VL53L5CX 
int sensorlpn2 = D10; //GPIO that is connected to the Reset pin on sensor 2 (pin digitale 13)
VL53L5CX_ResultsData measurementData2;

int  cont, mod ;
char ris, imp;
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Wire.begin(); //This resets I2C bus to 100kHz
  Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz

  pinMode(sensorlpn2, OUTPUT);
  digitalWrite(sensorlpn2, LOW); //Hold sensor 2 in low while we configure sensor 1

  pinMode(sensorlpn1, OUTPUT);
  digitalWrite(sensorlpn1, HIGH); //Reset sensor 1
  delay(100);

  if (sensor_L5CX_1.begin() == false)
  {
    Serial.println(F("Sensor 1 not found. Check wiring. Freezing..."));
    while (1) ;
  }

  if (sensor_L5CX_1.setAddress(sensorAddress1) == false)
  {
    while (1);
  }

//pull up all Lpn pin 

  digitalWrite(sensorlpn1, HIGH);
  digitalWrite(sensorlpn2, HIGH);

  int newAddress = sensor_L5CX_1.getAddress();


  //Serial.println(F("Initializing sensor 2. This can take up to 10s. Please wait."));
  if (sensor_L5CX_2.begin() == false)
  {
    while (1) ;
  }

}

void loop()
{    
    static char command1, command2 = '\0';  // Variabile per il primo comando
    static bool receivedFirstCommand = false;

    if(Serial.available()){  
      
      char ris=Serial.read();
      // Leggi il carattere dalla porta seriale
      

      if (!receivedFirstCommand) {
          command1 = ris;
          receivedFirstCommand = true;
        
      }
       else 
      {
        char command2 = ris;
        
        if(command1=='b')
        {

          if (command2== 'q'){

            sensor_L5CX_1.setResolution(4 * 4); //Enable all 64 pads,
            sensor_L5CX_2.setResolution(4 * 4); //Enable all 64 pads

            imageResolution = sensor_L5CX_1.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
            imageWidth = sqrt(imageResolution); //Calculate printing width, per calcolare la dimensione della matrice
    
            sensor_L5CX_1.setRangingFrequency(60); //60 se lavoriamo in 4*4
            sensor_L5CX_2.setRangingFrequency(60);

            
            command1 = 'a';
            //receivedFirstCommand = false;
          }else if(command2== 'o') {
            sensor_L5CX_1.setResolution(8 * 8); //Enable all 64 pads,
            sensor_L5CX_2.setResolution(8 * 8); //Enable all 64 pads

            imageResolution = sensor_L5CX_1.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
            imageWidth = sqrt(imageResolution); //Calculate printing width, per calcolare la dimensione della matrice
      
            sensor_L5CX_1.setRangingFrequency(15); //60 se lavoriamo in 4*4
            sensor_L5CX_2.setRangingFrequency(15);
            command1='a';
            //receivedFirstCommand = false;
          } 
        
          sensor_L5CX_1.startRanging();
          sensor_L5CX_2.startRanging();
        }
        else if(command1=='a')
        {  
              
              switch(command2)
              {
                case 49:
                  do{
                    if (sensor_L5CX_1.isDataReady() == true)
                    {
                      if (sensor_L5CX_1.getRangingData(&measurementData1)) //Read distance data into array
                      {
                        //The ST library returns the data transposed from zone mapping shown in datasheet
                        //Pretty-print data with increasing y, decreasing x to reflect reality
                        Serial.print("1:");
                        for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
                        {
                          for (int x = imageWidth - 1 ; x >= 0 ; x--)
                          {
                            Serial.print("\t");
                            Serial.print(measurementData1.distance_mm[x + y]); //nell'api di vl53l5cx
                          }
                          Serial.println();
                        }
                        Serial.println();
                      }
                    }
                  delay(2);
                  }while(Serial.available());
                  break;

                case 50:
                  do {
                    if (sensor_L5CX_2.isDataReady() == true)
                    {
                      if (sensor_L5CX_2.getRangingData(&measurementData2)) //Read distance data into array
                      {
                        //The ST library returns the data transposed from zone mapping shown in datasheet
                        //Pretty-print data with increasing y, decreasing x to reflect reality
                        Serial.print("2:");
                        for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
                        {
                          for (int x = imageWidth - 1 ; x >= 0 ; x--)
                          {
                            Serial.print("\t");
                            Serial.print(measurementData2.distance_mm[x + y]);
                          }
                        Serial.println();
                        }
                        Serial.println();
                      }
                    }
                    delay(2);
                  } while(Serial.available()); 
                  break;
              
                case 51:
                  //Poll sensor for new data
                  do{
                    if (sensor_L5CX_1.isDataReady() == true)
                    {
                    if (sensor_L5CX_1.getRangingData(&measurementData1)) //Read distance data into array
                      {
                      //The ST library returns the data transposed from zone mapping shown in datasheet
                      //Pretty-print data with increasing y, decreasing x to reflect reality
                        Serial.print("1:");
                        for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
                        {
                          for (int x = imageWidth - 1 ; x >= 0 ; x--)
                          {
                            Serial.print("\t");
                            Serial.print(measurementData1.distance_mm[x + y]); //nell'api di vl53l5cx
                          }
                          Serial.println();
                        }
                        Serial.println();
                      }
                    }
                    if (sensor_L5CX_2.isDataReady() == true)
                    {
                      if (sensor_L5CX_2.getRangingData(&measurementData2)) //Read distance data into array
                      {
                        //The ST library returns the data transposed from zone mapping shown in datasheet
                        //Pretty-print data with increasing y, decreasing x to reflect reality
                        Serial.print("2:");
                        for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
                        {
                          for (int x = imageWidth - 1 ; x >= 0 ; x--)
                          {
                            Serial.print("\t");
                            Serial.print(measurementData2.distance_mm[x + y]);
                          }
                          Serial.println();
                        }
                        Serial.println();
                      }
                    }
                    delay(2);
                  }while(Serial.available());
                  break;
              }
          }
      
      }
    }
  delay(1); //Small delay between polling
}