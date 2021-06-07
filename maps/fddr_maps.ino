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

//const size_t CAPACITY = JSON_ARRAY_SIZE(250);
TFT_eSPI tft = TFT_eSPI();
long timer_left;
long timer_up;
long timer_down;
long timer_right;

int note_speed = 5;

int old_y1_left = 10;
int old_y2_left = 5;
int old_y3_left = 15;

int old_y1_up = 5;
int old_y2_up = 10;

int old_y1_down = 15;
int old_y2_down = 10;

int old_y1_right = 10;
int old_y2_right = 5;
int old_y3_right = 15;

int left_arr[250];


int down_arr[250];

int up_arr[250];

int right_arr[250];

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
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(network, password);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);


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
  
  getNotes();
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background

  timer_left = millis();
  timer_up = millis();
  timer_down = millis();
  timer_right = millis();
  delay(2000);
}

void loop() {
  beats();
}

bool Contains(const std::vector<int> &list, int x)
{
  return std::find(list.begin(), list.end(), x) != list.end();
}

void beats() {
  if (Contains(left_list, millis())) {
    old_y1_left = 10;
    old_y2_left = 5;
    old_y3_left = 15;
  } else {
    left_arrow();
  }

  if (Contains(down_list, millis())) {
    old_y1_down = 15;
    old_y2_down = 10;
  } else {
    down_arrow();
  }

  if (Contains(up_list, millis())) {
    old_y1_up = 5;
    old_y2_up = 10;
  } else {
    up_arrow();
  }

  if (Contains(right_list, millis())) {
    old_y1_right = 10;
    old_y2_right = 5;
    old_y3_right = 15;
  } else {
    right_arrow();
  }  
}

void left_arrow() {
  if (millis() - timer_left > 25) {
    int curr_y1_left = old_y1_left + 5;
    int curr_y2_left = old_y2_left + 5;
    int curr_y3_left = old_y3_left + 5;

    tft.fillTriangle(25, old_y1_left, 30, old_y2_left, 30, old_y3_left, TFT_BLACK);
    tft.fillTriangle(25, curr_y1_left, 30, curr_y2_left, 30, curr_y3_left, TFT_BLUE);
    old_y1_left = curr_y1_left;
    old_y2_left = curr_y2_left;
    old_y3_left = curr_y3_left;
    timer_left = millis();
  }
}

void down_arrow() {
  if (millis() - timer_down > 25) {
    int curr_y1_down = old_y1_down + 5;
    int curr_y2_down = old_y2_down + 5;

    tft.fillTriangle(50, old_y1_down, 45, old_y2_down, 55, old_y2_down, TFT_BLACK);
    tft.fillTriangle(50, curr_y1_down, 45, curr_y2_down, 55, curr_y2_down, TFT_BLUE);
    old_y1_down = curr_y1_down;
    old_y2_down = curr_y2_down;
    timer_down = millis();
  }
}

void up_arrow() {
  if (millis() - timer_up > 25) {
    int curr_y1_up = old_y1_up + 5;
    int curr_y2_up = old_y2_up + 5;

    tft.fillTriangle(75, old_y1_up, 70, old_y2_up, 80, old_y2_up, TFT_BLACK);
    tft.fillTriangle(75, curr_y1_up, 70, curr_y2_up, 80, curr_y2_up, TFT_BLUE);
    old_y1_up = curr_y1_up;
    old_y2_up = curr_y2_up;
    timer_up = millis();
  }
}

void right_arrow() {
  if (millis() - timer_right > 25) {
    int curr_y1_right = old_y1_right + 5;
    int curr_y2_right = old_y2_right + 5;
    int curr_y3_right = old_y3_right + 5;

    tft.fillTriangle(100, old_y1_right, 95, old_y2_right, 95, old_y3_right, TFT_BLACK);
    tft.fillTriangle(100, curr_y1_right, 95, curr_y2_right, 95, curr_y3_right, TFT_BLUE);
    old_y1_right = curr_y1_right;
    old_y2_right = curr_y2_right;
    old_y3_right = curr_y3_right;
    timer_right = millis();
  }
}





void getNotes() {
  char note_output[3000];
  char request_buffer[300];
  StaticJsonDocument<3000> noteset;
  JsonArray notearr;
  int i = 0;
  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=blammed&note=0 HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body
  do_http_request(host, request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);
//  delay(2000);
  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for(JsonVariant v : notearr) 
  {
//    int* val = &left_arr[i];
//    *val = v.as<int>();
    left_list[i] = v.as<int>();
//    Serial.print(*val);
    i++;
  }
  noteset.clear();
  memset(request_buffer, 0, sizeof(request_buffer));


  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=blammed&note=1 HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);

  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for(JsonVariant v : notearr) 
  {
//    int* val = &down_arr[i];
    down_list[i] = v.as<int>();
    i++;
  }
  noteset.clear();
  memset(request_buffer, 0, sizeof(request_buffer));


  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=blammed&note=2 HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);

  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for(JsonVariant v : notearr) 
  {
//    int* val = &up_arr[i];
    up_list[i] = v.as<int>();
    i++;
  }
  noteset.clear();
  memset(request_buffer, 0, sizeof(request_buffer));


  sprintf(request_buffer, "GET /sandbox/sc/team43/fddr/get_notes.py?song=blammed&note=3 HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n"); //new line from header to body

  do_http_request("608dev-2.net", request_buffer, note_output, sizeof(note_output), RESPONSE_TIMEOUT, true);

  deserializeJson(noteset, note_output);
  notearr = noteset.as<JsonArray>();
  i = 0;
  for(JsonVariant v : notearr) 
  {
//    int* val = &right_arr[i];
    right_list[i] = v.as<int>();
    i++;
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
