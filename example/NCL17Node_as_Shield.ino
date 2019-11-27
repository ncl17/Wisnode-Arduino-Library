

#include "RAK811.h"
#include "SoftwareSerial.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 5
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);
#define WORK_MODE LoRaWAN   //  LoRaWAN or LoRaP2P
#define JOIN_MODE OTAA    //  OTAA or ABP

#if JOIN_MODE == OTAA
String DevEui = "YOUR DEV EUI"; // Fill this out
String AppEui = "YOUR APP EUI"; // Fill this out
String AppKey = "YOUR APP KEY"; // Fill This out
#else JOIN_MODE == ABP
String NwkSKey = "";
String AppSKey = "";
String DevAddr = "";


#endif

#define TXpin 3   // Set the virtual serial port pins
#define RXpin 2
#define DebugSerial Serial


SoftwareSerial RAKSerial(RXpin, TXpin);   // Declare a virtual serial port
RAK811 RAKLoRa(RAKSerial, DebugSerial);
bool conectadoAredLora = false;

 
void array_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}
void enviarValor() {
  // put your main code here, to run repeatedly:
  //float t = dht.readTemperature();
  uint32_t humidity = dht.readHumidity(false) * 100;
  uint32_t temperature = dht.readTemperature(false) * 100;
  uint32_t randomNumber;
  randomNumber = random(1, 100);
  DebugSerial.println("Humidity: " + String(humidity));
  DebugSerial.println("Temperature: " + String(temperature));

  byte payload[6];
  payload[0] = highByte(humidity);
  payload[1] = lowByte(humidity);
  payload[2] = highByte(temperature);
  payload[3] = lowByte(temperature);
  payload[4] = highByte(randomNumber);
  payload[5] = lowByte(randomNumber);
  char str[48] = "";
  array_to_string(payload, 6, str);

  //String stringOne =  (char*)payload;
  //DebugSerial.println(str);
  if (RAKLoRa.rk_sendData(2, str)){
     DebugSerial.println("Dato enviado Correctamente :\r\n**********************************\r\n" + (String)str + "\r\n**********************************");
    }
    else{DebugSerial.println("Dato No Enviados");};


}
bool InitLoRaWAN(void)
{
  if (RAKLoRa.rk_initOTAA(DevEui, AppEui, AppKey))
  { DebugSerial.println("You init OTAA parameter is OK!");
    while (!RAKLoRa.rk_joinLoRaNetwork());
    DebugSerial.println("You join Network success!");
    conectadoAredLora =true;
    return true;
  } else {
    DebugSerial.println("OTAA parameter is ERROR!");
    conectadoAredLora =false;
    return false;
  }
}
void setup() {
  dht.begin();
  // put your setup code here, to run once:
  //configure Serial1, this could also be a
  //software serial.
  RAKSerial.begin(9600);
  //configure the mail RX0 and TX0 port on arduino
  DebugSerial.begin(9600);
  while (DebugSerial.read() >= 0) {}
  while (!DebugSerial);
  DebugSerial.println("Iniciando...");
  // DebugSerial.println(RAKLoRa.rk_getVersion());
  // DebugSerial.println(RAKLoRa.rk_getDeviceStatus());
  //DebugSerial.println(RAKLoRa.rk_getLoraStatus());

  while (!InitLoRaWAN());

}
void loop() {
   delay(5000);
  if (conectadoAredLora){
      enviarValor();
   }
}
