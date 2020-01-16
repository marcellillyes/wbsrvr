#include <Arduino.h>
#include<SPI.h>
#include <stdio.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <DirectFormI.h>
#include <ArduinoJson.h>
#include <Hash.h>

// IIR filter pool, 50HZ cheby2 bandstop with Fs=500, cutoff=0.16,0.24 and stopband attenuation od 40dB
DirectFormI biquad1(14991,-24304,14991,16384,-24498,14909,14);
DirectFormI biquad2(16384,-26562,16384,16384,-26272,15073,14);

StaticJsonDocument<200> doc;
int c=0;

// PINS FOR ESP NODE MCU
const int CS = D8;
const int DRDY = D2;
const int PWDN = D0;
const int RST = D3;
const int START_HW = D1;

// VARIABLES FOR WIFI COMMUNICATION0
const char* ssid     = "AP1";
const char* password = "thereisnospoon";
char* host = "192.168.137.1";  //replace this ip address with the ip address of your Node.Js server(laptop)
const int espport= 3000;

// Commands //
#define NOP     0x00 
#define RESET   0x06 
#define START   0x08 
#define STOP    0x0A 
#define RDATA   0x12 
#define SYOCAL  0x16 
#define GANCAL  0x17
#define SFOCAL  0x19 
#define RREG    0x20
#define WREG    0x40 
#define LOCK    0xF2 
#define UNLOCK  0xF5

//Register rrh address//
//(when writing and reading use this value plus the base WREG and RREG address)
#define ID      0x00 
#define STATUS  0x01 
#define MODE0   0x02 
#define MODE1   0x03 
#define MODE2   0x04 
#define MODE3   0x05 
#define REF     0x06 
#define OFCAL0  0x07 
#define OFCAL1  0x08 
#define OFCAL2  0x09 
#define FSCAL0  0x0A 
#define FSCAL1  0x0B 
#define FSCAL2  0x0C 
#define IMUX    0x0D 
#define IMAG    0x0E 
#define RESERVED  0x0F 
#define PGA     0x10 
#define INPMUX  0x11 
#define INPBIAS 0x12 

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//UDP variables
WiFiUDP Udp;
unsigned int localUdpPort = 8082;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi";  // a reply string to send back
char char_array[15000];
char *parray[10];


String d="";
int32_t y;


/////
int32_t i= 12400;
uint8_t *buf;
size_t buf_len= sizeof(i);

int x;
int32_t raw_data;
//int16_t raw_data;
uint8_t data_buff [3];
uint8_t ADC_STATUS;
String command="";

//Downsampling variables
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long interval=2;
String stamp;
/////


void W_ADC_REG(int address, int value) {
  digitalWrite(START_HW, LOW);
  digitalWrite(CS, LOW);
  
  SPI.transfer(WREG + address);
  x = SPI.transfer(value);

  Serial.print("Modified registers at: ");
  Serial.println(x);

}

int msg_count=0;

void ADC_settings()
{
  W_ADC_REG(STATUS,0x65);
  W_ADC_REG(REF,0x05);
  W_ADC_REG(MODE0,0x64);
  W_ADC_REG(MODE3,0x40);
  W_ADC_REG(PGA,0x85);
  W_ADC_REG(INPMUX,0x12);
}


//Convert 3 unsigned 8bit ADC values to 32bit 2s complement format
void data_convert()
{
  raw_data = ((uint32_t)data_buff[2]<<16) | ((uint32_t)data_buff[1]<<8) | (uint32_t)data_buff[0];
                   
          if (raw_data & 0x800000)
                 {
                   raw_data |= 0xFF000000;
                 }
}

/*void data_convert()
{
  raw_data = ((uint16_t)data_buff[2]<<8) | ((uint16_t)data_buff[1]);
         raw_data=raw_data-32768;          
         
}*/

