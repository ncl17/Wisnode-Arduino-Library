/*
 * A library for controlling RAK811 LoRa radio.
 *
 * RAK811 wisnode-lora v1.2
 * Firmware RAK811_V3.0.0.12.H
 * RUI_RAK811_BOOT_Version3_0_2
 * @Author Ncl17
 * BASED ON RAKwireless library for controlling RAK811 LoRa radio.  
 * @Author Chace.cao
 * @Author john.zou
 * @Date 25/11/2019
 *
 * 
 * Test with chirpstack server OTTA mode join
 *
 */

#include "Arduino.h"
#include "RAK811.h"




// Convert Bytes To String Function by Rob Bricheno
const char *hexdigits = "0123456789ABCDEF";

char* convertBytesToString (uint8_t* inputBuffer, int inputSize) {
    int i, j;
    char* compositionBuffer = (char*) malloc(inputSize*2 + 1);
    for (i = j = 0; i < inputSize; i++) {
        unsigned char c;
        c = (inputBuffer[i] >> 4) & 0xf;
        compositionBuffer[j++] = hexdigits[c];
        c = inputBuffer[i] & 0xf;
        compositionBuffer[j++] = hexdigits[c];
    }
    return compositionBuffer;
}


RAK811::RAK811(Stream& serial,Stream& serial1):
_serial(serial),_serial1(serial1)
{
  _serial.setTimeout(2000);
  _serial1.setTimeout(1000);
}

String RAK811::rk_getVersion()
{
  String ret = sendRawCommand(F("at+version"));
  ret.trim();
  return ret;
}

String RAK811::rk_setBand(String band){

  String ret = sendRawCommand("at+set_config=lora:region:"+band);
  ret.trim();
  return ret;


}


bool RAK811::rk_setRate(int rate)
{
  String ret = sendRawCommand("at+set_config=lora:dr:" + (String)rate);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void RAK811::rk_sleep()
{
  sendRawCommand(F("at+set_config=device:sleep:1"));
}
void RAK811::rk_wakeup()
{
  sendRawCommand(F("at+set_config=device:sleep:0"));
}

void RAK811::rk_restar()
{//at+set_config=device:restart
sendRawCommand(F("at+set_config=device:restart"));
}

bool RAK811::rk_setWorkingMode(int mode)
{
  String ver;
  switch (mode)
  {
    case 0:
      ver = sendRawCommand(F("at+set_config=lora:work_mode:0")); //Set LoRaWAN Mode.
      break;
    case 1:
      ver = sendRawCommand(F("at+set_config=lora:work_mode:1")); //Set LoRaP2P Mode.
      break;
    default:
      return false;
  }
  if (ver.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

int RAK811::rk_joinLoRaNetwork2()
{
  String ver;
  int index=0;
  ver = sendRawCommand(F("at+join")); //join Network through OTAA mode.
    index = ver.indexOf("Success");
 return index;
}

bool RAK811::rk_joinLoRaNetwork()
{
  String ver;
  ver = sendRawCommand(F("at+join")); //join Network through OTAA mode.
  if (ver.indexOf("[LoRa]:Join Success\r\nOK")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_set_devEUI(String devEUI)
{
  String command = "";
 
  command = "at+set_config=lora:dev_eui:" + devEUI;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.indexOf("configure success\r\nOK")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_set_appEUI(String appEUI)
{
  String command = "";
 
  command = "at+set_config=lora:app_eui:"  + appEUI ;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.indexOf("configure success\r\nOK")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_set_appKEY(String appKEY)
{
  String command = "";
 
  command = "at+set_config=lora:app_key:" + appKEY;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.indexOf("configure success\r\nOK")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_initOTAA(String devEUI, String appEUI, String appKEY)
{
  String command = "";
 
  command = "at+set_config=lora:dev_eui:" + devEUI + "&lora:app_eui:" + appEUI + "&lora:app_key:" + appKEY;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.indexOf("configure success\r\nOK")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_initABP(String devADDR, String nwksKEY, String appsKEY)
{
  String command = "";

  command = "at+set_config=lora:dev_addr:" + _devADDR + "&lora:nwks_key:" + _nwksKEY + "&lora:apps_key:" + _appsKEY;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.indexOf("LoRa dev_eui configure success\r\nOK\r\nLoRa app_eui configure success\r\nOK\r\nLoRa app_key configure success\r\nOK")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_sendData(int port, String datahex)
{
  String command = "";
  command = "at+send=lora:2" + String(":") + datahex;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.indexOf("send success\r\nOK\r\nat+recv")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_sendBytes(int port, uint8_t* buffer, int bufSize)
{
  //Send Bytes Command

  String command = "";
  command = "at+send=lora:2" + String(":") + convertBytesToString(buffer,bufSize);
  //Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.indexOf("OK\r\nat+recv")>0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

String RAK811::rk_recvData(void)
{
  _serial.setTimeout(2000);
  String ret = _serial.readStringUntil('\n');
#if defined DEBUG_MODE
  _serial1.println("-> " + ret);
#endif
  ret.trim();
  return ret;
}

bool RAK811::rk_setConfig(String Key,String Value)
{
  String command = "";
  command = "at+set_config=" + Key + ":" + Value;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String RAK811::rk_getConfig(String Key)
{
  String ret = sendRawCommand("at+get_config=" + Key);
  ret.trim();
  return ret;
}

String RAK811::rk_getP2PConfig(void)
{
  String ret = sendRawCommand("at+rf_config");
  ret.trim();
  return ret;
}

String RAK811::rk_getLoraStatus(void)
{
  String ret = sendRawCommand("at+get_config=lora:status");
  ret.trim();
  return ret;
}

String RAK811::rk_getDeviceStatus(void)
{
  String ret = sendRawCommand("at+get_config=device:status");
  ret.trim();
  return ret;
}
bool RAK811::rk_initP2P(String FREQ, int SF, int BW, int CR, int PRlen, int PWR)
{
  String command = "";
  command = "at+rf_config=" + FREQ + "," + SF + "," + BW + "," + CR + "," + PRlen + "," + PWR;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_recvP2PData(int report_en)
{
  String ver;
  switch (report_en)
  {
    case 0:
      ver = sendRawCommand(F("at+rxc=0")); //
      break;
    case 1:
      ver = sendRawCommand(F("at+rxc=1")); //
      break;
    default:
      return false;
  }
  if (ver.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool RAK811::rk_sendP2PData(char* DATAHex)
{
  String command = "";
  command = "at+send=lorap2p:" + (String)DATAHex;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}


bool RAK811::rk_setUARTConfig(int Baud)
{
  String command = "";
  command = "at+set_config=device:uart:1:" + (String)Baud ;
//  Serial.println(command);
  String ret = sendRawCommand(command);
  if (ret.startsWith("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

String RAK811::sendRawCommand(String command)
{
  delay(100);
String ret = "";
  while(_serial.available()){
#if defined DEBUG_MODE
    _serial1.println("||-> " + _serial.readStringUntil('\0'));
#else
    _serial.read();
#endif
  }
  delay(100);
  _serial.println(command);
  delay(100);
  while(_serial.available()>0){
  	delay(5);
		ret = _serial.readStringUntil('\0');
	}
  
 // ret.trim();
  delay(100);
#if defined DEBUG_MODE
  _serial1.println("<- Comando: " + command);
  _serial1.println("-> Respuesta: " + ret);
#endif
  return ret;
}



