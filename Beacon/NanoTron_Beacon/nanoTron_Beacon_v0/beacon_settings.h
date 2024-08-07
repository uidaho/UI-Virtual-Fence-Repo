/*
Last Edited 7/22/24
By: Matthew Shryock, Jacob Karl
*/

#define LED1 30 //defined as output in beaconinit
#define LED2 31//defined as output in beaconinit
#define LED3 32//defined as output in beaconinit
#define LED4 33//defined as output in beaconinit
#define LED5 34//defined as output in beaconinit

#define SX1280LED 8
#define SX1280CS 10
#define SX1280RST 9
#define SX1280BUSY 7
#define SX1280DIO1 5

#define FLASHCS 21

#define PORTPOWER 22//defined as output in beaconinit
#define PORTCOM1 3
#define PORTCOM2 4

#define BUZZER 20

#define RELAYIN1 18//defined as input in beaconinit
#define RELAYIN2 19//defined as input in beaconinit

#define RELAYOUT1 14//defined as output in beaconinit
#define RELAYOUT2 15//defined as output in beaconinit

#define NANOTX 17
#define NANORX 16
#define NANOBAUD 115200

#define BEACONBAUD 115200

#define ANALOGAUX1 A8
#define ANALOGAUX2 A9
#define ANALOGAUX3 A10
#define ANALOGAUX4 A11
#define ANALOGAUX5 A12

//Structures to allow int arrays to be passed and returned from functions easier. Mostly used in the CDMA functions.
struct array8 {
  int array[8];
};
struct array32{
  int array[32];
};
struct array256{
  int array[256];
};

struct Tag {
  String ID; //the unique ID of this tag
  float distance = -1; //the distance of the tag from the boundary when we ranged it last. Initialized to -1 to prevent us from warning a tag that just entered the network.
  int cooldown_timestamp = 0; //the next time we know the tag will be awake. In mS ahead of current time (also in mS). i.e. if current time is 5000mS and we know that the tag will be awake in 2000mS this variable will be 7000mS.
  int encryption_key[32]; //this tags unique encryption_key. One of the 32 encryption keys from the encryption_key_table.
  bool warning_flag = false; //a switch that tells us whether or not ro sent the reset signal rather than do nothing when a tag is outside the boundary.
  int communication_attempts = 0; //the number of times we have tried to communicate with this tag without getting a response.
};

float boundary = 100.0; //the distance from the beacon that tags will start to recieve warnings.
Tag all_tags[32]; //a list of tag structures that constitutes all of the tags on the network.
Tag all_cool_tags[32]; //a subset of all_tags that contains tags with cooldown_timestamp < current time.
array256 message; //the CDMA encoded message that we broadcast at the end of the main loop.
int encryption_key_table[32][32]; //a set of 32 orthoganal binary encryption codes (Walsh matricies, maybe, I need to look into this).
int max_communication_attempts = 10; //the number of times we will try and communicate with a tag before we remove it from the network.
int number_of_tags = 0; //How many tags are on the network. Also serves as an index for the end of all_tags[].