void READDATA()
{
    digitalWrite(START_HW, HIGH);
    digitalWrite(CS, LOW);
          //  send in the address and value via SPI:
     SPI.transfer(RDATA);
     SPI.transfer(0x00);
     ADC_STATUS = SPI.transfer(0x00);
          if (bitRead(ADC_STATUS,2)== HIGH) // if (ADC_STATUS & 0x4)
             {
             data_buff[2] = (uint8_t)SPI.transfer(0x00);
             data_buff[1] = (uint8_t)SPI.transfer(0x00);
             data_buff[0] = (uint8_t)SPI.transfer(0x00);
             //Serial.println(ADC_STATUS & 0x4);
             }
             else{
              // CHECK IF DATA READ WAS SUCCESSFUL BY RETURNING 0 IF NOT
             data_buff[2] = 0x00;
             data_buff[1] = 0x00;
             data_buff[0] = 0x00;
              }
}



void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
    
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
      
		case WStype_CONNECTED: 
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			//webSocket.sendTXT("Connected");
			break;
      
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      
      command="";
      
      while( *payload != '\0')
      {
      command+= char(*payload);
      payload++;
      }
      
      Serial.println(command);
      
			// send message to server
			//webSocket.sendTXT("message here");
			break;
      
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);
      
      i=millis();
      buf =(uint8_t *)&i;
			// send data to server
			webSocket.sendBIN(buf, buf_len);
			break;
      
    case WStype_PING:
      // pong will be send automatically
      USE_SERIAL.printf("[WSc] get ping\n");
      break;
            
    case WStype_PONG:
      // answer to a ping we send
      USE_SERIAL.printf("[WSc] get pong\n");
      break;
    }

}

void setup() {

  pinMode(DRDY, INPUT);
  pinMode(PWDN, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(START_HW, OUTPUT);
  pinMode(CS, OUTPUT);
  
	USE_SERIAL.begin(115200);

  digitalWrite(CS, LOW);   
  SPI.begin();
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE1));
  SPI.endTransaction();
  digitalWrite(PWDN, HIGH);
  digitalWrite(RST, HIGH);
  digitalWrite(START_HW, LOW);

  ADC_settings();
  delay(100);

	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP(ssid, password);

	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	webSocket.begin(host, espport);

	// event handler
	webSocket.onEvent(webSocketEvent);

	// try every 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  delay(1000);
}

void loop() {
	webSocket.loop();

  currentMillis=millis();


  if(command=="start"){
    if (abs(currentMillis - previousMillis) >= interval) {
      
      previousMillis = currentMillis;
      READDATA();
      stamp=String (millis());
      data_convert();
      //y=biquad1.filter(raw_data);
      //y=biquad2.filter(y);
    
      
      if(msg_count < 500){
        d+= stamp+","+String(raw_data)+ "\n";
        
        msg_count++;
        }
      else{
        msg_count=0;
        d.toCharArray(char_array,15000);
        Udp.beginPacket(host, 8082);
        Udp.write(char_array);
        Udp.endPacket();
        d=stamp+","+String(raw_data)+ "\n";
        msg_count++;
        }
    }
    }
    
  else if(command=="init"){
    ADC_settings();
    delay(1000);
    command="";
      }

   /*else if(command.length() > 10){
    
     DeserializationError error = deserializeJson(doc, command);
       
       if(error){
        Serial.print("failed with");
        Serial.println(error.c_str());
        }

       int ord= doc["order"];
       const short int b0=doc["DirectForm"][0]["b0"];
       const short int b1=doc["DirectForm"][0]["b1"];
       const short int b2=doc["DirectForm"][0]["b2"];
       const short int a0=doc["DirectForm"][0]["a0"];
       const short int a1=doc["DirectForm"][0]["a1"];
       const short int a2=doc["DirectForm"][0]["a2"];
       DirectFormI biquad1(b0,b1,b2,a0,a1,a2,14);
       const short int b10=doc["DirectForm"][1]["b0"];
       const short int b11=doc["DirectForm"][1]["b1"];
       const short int b12=doc["DirectForm"][1]["b2"];
       const short int a10=doc["DirectForm"][1]["a0"];
       const short int a11=doc["DirectForm"][1]["a1"];
       const short int a12=doc["DirectForm"][1]["a2"];
       DirectFormI biquad2(b10,b11,b12,a10,a11,a12,14);

      
        
       Serial.println("filters created");
       command=""; 
       delay(1000);
       
    }*/

   
 }
