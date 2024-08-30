void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void Walsh(int s){
  
  int wMatrix[s][s];
  wMatrix[0][0] = 1;
  
  int k = 1;
  while(k < s){
    for(int i=0; i<k; i++){
      for(int j=0; j<k; j++){
        wMatrix[i+k][j] = wMatrix[i][j];
        wMatrix[i][j+k] = wMatrix[i][j];
        wMatrix[i+k][j+k] = -wMatrix[i][j];
      }
    }
    k+=k;
  }
}
