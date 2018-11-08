//**********************************************************************************************
//**********************************************************************************************
//*****                                                                                    *****
//*****  UNOino sterownik LEDów do projektu Leos - koło na ścianę                          *****
//*****                                                                                    *****
//*****  UNI-Tral 2018.09.04  białe migające przy tęczy                                    *****
//*****                                                                                    *****
//*****  DLA: Andrzej Bielsko                                                              *****
//*****                                                                                    *****
//**********************************************************************************************
//**********************************************************************************************

#include <Adafruit_NeoPixel.h>
#include <Timers.h>

#define PIXEL_PIN    5    //numer nóżki

#define PIERSCIEN    72
#define BIALE        20
#define WAZ          92         // 72 pierścień + 20 białe
//#define PIERSCIEN    30
//#define BIALE        7
//#define WAZ          37   // 32 pierścień + 8 białe  //całkowita liczba ledów; idealnie gdyby była podzielna przez 6 by sekcje kolorów były jednakowej długości


#define L_SEKCJI     6
#define WAIT         45   // regulacja predkości obrotu kolorów
#define WAITRAINBOW  60   // regulacja predkości zmiany tęczy

Adafruit_NeoPixel strip = Adafruit_NeoPixel(WAZ, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

Timer timer1, timerBiale;

//int i;     // na potrzeby pętli
int podzielnik;
float j;   // wartość pomocnicza dla jasność
int jasnosc;
int przesun2kol;
int przesun6kol;
int dlugoscSekcji;

byte r1;
byte g1;
byte b1;
byte r2;
byte g2;
byte b2;

//kolory ustawiać w części loop() - ładowanie do tabeli poprzez tabelkaK()

bool jestAdres;  // TRUE => wykryty adres
bool jestKolor;  // TRUE => pobrano numer koloru loda
bool jestBialy;  // TRUE => biale włączone
String adres;
char buf[1];     // bufor dla serial
char znak;
char smakLoda;
char tryb;

//****************************************************

// Kc - wybrano smak czekoladowy
// Ks - śmietankowy
// Kt - truskawkowy
// Kg - guma balonowa
// Kx - powrot do trybu 1 - płynące kolory

void setup() 
{
  adres = "K";       // adres, czyli "K"
  smakLoda = 'x';    // opis liter komunikatu 5 linii wyżej
  dlugoscSekcji = PIERSCIEN/L_SEKCJI;
  przesun2kol = -PIERSCIEN/L_SEKCJI;
  przesun6kol = -6*PIERSCIEN/L_SEKCJI;
  jasnosc =255;
  j=float(jasnosc)/255;
  //j=1;
  
  timerBiale.begin(300); // ustawienie szybkości migania białych
  
  Serial.begin(1200);
  //Serial.print(j);
  strip.begin();
  jestBialy=true;
  biale_on();
  //strip.show();
  
  tryb = 1;          // tryb 1 - tryb domyslny (tęcza), tryb 2 - wybrano smak loda
  
}

//****************************************************

void loop() 
{
   //Serial.print(j);
   
   if (jestAdres && jestKolor)
   {
      ustawTryb();      // tryb 1 - tryb domyslny kolorowe paski/tęcza, tryb 2 - wybrano kolor loda
      jestAdres=0;
      jestKolor=0;      

      switch (smakLoda) //załadowanie kolorów !!! tu ustawiać kolory !!!
      {
      case 'c':
         ustawKolor(106, 
                  8,
                  0,
                  95,
                  24,
                  0);
         break;
      case 's':
         ustawKolor(50, //ciemny
                  26,
                  0,
                  82, //jasny
                  49,
                  13);
         break;
      case 't':
         ustawKolor(255, // czerw
                  0,
                  0,
                  235, // lila
                  0,
                  25);
         break;
      case 'g':
         ustawKolor(255, // filolet
                  0,
                  100,
                  100, // nieb
                  0,
                  255); 
         break;
      // case 'x':
         // rainbow6col(WAITRAINBOW);
         // break;                 
      }
   }

   if (tryb==1)
   {
      //biale_on();
      wyswietl6kol(przesun6kol);
      
      if (przesun6kol<0)
        przesun6kol++;
      else
        przesun6kol=-6*PIERSCIEN/L_SEKCJI+1;

      podzielnik++;	//obsluga migania podswietlenia kubka
      if (podzielnik==1)
        biale_on();
      if (podzielnik==PIERSCIEN)
        biale_off();
      if (podzielnik==2*PIERSCIEN)
        podzielnik=0; 	

      timer1.begin(WAITRAINBOW);
      while (!timer1.available() && !(jestAdres&&jestKolor))
         checkSerial();
   }

   if (tryb==2)
   {
      wyswietl2kol(przesun2kol);
      
      if (przesun2kol<0)
         przesun2kol++;
      else
         przesun2kol=-2*PIERSCIEN/L_SEKCJI+1;

      // if (timerBiale.available()) {
         
         // timerBiale.restart();
         
         // if (jestBialy) {
             // biale_off();
             // jestBialy = false;
         // }
         // else {
             // biale_on();          
             // jestBialy = true;
         // }
      // }
      
      timer1.begin(WAIT);
      while (!timer1.available() && !(jestAdres&&jestKolor))
         checkSerial();
   }
}

//****************************************************
//****************************************************
//****************************************************

void checkSerial()
{
  while (Serial.available() && !jestKolor)     // czytam dane i sprawdzam czy jest rozkaz dla mnie
    {
    Serial.readBytes(buf,1);
    znak=buf[0];
    if (znak==adres[0])
      {
      jestAdres=1;
      }
    else
      {
      if (jestAdres && (char(znak)=='c' || char(znak)=='s' || char(znak)=='t' || char(znak)=='g' || char(znak)=='x'))
        {
        smakLoda=znak;
        jestKolor=1;
        }
      else
        {
        jestAdres=0;
        jestKolor=0;
        }
      }
    }
}

//****************************************************

void ustawTryb()
{
   if (smakLoda=='x')
      tryb = 1;
   else
      tryb = 2;
}

//****************************************************

void ustawKolor (byte pr1, byte pg1, byte pb1, byte pr2, byte pg2, byte pb2) 

{
    r1=pr1*j;
    g1=pg1*j;
    b1=pb1*j;
    r2=pr2*j;
    g2=pg2*j;
    b2=pb2*j;
}

//****************************************************

void wyswietl6kol(int przesPocz)  // wyświetlaj tęczę
{
   int i, n, nrSekcji;  // nrSekcji - przesuwanie o kolejną sekcję
   byte nrKoloru=1;     // nrKoloru
   
   for(nrSekcji=0; nrSekcji<=L_SEKCJI+5; nrSekcji++)
   {
      for (i=0; i<PIERSCIEN/L_SEKCJI; i++)
      {
         n = i+nrSekcji*dlugoscSekcji+przesPocz;
         if (n>=0 && n<PIERSCIEN) // wyświetlamy diody tylko z zakresu [0..PIERSCIEN)
              switch (nrKoloru) //załadowanie kolorów !!! tu ustawiać kolory !!!
              {
              case 1:
                 strip.setPixelColor(n, strip.Color(255*j,0,0)); 
                 break;
              case 2:
                 strip.setPixelColor(n, strip.Color(255*j,255*j,0)); 
                 break;
              case 3:
                 strip.setPixelColor(n, strip.Color(0,255*j,0)); 
                 break;
              case 4:
                 strip.setPixelColor(n, strip.Color(0,255*j,255*j)); 
                 break;
              case 5:
                 strip.setPixelColor(n, strip.Color(0,0,255*j)); 
                 break;
              case 6:
                 strip.setPixelColor(n, strip.Color(255*j,0,255*j)); 
                 break;
              }
      }
      
      if (nrKoloru==6)
         nrKoloru=1;
      else
         nrKoloru++;
   }
   strip.show();
}


//****************************************************

void wyswietl2kol (int przesPocz)
{
    
   int i, n, nrSekcji;  // nrSekcji - przesuwanie o kolejną sekcję
   bool parzysta=false; // czy sekcja jest parzysta
  
   for(nrSekcji=0; nrSekcji<=L_SEKCJI+3; nrSekcji++)
   {
      for (i=0; i<PIERSCIEN/L_SEKCJI; i++)
      {
         n = i+nrSekcji*dlugoscSekcji+przesPocz;
         if (n>=0 && n<PIERSCIEN) // wyświetlamy diody z zakresu [0..PIERSCIEN]
            if (!parzysta)  // nieparzysta sekcja
               strip.setPixelColor(n, strip.Color(r1,g1,b1)); 
            else            // parzysta sekcja
               strip.setPixelColor(n, strip.Color(r2,g2,b2)); 
      }
      if (parzysta)
         parzysta = false;
      else
         parzysta = true;

   }
   strip.show();
}


//****************************************************

void biale_on()
{
  int i;
  for(i=PIERSCIEN; i<=PIERSCIEN+BIALE; i++)
    strip.setPixelColor(i, strip.Color(255*j,255*j,255*j)); 
}

//****************************************************

void biale_off()
{
  int i;
  for(i=PIERSCIEN; i<=PIERSCIEN+BIALE; i++)
    strip.setPixelColor(i, strip.Color(0,0,0)); 
}

//****************************************************

//****************************************************
//****************************************************
//****************************************************


