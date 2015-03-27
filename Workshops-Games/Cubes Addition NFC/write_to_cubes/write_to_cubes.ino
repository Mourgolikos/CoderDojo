
/**************************************************************************/
/*! 
    @original_author   odopod thisispete
    @modified_by       Paschaleris Triantafyllos
    
    This code is about to write to the cubes for the CoderDojo Thessaloniki's Workshop "Cubes Addition NFC".
    It uses I2C or SPI for the PN532 module and reads a Mifare Tag.

*/
/**************************************************************************/


/////////////////////
//
//
//   TODO:  ADD code for Segment Display and the function for the "Addition"
//
//
/////////////////////


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
uint8_t Mifare::useKey = KEY_B;
uint8_t Mifare::keyA[6] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
uint8_t Mifare::keyB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint32_t Mifare::cardType = 0; //will get overwritten if it finds a different card

#include <NDEF.h>

#define PAYLOAD_SIZE 236
uint8_t payload[PAYLOAD_SIZE] = {};

// The number of cubes we will write to
#define CUBESDATATEXT_SIZE 5
// text to write in String Array //size:5
int cubesDataText_index = 0;
String cubesDataText[CUBESDATATEXT_SIZE] = { "1", "3", "4", "5", "addition" }; // "addition".toInt() will parse as zero


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
//    Serial.print("v: "); Serial.println((versiondata>>16) & 0xFF, DEC); 
//    Serial.println((versiondata>>8) & 0xFF, DEC);
//    Serial.print("Supports "); Serial.println(versiondata & 0xFF, HEX);
  

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

       // write the text to each cube using the array "blocksDataText" of size "BLOCKDATATEXT_SIZE" with incemental index "blocksDataText_index"
       String stringText = cubesDataText[cubesDataText_index];
       memcpy(payload, stringText.c_str(), cubesDataText[cubesDataText_index].length());
       uint8_t len = NDEF().encode_TEXT((uint8_t *)"en", payload);

       // a boolean variable to know if the writting was successful
       boolean writeSuccess = mifare.writePayload(payload, len);
       // set the next index for the next cube!
       cubesDataText_index = nextIndex(cubesDataText_index, writeSuccess);
   }
   //some delay to avoid The Flood
   delay(6543);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to return the next cube's index
int nextIndex(int currentIndex, boolean success){
    if (success){
        return successFunction(currentIndex);
    }else{
        return failFunction(currentIndex);
    }
}
////////////
// return a feedback about success or fail, and in case of success increase the index "blocksDataText_index" (handled inside successFunction() )
int successFunction(int cubesIndex){
     if (cubesIndex < 5) {
        cubesIndex++;
        Serial.println("Setting cubes' index to");
        Serial.println(cubesIndex);
    } else {
        Serial.println("Resetting cubes' index to 0");
        cubesIndex = 0;
    }
    Serial.println("success!");
    return cubesIndex;
}
int failFunction(int cubesIndex){
    Serial.println("fail");
    return cubesIndex;
}
///////////
