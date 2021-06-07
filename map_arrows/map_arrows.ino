#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h>
#include <SPI.h>
#include <string.h>
#include <list>
#include <vector>
#include <ArduinoJson.h>

char network[] = "MIT";
char password[] = "";
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const uint16_t OUT_BUFFER_SIZE = 3000; //size of buffer to hold HTTP response
char host[] = "608dev-2.net";


const int BUTTON_UP = 13;
const int BUTTON_RIGHT = 26;
const int BUTTON_DOWN = 21;
const int BUTTON_LEFT = 5;

int left_reading; // current readings for left, right, down & up buttons
int right_reading;
int up_reading;
int down_reading;
int old_left_reading;  // old readings for left, right, down, & up buttons
int old_right_reading; // to see if their state changes (button pressed)
int old_up_reading;
int old_down_reading;
int current_score;
int prev_score;
char scoreDisplay[50];

int left_index;
int left_list_max;
int up_index;
int up_list_max;
int down_index;
int down_list_max;
int right_index;
int right_list_max;

TFT_eSPI tft = TFT_eSPI();
long timer_left;
long timer_up;
long timer_down;
long timer_right;
long start_time;

int note_speed = 5;

int left_arr[250];
std::vector<int> left_arrows_screen;

int down_arr[250];
std::vector<int> down_arrows_screen;

int up_arr[250];
std::vector<int> up_arrows_screen;

int right_arr[250];
std::vector<int> right_arrows_screen;

std::vector<int> right_list(right_arr, right_arr + sizeof(right_arr) / sizeof(int));
std::vector<int> left_list(left_arr, left_arr + sizeof(left_arr) / sizeof(int));
std::vector<int> up_list(up_arr, up_arr + sizeof(up_arr) / sizeof(int));
std::vector<int> down_list(down_arr, down_arr + sizeof(down_arr) / sizeof(int));

void setup() {
  Serial.begin(115200); //for debugging if needed.

  while (!Serial);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    //    Serial.print(n);
    //    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      //      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        //        Serial.print(*cc, HEX);
        //        if (k != 5) Serial.print(":");
        cc++;
      }
      //      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  WiFi.begin(network, password);

  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  left_index = 0;
  left_list_max = 0;
  up_index = 0;
  up_list_max = 0;
  down_index = 0;
  down_list_max = 0;
  right_index = 0;
  right_list_max = 0;

  getNotes("blammed");
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  current_score = 0;
  prev_score = 0;
  
  timer_left = millis();
  timer_up = millis();
  timer_down = millis();
  timer_right = millis();

  delay(2000);
  start_time = millis();
}

void loop() {
  tft.fillRect(0, 145, 140, 160, TFT_RED);

  tft.drawLine(0, 2, 140, 2, TFT_RED);
  tft.drawLine(0, 4, 140, 4, TFT_YELLOW);
  tft.drawLine(0, 6, 140, 6, TFT_GREEN);
  tft.drawLine(0, 8, 140, 8, TFT_BLUE);
  if (prev_score != current_score)
  {
    tft.fillRect(0, 10, 70, 40, TFT_BLACK);
    prev_score = current_score;
  }
  tft.setCursor(5, 10, 1.5);
  sprintf(scoreDisplay, "Score: %d", current_score);
  tft.println(scoreDisplay);
  hitDetection();
  beats();

}

