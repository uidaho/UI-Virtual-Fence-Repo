void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

//for walsh matrix of height 16, we have 4 elements (2^4=16) but the first is always true
// |4  3  2  1
// |----------
//1|T  T  T
//2|T  T  F
//3|T  F  T
//4|T  F  F
//5|F  T  T
//6|F  T  F
//7|F  F  T
//8|F  F  F

int[] Walsh_row(int row, int height){
  int wRow[height];
  //mod the row number to determ  ine whether we're reflecting the first or second row of the original
  if(row%2 == 1){
    wRow[0] = 1;
    wRow[1] = 1;
  } else {
    wRow[0] = 1;
    wRow[1] = 0;
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
        wRow[wRowPos+j] = wRow[j];
        wRow = wRow*2;
      }
    }
    else{//False reflection
      for(int j=0; j<wRowPos; j++){
        wRow[wRowPos+j] = abs(wRow[j]-1);
        wRow = wRow*2;
      }
    }
    bitGet << 1;
  }
  return wRow[];
}
