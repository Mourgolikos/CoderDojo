
/**************************************************************************/
/*! 
    @original_author   odopod thisispete
    @modified_by       Paschaleris Triantafyllos
    
    This code is about to read from the cubes for the CoderDojo Thessaloniki's Workshop "Cubes Addition NFC".
    It uses I2C or SPI for the PN532 module and reads a Mifare Tag.

*/
/**************************************************************************/


//compiler complains if you don't include this even if you turn off the I2C.h 
//@TODO: look into how to disable completely
#include <Wire.h>

//I2C:
//#include <PN532_I2C.h>
//#define IRQ   2
//#define RESET 3
//PN532 * board = new PN532_I2C(IRQ, RESET);
//end I2C -->

//SPI:
#include <PN532_SPI.h>
#define SCK 13
#define MOSI 11
#define SS 10
#define MISO 12
PN532 * board = new PN532_SPI(SCK, MISO, MOSI, SS);
//end SPI -->

#include <Mifare.h>
Mifare mifare;
//init keys for reading classic
uint8_t Mifare::useKey = KEY_A;
uint8_t Mifare::keyA[6] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
uint8_t Mifare::keyB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint32_t Mifare::cardType = 0; //will get overwritten if it finds a different card

#include <NDEF.h>

#define PAYLOAD_SIZE 224
uint8_t payload[PAYLOAD_SIZE] = {};

// The number of cubes we will write to
#define CUBESDATATEXT_SIZE 5
// text to write in String Array //size:5
int cubesDataText_index = 0;
String cubesDataText[CUBESDATATEXT_SIZE] = { "number_1", "number_3", "number_4", "number_5", "addition" };

void setup(void) {
  Serial.begin(9600);

  board->begin();

  uint32_t versiondata = board->getFirmwareVersion();
  if (! versiondata) {
    Serial.println("err");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("5");Serial.println((versiondata>>24) & 0xFF, HEX); 
//  Serial.print("v: "); Serial.println((versiondata>>16) & 0xFF, DEC); 
//  Serial.println((versiondata>>8) & 0xFF, DEC);
//  Serial.print("Supports "); Serial.println(versiondata & 0xFF, HEX);
  


  if(mifare.SAMConfig()){
    Serial.println("ok");  
  }else{
    Serial.println("er");
  }
  
  
}



void loop(void) {
   uint8_t * uid = mifare.readTarget();
   if(uid){
        Serial.println(Mifare::cardType == MIFARE_CLASSIC ?"Classic" : "Ultralight");
    
        memset(payload, 0, PAYLOAD_SIZE);
    
        //read 
    
        mifare.readPayload(payload, PAYLOAD_SIZE);
  
        FOUND_MESSAGE m = NDEF().decode_message(payload);
      
        switch(m.type){
           case NDEF_TYPE_TEXT:
           Serial.print("Cube's Data: "); 
           Serial.println(m.format);
           Serial.println((char*)m.payload);
          break;
         default:
           Serial.println("unsupported");
          break; 
        }
        delay(3456);
   }
   delay(1234);
}
