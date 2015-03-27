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
//      Serial.print("v: "); Serial.println((versiondata>>16) & 0xFF, DEC); 
//      Serial.println((versiondata>>8) & 0xFF, DEC);
//      Serial.print("Supports "); Serial.println(versiondata & 0xFF, HEX);
  


      if(mifare.SAMConfig()){
          Serial.println("ok");  
      }else{
          Serial.println("er");
      }
  
//////// Setting Up Segment Display PinOut
      pinMode(2, OUTPUT); 
      pinMode(3, OUTPUT);
      pinMode(4, OUTPUT);
      pinMode(5, OUTPUT);
      pinMode(6, OUTPUT);
      pinMode(7, OUTPUT);
      pinMode(8, OUTPUT);
      pinMode(9, OUTPUT);
      digitalWrite(9, 0);  // start with the "dot" off
}//end of setup()



void loop(void) {
       uint8_t * uid = mifare.readTarget();
       if(uid){
            Serial.println(Mifare::cardType == MIFARE_CLASSIC ?"Classic" : "Ultralight");
    
            memset(payload, 0, PAYLOAD_SIZE);
    
            // reading 
            mifare.readPayload(payload, PAYLOAD_SIZE);
  
            // Decode the data from the NFC read
            FOUND_MESSAGE m = NDEF().decode_message(payload);
            char* cubesData = (char*)m.payload; //has to be char* because of precision problems...  m.payload is: uint8_t* {aka unsigned char*} // Love C++ ...NOT! Python ftw! :)
            
            switch(m.type){
                 case NDEF_TYPE_TEXT:
                 Serial.print("Data in Cube: "); 
                 Serial.println(m.format);
                 Serial.println(cubesData);
                 Serial.println(messageSegment( cubesData ) ? "Cube Decoded" : "Failed Decoding Cube");
                 break;
            default:
                 Serial.println("unsupported");
                 break; 
            }
            delay(3456);
       }
       delay(1234);
}//end of loop()



///////////////////////////////////////////////////////////////
//// Functions about the Addition of the Numbers
/////////////
int addition(int number, boolean nextNumber){
     //I'll need some static variables to keep track of the sum...
     static int firstNumber;
     static int secondNumber;
     static boolean currentNumberIsTheFirst = true;
     
     ///// Warning! omg-complex-ifs are coming!
     if (nextNumber && currentNumberIsTheFirst==false){ //at this state we are on the second digit and we have triggered a new addition (maybe via the "Addiion" Cube!)
           currentNumberIsTheFirst = true;             //...so we are gonna reset all the static variables to their zero state
           secondNumber = 0;                            //
           
           firstNumber = number;  // ...and set the firstNumber to the number that we scanned
     } else if (nextNumber && currentNumberIsTheFirst){ //at this state we are moving to the second digit of the addition
           currentNumberIsTheFirst = false;
           secondNumber = number; // ...and set the secondNumber to the number that we scanned
     } else { // nextNumber is false so we are gonna refresh the current digit withe the new number from Cube
           if (currentNumberIsTheFirst){ //still we are at the first digit
                 firstNumber = number;
                 secondNumber = 0;  
           } else { //still we are at the second digit (if we are not in the first! lol!)
                 secondNumber = number;
           }           
     }
     ///// END of the omg-complex-ifs

     
     // Print some output in order to know where we are!
     Serial.print("FirstNumber: ");
     Serial.println(firstNumber);
     Serial.print("SecondNumber: ");
     Serial.println(secondNumber);
     
     return (firstNumber + secondNumber);
}
void storeNumber(int number){

}
/////////////////////// END of Functions about the Addition of the Numbers
///////////////////////////////////////////////////////////////
//// Functions about decoding NFC message to Segment Display Numbers
/////////////
boolean messageSegment(char* message){
      for (int i=0; i < CUBESDATATEXT_SIZE; i++){
            if ( message == (char*)cubesDataText[i].c_str() ){ //C++ "String"... yay...
                  return numberSegment( cubesDataText[i].toInt() ); //instead of using (correctly) the variable message, i use the equal char-ready cubesDataText[i], to avoid conversions...
            }
      }
      return false;
}
/////////////////////// END of Functions about decoding NFC message to Segment Display Numbers
///////////////////////////////////////////////////////////////
//// Functions about lighting up Segment Display Numbers
/////////////
boolean numberSegment(int number){
        switch (number){
              case 0:
                    //Zero-Ten
                    writeZero();
                    addition(number,true);
                    return true;
              case 1:
                    //One
                    writeOne();
                    addition(number,false);
                    return true;
              case 2:
                    //Two
                    writeTwo();
                    addition(number,false);
                    return true;
              case 3:
                    //Three
                    writeThree();
                    addition(number,false);
                    return true;
              case 4:
                    //Four
                    writeFour();
                    addition(number,false);
                    return true;
              case 5:
                    //Five
                    writeFive();
                    addition(number,false);
                    return true;
              case 6:
                    //Six
                    writeSix();
                    addition(number,false);
                    return true;
              case 7:
                    //Seven
                    writeSeven();
                    addition(number,false);
                    return true;
              case 8:
                    //Eight
                    writeEight();
                    addition(number,false);
                    return true;
              case 9:
                    //Nine
                    writeNine();
                    addition(number,false);
                    return true;
              default:
                    //Not Found, Return false
                    return false;
        }        
        return false;
}

