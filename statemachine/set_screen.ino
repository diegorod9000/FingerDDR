void update_state() {
  switch(page_state){
    case MENU:
      set_screen(MENU);
      // left button pushed -> leaderboard
      if (left_reading != old_left_reading) {
        if (!left_reading) { 
          Serial.println("left button pressed, going to leaderboard");
          from_menu = true;
          tft.fillScreen(TFT_BLACK);
          set_screen(LEADERBOARD);
          page_state = LEADERBOARD;
          delay(200);
        }
      }
      // right button pushed -> choose song
      else if (right_reading != old_right_reading) {
        if (!right_reading) { 
          Serial.println("right button pressed, going to song choices");
          from_lboard = false;
          tft.fillScreen(TFT_BLACK);
          song_index = 0;
          old_song_index = -1;
          page_state = CHOOSE_SONG;
          delay(200);
        }
      }break;

    case LEADERBOARD:
      // left button pushed -> menu
      if (left_reading != old_left_reading) {
        if (!left_reading) { 
          Serial.println("left button pressed, going to menu");
          tft.fillScreen(TFT_BLACK);
          page_state = MENU;
          delay(200);
        }
      }
      // right button pushed -> choose song (ONLY IF WE DON'T COME FROM MENU)
      else if (right_reading != old_right_reading) {
        if (!right_reading and !from_menu) { 
          Serial.println("right button pressed, going to song choices");
          from_lboard = true;
          tft.fillScreen(TFT_BLACK);
          song_index = 0;
          old_song_index = -1;
          page_state = CHOOSE_SONG;
          delay(200);
        }
      }break;

    case CHOOSE_SONG:
      set_screen(CHOOSE_SONG);
      stateband = NO_BEEP;
      num_beeps = 0;
      player_ready = false; 
      // left button pushed -> menu
      if (left_reading != old_left_reading) {
        if (!left_reading) { 
          Serial.println("left button pressed, going to menu");
          tft.fillScreen(TFT_BLACK);
          page_state = MENU;
          delay(200);
        }
      }
      // right button pushed -> enter name or straight to song
      else if (right_reading != old_right_reading) {
        if (!right_reading) { 
          song_choice = songs[song_index];
          //getNotes("rararasputin");
          Serial.printf("song choice: %s\n", song_choice);
          if (song_choice == "SUNFLOWER") {
            getNotes("sunflower");
          } else if (song_choice == "ALL STAR") {
            getNotes("allstar");
          } else if (song_choice == "KUNG FU FIGHTING") {
            getNotes("kungfufighting");
          } else if (song_choice == "CASTAWAYS") {
            getNotes("castaways");
          } else if (song_choice == "ROCKETMAN") {
            getNotes("rocketman");
          } else if (song_choice == "TECHNO") {
            getNotes("techno");
          } else {
            getNotes("rararasputin");
          }

          postSong();
          tft.fillScreen(TFT_BLACK);
          
          if (from_lboard){ // if we're coming from the lboard, skip the choose name step
            Serial.println("right button pressed, going to play song");
            tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_YELLOW);
            tft.drawCentreString("READY FOR BEEPS",SCREEN_WIDTH/2, SCREEN_HEIGHT*2/5,2);
            page_state = PLAY_SONG;
            waiting_timer = millis();
          }
          else { // otherwise, we want to choose a new name 
            Serial.println("right button pressed, going to enter name");
            strcpy(username, ""); // reset the username, name index, & letter index
            name_index = 0;
            old_name_index = -1;
            alphabet_index = 0;
            page_state = ENTER_NAME;
          }
          delay(200);
        }
      }
      // joystick up or down - go up or down in list of song
      if (y_reading > JOYSTICK_MAX*0.75) { 
        Serial.println("joystick up, flipping through songs");
        song_index = mod(song_index-1, num_songs);
        delay(500);
      }
      else if (y_reading < JOYSTICK_MAX*0.25) { 
        Serial.println("joystick down, flipping through songs");
        song_index = mod(song_index+1, num_songs);
        delay(500);
      }break;

    case ENTER_NAME:
      set_screen(ENTER_NAME);
      
      // right button pushed -> play song
      if (right_reading != old_right_reading) {
        if (!right_reading) { 
          Serial.println("right button pressed, going to play song");
          strcat(username,alphabet[alphabet_index]); // append the last letter that you chose
          tft.fillScreen(TFT_BLACK);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString("READY FOR BEEPS",SCREEN_WIDTH/2, SCREEN_HEIGHT*2/5,2);
          page_state = PLAY_SONG;
          waiting_timer = millis();
          delay(200);
        }
      }
      // joystick to the right -> choose next letter
      else if (x_reading > JOYSTICK_MAX*0.75) { 
        Serial.println("joystick right, moving to next letter");
        name_index++;
        strcpy(chosen_letter,alphabet[alphabet_index]);
        alphabet_index = 0;
        delay(400);
      }
      // joystick down -> next letter in alphabet
      else if (y_reading < JOYSTICK_MAX*0.25) { 
        Serial.println("joystick down, choosing next letter");
        alphabet_index = mod(alphabet_index+1, 26);
        delay(300);
      }
      // joystick up -> previous letter in alphabet
      else if (y_reading > JOYSTICK_MAX*0.75) { 
        Serial.println("joystick up, choosing previous letter");
        alphabet_index = mod(alphabet_index-1, 26);
        delay(300);
      }break;

    case PLAY_SONG:
      set_screen(PLAY_SONG);
      // 10s go by without hearing the beeps - go back to menu
      if (!player_ready && millis() - waiting_timer > 10000){
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString("NO BEEPS HEARD,", SCREEN_WIDTH/2, SCREEN_HEIGHT*2/5, 1);
        tft.drawCentreString("RETURNING TO MENU", SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 1);
        delay(1500);
        tft.fillScreen(TFT_BLACK);
        page_state = MENU;
        set_screen(MENU);
      }
      if (player_ready) {
        tft.fillScreen(TFT_BLACK);
        start_time = millis();
        page_state = PLAYING;
        song_timer = millis();
      }break;

    case PLAYING:
      set_screen(PLAYING);
      if (millis() - start_time > 60000) { // after 1:04, stop the song
        tft.fillScreen(TFT_BLACK);
        page_state = FINISHED;
        set_screen(FINISHED);
      }
      
    case FINISHED:
      if (right_reading != old_right_reading) {
        if (!right_reading) {
          from_menu = false;
          tft.fillScreen(TFT_BLACK);
          page_state = LEADERBOARD;
          set_screen(LEADERBOARD);
        }
      }
  }
}

