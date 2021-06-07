#include <SPI.h>
#include <TFT_eSPI.h>

void change_state(bool beeped){
  if(stateband == NO_BEEP){
    if(beeped){ 
      stateband = BEEPING; 
      num_beeps = 1;
      last_beep = millis();
    }
  } else if(stateband == BEEPING){
      if(millis() - last_beep > 3000){
        stateband = NO_BEEP;
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString("READY FOR BEEPS",SCREEN_WIDTH/2, SCREEN_HEIGHT*2/5,2);
        num_beeps = 0; 
      } else if(millis() - last_beep > 700 && beeped){
        num_beeps += 1;
        last_beep = millis();
      }
      
      if(num_beeps == 1) tft.fillScreen(TFT_RED);
      if(num_beeps == 2) tft.fillScreen(TFT_YELLOW);
      if(num_beeps == 3) stateband = GO;
  } else if (stateband == GO) { // state == GO
    tft.fillScreen(TFT_GREEN);
    num_beeps = 0;
    player_ready = true;
  }
}

bool is_beep(double peak){
  return (peak > 490 && peak < 510); // the mic is about 5 Hz off, so this range is 490-510
}
