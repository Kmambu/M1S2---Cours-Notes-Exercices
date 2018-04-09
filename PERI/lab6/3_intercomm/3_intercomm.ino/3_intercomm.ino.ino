#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

struct mailbox {
  enum {EMPTY, FULL} state;
  unsigned long val;
} mb0 = {.state = mb0.EMPTY};

#define MAX_WAIT_FOR_TIMER 5
unsigned int waitFor(int timer, unsigned long period){
  static unsigned long waitForTimer[MAX_WAIT_FOR_TIMER];
  unsigned long newTime = micros() / period;              // numéro de la période modulo 2^32 
  int delta = newTime - waitForTimer[timer];              // delta entre la période courante et celle enregistrée
  if ( delta < 0 ) delta += 1 + (0xFFFFFFFF / period);    // en cas de dépassement du nombre de périodes possibles sur 2^32 
  if ( delta ) waitForTimer[timer] = newTime;             // enregistrement du nouveau numéro de période
  return delta;
}

//--------- définition de la tache Led

struct Led_st {
  int timer;                                              // numéro de timer utilisé par WaitFor
  unsigned long period;                                             // periode de clignotement
  int pin;                                                // numéro de la broche sur laquelle est la LED
  int etat;                                               // etat interne de la led
}; 

void setup_Led( struct Led_st * ctx, int timer, unsigned long period, byte pin) {
  ctx->timer = timer;
  ctx->period = period;
  ctx->pin = pin;
  ctx->etat = 0;
  pinMode(pin,OUTPUT);
  digitalWrite(pin, ctx->etat);
}

void loop_Led(struct Led_st * ctx) {
  if (!waitFor(ctx->timer, ctx->period)) return;          // sort s'il y a moins d'une période écoulée
  if (mb0.state == mb0.FULL) {
    ctx->period = mb0.val*100;
    mb0.state = mb0.EMPTY;
  }
  digitalWrite(ctx->pin,ctx->etat);                       // ecriture
  ctx->etat = 1 - ctx->etat;                              // changement d'état
}

//------------ definition de la tache Photo
struct Photo_st
{
  int timer;
  unsigned long period;
};

void setup_Photo(struct Photo_st *ctx, int timer, unsigned long period)
{
  ctx->timer = timer;
  ctx->period = period;
}

void loop_Photo(struct Photo_st *ctx)
{
  if (!waitFor(ctx->timer, ctx->period)) return;
  if(mb0.state == mb0.EMPTY) {
    mb0.val = analogRead(15);
    mb0.state = mb0.FULL;
  }
}

//------------- définition
struct Oled_st {
  int timer;
  unsigned long period;
  int cpt;
};

void setup_Oled(struct Oled_st *ctx, int timer, unsigned long period)
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  ctx->timer = timer;
  ctx->period = period;
  ctx->cpt = 0;
}

void loop_Oled(struct Oled_st *ctx)
{
  if(!waitFor(ctx->timer,ctx->period)) return;
  ctx->cpt += 1;
  display.setCursor(0,0);
  display.println(ctx->cpt);
  display.display();
  display.clearDisplay();
}

//--------- definition de la tache Mess

struct Mess_st {
  int timer;                                              // numéro de timer utilisé par WaitFor
  unsigned long period;                                             // periode d'affichage
  char mess[20];
} Mess_t ; 

void setup_Mess(struct Mess_st * ctx, int timer, unsigned long period, const char * mess) {
  ctx->timer = timer;
  ctx->period = period;
  strcpy(ctx->mess, mess);
  Serial.begin(9600);                                     // initialisation du débit de la liaison série
}

void loop_Mess(struct Mess_st *ctx) {
  if (!(waitFor(ctx->timer,ctx->period))) return;         // sort s'il y a moins d'une période écoulée
  Serial.println(ctx->mess);                              // affichage du message
}

//--------- Déclaration des tâches

struct Led_st Led1;
struct Mess_st Mess1, Mess2;
struct Oled_st Oled;
struct Photo_st Photo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  setup_Led(&Led1, 0, 100000, 13);                        // Led est exécutée toutes les 100ms 
  setup_Mess(&Mess1, 1, 1000000, "au revoir");              // Mess est exécutée toutes les secondes 
  setup_Mess(&Mess2, 2, 1500000, "merci");              // Mess est exécutée toutes les secondes 
  setup_Oled(&Oled, 3, 1000000);
  setup_Photo(&Photo, 5, 500000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  loop_Led(&Led1);                                        
  loop_Mess(&Mess1); 
  loop_Mess(&Mess2);
  loop_Oled(&Oled); 
  loop_Photo(&Photo);
}
