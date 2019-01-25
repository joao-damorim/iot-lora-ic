#include <SoftwareSerial.h>
#include <TinyGPS.h>

union meusDados{
   long lval;
   float fval;
   byte  b[4];     // Array de bytes de tamanho igual ao tamanho da primeira variavel: int = 2 bytes, float = 4 bytes
} gps_latitude, gps_longitude, destino_latitude, destino_longitude; // Cada uma é independente da outra e guardam valores distintos.

void sendBytesLoRa(uint8_t *buf, uint16_t len) {

  // send on the RadioMote format 01 <pkg size> E0 01 00 00 <data> FF FF
  Serial3.write(0x01);
  Serial3.write(len + 8);
  Serial3.write(0xE0);
  Serial3.write(0x01);
  Serial3.write(0x00);
  Serial3.write(0x00);
  Serial3.write(buf, len);
  Serial3.write(0xFF);
  Serial3.write(0xFF);
}

//const int tx = 3;
//const int rx = 2;
//SoftwareSerial Serial1(rx, tx); // RX, TX

TinyGPS gps1;

void setup() {
   Serial3.begin(9600); 
   Serial1.begin(9600);
   Serial.begin(9600);

   Serial.println("Esperando pelo sinal dos satelites...");
}

void loop() {
  bool recebido = false;

  while (Serial1.available()) {
     char cIn = Serial1.read();
     recebido = gps1.encode(cIn);
  }

  if (recebido) {
     Serial.println("----------------------------------------");
     
     //Latitude e Longitude
     long latitude, longitude;
     unsigned long idadeInfo;
     gps1.get_position(&latitude, &longitude, &idadeInfo);     

     if (latitude != TinyGPS::GPS_INVALID_F_ANGLE && longitude != TinyGPS::GPS_INVALID_F_ANGLE){
        gps_latitude.fval = ((float)latitude / 100000);
        gps_longitude.fval = ((float)longitude/ 100000);
        Serial.print("Latitude: ");
        Serial.println(gps_latitude.fval, 6);
        Serial.print("Longitude: ");
        Serial.println(gps_longitude.fval, 6);
        
        for(int i=0 ; i < sizeof(gps_latitude.fval) ; i++)
        destino_latitude.b[i] = gps_latitude.b[i];
      
        for(int i=0 ; i < sizeof(gps_longitude.fval) ; i++)
        destino_longitude.b[i] = gps_longitude.b[i];
  
        unsigned char bytes_latitude[] = {destino_latitude.b[0], destino_latitude.b[1], destino_latitude.b[2], destino_latitude.b[3], destino_longitude.b[0], destino_longitude.b[1], destino_longitude.b[2], destino_longitude.b[3]}; 
        sendBytesLoRa(bytes_latitude, 8);
        delay(5000);
     }
}
