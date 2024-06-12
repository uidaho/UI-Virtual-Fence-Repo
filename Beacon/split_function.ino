struct StringList {
  int id;
  String element;
  StringList* next;
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  String alpha = "A, B, C, D, E";

  StringList alphaSplit = split(alpha, ',');
  //Serial.println(getListElement(alphaSplit, 2));
  //Serial.println(getListElement(alphaSplit, 7));
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

StringList split (String input, char splitChar) {
  int inputSize = input.length();
  StringList node;
  for(int i=0; i<inputSize; i++){
    if(input.charAt(i) == splitChar){
      node.element = input.substring(0,i);
      node.next = &split(input.substring(i+1), splitChar); //recusive call
      StringList follow = *node.next;
      node.id = follow.id + 1;
      return node;
    }
  }
  //base case
  //If the input doesn't contain the splitChar return the unmodified input and point to ourself
  node.id = 0;
  node.element = input;
  node.next = &node;
  return node;
}

String getListElement(StringList list, int p){

  String elem = "";
  Serial.println(list.id);
  Serial.println(list.element);
  delay(1000);
  if(list.id == p){
    return list.element;
  }
  else if(list.id == 0){
    return "Invalid Querry";
  }
  else{
    StringList nextEntry = *list.next;
    Serial.println(nextEntry.id);
    Serial.println(nextEntry.element);
    elem = getListElement(nextEntry, p);
  }
  return elem;
}
