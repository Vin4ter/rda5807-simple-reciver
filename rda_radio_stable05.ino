/*FM RADIO WITH HAM 2M 6M AIR FM OIRT BLUETOOTH AUDIO 16.06.2025
 * LOG 16.06.25
 * Added mono stereo switcher
 * 
 * FOR DEVOLOPERS
 * 
 * LOG 08.11.25
 * Added reset to settings
 * Added information screen of firmware flashing than arduino connected to pc mucro usb by over voltage 4.4v checker.
 * 
 * FOR DEVOLOPERS
 * 
 *   EEPROM.get(0, afc_setting);
  EEPROM.get(5, bass_setting);
  EEPROM.get(10, LnaPortSel_setting);
  EEPROM.get(15, LnaIcSel_setting);
  EEPROM.get(20, RDS_setting);
  EEPROM.get(25, FmDeemphasis_setting);
  EEPROM.get(30, blend_noise);
  EEPROM.get(35, new_demodulator_setting);
  EEPROM.get(40, index_list);
  EEPROM.get(45, cur_freq);
  EEPROM.get(50,  cur_band);
  EEPROM.get(55,  volume);

        EEPROM.put(60, 9660); //last freq of fm band
      EEPROM.put(65, 14500); //last freq of 2m band
            EEPROM.put(70, 11930); //last freq of air band
            EEPROM.put(75, 5000); //last freq of 6m band
const int fm_band=1;
const int r_6m_band=0;
const int r_2m_band=3;
const int air_band=2;

 * 
 * 
 * 
 * 
 * 
 */




#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "GyverEncoder.h"
#include <GyverPower.h>
#include <RDA5807.h>
#include <EEPROM.h>
Adafruit_SSD1306 display(-1);

#define CLK 2
#define DT 3
#define SW 4
#define Blueotooth_JDY62 10

#define fm_reciver_ui 1
#define bluetooth_ui 4
#define settings_page 2
#define fm_band 1
#define r_6m_band 0
#define r_2m_band 3
#define air_band 2

Encoder enc1(CLK, DT, SW);
int volume=1;
 uint16_t  beforeUpdateTimer_rssi=0;
 uint16_t  beforeUpdateTimer_rot=0;
 uint16_t  power_voltage_timer=0;
 uint16_t cur_freq=9660;
 uint16_t bluetooth_timer=0;
 uint16_t setFreqInEEPROM=0;
RDA5807 rx;


int index_list=1;
int ui_status=fm_reciver_ui;
int cur_band=fm_band;
int selectMenu=0;
int countLowerVoltageTick=0;
int fm_freq=9660;
int rx_mono=false;
//RDA5807 SETTINGS
bool afc_setting=true;
bool bass_setting=false;
int LnaPortSel_setting=3;
int LnaIcSel_setting=3;
bool RDS_setting=false;
int FmDeemphasis_setting=-1;
bool blend_noise=1;
bool new_demodulator_setting=false;
bool dxMode=false;

int rx_step=100;
int band_3_60_65=0;
int rx_space=0;
int rx_band=0;
bool status_firmware_flash=false;

int settingsList=1;
bool rotatoryStatus;

const float lowVoltageLine = 3.35;
const float R1 = 42640.0; // 47k
const float R2 = 9800.0;  // 10k

const float divider =  R2/(R1 + R2); // Коэффициент передачи
const float Vref = 1.1; // Опорное напряжение
  float voltage=0;

bool lowVoltage=false;
  int ti=0;

