

#ifndef MENUS_H
#define MENUS_H
#include "Local_Config.h"
#include "Arduino.h"
#include <avr/pgmspace.h>



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

#define SELECT_NETWORK_MANUALLY       17

// LEVEL 4
#define TEXT_COLOUR_RED               18
#define TEXT_COLOUR_GREEN             19
#define TEXT_COLOUR_BLUE              20
#define TEXT_COLOUR_HUE               21

#define NULL_STRING                   22
#define RETURN_MENU                   23

#define SCREEN_MODE_0                 24
#define SCREEN_MODE_1                 25
#define SCREEN_MODE_2                 26
#define SCREEN_MODE_3                 27







#define STARTUP_R 1   //expanding rings startup, blue/purple colour
#define STARTUP_G 0
#define STARTUP_B 5
#define MENU_OPTION_R 4
#define MENU_OPTION_G 0
#define MENU_OPTION_B 2
#define MENU_TITLE_R 5
#define MENU_TITLE_G 5
#define MENU_TITLE_B 5


#define MENU_POINTER_X0 1   //NOTE: x coordinates are relative to left side of menu area
#define MENU_POINTER_Y0 17
#define MENU_POINTER_X1 1
#define MENU_POINTER_Y1 17+9
#define MENU_POINTER_X2 5
#define MENU_POINTER_Y2 17+5

#define MENU_POINTER_COLOUR_R 5
#define MENU_POINTER_COLOUR_G 5
#define MENU_POINTER_COLOUR_B 5

#define STARTUP_RING_EXPANSION_RATE 100
#define STARTUP_RING_MAX_RADIUS 150 // max number of pixels the ring will expand to, save time rendering if its definitely off the screen


//struct for menu items that need to be displayable but not directly used by the megas
struct Menu_Struct{
  byte current1 =0;
  byte current2 =0;
  byte light1 = 0;
  byte light2 = 0;
  byte temp1 = 0;
  byte temp2 = 0;
  byte temp3 = 0;
  byte fan_speed = 0;
  byte led_strip_brightness= 0;
  bool sd_card1_detected = false;
  bool sd_card2_detected = false;
  bool ethernet_connected = false;
  bool wifi_connected = false;
  bool debug = false;

  uint32_t sensor_value_received =0;    //use as array of 32 bits to detect if value has been transmitted
                                        // bits to save space, bool is 8 bits so wasteful 


//  to implement
  byte scroll_speed = 0;
  byte min_fan_speed=0;                                      
};

struct Menu_tree_items{

//level 1 menu items  
  const char main_menu[]                PROGMEM           = "Main Menu";
  const char RETURN[]                   PROGMEM           = "RETURN";
  const char screen_mode[]              PROGMEM           = "Screen Mode";
  const char brightness[]               PROGMEM           = "Brightness";
  const char text_settings[]            PROGMEM           = "Text Settings";
  const char fan_settings[]             PROGMEM           = "Fan Settings";
  const char internet_settings[]        PROGMEM           = "Internet Config";
  const char sd_card_settings[]         PROGMEM           = "Sd Cards"; 
  const char led_strip_settings[]       PROGMEM           = "LED Strip";


//level 2 menu items
//change mode folder
  const char screen_mode_menu[]         PROGMEM           = "Mode Menu";
  const char screen_mode0[]             PROGMEM           = "Both on";
  const char screen_mode1[]             PROGMEM           = "Front Side";
  const char screen_mode3[]             PROGMEM           = "Back Side";
  const char screen_mode2[]             PROGMEM           = "Both Off";

// brightness folder

