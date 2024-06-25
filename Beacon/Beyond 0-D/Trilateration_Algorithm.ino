void setup() {
  Serial.begin(115200);
  Serial.println("Distance is not 0, yay!");

  //float CowDistanceMatrix[1][4] = {55.17,51.225,86.28,48.42}; //Known distances, output should be around (40,38)
  //float CowDistanceMatrix[1][4] = {39.4,56.85,103,44.4}; //Known distances, output should be around (32.2, 22.9);
  float CowDistanceMatrix[1][4] = {70.71, 70.71, 70.71, 70.71};

  
  float D[8];                             //variable to store distance values for looping

  for (int i = 0; i < 4; i++) {
    D[i] = CowDistanceMatrix[0][i];       //copies the relevent values into trilateration loop variable
    CowDistanceMatrix[0][i] = 0;          //clears the Distance Matrix after value has been copied for further use
  }

  //Trilateration code loops through the system twice to assure all possible combinations are calculated, this reassigns distances for that loop
  D[4] = D[0];
  D[5] = D[1];
  D[6] = D[2];
  D[7] = D[3];

  float R = 500;                          //maximum system radius
  float dd[8];                            //distance compare to confirm the overlap of two circles
  float delta[8];                         //variable used to reduce x,y equation length

  //Location Arrays for ALL x,y coordinate pairs
  float x[8][2];
  float y[8][2];

  //Beacon location Array ... values ARE repeated TWICE
  /*float beacon[8][3] = {
    0.0, 0.0, 1.0,
    0.0, 70.0, 1.0,
    70.0, 70.0, 1.0,
    70.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 70.0, 1.0,
    70.0, 70.0, 1.0,
    70.0, 0.0, 1.0
  };*/
  float beacon[8][3] = {
    0.0, 0.0, 1.0,
    0.0, 100.0, 1.0,
    100.0, 100.0, 1.0,
    100.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 100.0, 1.0,
    100.0, 100.0, 1.0,
    100.0, 0.0, 1.0,
  };

  //Data Validation ... Checks to see that the distance provided does not exceed the max range of the nanotron / unreliable
  for (int i = 0; i < 4; i++) {
    if (D[i] > R) {
      D[i] = 0;
    }
  }

  //Data Validation ... Looking for minimum of 3 distance values
  if (D[0] == 0 && D[1] == 0 || D[1] == 0 && D[2] == 0 || D[2] == 0 && D[3] == 0 || D[3] == 0 && D[0] == 0) {
    Serial.println("Error, Multiple Solutions Exist");
  }

  //X,Y Calculations ... confirms that the 2 circles exist and calculates common x,y points / if not, returns a 0
  for (int i = 0; i < 8; i++) {
    if (D[i] > 0 && D[i + 1] > 0) {

      dd[i] = abs(sqrt(pow((beacon[i][0] - beacon[i + 1][0]), 2) + pow((beacon[i][1] - beacon[i + 1][1]), 2)));
      if (dd[i] > D[i] + D[i + 1]) {
        Serial.print("Error No Solutions Exist between ");
        Serial.print(i);
        Serial.print(" and ");
        Serial.println(i + 1);
        x[i][0] = 0;
        x[i][1] = 0;
        y[i][0] = 0;
        y[i][1] = 0;
      }
      else {
        delta[i] = (.25) * sqrt((dd[i] + D[i] + D[i + 1]) * (dd[i] + D[i] - D[i + 1]) * (dd[i] - D[i] + D[i + 1]) * (-dd[i] + D[i] + D[i + 1]));
        x[i][0] = ((beacon[i][0] + beacon[i + 1][0]) / 2) + ((beacon[i + 1][0] - beacon[i][0]) * (pow(D[i], 2) - pow(D[i + 1], 2)) / (2 * pow(dd[i], 2))) - (2 * delta[i] * ((beacon[i][1] - beacon[i + 1][1]) / pow(dd[i], 2)));
        x[i][1] = ((beacon[i][0] + beacon[i + 1][0]) / 2) + ((beacon[i + 1][0] - beacon[i][0]) * (pow(D[i], 2) - pow(D[i + 1], 2)) / (2 * pow(dd[i], 2))) + (2 * delta[i] * ((beacon[i][1] - beacon[i + 1][1]) / pow(dd[i], 2)));
        y[i][0] = ((beacon[i + 1][1] + beacon[i][1]) / 2) + ((beacon[i + 1][1] - beacon[i][1]) * (pow(D[i], 2) - pow(D[i + 1], 2)) / (2 * pow(dd[i], 2))) + (2 * delta[i] * ((beacon[i][0] - beacon[i + 1][0]) / pow(dd[i], 2)));
        y[i][1] = ((beacon[i + 1][1] + beacon[i][1]) / 2) + ((beacon[i + 1][1] - beacon[i][1]) * (pow(D[i], 2) - pow(D[i + 1], 2)) / (2 * pow(dd[i], 2))) - (2 * delta[i] * ((beacon[i][0] - beacon[i + 1][0]) / pow(dd[i], 2)));
      }
    }
    else {
      x[i][0] = 0;
      x[i][1] = 0;
      y[i][0] = 0;
      y[i][1] = 0;
    }
  }

  for (int i = 0; i < 8; i++) {
  Serial.print(x[i][0]);
  Serial.print(" ");
  Serial.println(y[i][0]);
  }
 
  //Final Location Variable ... Initilized to 0
  float Xf = 0;
  float Yf = 0;

  //Selects trusted X values
  if (x[1][0] != 0 && x[3][0] != 0)
    Xf = (x[1][0] + x[1][1] + x[3][0] + x[3][1]) / 4;
  else if (x[1][0] != 0)
    Xf = (x[1][0] + x[1][1]) / 2;
  else if (x[3][0] != 0)
    Xf = (x[3][0] + x[3][1]) / 2;
  else
    Xf = 0;

  //Selects trusted Y values
  if (y[0][0] != 0 && y[2][0] != 0)
    Yf = (y[0][0] + y[0][1] + y[2][0] + y[2][1]) / 4;
  else if (y[0][0] != 0)
    Yf = (y[0][0] + y[0][1]) / 2;
  else if (y[2][0] != 0)
    Yf = (y[2][0] + y[2][1]) / 2;
  else
    Yf = 0;

  //Averages ALL X values into a final answer ... MUST be with 5 units to be included in final answer
  for (int i = 0; i < 8; i++) {
    if (x[i][0] <= Xf + 5 && x[i][0] >= Xf - 5)
      Xf = (x[i][0] + Xf) / 2;
    if (x[i][1] <= Xf + 5 && x[i][1] >= Xf - 5)
      Xf = (x[i][1] + Xf) / 2;
  }

  //Averages ALL Y values into a final answer ... MUST be with 5 units to be included in final answer
  for (int i = 0; i < 8; i++) {
    if (y[i][0] <= Yf + 5 && y[i][0] >= Yf - 5)
      Yf = (y[i][0] + Yf) / 2;
    if (y[i][1] <= Yf + 5 && y[i][1] >= Yf - 5)
      Yf = (y[i][1] + Yf) / 2;
  }

  Serial.print("X = ");
  Serial.println(Xf);
  Serial.print("Y = ");
  Serial.println(Yf);

  //zap(Xf, Yf);
}

void loop(){
  
}
