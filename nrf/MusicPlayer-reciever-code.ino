const int CEPIN = 9;
const int CSNPIN = 10;
const int servoPIN = 5;

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(CEPIN, CSNPIN);                // CE, CSN
const byte address[6] = "400";

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer =
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
int data1 = 0;
bool play = true;
void setup() {
  Serial.begin(9600);

  //music
  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20, 20);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int


  // RF24 setup
  if (!radio.begin()) {
    Serial.println("radio  initialization failed");
    while (1);
  } else {
    Serial.println("radio successfully initialized");
  }
  radio.openReadingPipe(0, address);  //destination addres
  radio.setPALevel(RF24_PA_MIN);   // min or max
  radio.startListening();
}

void loop() {
  uint8_t pipeNum;

  if (radio.available(&pipeNum))  //Looking for the data.
  {
    int data;
    Serial.print("data available on pipe ");
    Serial.println(pipeNum);

    radio.read(&data, sizeof(data));  //Reading the data
    //    Serial.print("data = ");
    //    Serial.println( data);
    if (data != 0) {
      if (data != data1) {
        data1 = data;
      }
    }
    Serial.print("data1 = ");
    Serial.println( data1);

    //    switch (data) {
    //      case 0:
    //      Serial.println(F("Playing track 001"));
    //        playMusic("/track001.mp3");
    //        break;
    //
    //      case 1:
    //        Serial.println(F("Playing track 001"));
    //        playMusic("/track001.mp3");
    //        break;
    //
    //      case 2:
    //        Serial.println(F("Playing track 002"));
    //        playMusic("/track002.mp3");
    //        break;
    //
    //      case 3:
    //        musicPlayer.stopPlaying();
    //        break;
    //    }

    if (data1 == 1) {
      if (musicPlayer.stopped()) {
        play = true;
      }
      else {
        musicPlayer.stopPlaying();
        play = false;
      }
      if (play) {
        Serial.println(F("Playing track 001"));
        musicPlayer.startPlayingFile("/dirtmout.mp3");
      }
    }
    
    if (data1 == 2) {   
      if (musicPlayer.stopped()) {
        play = true;
      }
      else {
        musicPlayer.stopPlaying();
        play = false;
      }
      if (play) {
        Serial.println(F("Playing track 002"));
        musicPlayer.startPlayingFile("/whitelad.mp3");
      }
    }
    if (data1 == 3) {
      musicPlayer.stopPlaying();
    }
    if (data1 == 0) {
      musicPlayer.stopPlaying();
    }
    delay(5);
  }
}


void playMusic(char track) {

  musicPlayer.playFullFile(track);

  if (musicPlayer.stopped()) {
    Serial.println("Done playing music");
    while (1) {
      delay(10);  // we're done! do nothing...
    }
  }

  //  if (Serial.available()) {
  //    char c = Serial.read();

  //    // if we get an 's' on the serial console, stop!
  //    if (c == 's') {
  //      musicPlayer.stopPlaying();
  //    }
  //
  //    // if we get an 'p' on the serial console, pause/unpause!
  //    if (c == 'p') {
  //      if (! musicPlayer.paused()) {
  //        Serial.println("Paused");
  //        musicPlayer.pausePlaying(true);
  //      } else {
  //        Serial.println("Resumed");
  //        musicPlayer.pausePlaying(false);
  //      }
  //    }
  //  }
}


void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