  const char brightness_menu[]          PROGMEM           = "Brightness";  

//text adjustment folder
  const char text_settings_menu[]       PROGMEM           = "Text Settings";
  const char text_size_settings[]       PROGMEM           = "Text Size";
  const char text_colour_settings[]     PROGMEM           = "Text Colour";
  const char scroll_speed_settings[]    PROGMEM           = "Scroll Speed";
  const char flip_dir_settings[]        PROGMEM           = "Flip Direction";

//fan settings folder
  const char fan_settings_menu[]        PROGMEM           = "Fan Settings";
  const char fan_speed_settings[]       PROGMEM           = "Fan Speed";
  const char fan_enable[]               PROGMEM           = "Enable";
  const char fan_disable[]              PROGMEM           = "Disable";
  const char minimum_rotating_speed[]   PROGMEM           = "Minimum speed";

// internet configuration folder  
  const char internet_config_menu[]     PROGMEM           = "Internet";
  const char select_network_manually[]  PROGMEM           = "Connect To...";
  const char ethernet_enable[]          PROGMEM           = "Enable Eth";
  const char ethernet_disable[]         PROGMEM           = "Disable Eth";
  const char wifi_enable[]              PROGMEM           = "Enable Wifi";
  const char wifi_disable[]             PROGMEM           = "Disable Wifi";
  
// SD cards folder
  const char sd_cards_menu[]            PROGMEM           = "SD Cards";
  const char enable_ext_card[]          PROGMEM           = "Enable Port";
  const char disable_ext_card[]         PROGMEM           = "Disable Port";
  const char sd_card_folders[]          PROGMEM           = "Read Folders";

// Led Strip folder
  const char led_strip_menu[]           PROGMEM           = "Led Strip";
  const char enable_led_strip[]         PROGMEM           = "Enable LED Strip";
  const char disable_led_strip[]        PROGMEM           = "Disable LED Strip";
  const char led_strip_brightness[]     PROGMEM           = "Brightness";


//level 3 folders
//text size
  const char text_size_menu[]           PROGMEM           = "Text Size";

//text colour
  const char text_colour_menu[]         PROGMEM           = "Text Colour";
  const char text_colour_red[]          PROGMEM           = "Red";
  const char text_colour_green[]        PROGMEM           = "Green";
  const char text_colour_blue[]         PROGMEM           = "Blue";
  const char text_colour_hue[]          PROGMEM           = "Hue";
  const char text_colour_use_hue[]      PROGMEM           = "Use Hue";
  const char text_colour_use_rgb[]      PROGMEM           = "Use RGB";


// scroll speed
  const char scroll_speed_menu[]        PROGMEM           = "Scroll Speed";

//set fan speed
  const char fan_speed_menu[]           PROGMEM           = "Set Fan Speed";
  
// fan minimum speed
  const char minimum_fan_speed_menu[]   PROGMEM           = "Set Min Speed";  

// read sd card folders 
  const char SD_card_folders_menu[]     PROGMEM           = "Card Files";

// led brightness menu
  const char led_strip_brightness_menu[]   PROGMEM        = "Set Brightness";

//level 4
//text_colour_red
  const char text_colour_red_menu[]     PROGMEM           = "Red";
  
//text_colour_green
  const char text_colour_green_menu[]     PROGMEM           = "Green";
  
//text_colour_blue
  const char text_colour_blue_menu[]     PROGMEM           = "Blue";
  
//text_colour_hue
  const char text_colour_hue_menu[]     PROGMEM           = "Hue";

//dummy string
  const char null_string[]              PROGMEM           = " ";    //empty string to display in case less than three options in subfolder 
  const char default_string[]           PROGMEM           = "Error: no string found"; // default in write_menu_option function 
  const char default_title[]            PROGMEM           = "Error: no title found"; // default in write_menu_option function 

};

struct Menu_colour_struct{
  byte red = DEFAULT_TEXT_RED_BRIGHTNESS;    //default is bright white, set to correct colour on initialisation
  byte green = DEFAULT_TEXT_GREEN_BRIGHTNESS;
  byte blue = DEFAULT_TEXT_BLUE_BRIGHTNESS;
};


class Menu{
  
private:
  
  byte current_menu = DEFAULT_MENU;
  void display_background_text();  
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
  
  Menu(){}

  int init_menu_tree();
  void display_menu();;



  
};

#endif // MENUS_H
