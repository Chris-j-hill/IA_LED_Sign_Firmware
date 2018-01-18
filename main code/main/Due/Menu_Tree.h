#ifndef Menu_H
#define Menu_H

#include "Config_Local.h"


#define MENU_VISABLITIY_TIMOUT 15000  //15 seconds until menu will disapear/default to standard view

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
  String Flip_dir_settings        PROGMEM           = "Flip Direction";

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
  String SD_cards_menu            PROGMEM           = "SD Cards";
  String enable_ext_card          PROGMEM           = "Enable Port";
  String disable_ext_card         PROGMEM           = "Disable Port";
  String SD_card_folders          PROGMEM           = "Read Folders";

// Led Strip folder
  String Led_strip_menu            PROGMEM           = "Led Strip";
  String enable_led_strip          PROGMEM           = "Enable LED Strip";
  String disable_led_strip         PROGMEM           = "Disable LED Strip";
  String led_strip_brightness      PROGMEM           = "Brightness";



//level 3 folders
//text size
  String text_size_menu           PROGMEM           = "Text Size";

//text colour
  String text_colour_menu         PROGMEM           = "Text Colour";
  String text_colour_red          PROGMEM           = "Red";
  String text_colour_green        PROGMEM           = "Green";
  String text_colour_blue         PROGMEM           = "Blue";

// scroll speed
  String scroll_speed_menu        PROGMEM           = "Scroll Speed";
  
  
};


class Menu{

private:
  int current_menu = 0;    //use to keep track of where we are. use switch statement to navigate based on this value
  int previous_menu = 0;    //can be used if one menu option may be available from two locations in the tree
  int background_porcess = 1;
  // menu functions, these dont send menu option text, 
  //NB: menu items stored in LUT on both mega and due for display and feedback over com portnote 
  //    menu options sent back regardless of DEBUG, but rather dependant on NAVIGATE_MENU_OPTIONS_OVER_USB_SERIAL
  void display_startup_sequence(); 
  void default_display();
  void display_menu_tree();

public:

  Menu(){};
  int init_menu_tree();
  int change_menu();
  int set_menu_width();   //change the width of the menu on the screen
  
  
};


#endif  // Menu_H