void set_screen(uint8_t page_state) {
  switch (page_state) {
    case MENU:
      strcpy(username, ""); // reset the username
      current_score = 0;
      prev_score = 0;
      name_index = 0;
      old_name_index = -1;
      alphabet_index = 0;
      
      indx = base_index;
      
      indx = coloradd(indx);
      tft.setTextColor(colors[indx], TFT_BLACK);
      tft.setCursor(40, 15, 2);
      tft.println("FINGER");
      
      indx = coloradd(indx);
      tft.setTextColor(colors[indx], TFT_BLACK);
      tft.setCursor(70, 40);
      tft.println("DANCE");
      
      indx = coloradd(indx);
      tft.setTextColor(colors[indx], TFT_BLACK);
      tft.setCursor(15, 60);
      tft.println("DANCE");
      
      indx = coloradd(indx);
      tft.setTextColor(colors[indx], TFT_BLACK);
      tft.setCursor(25, 90);
      tft.println("REVOLUTION");
      
      indx = coloradd(indx);
      tft.setTextColor(colors[indx], TFT_BLACK);
      tft.setCursor(3, SCREEN_HEIGHT-10, 1);
      tft.println("LEADERBOARD < > PLAY");
      
      base_index += 1;
      delay(50);
      break;
    
    case LEADERBOARD:
      tft.setTextColor(TFT_PINK, TFT_BLACK);
      tft.drawCentreString("LEADERBOARD",SCREEN_WIDTH/2,1,2);

      if (from_menu != false){
        open_leaderboard(false);
      }
      else{
        open_leaderboard(true);
      }
      tft.setTextSize(1);
      tft.setCursor(0,22,1);
      
      tft.printf("1. %s\n\n", lboard1);
      tft.printf("2. %s\n\n", lboard2);
      tft.printf("3. %s\n\n", lboard3);
      tft.printf("4. %s\n\n", lboard4);
      tft.printf("5. %s\n\n", lboard5);
      tft.printf("6. %s\n\n", lboard6);
      tft.printf("7. %s\n\n", lboard7);
      tft.printf("8. %s\n\n", lboard8);
      
      if (from_menu){ // coming from the menu, so our only choice is back to menu
        tft.drawString("MENU <", 3, SCREEN_HEIGHT-10, 1);
      }
      else { // not coming from the menu, so we can also choose a song from this page
        tft.drawCentreString("MENU < > CHOOSE SONG",SCREEN_WIDTH/2,SCREEN_HEIGHT-10,1);
      }
      break;
  
    case CHOOSE_SONG:
      current_score = 0;
      prev_score = 0;
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
      if (song_index != old_song_index){ // if we scroll for a different song
        tft.fillScreen(TFT_BLACK);
        tft.drawCentreString(songs[song_index],SCREEN_WIDTH/2,SCREEN_HEIGHT/2.5,2); // center song
        tft.drawCentreString(songs[mod(song_index-1,num_songs)],SCREEN_WIDTH/2,SCREEN_HEIGHT/2.5-15,1); // prev song on top
        tft.drawCentreString(songs[mod(song_index+1,num_songs)],SCREEN_WIDTH/2,SCREEN_HEIGHT/2.5+25,1); // next song underneath
        
        tft.drawCentreString("^",SCREEN_WIDTH/2,10,1);
        tft.drawCentreString("PREV",SCREEN_WIDTH/2,20,1);
        tft.drawCentreString("v",SCREEN_WIDTH/2,SCREEN_HEIGHT-45,1);
        tft.drawCentreString("NEXT",SCREEN_WIDTH/2,SCREEN_HEIGHT-35,1);
        tft.drawCentreString("MENU <  > ENTER NAME", SCREEN_WIDTH/2, SCREEN_HEIGHT-10, 1);
      }
      old_song_index = song_index;
      break;
  
    case ENTER_NAME:
      // things get weirdly formatted when the name is over 12 letters, let's just limit it there
      if (name_index < 13){ 
        if (name_index != old_name_index){ // if we move on to the next letter in the name
          tft.fillScreen(TFT_BLACK);
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.drawCentreString("ENTER NAME", SCREEN_WIDTH/2, 10, 2);
          tft.drawRightString("> PLAY", SCREEN_WIDTH-3, SCREEN_HEIGHT-10, 1);
          tft.setCursor(12, SCREEN_HEIGHT/2, 2);
          strcat(username,chosen_letter);
          tft.print(username);
        }
        tft.setCursor(10+8*name_index, 80, 2);
        tft.setTextColor(TFT_BLACK, TFT_GREEN);
        tft.print(alphabet[alphabet_index]);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        
        tft.drawCentreString("^", 14+8*name_index, 60, 2);
        tft.drawCentreString("v", 14+8*name_index, 110, 1);
        old_name_index = name_index;
      }
      break;
  
    case PLAY_SONG:{
      // sample SAMPLES times
      for (int i = 0; i < SAMPLES; i++){
        microSeconds = micros();
    
        vReal[i] = analogRead(sensorPin);
        vImag[i] = 0;
    
        while (micros() < microSeconds + samplingPeriod){
          // do nothing
        }
      }
    
      // perform FFT on samples
      FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    
      // find peak frequency
      double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
      bool beeped = is_beep(peak); // see if it's in the right range
      change_state(beeped);
      }break;

    case PLAYING:{
      tft.fillRect(0, 145, 140, 160, TFT_RED);
      tft.drawLine(0, 2, 140, 2, TFT_RED);
      tft.drawLine(0, 4, 140, 4, TFT_YELLOW);
      tft.drawLine(0, 6, 140, 6, TFT_GREEN);
      tft.drawLine(0, 8, 140, 8, TFT_BLUE);

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
      tft.setCursor(5, 13, 1.5);
      tft.setTextColor(TFT_WHITE);
      if(current_score < 0){
        current_score = 0;
      }
      sprintf(scoreDisplay, "Score: %d", current_score);
      tft.println(scoreDisplay);

      hitDetection();
      beats();
      }break;

    case FINISHED:
      tft.fillScreen(TFT_BLACK);
      tft.drawCentreString("SONG COMPLETED!", SCREEN_WIDTH/2, SCREEN_HEIGHT*2/5, 2);
      tft.setTextColor(TFT_WHITE);
      tft.drawRightString("> LEADERBOARD", SCREEN_WIDTH-5, SCREEN_HEIGHT-10, 1);
      break;
  }
}

