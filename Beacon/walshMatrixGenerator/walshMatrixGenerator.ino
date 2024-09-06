int walsh_key[256];

void setup() {
  // put your setup code here, to run once:
  for(int i=0; i<16; i++){
    Walsh_row(i,16);
    for(int j=0; j<16; j++){
      Serial.print(walsh_key[j]+", ");
    }
    Serial.println();
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}

//for walsh matrix of height 16, we have 4 elements (2^4=16) but the first is always true
// |4  3  2  1
// |----------
//1|T  T  T  T
//2|T  T  F  T
//3|T  F  T  T
//4|T  F  F  T
//5|F  T  T  T
//6|F  T  F  T
//7|F  F  T  T
//8|F  F  F  T

void Walsh_row(int row, int height){
  int wRow[height];
  //mod the row number to determ  ine whether we're reflecting the first or second row of the original
  //This is the first element that is always true. Because the fractal is square rows are reflections of either the first or second row of the seed.
  if(row%2 == 1){
    walsh_key[0] = 1;
    walsh_key[1] = 1;
  } else {
    walsh_key[0] = 1;
    walsh_key[1] = 0;
  }  
  //Use logical operators to determine the truth value of each bit in sequence and reflect accordingly
  int bitGet = 1;
  int rowHalf = row/2;
  int wRowPos = 2; //index of wRow
  for(int i=0; i<sqrt(height); i++){
    //use bitGet to get the bit of rowHalf
    int rowBit = rowHalf & bitGet;
    if(rowBit == 0){//True reflection
      for(int j=0; i<wRowPos; j++){
        walsh_key[wRowPos+j] = walsh_key[j];
        wRowPos = wRowPos*2;
      }
    }
    else{//False reflection
      for(int j=0; j<wRowPos; j++){
        walsh_key[wRowPos+j] = abs(walsh_key[j]-1);
        wRowPos = wRowPos*2;
      }
    }
    bitGet << 1;
  }
  return &wRow;
}
