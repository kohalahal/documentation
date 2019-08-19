#define PM_0      0
#define PM_1      1
#define PM_12     2
#define PM_123    3
#define PM_2      4
#define PM_23     5
#define PM_3      6
#define PM_31     7

#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

int curr_playmode = PM_0;
int save_playmode = PM_0;

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
//*************THINGS DECLARED*************************
int fsrPin1 = 0;     
int fsrReading1;   
int fsrPin2 = 1;   
int fsrReading2;
int fsrPin3 = 2;     
int fsrReading3;

//**********************************************************
// this handy function will return the number of bytes currently free in RAM, great for debugging!   
int freeRam(void)
{
  extern int  __bss_end; 
  extern int  *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
}
void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
  // set up serial port
  void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Wave test!");
  
  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!
  
  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  
  if (!card.init()) {         //play with 4 MHz spi (default faster!)  
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while(1);                            // then 'halt' - do nothing!
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
// Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while(1);                            // then 'halt' - do nothing!
  }
  
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(),DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while(1);                             // then 'halt' - do nothing!
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Ready!");
}
//**********************************************************
void loop(void) {
  fsrReading1 = analogRead(fsrPin1);  
  fsrReading2 = analogRead(fsrPin2);  
  fsrReading3 = analogRead(fsrPin3);  

  if(fsrReading1 > 900) {
    curr_playmode = PM_1;
    if(fsrReading2 > 900) {
      curr_playmode = PM_12;
      if(fsrReading3 > 900) {
        curr_playmode = PM_123;
      }
    }
  }
  
  if(fsrReading2 > 900) {
    curr_playmode = PM_2;
    if(fsrReading3 > 900) {
      curr_playmode = PM_23;
    }
  }
  
  if(fsrReading3 > 900) {
    curr_playmode = PM_3;
    if(fsrReading1 > 900) {
      curr_playmode = PM_31;
    }
  }    

  if((fsrReading1 < 900) && (fsrReading2 < 900) && (fsrReading3 < 900))
    curr_playmode = PM_0;

if(curr_playmode != save_playmode ) {
    save_playmode = curr_playmode;
    switch(curr_playmode) {
      case PM_0 :
        wave.stop();            break;
      case PM_1 :
        playfile("il.WAV");     break;
      case PM_12 :
        playfile("ili.WAV");    break;
      case PM_123 :
        playfile("all.WAV");    break;
      case PM_2 :
        playfile("i.WAV");      break;
      case PM_23 :
        playfile("isam.WAV");   break;
      case PM_3 :
        playfile("sam.WAV");    break;
      case PM_31 :
        playfile("ilsam.WAV");  break;
    }
  }
}