void setup()   
{        
        
  //Serial.begin(9600); 
   pinMode(10, HIGH);  
    digitalWrite(10 , HIGH); 
 analogReference(INTERNAL);
  analogRead(A7);
  delay(100);
  voltage = (float) (analogRead(A7) * Vref / 1024) / divider;
  
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  // Очистить буфер.
  display.clearDisplay();
    display.display();
    display.setTextSize(1);
  display.setTextColor(WHITE);

   
  if(voltage>4.4){//firmware flash
  display.setCursor(0,0);
   display.println("Firmware flash");
     display.display();
   status_firmware_flash=true;
   rx.powerDown();
   delay(500);
     power.setSleepMode(POWERDOWN_SLEEP);
      power.sleepDelay(99999999999999999999999999999999);  
   
  }else{
 // Serial.begin(9600);
 // enc1.setTickMode(AUTO);

//  enc1.setType(TYPE1);              // TYPE1 / TYPE2 - тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
enc1.setTickMode(AUTO);      // MANUAL / AUTO - ручной или автоматический опрос энкодера функцией tick(). (по умолчанию ручной)
//enc1.setDirection(NORM);    // NORM / REVERSE - направление вращения энкодера
//enc1.setFastTimeout(25);        // установка таймаута быстрого поворота
//enc1.setPinMode(HIGH_PULL);              // тип подключения энкодера, подтяжка HIGH_PULL (внутренняя) или LOW_PULL (внешняя на GND)
//void setBtnPinMode(bool mode);   
 
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
   display.println("Alexey Tikhanov");


    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("v0.7 ham air fm rds");
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.println("rda5807 reciver");
  
   display.display();
   delay(2000);
/*
  // Показать текст
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("RADIO DX!");

    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("107.6MHZ");

  
 
*/
rx.setup();

int estatus=0;
EEPROM.get(200,estatus);
 if(  estatus==73 ){
  //Serial.print("EPPROM VARIED!");
  EEPROM.get(0, afc_setting);
  EEPROM.get(5, bass_setting);
  EEPROM.get(10, LnaPortSel_setting);
  EEPROM.get(15, LnaIcSel_setting);
  EEPROM.get(20, RDS_setting);
  EEPROM.get(25, FmDeemphasis_setting);
  EEPROM.get(30, blend_noise);
  EEPROM.get(35, new_demodulator_setting); 
  EEPROM.get(40, index_list);
  EEPROM.get(45, cur_freq);
  EEPROM.get(50,  cur_band);
  EEPROM.get(55,  volume);
  EEPROM.get(80,  rx_mono);

  
 }else{
  initEEPROM();
   // Serial.print("EPPROM init!");
 }


rx.setVolume(volume);
 rx.setAFC( afc_setting);  // Sets Automatic Frequency Control
  rx.setBass( bass_setting);
 rx.setSpace(1);
 rx.setStep(100); 
 rx.setLnaIcSel(LnaIcSel_setting); 
 rx.setLnaPortSel(LnaPortSel_setting);
  rx.setRDS(RDS_setting);
  rx.setRdsFifo(RDS_setting);
   //rx.setFmDeemphasis(0); 

  rx.setSoftmute (false);
  if(FmDeemphasis_setting!=-1){
      rx.setFmDeemphasis (FmDeemphasis_setting);
  }

  rx.setNewDemodulateMethod(new_demodulator_setting);
rx.setSoftBlendEnable(blend_noise);

rx.setMono (true);

 if(index_list==fm_band){
    ui_status=fm_reciver_ui;
 EEPROM.get(60, cur_freq);
 rx.setMono (rx_mono);
   rx.setFrequency(cur_freq);
}else if(index_list==r_6m_band){
    ui_status=fm_reciver_ui;
   EEPROM.get(75, cur_freq);
     rx.setFrequency(cur_freq);
}else if(index_list==r_2m_band){
    ui_status=fm_reciver_ui;
   EEPROM.get(65, cur_freq);
     rx.setFrequency(cur_freq);
}else if(index_list==air_band){
    ui_status=fm_reciver_ui;
   EEPROM.get(70, cur_freq);
     rx.setFrequency(cur_freq);
 }else if(index_list==bluetooth_ui){
   //EEPROM.get(70, cur_freq);
    ui_status=bluetooth_ui;
     showBluetoothUi();
          rx.powerDown();
       digitalWrite(10 , LOW); // высокий сигнал bluettoth jdy62 на D10
 }

 //rx.setSpace(3); 

}
}

void initEEPROM(){
  EEPROM.put(0, true);
  EEPROM.put(5, false);
  EEPROM.put(10, 3);
  EEPROM.put(15, 3);
  EEPROM.put(20, false);
  EEPROM.put(25, -1);
  EEPROM.put(30, true);
  EEPROM.put(35, false); 
  EEPROM.put(40, fm_band);
  EEPROM.put(45, 9660);
  EEPROM.put(50,  fm_band);
  EEPROM.put(55,  2);
   EEPROM.put(60, 9660); //last freq of fm band
      EEPROM.put(65, 14500); //last freq of 2m band
            EEPROM.put(70, 11930); //last freq of air band
            EEPROM.put(75, 5100); //last freq of 6m band
                        EEPROM.put(80, true); //mono
   EEPROM.put(200, 73);
  
}



 void(* resetFunc) (void) = 0;