void allOff(){
        digitalWrite(2, 0);
	digitalWrite(3, 0);
	digitalWrite(4, 0);
	digitalWrite(5, 0);
	digitalWrite(6, 0);
	digitalWrite(7, 0);
	digitalWrite(8, 0);
        digitalWrite(9, 0);
}
void writeDP(){ //The Dot Point
        digitalWrite(9, 1);
}
void writeZero(){
        allOff();
        digitalWrite(2, 1);
	digitalWrite(3, 1);
	digitalWrite(4, 1);
	digitalWrite(5, 1);
	digitalWrite(6, 1);
	digitalWrite(7, 1);
        writeDP(); //i like Zero to be shown with the Dot Point ON  ;)
}
void writeOne(){
        allOff();
	digitalWrite(3, 1);
	digitalWrite(4, 1);
}
void writeTwo(){
        allOff();
        digitalWrite(2, 1);
	digitalWrite(3, 1);
	digitalWrite(5, 1);
	digitalWrite(6, 1);
	digitalWrite(8, 1);
}
void writeThree(){
        allOff();
        digitalWrite(2, 1);
	digitalWrite(3, 1);
	digitalWrite(4, 1);
	digitalWrite(5, 1);
	digitalWrite(8, 1);
}
void writeFour(){
        allOff();
	digitalWrite(3, 1);
	digitalWrite(4, 1);
	digitalWrite(7, 1);
	digitalWrite(8, 1);
}
void writeFive(){
        allOff();
        digitalWrite(2, 1);
	digitalWrite(4, 1);
	digitalWrite(5, 1);
	digitalWrite(7, 1);
	digitalWrite(8, 1);
}
void writeSix(){
        allOff();
        digitalWrite(2, 1);
	digitalWrite(4, 1);
	digitalWrite(5, 1);
	digitalWrite(6, 1);
	digitalWrite(7, 1);
	digitalWrite(8, 1);
}
void writeSeven(){
        allOff();
        digitalWrite(2, 1);
	digitalWrite(3, 1);
	digitalWrite(4, 1);
}
void writeEight(){
        digitalWrite(2, 1);
	digitalWrite(3, 1);
	digitalWrite(4, 1);
	digitalWrite(5, 1);
	digitalWrite(6, 1);
	digitalWrite(7, 1);
	digitalWrite(8, 1);
}
void writeNine(){
        allOff();
        digitalWrite(2, 1);
	digitalWrite(3, 1);
	digitalWrite(4, 1);
	digitalWrite(7, 1);
	digitalWrite(8, 1);
}
/////////////////////// END of Functions about lighting up Segment Display Numbers