void hitDetection()
{
  left_reading = digitalRead(BUTTON_LEFT);
  right_reading = digitalRead(BUTTON_RIGHT);
  up_reading = digitalRead(BUTTON_UP);
  down_reading = digitalRead(BUTTON_DOWN);

  if (!left_reading && old_left_reading)
  {
    if (left_arrows_screen.size() != 0)
    {
      current_score += left_arrows_screen[0];
      tft.fillTriangle(25, left_arrows_screen[0], 30, left_arrows_screen[1], 30, left_arrows_screen[2], TFT_BLACK);
      left_arrows_screen.erase (left_arrows_screen.begin(), left_arrows_screen.begin() + 3);
    }
    else
    {
      current_score -= 50;
    }
  }
  if (!down_reading && old_down_reading)
  {
    if (down_arrows_screen.size() != 0)
    {
      current_score += down_arrows_screen[0];
      tft.fillTriangle(50, down_arrows_screen[0], 45, down_arrows_screen[1], 55, down_arrows_screen[1], TFT_BLACK);
      down_arrows_screen.erase (down_arrows_screen.begin(), down_arrows_screen.begin() + 2);
    }
    else
    {
      current_score -= 50;
    }
  }
  if (!up_reading && old_up_reading)
  {
    if (up_arrows_screen.size() != 0)
    {
      current_score += up_arrows_screen[0];
      tft.fillTriangle(75, up_arrows_screen[0], 70, up_arrows_screen[1], 80, up_arrows_screen[1], TFT_BLACK);
      up_arrows_screen.erase (up_arrows_screen.begin(), up_arrows_screen.begin() + 2);
    }
    else
    {
      current_score -= 50;
    }
  }
  if (!right_reading && old_right_reading)
  {
    if (right_arrows_screen.size() != 0)
    {
      current_score += right_arrows_screen[0];
      tft.fillTriangle(100, right_arrows_screen[0], 95, right_arrows_screen[1], 95, right_arrows_screen[2], TFT_BLACK);
      right_arrows_screen.erase (right_arrows_screen.begin(), right_arrows_screen.begin() + 3);
    }
    else
    {
      current_score -= 50;
    }
  }
  old_right_reading = right_reading;
  old_left_reading = left_reading;
  old_up_reading = up_reading;
  old_down_reading = down_reading;
}

bool Contains(const std::vector<int> &list, int x)
{
  return std::find(list.begin(), list.end(), x) != list.end();
}

void beats() {
  if (left_index<left_list_max && left_list[left_index]<= millis() - start_time) {
    Serial.println("LEFT");
    left_index++;
    left_arrows_screen.push_back(10);
    left_arrows_screen.push_back(5);
    left_arrows_screen.push_back(15);

  } else {
    left_arrow();
  }

  if (down_index<down_list_max && down_list[down_index]<= millis() - start_time) {
    Serial.println("DOWN");
    down_index++;
    down_arrows_screen.push_back(15);
    down_arrows_screen.push_back(10);

  } else {
    down_arrow();
  }

  if (up_index<up_list_max && up_list[up_index]<= millis() - start_time) {
    Serial.println("UP");
    up_index++;
    up_arrows_screen.push_back(5);
    up_arrows_screen.push_back(10);

  } else {
    up_arrow();
  }

  if (right_index<right_list_max && right_list[right_index]<= millis() - start_time) {
    Serial.println("RIGHT");
    right_index++;
    right_arrows_screen.push_back(10);
    right_arrows_screen.push_back(5);
    right_arrows_screen.push_back(15);

  } else {
    right_arrow();
  }
}

void left_arrow() {
  if (millis() - timer_left > 25) {

    for (int i = 0; i < left_arrows_screen.size(); i = i + 3) {

      if (left_arrows_screen[i + 2] > 150) {
        left_arrows_screen.erase (left_arrows_screen.begin(), left_arrows_screen.begin() + 3);
        current_score -= 50;
      } else {
        left_arrows_screen[i] = left_arrows_screen[i] + 5;
        left_arrows_screen[i + 1] = left_arrows_screen[i + 1] + 5;
        left_arrows_screen[i + 2] = left_arrows_screen[i + 2] + 5;
      }
    }

    for (int i = 0; i < left_arrows_screen.size(); i = i + 3) {
      tft.fillTriangle(25, left_arrows_screen[i] - 5, 30, left_arrows_screen[i + 1] - 5, 30, left_arrows_screen[i + 2] - 5, TFT_BLACK);
      tft.fillTriangle(25, left_arrows_screen[i], 30, left_arrows_screen[i + 1], 30, left_arrows_screen[i + 2], TFT_BLUE);
    }

    timer_left = millis();
  }
}

