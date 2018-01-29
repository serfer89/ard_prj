int led_1 = 8;
int led_2 = 2;
int but_1 = 3;
int but_2 = 4;
int t;
int but1_state = LOW;
int but2_state = LOW;


void setup() {
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(but_1, INPUT);
  pinMode(but_2, INPUT);
  Serial.begin(9600);
  Serial.println("Booting...");

}

void loop() {

    if (Serial.available() > 0) {
    // read the incoming byte:
                t = Serial.read();
        t = t-48;
                // say what you got:
                Serial.print("I received: ");
                Serial.println(t, DEC);
            if (t == 1) {digitalWrite(led_1, HIGH); but1_state = LOW; Serial.println("serial - HIGH");}  
        if (t == 0) {digitalWrite(led_1, LOW); but1_state = HIGH; Serial.println("serial - LOW");}
          if (t == 2) {digitalWrite(led_2, HIGH); but2_state = LOW; Serial.println("seria2 - HIGH");}
          if (t == 3) {digitalWrite(led_2, LOW); but2_state = HIGH; Serial.println("seria2 - LOW");}  
        }  
  
  if (but1_state != digitalRead(but_1)) 
  {
    Serial.println(but1_state);
    Serial.println(digitalRead(but_1));
   
  if (digitalRead(but_1) == HIGH )
  {digitalWrite(led_1, HIGH); but1_state = HIGH; Serial.println("btn 1 - HIGH");}
  
  if (digitalRead(but_1) == LOW )
  {digitalWrite(led_1, LOW); but1_state = LOW; Serial.println("btn 1 - LOW");}
  }
  
  
    if (but2_state != digitalRead(but_2)) 
  {
    Serial.println(but2_state);
    Serial.println(digitalRead(but_2));
   
  if (digitalRead(but_2) == HIGH )
  {digitalWrite(led_2, HIGH); but2_state = HIGH; Serial.println("btn 2 - HIGH");}
  
  if (digitalRead(but_2) == LOW )
  {digitalWrite(led_2, LOW); but2_state = LOW; Serial.println("btn 2 - LOW");}
  }

}