int coloradd(int ind){
  ind += 1;
  if (ind > 15){
    ind = ind%15;
  }
  return ind;
}

int mod(int original_num, int num_to_mod){
  if (original_num == -1){
    return num_to_mod-1;
  }
  else if (original_num == num_to_mod){
    return 0;
  }
  else{
    return original_num;
  }
}

void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

void postSong(){
  Serial.println("HERE");
  char request[500];
  memset(request, 0, sizeof(request));
  memset(response, 0, sizeof(response));
  // POST to server (game.py) to enter right song
  char body[1000];

  char* song_post = song_posts[song_index];

  sprintf(body, "song=%s", song_post);
  int body_len = strlen(body);
  sprintf(request, "POST http://608dev-2.net/sandbox/sc/team43/fddr/game.py HTTP/1.1\r\n");
  strcat(request, "Host: 608dev-2.net\r\n");
  strcat(request, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request + strlen(request), "Content-Length: %d\r\n\r\n", body_len);
  strcat(request, body);
  strcat(request,"\r\n");

  do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}


void open_leaderboard(bool POST){
  char request[500];
  memset(request, 0, sizeof(request));
  memset(response, 0, sizeof(response));
  
  if (POST == true){
    // make POST request to server to enter current username
    char body[1000];
    sprintf(body, "user=%s&score=%d&song=%s", username, current_score, song_posts[song_index]);
    int body_len = strlen(body);
    sprintf(request, "POST http://608dev-2.net/sandbox/sc/team43/fddr/lboard.py HTTP/1.1\r\n");
    strcat(request, "Host: 608dev-2.net\r\n");
    strcat(request, "Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(request + strlen(request), "Content-Length: %d\r\n\r\n", body_len);
    strcat(request, body);
    strcat(request,"\r\n"); 
  }
  else {
    sprintf(request, "GET http://608dev-2.net/sandbox/sc/team43/fddr/lboard.py HTTP/1.1\r\n");
    strcat(request, "Host: 608dev-2.net\r\n");
    strcat(request,"\r\n");
  }
  do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  
  memset(lboard1, 0, 25);
  memset(lboard2, 0, 25);
  memset(lboard3, 0, 25);
  memset(lboard4, 0, 25);
  memset(lboard5, 0, 25);
  memset(lboard6, 0, 25);
  memset(lboard7, 0, 25);
  memset(lboard8, 0, 25);
  
  char * start= strchr(response, '{');
  char * end_ = strrchr(response, '}');
  *(end_+1) = '\0';
  
  DynamicJsonDocument message(1000);
  deserializeJson(message, start);
  
  int message_length = message["length"];
  
  if (message_length >= 1){
    Serial.println(message_length);
    const char* lb1 = message["data"][0];
    Serial.println(lb1);
    sprintf(lboard1, "%s", lb1);
    Serial.println(lboard1);
  }
  if (message_length >= 2){
    const char* lb2 = message["data"][1];
    sprintf(lboard2, "%s", lb2);
  }
  if (message_length >= 3){
    const char* lb3 = message["data"][2];
    sprintf(lboard3, "%s", lb3);
  }
  if (message_length >= 4){
    const char* lb4 = message["data"][3];
    sprintf(lboard4, "%s", lb4);
  }
  if (message_length >= 5){
    const char* lb5 = message["data"][4];
    sprintf(lboard5, "%s", lb5);
  }
  if (message_length >= 6){
    const char* lb6 = message["data"][5];
    sprintf(lboard6, "%s", lb6);
  }
  if (message_length >= 7){
    const char* lb7 = message["data"][6];
    sprintf(lboard7, "%s", lb7);
  }
  if (message_length >= 8){
    const char* lb8 = message["data"][7];
    sprintf(lboard8, "%s", lb8);
  }

}