void down_arrow() {
  if (millis() - timer_down > 25) {

    for (int i = 0; i < down_arrows_screen.size(); i = i + 2) {

      if (down_arrows_screen[i] > 150) {
        down_arrows_screen.erase (down_arrows_screen.begin(), down_arrows_screen.begin() + 2);
        current_score -= 50;
      } else {
        down_arrows_screen[i] = down_arrows_screen[i] + 5;
        down_arrows_screen[i + 1] = down_arrows_screen[i + 1] + 5;
      }
    }


    for (int i = 0; i < down_arrows_screen.size(); i = i + 2) {
      tft.fillTriangle(50, down_arrows_screen[i] - 5, 45, down_arrows_screen[i + 1] - 5, 55, down_arrows_screen[i + 1] - 5, TFT_BLACK);
      tft.fillTriangle(50, down_arrows_screen[i], 45, down_arrows_screen[i + 1], 55, down_arrows_screen[i + 1], TFT_BLUE);
    }

    timer_down = millis();
  }
}

void up_arrow() {
  if (millis() - timer_up > 25) {

    for (int i = 0; i < up_arrows_screen.size(); i = i + 2) {

      if (up_arrows_screen[i + 1] > 150) {
        up_arrows_screen.erase (up_arrows_screen.begin(), up_arrows_screen.begin() + 2);
        current_score -= 50;
      } else {
        up_arrows_screen[i] = up_arrows_screen[i] + 5;
        up_arrows_screen[i + 1] = up_arrows_screen[i + 1] + 5;
      }
    }

    for (int i = 0; i < up_arrows_screen.size(); i = i + 2) {
      tft.fillTriangle(75, up_arrows_screen[i] - 5, 70, up_arrows_screen[i + 1] - 5, 80, up_arrows_screen[i + 1] - 5, TFT_BLACK);
      tft.fillTriangle(75, up_arrows_screen[i], 70, up_arrows_screen[i + 1], 80, up_arrows_screen[i + 1], TFT_BLUE);
    }

    timer_up = millis();
  }
}

void right_arrow() {
  if (millis() - timer_right > 25) {

    for (int i = 0; i < right_arrows_screen.size(); i = i + 3) {

      if (right_arrows_screen[i + 2] > 150) {
        right_arrows_screen.erase (right_arrows_screen.begin(), right_arrows_screen.begin() + 3);
        current_score -= 50;
      } else {
        right_arrows_screen[i] = right_arrows_screen[i] + 5;
        right_arrows_screen[i + 1] = right_arrows_screen[i + 1] + 5;
        right_arrows_screen[i + 2] = right_arrows_screen[i + 2] + 5;
      }
    }

    for (int i = 0; i < right_arrows_screen.size(); i = i + 3) {
      tft.fillTriangle(100, right_arrows_screen[i] - 5, 95, right_arrows_screen[i + 1] - 5, 95, right_arrows_screen[i + 2] - 5, TFT_BLACK);
      tft.fillTriangle(100, right_arrows_screen[i], 95, right_arrows_screen[i + 1], 95, right_arrows_screen[i + 2], TFT_BLUE);
    }

    timer_right = millis();
  }
}
char note_output[4000];
StaticJsonDocument<4000> noteset;
void getNotes(char* songName) {
  char request_buffer[300];
  JsonArray notearr;
  int i = 0;
  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=%s&note=0 HTTP/1.1\r\n", songName);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body
  do_http_request(host, request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);

  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for (JsonVariant v : notearr)
  {
    left_list[i] = v.as<int>();
    i++;
    left_list_max++;
  }
  noteset.clear();
  memset(request_buffer, 0, sizeof(request_buffer));

  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=%s&note=1 HTTP/1.1\r\n", songName);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);

  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for (JsonVariant v : notearr)
  {
    down_list[i] = v.as<int>();
    i++;
    down_list_max++;
  }
  noteset.clear();
  memset(request_buffer, 0, sizeof(request_buffer));

  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=%s&note=2 HTTP/1.1\r\n", songName);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);

  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for (JsonVariant v : notearr)
  {
    up_list[i] = v.as<int>();
    i++;
    up_list_max++;
  }
  noteset.clear();
  memset(request_buffer, 0, sizeof(request_buffer));

  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=%s&note=3 HTTP/1.1\r\n", songName);
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);

  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for (JsonVariant v : notearr)
  {
    right_list[i] = v.as<int>();
    i++;
    right_list_max++;
  }
  noteset.clear();
  memset(request_buffer, 0, sizeof(request_buffer));
}

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
