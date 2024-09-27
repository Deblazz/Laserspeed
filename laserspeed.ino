#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FreqCount.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define INTERRUPT_PIN 3 // pin to receive the interrupt signal
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
#define RECEIVER_PIN 8
//#define TIMER_FREQ 1385

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

volatile unsigned long count = 0, startCount = 0, endCount = 0;
int theresLight = 0;
int cVal = 0;
int shot = 0;
float timer_freq;
float milliseconds, shutterSpeed;
void pulse(){
  count++;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(INTERRUPT_PIN, INPUT);
  pinMode(RECEIVER_PIN, INPUT);
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("ERROR> Screen didn't connect for some odd reason...");
    while (true);
  }

  splash();

  FreqCount.begin(1000);

  timer_freq = calibrate();


  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pulse, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:

  cVal = digitalRead(RECEIVER_PIN);
  //Serial.println(cVal);
  //float voltage = cVal * (5.0 / 1023.0);
  //printOLED_LOW(voltage);
  if(cVal && !theresLight){
    startCount = count;
    theresLight = 1;
  }else if(theresLight && !cVal){
    //Shutter has finished
    //noInterrupts();
    theresLight = 0;
    endCount = count;
    shot = 1;
    //Serial.println(count);
  }

  if(shot){
    noInterrupts();
    //Serial.println(startCount);
    //Serial.println(endCount);
    milliseconds = (endCount-startCount)/timer_freq;
    shutterSpeed = 1/milliseconds;
 /*    Serial.println((String)(endCount-startCount));
    Serial.println((String)(milliseconds)); */
    Serial.println("Shutter speed> 1/"+(String)(shutterSpeed) + "s");

    shot = 0;
    interrupts();
    printShutterSpeed(shutterSpeed);
  }
 
}

int calibrate(){
  float count;
  int nTimes = 0;
  for(int i = 0; i < 1000; i++){
    delay(10);
    if (FreqCount.available()) {
      nTimes++;
      count += FreqCount.read();
      Serial.println(count);
      Serial.println(nTimes);
    }
  }
    Serial.println("CHOSEN FREQ: " + (String)(count/nTimes)+ "Hz");
    viewChosenFrequency(count/nTimes);
    return count/nTimes;
}

void splash(){
    //Ripulisco il buffer
  display.clearDisplay();

  //Setto il colore del testo a "bianco"
  display.setTextColor( WHITE);

  //Setto dimensione del testo
  display.setTextSize(2);

  //Sposto il cursore a metà altezza del display
  display.setCursor(5, (SCREEN_HEIGHT / 2 )- 20);
    //Stampo una scritta
  display.println("LASERSPEED");

  display.drawLine(10, (SCREEN_HEIGHT / 2), SCREEN_WIDTH-10, (SCREEN_HEIGHT / 2), WHITE);

  display.setTextSize(1);
    //Sposto il cursore a metà altezza del display
  display.setCursor(5, SCREEN_HEIGHT -10);
    //Stampo una scritta
  display.println("v1.X  Calibrating...");


  //La mando in stampa
  display.display();

  delay(100);
}

void viewChosenFrequency(float frequency){
  display.clearDisplay();

  //Setto il colore del testo a "bianco"
  display.setTextColor( WHITE);

  //Setto dimensione del testo
  display.setTextSize(1);

  //Sposto il cursore a metà altezza del display
  display.setCursor(0, (SCREEN_HEIGHT / 2 )- 20);
    //Stampo una scritta
  display.println("Chosen frequency:");

  display.setTextSize(2);
    //Sposto il cursore a metà altezza del display
  display.setCursor(5, (SCREEN_HEIGHT / 2 ));
    //Stampo una scritta
  display.println((String)frequency+"Hz");


  //La mando in stampa
  display.display();

  delay(2000);

  display.clearDisplay();
  //Setto dimensione del testo
  display.setTextSize(2);

  //Sposto il cursore a metà altezza del display
  display.setCursor(0, (SCREEN_HEIGHT / 2 ));

  //Stampo una scritta
  display.println("SHOOT NOW!");

  display.display();

}

void printShutterSpeed(float shutterSpeed){
  display.clearDisplay();

  //Setto il colore del testo a "bianco"
  display.setTextColor( WHITE);

  //Setto dimensione del testo
  display.setTextSize(1);

  //Sposto il cursore a metà altezza del display
  display.setCursor(0, 0);
    //Stampo una scritta
  display.println("Measured speed:");

  display.setTextSize(2);
    //Sposto il cursore a metà altezza del display
  display.setCursor(5, (SCREEN_HEIGHT / 2 ));
    //Stampo una scritta
  display.println("1/"+(String)shutterSpeed+"s");

  display.display();

}
