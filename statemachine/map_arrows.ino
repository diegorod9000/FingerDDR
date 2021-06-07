#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h>
#include <SPI.h>
#include <string.h>
#include <list>
#include <vector>
#include <ArduinoJson.h>

char host[] = "608dev-2.net";
int note_speed = 5;
long start_time = millis();

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

int left_index;
int left_list_max;
int up_index;
int up_list_max;
int down_index;
int down_list_max;
int right_index;
int right_list_max;

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
      down_arrows_screen.erase(down_arrows_screen.begin(), down_arrows_screen.begin() + 2);
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
      up_arrows_screen.erase(up_arrows_screen.begin(), up_arrows_screen.begin() + 2);
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
      right_arrows_screen.erase(right_arrows_screen.begin(), right_arrows_screen.begin() + 3);
    }
    else
    {
      current_score -= 50;
    }
  }
  
  if (current_score < 0){ 
    current_score = 0;
  }

  old_right_reading = right_reading;
  old_left_reading = left_reading;
  old_up_reading = up_reading;
  old_down_reading = down_reading;
}

void beats() {
  
  if (left_index<left_list_max && left_list[left_index]<= millis() - start_time) {
    left_index++;
    left_arrows_screen.push_back(10);
    left_arrows_screen.push_back(5);
    left_arrows_screen.push_back(15);

  } else {
    left_arrow();
  }

  if (down_index<down_list_max && down_list[down_index]<= millis() - start_time) {
    down_index++;
    down_arrows_screen.push_back(15);
    down_arrows_screen.push_back(10);

  } else {
    down_arrow();
  }

  if (up_index<up_list_max && up_list[up_index]<= millis() - start_time) {
    up_index++;
    up_arrows_screen.push_back(5);
    up_arrows_screen.push_back(10);

  } else {
    up_arrow();
  }

  if (right_index<right_list_max && right_list[right_index]<= millis() - start_time) {
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
  left_index = 0;
  left_list_max = 0;
  up_index = 0;
  up_list_max = 0;
  down_index = 0;
  down_list_max = 0;
  right_index = 0;
  right_list_max = 0;
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
