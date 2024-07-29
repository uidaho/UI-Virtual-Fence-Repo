struct array4 {
  int array[4];
};
struct array32{
  int array[32];
};
struct array128{
  int array[128];
};

int key1[32] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int key2[32] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1 };
int key3[32] = {1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1,  1,  1, -1, -1};

int m1[4] = {1,1,1,1};
int m2[4] = {1,0,0,1};
int m3[4] = {0,1,1,0};

int message[128];
void setup() {
  Serial.begin(115200);
  Serial.println("Program Start");
  array128 e1;
  array128 e2;
  array128 e3;
  array128 message;
  e1 = kronecker(key1, encode(m1));
  e2 = kronecker(key2, encode(m2));
  e3 = kronecker(key3, encode(m3));
  message = messageAdd(e1,e2);
  message = messageAdd(message, e3);
  /*for(int i=0; i<4; i++){
    Serial.print(e1.array[i]);
    Serial.print(", ");
  }
  Serial.println();*/
  for(int i=0; i<128; i++){
    Serial.print(message.array[i]);
    Serial.print(", ");
  }//*/
  Serial.println();

  array4 d1 = decode(e3, key3);

  for(int i=0; i<4; i++){
    Serial.print(d1.array[i]);
    Serial.print(", ");
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}

array4 encode(int m[4]){
  Serial.println("encode");
  array4 e;
  for(int i=0; i<4; i++){
    e.array[i] = 2*m[i]-1;
  }
  return e;
}

array128 kronecker(int key[32], array4 m){
  Serial.println("kronecker");
  array128 k;
  for(int i=0; i<32; i++){
    for(int j=0; j<4; j++){
      k.array[(i*4)+j] = key[i]*m.array[j];
    }
  }
  return k;
}

array128 messageAdd(array128 m, array128 e){
  Serial.println("add");
  array128 mAdd;
  for(int i=0; i<128; i++){
    mAdd.array[i] = m.array[i]+e.array[i];
  }
  return mAdd;
}

array4 decode(array128 message, int key[32]){
  Serial.println("decode");
  array128 patternedMessage;
  array4 decodedMessage;
  //partswise multiplication of message by key
  int keyItter = 0;
  for(int i=0; i<128; i++){
    if(keyItter >= 32){
      keyItter = 0;
    }
    patternedMessage.array[i] = message.array[i]*key[keyItter];
    keyItter++;
  }
  Serial.println("parts multiply");
  //add 32 bit sections to get a 4 bit message
  for(int i=0; i<4; i++){
    int messageChunk = 0;
    for(int j=0; j<32; j++){
      messageChunk = messageChunk + patternedMessage.array[(i*4)+j];
    }
    decodedMessage.array[i] = messageChunk;
  }
  Serial.println("decoded");

  return decodedMessage;
}