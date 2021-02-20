// Demo Code for SerialCommand Library
// Craig Versek, Jan 2014
// based on code from Steven Cogswell, May 2011

#include <SerialCommand.h>
#include <SPI.h>
#include <RH_RF95.h>
//#include <ArduinoJson.h> //https://arduinojson.org/v6/doc/installation/

// heltec wifi lora 32 v2
#define RFM95_CS 18
#define RFM95_RST 14
#define RFM95_INT 26

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define LED 5   // Arduino LED on board

SerialCommand sCmd(Serial);         // The demo SerialCommand object, initialize with any Stream object

void setup() {
  //analogReadResolution(12);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  
  pinMode(LED, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(LED, LOW);    // default to LED off

  Serial.begin(9600);

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("ON",    LED_on);          // Turns LED on
  sCmd.addCommand("OFF",   LED_off);         // Turns LED off
//  sCmd.addCommand("BRUSH",   brushTeeth);         // Turns LED off
  sCmd.addCommand("HELLO", sayHello);        // Echos the string argument back
  //sCmd.addCommand("SERVO",     processCommand);  // Converts two arguments to integers and echos them back
  //sCmd.addCommand("SOIL_MOISTURE", get_moisture);  // Converts two arguments to integers and echos them back
  sCmd.addCommand("HELP", get_help);  // Converts two arguments to integers and echos them back

  
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  Serial.println("Serialcommand Ready");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  
}

int16_t packetnum = 0;  // packet counter, we increment per xmission


void loop() {
  int num_bytes = sCmd.readSerial();      // fill the buffer
  if (num_bytes > 0){
    sCmd.processCommand();  // process the command
  }
  delay(10);
}


void LED_on(SerialCommand this_sCmd) {
  this_sCmd.println("LED on");
  digitalWrite(LED, HIGH);
}

 
 void get_help(SerialCommand this_sCmd) {
  this_sCmd.println(":::: hallo from cabalbot! ::::");
  this_sCmd.println("@cabalbot ON -- turns the led on");
  this_sCmd.println("@cabalbot OFF -- turn led off");
  this_sCmd.println("@cabalbot HELLO -- echoes the string argument back to you");
  this_sCmd.println("@cabalbot SOIL_MOISTURE -- get the latest soil moisture reading");
}

void LED_off(SerialCommand this_sCmd) {
  this_sCmd.println("LED off");
  digitalWrite(LED, LOW);
}

void sayHello(SerialCommand this_sCmd) {
  char *arg;
  arg = this_sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it

    
    this_sCmd.print("Hello ");
    this_sCmd.println(arg);
    uint8_t len = sizeof(arg);
    delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
    
    
    //DynamicJsonDocument radiodoc(1024);
      //deserializeJson(radiodoc, arg);
      
    Serial.println("Transmitting..."); // Send a message to rf95_server
    
    Serial.println("Sending...");
    delay(10);

    //char radiopacket[20] = "Hello World #      ";
    
    rf95.send((uint8_t *)arg, 240);
  
    Serial.println("Waiting for packet to complete..."); 
    delay(10);
    digitalWrite(LED, HIGH);
    rf95.waitPacketSent();
    Serial.println("Sent."); 
    digitalWrite(LED, LOW);
    
  
    
  }
  else {
    this_sCmd.println("Hello, whoever you are");
  }
}


void processCommand(SerialCommand this_sCmd) {
  int aNumber;
  char *arg;

  this_sCmd.println("We're in processCommand");
  arg = this_sCmd.next();
  if (arg != NULL) {
    aNumber = atoi(arg);    // Converts a char string to an integer
    this_sCmd.print("First argument was: ");
    this_sCmd.println(aNumber);

    //servo.write(aNumber); 
  }
  else {
    this_sCmd.println("No arguments");
  }

  arg = this_sCmd.next();
  if (arg != NULL) {
    aNumber = atol(arg);
    this_sCmd.print("Second argument was: ");
    this_sCmd.println(aNumber);
  }
  else {
    this_sCmd.println("No second argument");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(SerialCommand this_sCmd) {
  SerialCommand::CommandInfo command = this_sCmd.getCurrentCommand();
  this_sCmd.print("Did not recognize \"");
  this_sCmd.print(command.name);
  this_sCmd.println("\" as a command.");
}
