struct Point {
  int x, y;
};

struct Line {
  Point p1, p2;
};

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  Point polygon[] = { { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } };
  Point p[] = {{ 5, 3 },{11,11},{-5,-5},{10,10}};
  int n = 4;
 
  // Function call
  for(int i = 0; i<4; i++){
    if (checkInside(polygon, n, p[i]))
      Serial.println("Point is inside.");
    else
      Serial.println("Point is outside.");
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}

bool onLine(Line l1, Point p)
{
    // Check whether p is on the line or not
    if (p.x <= max(l1.p1.x, l1.p2.x)
        && p.x >= min(l1.p1.x, l1.p2.x)
        && (p.y <= max(l1.p1.y, l1.p2.y)
            && p.y >= min(l1.p1.y, l1.p2.y)))
        return true;
 
    return false;
}

int direct(Point a, Point b, Point c)
{
    int val = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
 
    if (val == 0)
 
        // Collinear
        return 0;
 
    else if (val < 0)
 
        // Anti-clockwise direction
        return 2;
 
    // Clockwise direction
    return 1;
}

bool isIntersect(Line l1, Line l2)
{
    // Four direction for two lines and points of other line
    int dir1 = direct(l1.p1, l1.p2, l2.p1);
    int dir2 = direct(l1.p1, l1.p2, l2.p2);
    int dir3 = direct(l2.p1, l2.p2, l1.p1);
    int dir4 = direct(l2.p1, l2.p2, l1.p2);
 
    // When intersecting
    if (dir1 != dir2 && dir3 != dir4)
        return true;
 
    // When p2 of line2 are on the line1
    if (dir1 == 0 && onLine(l1, l2.p1))
        return true;
 
    // When p1 of line2 are on the line1
    if (dir2 == 0 && onLine(l1, l2.p2))
        return true;
 
    // When p2 of line1 are on the line2
    if (dir3 == 0 && onLine(l2, l1.p1))
        return true;
 
    // When p1 of line1 are on the line2
    if (dir4 == 0 && onLine(l2, l1.p2))
        return true;
 
    return false;
}

bool checkInside(Point poly[], int n, Point p)
{
 
    // When polygon has less than 3 edge, it is not polygon
    if (n < 3)
        return false;
 
    // Create a point at infinity, y is same as point p
    Line exline = { p, { 9999, p.y } };
    int count = 0;
    int i = 0;
    do {
 
        // Forming a line from two consecutive points of
        // poly
        Line side = { poly[i], poly[(i + 1) % n] };
        if (isIntersect(side, exline)) {
            // If side is intersects exline
            if (direct(side.p1, p, side.p2) == 0){
                return onLine(side, p);
            }
            count++;
        }
        i = (i + 1) % n;
    } while (i != 0);
 
    // When count is odd
    return count & 1;
}
