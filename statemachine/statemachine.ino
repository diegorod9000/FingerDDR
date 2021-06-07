#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include<math.h>
#include <ArduinoJson.h>
#include<string.h>
#include <arduinoFFT.h>

#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F   
#define BROWN       0x9A60      /* 150,  75,   0 */
#define GOLD        0xFEA0      /* 255, 215,   0 */
#define SILVER      0xC618      /* 192, 192, 192 */
#define SKYBLUE     0x867D      /* 135, 206, 235 */
#define VIOLET      0x915C      /* 180,  46, 226 */
uint16_t colors[] = {RED, ORANGE, GOLD, YELLOW, GREENYELLOW, GREEN, CYAN, SKYBLUE, DARKCYAN,
                      BLUE, NAVY, PURPLE, VIOLET, MAROON, MAGENTA, PINK};

TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;

// * we'll probably have to coordinate with this so 
// that everyone has the same buttons set up *
const int BUTTON_UP = 13;
const int BUTTON_RIGHT = 26;
const int BUTTON_DOWN = 21;
const int BUTTON_LEFT = 5;
const int JOYSTICK_X = 33; // ** X & Y need to be ADC1 channels, 
const int JOYSTICK_Y = 32; // which include pins 32-39 **
const int JOYSTICK_MAX = 4095; // max value of the x or y on the joystick reading
const int LOOP_PERIOD = 40;

int left_reading; // current readings for left, right, down & up buttons
int right_reading;
int up_reading;
int down_reading;
int x_reading;
int y_reading;

int old_left_reading;  // old readings for left, right, down, & up buttons
int old_right_reading; // to see if their state changes (button pressed)
int old_up_reading; 
int old_down_reading; 
int old_x_reading;
int old_y_reading;

long timer_left;
long timer_up;
long timer_down;
long timer_right;

const int MENU = 0;
const int LEADERBOARD = 1;
const int CHOOSE_SONG = 2;
const int ENTER_NAME = 3;
const int PLAY_SONG = 4;
const int PLAYING = 5;
const int FINISHED = 6;
uint8_t page_state;

int indx = 0;
int base_index = 0;
bool from_menu = false; // this is for checking if we got to the leaderboard from
                        // the menu or after the song, since we don't want to be
                        // able to press the right button if we came from the menu
                        
bool from_lboard = false; // this is for checking if we started the round from the
                          // leaderboard or from the menu, since we don't want to have
                          // to enter the name again if we came from the leaderboard

char* songs[] = {"SUNFLOWER", "ALL STAR", "KUNG FU FIGHTING", "RA RA RASPUTIN", "CASTAWAYS", "ROCKETMAN", "TECHNO"};
char* song_posts[] = {"sunflower", "allstar", "kungfufighting", "rararasputin", "castaways", "rocketman", "techno"};
const uint8_t num_songs = 7; 
int song_index;
int old_song_index;
char* song_choice = "";

int name_index;
int old_name_index;
char username[100];
char chosen_letter[2];
char* alphabet[] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
int alphabet_index;

int current_score;
int prev_score;
char scoreDisplay[50];

char lboard1[25];
char lboard2[25];
char lboard3[25];
char lboard4[25];
char lboard5[25];
char lboard6[25];
char lboard7[25];
char lboard8[25];


char network[] = "MIT";  
char password[] = "";
const int UPDATE_DELAY = 10000;
const int RESPONSE_SIZE = 100;
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request

arduinoFFT FFT = arduinoFFT();
const int SAMPLES = 128;
const int SAMPLING_FREQUENCY = 2048;
unsigned long microSeconds;
unsigned long samplingPeriod = round(1000000 * (1.0/SAMPLING_FREQUENCY)); // period in microseconds
double vReal[SAMPLES]; // vector to hold real values
double vImag[SAMPLES]; // vector to hold imaginary values

int sensorPin = A0;          //pin number to use the ADC
int sensorValue = 0;        //initialization of sensor variable, equivalent to EMA Y

const int NO_BEEP = 0;
const int BEEPING = 1;
const int GO = 2;
int stateband;
int num_beeps;
uint32_t last_beep;

bool player_ready = false;
long song_timer;
long waiting_timer;

void setup() {
  Serial.begin(115200);
  WiFi.begin(network, password); // connect to wifi
  uint8_t count = 0; 
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(500);
  if (WiFi.isConnected()) { 
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());    delay(500);
  } else { 
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); 
  }
  
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  stateband = NO_BEEP;
  last_beep = -100; 

  current_score = 0;
  prev_score = 0;
  timer_left = millis();
  timer_up = millis();
  timer_down = millis();
  timer_right = millis();
  delay(2000);

  tft.init(); 
  tft.setRotation(2);
  tft.setTextSize(1); 
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_PURPLE, TFT_BLACK);
  page_state = MENU;
  set_screen(MENU);
}

void loop() {
  
  left_reading = digitalRead(BUTTON_LEFT);
  right_reading = digitalRead(BUTTON_RIGHT);
  up_reading = digitalRead(BUTTON_UP);
  down_reading = digitalRead(BUTTON_DOWN);
  x_reading = analogRead(JOYSTICK_X);
  y_reading = analogRead(JOYSTICK_Y);

  update_state();
  
  // update all the button/joystick readings
  old_right_reading = right_reading;
  old_left_reading = left_reading;
  old_up_reading = up_reading;
  old_down_reading = down_reading;
  old_x_reading = x_reading;
  old_y_reading = y_reading;
}
