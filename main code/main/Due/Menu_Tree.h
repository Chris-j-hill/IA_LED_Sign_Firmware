#ifndef Menu_H
#define Menu_H

#include "Config_Local.h"
#include "Graphics.h"

#define MENU_VISABLITIY_TIMOUT 15000  //15 seconds until menu will disapear/default to standard view
#define TIME_TO_DISPLAY_STARTUP 5000  // 5 second startup animation


// LEVEL 1
#define STARTUP                       0
#define DEFAULT_MENU                  1
#define MAIN_MENU                     2

//LEVEL 2
#define SCREEN_MODE_MENU              3
#define BRIGHTNESS_MENU               4
#define TEXT_SETTINGS_MENU            5
#define FAN_SETTINGS_MENU             6
#define INTERNET_CONFIG_MENU          7 
#define SD_CARD_MENU                  8
#define LED_STRIP_MENU                9

//LEVEL 3
#define TEXT_SIZE_MENU                10
#define TEXT_COLOUR_MENU              11
#define SCROLL_SPEED_MENU             12
#define FAN_SPEED_MENU                13
#define MIN_FAN_SPEED_MENU            14
#define SD_FOLDERS_MENU               15
#define LED_STRIP_BRIGHTNESS_MENU     16

//#define SELECT_NETWORK_MANUALLY       17

// LEVEL 4
#define TEXT_COLOUR_RED               18
#define TEXT_COLOUR_GREEN             19
#define TEXT_COLOUR_BLUE              20
#define TEXT_COLOUR_HUE               21



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

struct Menu_tree_menu_limits{ // lengths of the menus to limit scroll distances
                              // rule of thumb is number of items + return (unless hidden menus?)
                              // adjustment menus are hard limited by byte 
                              // range (0-255) and use buton to return
  byte main_menu                        = 8;
  byte screen_mode_menu                 = 5;
  byte brightness_menu                  = 100;
  byte text_settings_menu               = 5;
  byte fan_settings_menu                = 5;
  byte internet_config_menu             = 6;
  byte sd_cards_menu                    = 4;
  byte led_strip_menu                   = 4;
  byte text_size_menu                   = 4;
  byte text_colour_menu                 = 4;
  byte scroll_speed_menu                = 255;
  byte fan_speed_menu                   = 100;
  byte minimum_fan_speed_menu           = 255;
  unsigned int SD_card_folders_menu     = 4294967295;    //32 bit number, assume there arent this many files on the card!!!
  byte led_strip_brightness_menu        = 255;
  byte text_colour_red_menu             = RED_MAX_VALUE;
  byte text_colour_green_menu           = GREEN_MAX_VALUE;
  byte text_colour_blue_menu            = BLUE_MAX_VALUE;
  byte text_colour_hue_min              = HUE_MIN_VALUE;
  byte text_colour_hue_max              = HUE_MAX_VALUE;
  
  
  
  
};


class Menu{

private:
  int current_menu = 0;    //use to keep track of where we are. use switch statement to navigate based on this value
  int previous_menu = 0;    //can be used if one menu option may be available from two locations in the tree
  int background_porcess = 1;
  
  
  // menu functions, these dont send menu option text, only pointers to the megas string of the same 
  //NB: menu items stored in LUT on both mega and due for display and feedback over com port note 
  //    menu options sent back irrespective of DEBUG, but rather dependant on NAVIGATE_MENU_OPTIONS_OVER_USB_SERIAL

  //level 1
  void display_startup_sequence(); 
  void default_display();
  void display_main_menu();

  //level 2
  void display_screen_mode_menu();
  void display_brightness_menu();
  void display_text_settings_menu();
  void display_fan_settings_menu();
  void display_internet_config_menu();
  void display_SD_cards_menu();
  void display_led_strip_menu();

  //level 3
  void display_text_size_menu();
  void display_text_colour_menu();
  void display_scroll_speed_menu();
  void display_fan_speed_menu();
  void display_min_fan_speed_menu();
  void display_sd_folder_menu();
  void display_led_strip_brightness_menu();

  //level 4  
  void display_text_colour_red_menu();
  void display_text_colour_green_menu();
  void display_text_colour_blue_menu();
  void display_text_colour_hue_menu();

public:

  Menu(){};
  int init_menu_tree();
  int display_menu();
  int set_menu_width();   //change the width of the menu on the screen
  void LDR_configuration(){};
//  void add_menu_to_send_queue(int menu);  
};


#endif  // Menu_H