void loop() {

//if(status_firmware_flash==true){
//return 0;
  
//}



 


if(lowVoltage==false){
  


  
if(ui_status==fm_reciver_ui){



if(dxMode==false){
if(beforeUpdateTimer_rot<millis()){
  
    //  Serial.println(getVcc());
     beforeUpdateTimer_rot=millis()+10000;
     rotatoryStatus=false;
}












  if(beforeUpdateTimer_rssi<millis()){



voltage = (float) (analogRead(A7) * Vref / 1024) / divider;
         if(voltage<=lowVoltageLine){
    countLowerVoltageTick=countLowerVoltageTick+1;
  }else{
      countLowerVoltageTick=0; 
     
    
      
  }

    
   
  
    //  Serial.println(getVcc());
    if( rotatoryStatus==true){
      
    }else{
       
 if(setFreqInEEPROM!=cur_freq){
 if(cur_band==fm_band){
 EEPROM.put(60, cur_freq);
 //Serial.print("FM FREQ WRITED TO EPROM\n!");
 setFreqInEEPROM=cur_freq;
}else if(cur_band==r_6m_band){
   EEPROM.put(75, cur_freq);
    setFreqInEEPROM=cur_freq;
}else if(cur_band==r_2m_band){
   EEPROM.put(65, cur_freq);
    setFreqInEEPROM=cur_freq;
}else if(cur_band==air_band){
   EEPROM.put(70, cur_freq);
    setFreqInEEPROM=cur_freq;
}
 }
 power.setSystemPrescaler(PRESCALER_4);
 power.setSleepMode(EXTSTANDBY_SLEEP ); //
  power.sleepDelay(2048);  
    }
     beforeUpdateTimer_rssi=millis()+2046;
     
     

// power.setSleepMode(POWERDOWN_SLEEP); //
  // power.sleep(SLEEP_2048MS); 
    //voltage = (float) (analogRead(A7) * Vref / 1024) / divider;

     
if (rx.getRdsReady()) {
    if (rx.hasRdsInfo()) {
       display.clearDisplay();
          showReciverUi(cur_band);
    

           display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,20);

String rds_station_name=String(rx.getRdsStationName());
 
  display.println("RDS: "+  rds_station_name);
  
  display.display();
      
   
    
  }

     
 }else{
          showReciverUi(cur_band);
    }



     if(4<=countLowerVoltageTick){
     digitalWrite(10 , HIGH);  
     rx.powerDown();
     lowVoltage=true;
    // tstep=30000
display.clearDisplay();
   display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Low battery!");
  display.display();
    power.setSystemPrescaler(PRESCALER_4); 
  }else{
   
  }

  }






}else{
       
      power.setSystemPrescaler(PRESCALER_8); 
      power.setSleepMode(EXTSTANDBY_SLEEP ); //
      power.sleepDelay(500);  
}







  
    if (enc1.isRight()){
        power.setSystemPrescaler(PRESCALER_1); 
        rotatoryStatus=true;
        if(cur_band==fm_band){
           cur_freq=cur_freq+10; //10902
        }else{
              cur_freq=cur_freq+1; //10902
        }
       
//rx.setFrequencyUp();
rx.setFrequency(cur_freq);
showReciverUi(cur_band);

  
    
          
    }
    

  if (enc1.isLeft()){
 power.setSystemPrescaler(PRESCALER_1); 
       rotatoryStatus=true;
             if(cur_band==fm_band){
           cur_freq=cur_freq-10; //10902
        }else{
              cur_freq=cur_freq-1; //10902
        }
        //cur_freq=cur_freq-1;
//rx.setFrequencyUp();
rx.setFrequency(cur_freq);
 showReciverUi(cur_band);
  }
  
  if (enc1.isRightH()){
     power.setSystemPrescaler(PRESCALER_1); 
           rotatoryStatus=true;
           if(volume<15){
                volume=volume+1;
    rx.setVolume(volume);
     EEPROM.put(55,  volume);
           }

    // если было удержание + поворот
  }
  if (enc1.isLeftH()){
     power.setSystemPrescaler(PRESCALER_1); 
           rotatoryStatus=true;
 if(0<volume){
        volume=volume-1;
            rx.setVolume(volume); 
               EEPROM.put(55,  volume);
 }
             
  }
  
  if (enc1.isPress()){
    // нажатие на кнопку (+ дебаунс)
  }


  //if (enc1.isHold()) Serial.println("Hold");         // возвращает состояние кнопки
  


  }else if(ui_status==bluetooth_ui){

  if(bluetooth_timer<millis()){

     voltage = (float) (analogRead(A7) * Vref / 1024) / divider;
     
  showBluetoothUi();

     if(voltage<=lowVoltageLine){
    countLowerVoltageTick=countLowerVoltageTick+1;
  }else{
      countLowerVoltageTick=0; 
     
    
      
  }

    
    if(4<=countLowerVoltageTick){
     digitalWrite(10 , HIGH);  
     rx.powerDown();
     lowVoltage=true;
    // tstep=30000
display.clearDisplay();
   display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Low battery!");
  display.display();
    power.setSystemPrescaler(PRESCALER_4); 
  }else{
   
  }

  
    bluetooth_timer=millis()+2046;
 }
  // showBluetoothUi();
  }else if(ui_status==settings_page) {
 /*
 bool afc_setting=true;
bool bass_setting=false;
int LnaPortSel_setting=3;
int LnaIcSel_setting=3;
bool RDS_setting=false;
int FmDeemphasis=-1;
bool blend_noise=1;
bool new_demodulator_setting=false;
  */
  
   if (enc1.isRelease()){
   if(settingsList==1){ //pervaya str
      if(selectMenu==0){
   //LNApower
   if(LnaIcSel_setting==3){
      LnaIcSel_setting=0;
   }else{
    LnaIcSel_setting=LnaIcSel_setting+1;
   }
  // rx.set
     EEPROM.put(15, LnaIcSel_setting);
   rx.setLnaIcSel(LnaIcSel_setting); 
 
   

   
        
    }else if(selectMenu==1){ //lna port
         if(LnaPortSel_setting==3){
      LnaPortSel_setting=0;
   }else{
    LnaPortSel_setting=LnaPortSel_setting+1;
   }
  // rx.set
    EEPROM.put(10, LnaPortSel_setting);
 rx.setLnaPortSel(LnaPortSel_setting);
 
   
    }else if(selectMenu==2){ //pwr off timer


if(ti==3){
  ti=0;
}else{
  ti=ti+1;
}
if(ti==0){
  rx_step=100;
}else if(ti==1){
  rx_step=200;
}else if(ti==2){
  rx_step=25;
}else if(ti==3){
  rx_step=50; 
}
rx.setStep(rx_step);

      
    }
   }else if(settingsList==2){
          if(selectMenu==0){ //RDS

 
   if(RDS_setting==true){
    RDS_setting=false;
   }else{
    RDS_setting=true;
   }
  EEPROM.put(20, RDS_setting);
   rx.setRDS(  RDS_setting);
  rx.setRdsFifo(  RDS_setting);
        
    }else if(selectMenu==1){ //BASS BOOST
       if(bass_setting==true){
    bass_setting=false;
   }else{
    bass_setting=true;
   }
  EEPROM.put(5, bass_setting);
 rx.setBass(bass_setting);
 
   
    }else if(selectMenu==2){ //FM DEEMPHASIS
      if(FmDeemphasis_setting==-1){
        FmDeemphasis_setting=0;
        //off
      }else if(FmDeemphasis_setting==0){
        FmDeemphasis_setting=1;
      }else if(FmDeemphasis_setting==1){
        FmDeemphasis_setting=-1;
      }
        EEPROM.put(25, FmDeemphasis_setting);
      if(FmDeemphasis_setting==-1){
        // rx.setFmDeemphasis(0); 
 // rx.setFrequency(9660);
//  rx.setSoftmute (false);
  //rx.setNewDemodulateMethod(true);
      }else{
        rx.setFmDeemphasis(  FmDeemphasis_setting); 
    
      }
      
    }
   }else if(settingsList==3){
    if(selectMenu==0){ //AFC
         if(afc_setting==true){
       afc_setting=false;
      }else{
       afc_setting=true;
      }
      rx.setAFC( afc_setting);
  EEPROM.put(0, afc_setting);
    }else if(selectMenu==1){ //noise blend filter active
      if(blend_noise==true){
        blend_noise=false;
      }else{
        blend_noise=true;
      }
        EEPROM.put(30, blend_noise);
      rx.setSoftBlendEnable( blend_noise);
    }else if(selectMenu==2){ //new demodulated method
         if(new_demodulator_setting==true){
       new_demodulator_setting=false;
      }else{
       new_demodulator_setting=true;
      }
        EEPROM.put(35, new_demodulator_setting);
    rx.setNewDemodulateMethod(new_demodulator_setting); 
    }
 }else if(settingsList==4){
    if(selectMenu==0){ //band3
     if(band_3_60_65==1){
      band_3_60_65=0;
     }else{
     band_3_60_65= band_3_60_65+1;
     }
   rx.setBand3_50_65_Mode( band_3_60_65);
     
    }else if(selectMenu==1){ //rx_band
       if(rx_band==3){
     rx_band=0;
     }else{
     rx_band= rx_band+1;
     }
       
      rx.setBand(rx_band);
    }else if(selectMenu==2){ //rx_space
       if(rx_space==3){
     rx_space=0;
     }else{
     rx_space= rx_space+1;
     }
       
      rx.setSpace(rx_space);
    }
  }else if(settingsList==5){
    if(selectMenu==0){ //DX MODE
     if(dxMode==true){
      dxMode=false;
     }else{
    dxMode=true;
     }
     }else if(selectMenu==1){ //mono_stereo switcher
       if(rx_mono==true){
         rx_mono=false;
         
     }else{
      rx_mono=true;
     }
        rx.setMono (rx_mono);
              EEPROM.put(80, rx_mono); //mono
   }else if(selectMenu==2){ //reset settings
                EEPROM.put(200, 53);
                delay(200);
                resetFunc();
  }
  }
   
        display.clearDisplay();
              if(settingsList==1){
               showSettingsPage1();
          }else if(settingsList==2){
               showSettingsPage2();
          }else if(settingsList==3){
            showSettingsPage3();
          }else if(settingsList==4){
            showSettingsPage4();
          }else if(settingsList==5){
            showSettingsPage5();
          }
                    display.setTextSize(1); 
  display.setTextColor(WHITE);
  display.setCursor(5,10*selectMenu);
    display.println("*");
    display.display();
   }
   
   
   if (enc1.isLeft()){
    
    if(selectMenu<2 and 0<=selectMenu){
    selectMenu=selectMenu+1;
    
          display.clearDisplay();
          if(settingsList==1){
               showSettingsPage1();
          }else if(settingsList==2){
               showSettingsPage2();
          }else if(settingsList==3){
            showSettingsPage3();
         }else if(settingsList==4){
            showSettingsPage4();
         } else if(settingsList==5){
            showSettingsPage5();
          }
       
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,10*selectMenu);
    display.println("*");
    display.display();
    
   
   }else if(selectMenu==2){
    if(settingsList==1){
      selectMenu=0;
      settingsList=2;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
    display.println("*");
      showSettingsPage2();
    }else if(settingsList==2){
     selectMenu=0;
      settingsList=3;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
    display.println("*");
      showSettingsPage3();
      }else if(settingsList==3){
     selectMenu=0;
      settingsList=4;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
    display.println("*");
      showSettingsPage4();
       }else if(settingsList==4){
     selectMenu=0;
      settingsList=5;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
    display.println("*");
      showSettingsPage5();
    }
   
  }
  
  }
   if (enc1.isRight()){
   if(selectMenu<=2 and 0<selectMenu){
    selectMenu=selectMenu-1;
    
          display.clearDisplay();
          if(settingsList==1){
               showSettingsPage1();
          }else if(settingsList==2){
               showSettingsPage2();
          }else if(settingsList==3){
            showSettingsPage3();
          }else if(settingsList==4){
            showSettingsPage4();
          }else if(settingsList==5){
            showSettingsPage5();
          }
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,10*selectMenu);
    display.println("*");
    display.display();

    }else if(selectMenu==0){
      if(settingsList==2){
      selectMenu=2;
      settingsList=1;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,10*selectMenu);
    display.println("*");
      showSettingsPage1();
    }else if(settingsList==3){
           selectMenu=2;
      settingsList=2;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,10*selectMenu);
    display.println("*");
      showSettingsPage2();
       }else if(settingsList==4){
           selectMenu=2;
      settingsList=3;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,10*selectMenu);
    display.println("*");
      showSettingsPage3();
          }else if(settingsList==5){
           selectMenu=2;
      settingsList=4;
      display.clearDisplay();
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,10*selectMenu);
    display.println("*");
      showSettingsPage4();
    }
    }

  }

  
  }
