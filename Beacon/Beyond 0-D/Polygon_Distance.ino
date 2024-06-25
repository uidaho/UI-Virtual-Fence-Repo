struct Point {
  float x, y;
};

struct Line {
  Point p1, p2;
};

float polygon_1[5][2] = {{7,5},{12,5},{15,10},{10,13},{5,10}};
float polygon_2[13][2] = {{5,5},{10,-5},{15,5},{15,10},{20,10},{20,15},{15,20},{10,15},{5,20},{0,15},{10,10},{0,10}};
float points[5][2] = {{1,1},{0,0},{10,10},{10,0},{15,15}};

Point pt_polygon_1[5] = {{7,5},{12,5},{15,10},{10,13},{5,10}};
Point pt_polygon_2[13] = {{5,5},{10,-5},{15,5},{15,10},{20,10},{20,15},{15,20},{10,15},{5,20},{0,15},{10,10},{0,10}};
Point pt_points[5] = {{1,1},{0,0},{10,10},{10,0},{17,12}};

float minDist = 100;
float dist = 101;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  float func_dist;
  
  for (int i = 0; i <= 4; i++){
    Serial.print("Point: ");
    Serial.print(pt_points[i].x);
    Serial.print(", ");
    Serial.println(pt_points[i].y);
    unsigned long  func_time_start = millis();
    func_dist = polygon_calc(pt_polygon_1, 5, pt_points[i]);
    unsigned long  func_time = millis() - func_time_start;
    Serial.print("Function return: ");
    Serial.println(func_dist);
    Serial.print("Calculation Time: ");
    Serial.println(func_time);
    minDist = 100;
    dist = 101;
  }
  
  Serial.println("----------------------------------------------");
  
  for (int i = 0; i <= 4; i++){
    Serial.print("Point: ");
    Serial.print(pt_points[i].x);
    Serial.print(", ");
    Serial.println(pt_points[i].y);
    unsigned long  func_time_start = millis();
    func_dist = polygon_calc(pt_polygon_2, 12, pt_points[i]);
    unsigned long  func_time = millis() - func_time_start;
    Serial.print("Function return: ");
    Serial.println(func_dist);
    Serial.print("Calculation Time: ");
    Serial.println(func_time);
    minDist = 100;
    dist = 101;
  }
    
  Serial.println("----------------------------------------------");

}

void loop() {
  // put your main code here, to run repeatedly:
  
}


  
  /**
  for p1, p2 in vertices:

  var r = dotProduct(vector(p2 - p1), vector(x - p1))
  //x is the point you're looking for

  r = r/(magnitude(vector(p2 - p1)) ** 2)

  if r < 0:
    var dist = magnitude(vector(x - p1))
  else if r > 1:
    dist = magnitude(vector(p2 - x))
  else:
    dist = sqrt(magnitude(vector(x - p1)) ^ 2 - (r * magnitude(vector(p2-p1))) ^ 2)

  minDist = min(dist,minDist)
   */
float polygon_calc(Point poly[], int poly_size, Point pt){
  
  //Serial.println("Start polygon calc");
  
  for (int i = 0; i < poly_size; i++){
    
    int j = i + 1;
    if (j > poly_size-1){
      j = 0;
    }

    //Create all relavent vectors (p1 -> p2), (point -> p1), (p2 -> point)
    Point edge;
    Point pt_to_1;
    Point pt_to_2;
    edge = vector(poly[j], poly[i], edge);
    pt_to_1 = vector(pt, poly[i], pt_to_1);
    pt_to_2 = vector(poly[j], pt, pt_to_2);

    float r = dotProduct(edge, pt_to_1);

    r = r/((magnitude(edge)) * (magnitude(edge)));

    if (r<0){
      dist = magnitude(pt_to_1);
    }
    else if (r>1){
      dist = magnitude(pt_to_2);
    }
    else{
      dist = sqrt((magnitude(pt_to_1)*(magnitude(pt_to_1))) - ((r * magnitude(edge))*(r * magnitude(edge))));
    }
    
    if(dist<minDist){
      minDist = dist;
    }
    
  }
  return minDist;
}

float dotProduct(Point v1, Point v2){
  float dot = (v1.x*v2.x)+(v1.y*v2.y);
  
  return dot;
}

float magnitude(Point v){
  float mag = sqrt((v.x*v.x)+(v.y*v.y));
  
  return mag;
}

Point vector(Point v1, Point v2, Point v){
  v.x = v1.x-v2.x;
  v.y = v1.y-v2.y;
  
  return v;
}
