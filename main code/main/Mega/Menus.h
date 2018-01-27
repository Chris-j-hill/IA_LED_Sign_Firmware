

#ifndef MENUS_H
#define MENUS_H
#include "Local_Config.h"
#include "Arduino.h"

struct Menu{
  byte current1 =0;
  byte current2 =0;
  byte light1 = 0;
  byte light2 = 0;
  byte temp1 = 0;
  byte temp2 = 0;
  byte temp3 = 0;
  byte fan_speed = 0;
  byte led_strip_brightness= false;
  bool sd_card1_detected = false;
  bool sd_card2_detected = false;
  bool ethernet_connected = false;
  bool wifi_connected = false;
  byte screen_brightness = 0;
  byte text_size = 1;
  byte text_colour_r = 0;
  byte text_colour_g = 0;
  byte text_colour_b = 0;
  int text_colour_hue = 0;
  bool use_hue = false;
  bool debug = false;
  byte screen_mode =0;

  uint32_t sensor_value_received =0;    //use as array of 32 bits to detect if value has been transmitted
                                        // bits to save space, bool is 8 bits so wasteful 
};

struct Menu_tree_items{

//level 1 menu items  
  String main_menu                PROGMEM           = "Main Menu";
  String RETURN                   PROGMEM           = "RETURN";
  String screen_mode              PROGMEM           = "Screen Mode";
  String brightness               PROGMEM           = "Brightness";
  String text_settings            PROGMEM           = "Text Settings";
  String fan_settings             PROGMEM           = "Fan Settings";
  String internet_settings        PROGMEM           = "Internet Config";
  String sd_card_settings         PROGMEM           = "Sd Cards"; 
  String led_strip_settings       PROGMEM           = "LED Strip";


//level 2 mennu items
//change mode folder
  String screen_mode_menu         PROGMEM           = "Mode Menu";
  String screen_mode0             PROGMEM           = "Both on";
  String screen_mode1             PROGMEM           = "Front Side";
  String screen_mode3             PROGMEM           = "Back Side";
  String screen_mode2             PROGMEM           = "Both Off";

// brightness folder

  String brightness_menu          PROGMEM           = "Brightness";  

//text adjustment folder
  String text_settings_menu       PROGMEM           = "Text Settings";
  String text_size_settings       PROGMEM           = "Text Size";
  String text_colour_settings     PROGMEM           = "Text Colour";
  String scroll_speed_settings    PROGMEM           = "Scroll Speed";
  String flip_dir_settings        PROGMEM           = "Flip Direction";

//fan settings folder
  String fan_settings_menu        PROGMEM           = "Fan Settings";
  String fan_speed_settings       PROGMEM           = "Fan Speed";
  String fan_enable               PROGMEM           = "Enable";
  String fan_disable              PROGMEM           = "Disable";
  String minimum_rotating_speed   PROGMEM           = "Minimum speed";

// internet configuration folder  
  String internet_config_menu     PROGMEM           = "Internet";
  String select_network_manually  PROGMEM           = "Connect";
  String ethernet_enable          PROGMEM           = "Enable Eth";
  String ethernet_disable         PROGMEM           = "Disable Eth";
  String wifi_enable              PROGMEM           = "Enable Wifi";
  String wifi_disable             PROGMEM           = "Disable Wifi";
  
// SD cards folder
  String sd_cards_menu            PROGMEM           = "SD Cards";
  String enable_ext_card          PROGMEM           = "Enable Port";
  String disable_ext_card         PROGMEM           = "Disable Port";
  String sd_card_folders          PROGMEM           = "Read Folders";

// Led Strip folder
  String led_strip_menu           PROGMEM           = "Led Strip";
  String enable_led_strip         PROGMEM           = "Enable LED Strip";
  String disable_led_strip        PROGMEM           = "Disable LED Strip";
  String led_strip_brightness     PROGMEM           = "Brightness";


//level 3 folders
//text size
  String text_size_menu           PROGMEM           = "Text Size";

//text colour
  String text_colour_menu         PROGMEM           = "Text Colour";
  String text_colour_red          PROGMEM           = "Red";
  String text_colour_green        PROGMEM           = "Green";
  String text_colour_blue         PROGMEM           = "Blue";
  String text_colour_hue          PROGMEM           = "Hue";
  String text_colour_use_hue      PROGMEM           = "Use Hue";
  String text_colour_use_rgb      PROGMEM           = "Use RGB";


// scroll speed
  String scroll_speed_menu        PROGMEM           = "Scroll Speed";

//set fan speed
  String fan_speed_menu           PROGMEM           = "Set Fan Speed";
  
// fan minimum speed
  String minimum_fan_speed_menu   PROGMEM           = "Set Min Speed";  

// read sd card folders 
  String SD_card_folders_menu     PROGMEM           = "Card Files";

// led brightness menu
  String led_strip_brightness_menu   PROGMEM        = "Set Brightness";

//level 4
//text_colour_red
  String text_colour_red_menu     PROGMEM           = "Red";
  
//text_colour_green
  String text_colour_green_menu     PROGMEM           = "Green";
  
//text_colour_blue
  String text_colour_blue_menu     PROGMEM           = "Blue";
  
//text_colour_hue
  String text_colour_hue_menu     PROGMEM           = "Hue";

  

};




class Menus{

private:

public:
Menus(){}
  
};

#endif // MENUS_H