if(ui_status==settings_page){

}else{
    if (enc1.isRelease()){
    // отпускание кнопки (+ дебаунс)
    if(index_list==4){
      index_list=0;
      
    }else{
      index_list=index_list+1;
    }

     EEPROM.put(40, index_list);


    
    if(index_list==fm_band){
    ui_status=fm_reciver_ui;
    rx.setMono (rx_mono);
    cur_band=fm_band;
    
    //cur_freq=  
    EEPROM.get(60, cur_freq);
      rx.setBand(2);
    rotatoryStatus=true;
    rx.setFrequency(cur_freq);
    showReciverUi(cur_band);
     rx.setSpace(1);
 rx.setStep(100);
   // digitalWrite(Blueotooth_JDY62 , HIGH); // low сигнал bluettoth jdy62 на D10
  //  rx.powerUp();
    power.setSystemPrescaler(PRESCALER_1);  
    }else if(index_list==r_6m_band){
          rx.powerUp();
          rx.setMono (true);
     
    ui_status=fm_reciver_ui;
      rx.setBand(3);
      rx.setBand3_50_65_Mode(0);
    cur_band=r_6m_band;
          EEPROM.get(75, cur_freq);   
   // cur_freq=5000;
        rx.setFrequency(cur_freq);
    rotatoryStatus=true;
    showReciverUi(cur_band);
    digitalWrite(Blueotooth_JDY62 , HIGH); // low сигнал bluettoth jdy62 на D10

    power.setSystemPrescaler(PRESCALER_1); 
      
    }else if(index_list==air_band){
           cur_band=air_band;
           rx.setMono (true);
           ui_status=fm_reciver_ui;
          EEPROM.get(70, cur_freq);
           // rx.setSpace(3);
       
       //rx.setStep(25);
    rotatoryStatus=true;
    showReciverUi(cur_band);
        rx.setFrequency(cur_freq);
     power.setSystemPrescaler(PRESCALER_1); 
      
    }else if(index_list==r_2m_band){
    rx.setMono (true);
    cur_band=r_2m_band;
     EEPROM.get(65, cur_freq);
    ui_status=fm_reciver_ui;
    rotatoryStatus=true;
    showReciverUi(cur_band);
            rx.setFrequency(cur_freq);
    power.setSystemPrescaler(PRESCALER_1); 
     
    }else if(index_list==bluetooth_ui){
         ui_status=bluetooth_ui;
     showBluetoothUi();
          rx.powerDown();
       digitalWrite(10 , LOW); // высокий сигнал bluettoth jdy62 на D10
 //power.setSystemPrescaler(PRESCALER_4);
    }

  EEPROM.put(50,  cur_band);

    
    }
}
  
     


  if (enc1.isHolded()){
    // если была удержана и энк не поворачивался
   if(ui_status==settings_page){
        power.setSystemPrescaler(PRESCALER_1); 
   ui_status=fm_reciver_ui;
   showReciverUi(cur_band);

   }else if (ui_status==fm_reciver_ui){
     power.setSystemPrescaler(PRESCALER_1); 
     rotatoryStatus=true;
    ui_status=settings_page;
         display.clearDisplay();
          display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
    display.println("*");
    display.display();
 showSettingsPage1();
    
  }
  }

  

  
  }
}



  void showBluetoothUi(){
        display.clearDisplay();

      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
    display.println("BLE 4.2");


     
      
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(98,0);
  String voltage_str = String(voltage);
    display.println(voltage_str+"v");

        display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0,10);

 
  display.println("bluetooth mode" );
  
  display.display();
  }


  void showReciverUi(int band){
     display.clearDisplay();
     display.setTextSize(1);
     display.setTextColor(WHITE);
     display.setCursor(0,0);
  if(band == r_2m_band){
     display.println("2m");
  }else if(band==r_6m_band){
     display.println("6m");
    }else if(band==fm_band){
     display.println("FM"); 
    }else if(band==air_band){
     display.println("air");
    }
  


      if(dxMode==false){
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(45,0);
  String rssiString = String(rx.getRssi());
    display.println(rssiString+"db");
      
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(98,0);
     String voltage_str = String(voltage);
  display.println(voltage_str+"v");
      }

    
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
uint16_t freq= cur_freq;  //rx.getFrequency();
int mhz2=  freq  % 100 / 1;
int mhz1 = freq /100;
  String intString1= String(mhz1);
      String intString2 = String(mhz2);
  display.println(intString1+"."+intString2+"MHZ" );
  

  display.display();
  }















 void showSettingsPage1(){
//   display.clearDisplay();


      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
   display.println("LNApower");

     display.setTextColor(WHITE);
  display.setCursor(95,0);
 String LnaIcSel_str= String(LnaIcSel_setting);
   display.println("<"+LnaIcSel_str+">");


    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,10);
  display.println("lna_port");

       display.setTextColor(WHITE);
  display.setCursor(95,10);
 String LnaPortSel_str= String(LnaPortSel_setting);
   display.println("<"+LnaPortSel_str+">");
  
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("step");

       display.setTextColor(WHITE);
  display.setCursor(95,20);
    String intString1= String(rx_step);
   display.println("<"+intString1+">");
  
   display.display();
 }

  void showSettingsPage2(){
  // display.clearDisplay();
 
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
   display.println("rds");

     display.setTextColor(WHITE);
  display.setCursor(95,0);
  if(RDS_setting==true){
    display.println("<on>");
  }else{
    display.println("<off>");
  }
   


    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,10);
  display.println("bass boost");

       display.setTextColor(WHITE);
  display.setCursor(95,10);
  if(bass_setting==true){
    display.println("<on>");
  }else{
    display.println("<off>");
  }
   

  
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("FmDeemphasis");

       display.setTextColor(WHITE);
  display.setCursor(95,20);

   if(FmDeemphasis_setting==-1){
       display.println("<off>");
   }else if(FmDeemphasis_setting==0){
      display.println("<75>");
   }else if(FmDeemphasis_setting==1){
       display.println("<50>");
   }
 
  
   display.display();
 }


  void showSettingsPage3(){
  // display.clearDisplay();
 
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
   display.println("AFC");

     display.setTextColor(WHITE);
  display.setCursor(95,0);
  if(afc_setting==true){
       display.println("<on>");
  }else{
       display.println("<off>");
  }



    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,10);
  display.println("a-noise fr");

       display.setTextColor(WHITE);
  display.setCursor(95,10);
  if(blend_noise==true){
        display.println("<on>");
  }else if(blend_noise==false){
       display.println("<off>");
  }

  
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("new fm d-r");
   display.setTextColor(WHITE);
  display.setCursor(95,20);
  if(new_demodulator_setting==true){
        display.println("<on>");
  }else{
       display.println("<off>");
  }


  
   display.display();
 }

  void showSettingsPage4(){
  // display.clearDisplay();
 
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
   display.println("BAND3_50");

     display.setTextColor(WHITE);
  display.setCursor(95,0);
  if( band_3_60_65==1){
       display.println("<65>");
   }else if( band_3_60_65==0){
      display.println("<50>");
   }


    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,10);
  display.println("Band");

       display.setTextColor(WHITE);
  display.setCursor(95,10);
  if( rx_band==0 ){
       display.println("wfm");
   }else if( rx_band==1){
      display.println("jap");
    }else if( rx_band==2){
      display.println("jpw");
     }else if( rx_band==3){
      display.println("50_60");
   }

  
    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("space");
   display.setTextColor(WHITE);
  display.setCursor(95,20);
if( rx_space==0 ){
       display.println("<100>");
   }else if( rx_space==1){
      display.println("<200>");
    }else if( rx_space==2){
      display.println("<50>");
     }else if( rx_space==3){
      display.println("<25>");
   }


  
   display.display();
 }
   void showSettingsPage5(){
  // display.clearDisplay();
 
      display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,0);
   display.println("DX mode");

     display.setTextColor(WHITE);
  display.setCursor(95,0);
  if( dxMode==false){
       display.println("<off>");
   }else if(dxMode==true){
      display.println("<on>");
   }


    display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,10);
  display.println("stereo");

       display.setTextColor(WHITE);
  display.setCursor(95,10);
  if( rx_mono==true ){
       display.println("<off>");
   }else if( rx_mono==false){
      display.println("<on>");
    }
    
 display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20,20);
  display.println("reset");

   
   display.display();
 }






 
