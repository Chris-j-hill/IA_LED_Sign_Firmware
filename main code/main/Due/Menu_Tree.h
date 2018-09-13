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
#define FAN_SPEED_MENU                13
#define MIN_FAN_SPEED_MENU            14
#define SD_FOLDERS_MENU               15
#define LED_STRIP_BRIGHTNESS_MENU     16

//#define SELECT_NETWORK_MANUALLY       17

// LEVEL 4
#define TEXT_SIZE_MENU                10
#define TEXT_COLOUR_MENU              11
#define SCROLL_SPEED_MENU             12


//LEVEL 5
#define TEXT_COLOUR_RED               18
#define TEXT_COLOUR_GREEN             19
#define TEXT_COLOUR_BLUE              20
#define TEXT_COLOUR_HUE               21
#define SCROLL_SPEED_MENU_X           22
#define SCROLL_SPEED_MENU_Y           23


//added later...
#define TEXT_OBJ_SELECTION_MENU       24
#define INTERNET_CONNECTION_MENU      25
#define GIT_SETTING_MENU              26

#define CURRENT_IP_MENU               27
#define CURRENT_NETWORK_MENU          28
#define GIT_AUTHER_MENU               29
#define GIT_COMMIT_MSG_MENU           30
#define GIT_COMMIT_TAG_MENU           31


struct Menu_tree_items {


  String main_menu                      = "Main Menu";

  //level 1 menu items
  String RETURN                         = "RETURN";
  String screen_mode                    = "Screen Mode";
  String brightness                     = "Brightness";
  String text_settings                  = "Text Settings";
  String fan_settings                   = "Fan Settings";
  String internet_settings              = "Internet Config";
  String sd_card_settings               = "Sd Cards";
  String led_strip_settings             = "LED Strip";


  //level 2 menu items
  //change screen mode folder
  String screen_mode_menu               = "Mode Menu";
  String screen_mode0                   = "Both on";
  String screen_mode1                   = "Front Side";
  String screen_mode3                   = "Back Side";
  String screen_mode2                   = "Both Off";

  // brightness folder

  String brightness_menu                = "Brightness";

  //text adjustment folder
  String text_obj_select_menu           = "Select Obj";
  String text_obj_0_menu                = "Obj 0";
  String text_obj_1_menu                = "Obj 1";
  String text_obj_2_menu                = "Obj 2";
  String text_obj_3_menu                = "Obj 3";
  String text_obj_4_menu                = "Obj 4";

  //fan settings folder
  String fan_settings_menu              = "Fan Settings";
  String fan_speed_settings             = "Fan Speed";
  String fan_enable                     = "Enable";
  String fan_disable                    = "Disable";
  String minimum_rotating_speed         = "Minimum speed";

  // internet configuration folder
  String internet_config                = "Internet";
  String select_network_manually        = "Connect";
  String current_network                = "Display current network";
  String wifi_enable                    = "Enable Wifi";
  String wifi_disable                   = "Disable Wifi";
  String git_settings                   = "GitHub";

  // SD cards folder
  String sd_cards_menu                  = "SD Cards";
  String enable_ext_card                = "Mount External Card";
  String disable_ext_card               = "Eject External Card";
  String enable_int_card                = "Mount Internal Card";
  String disable_int_card               = "Eject Internal Card";
  String sd_card_folders                = "Read Folders";

  // Led Strip folder
  String led_strip_menu                 = "Led Strip";
  String enable_led_strip               = "Enable LED Strip";
  String disable_led_strip              = "Disable LED Strip";
  String led_strip_brightness           = "Brightness";


  //level 3 folders
  // text adjustment menu
  String text_settings_menu             = "Text Settings";
  String text_size_settings             = "Text Size";
  String text_colour_settings           = "Text Colour";
  String scroll_speed_settings          = "Scroll Speed";
  String flip_dir_settings              = "Flip Direction";

  //set fan speed
  String fan_speed_menu                 = "Set Fan Speed";

  // fan minimum speed
  String minimum_fan_speed_menu         = "Set Min Speed";

  // read sd card folders
  String SD_card_folders_menu           = "Card Files";

  // led brightness menu
  String led_strip_brightness_menu      = "Set Brightness";

  //level 4
  //text size
  String text_size_menu                 = "Text Size";

  //text colour
  String text_colour_menu               = "Text Colour";
  String text_colour_red                = "Red";
  String text_colour_green              = "Green";
  String text_colour_blue               = "Blue";
  String text_colour_hue                = "Hue";
  String text_colour_use_hue            = "Use Hue";
  String text_colour_use_rgb            = "Use RGB";

  // scroll speed
  String scroll_speed_menu              = "Scroll Speed Menu";
  String scroll_speed_x                 = "X Direction";
  String scroll_speed_y                 = "Y Direction";

  //level 5
  //text_colour_red
  String text_colour_red_menu           = "Red";

  //text_colour_green
  String text_colour_green_menu         = "Green";

  //text_colour_blue
  String text_colour_blue_menu          = "Blue";

  //text_colour_hue
  String text_colour_hue_menu           = "Hue";

  //internet connection menu
  String current_connected_network      = "get current network";
  String current_ip_address             = "get ip address";

  //git settings menu
  String show_git_auther                = "show git auther";
  String show_git_commit_msg            = "show git commit msg";
  String show_git_commit_tag            = "show git commit tag";
  String get_firmware_update            = "get firmware update";


};

struct Menu_tree_menu_limits { // lengths of the menus to limit scroll distances

  byte main_menu                        = 7;
  byte screen_mode_menu                 = 4;
  byte brightness_menu                  = 100;
  byte text_settings_menu               = 3;
  byte fan_settings_menu                = 3;
  byte internet_config_menu             = 5;
  byte sd_cards_menu                    = 3;
  byte led_strip_menu                   = 2;
  byte text_size_menu                   = 4;    //size of text characters
  byte text_colour_menu                 = 6;
  byte scroll_speed_menu                = 2;
  byte fan_speed_menu                   = 100;
  byte minimum_fan_speed_menu           = 255;
  uint16_t SD_card_folders_menu         = 2 ^ 16;  // assume there arent this many files on the card!!!
  byte led_strip_brightness_menu        = 255;
  byte text_colour_red_menu             = RED_MAX_VALUE;
  byte text_colour_green_menu           = GREEN_MAX_VALUE;
  byte text_colour_blue_menu            = BLUE_MAX_VALUE;
  uint16_t text_colour_hue_min          = HUE_MIN_VALUE;
  uint16_t text_colour_hue_max          = HUE_MAX_VALUE;
  byte scroll_speed_menu_x              = 255;
  byte scroll_speed_menu_y              = 255;
  byte select_text_obj_menu             = MAX_NUM_OF_TEXT_OBJECTS;
  byte current_network_config_menu      = 2;
  byte git_settings_menu                = 4;
};


class Menu {

  private:
    byte current_menu = 0;    //use to keep track of where we are. use switch statement to navigate based on this value
    byte previous_menu = 0;    //can be used if one menu option may be available from two locations in the tree

    byte obj_selected = 0; //text obj selected in text settings sub menu
    byte obj_enabled[MAX_NUM_OF_TEXT_OBJECTS] = {0};    //list of enabled text objects eg 1,3,4 are enabled
    byte num_obj_enabled = 0;

    byte menu_width = DEFAULT_MENU_WIDTH;

    //used to select which screen should display data related to the due, no need to push to all
    // variable set in constructor
    byte left_most_address_displaying_menu = 0; // (TOTAL_WIDTH / menu.get_menu_width()) - 1; //  (256/64)-1 = 3 -> (256/65)-1 = 2.9... = 2 etc


    // menu functions, these dont send menu option text, only menu reference variable to the megas
    // NB: menu items stored in LUT on both mega and due for display and feedback over com port

    void check_obj_enabled();

    //level 1
    void display_startup_sequence();
    void default_display();
    void display_main_menu();

    //level 2
    void display_screen_mode_menu();
    void display_screen_brightness_menu();
    void display_text_settings_menu();
    void display_fan_settings_menu();
    void display_internet_config();
    void display_SD_cards_menu();
    void display_led_strip_menu();

    //level 3
    void display_text_obj_selection_menu();
    void display_fan_speed_menu();
    void display_min_fan_speed_menu();
    void display_sd_folder_menu();
    void display_led_strip_brightness_menu();
    void display_current_network_config();
    void display_git_menu();

    //level 4
    void display_text_size_menu();
    void display_text_colour_menu();
    void display_scroll_speed_menu();

    //level 5
    void display_text_colour_red_menu();
    void display_text_colour_green_menu();
    void display_text_colour_blue_menu();
    void display_text_colour_hue_menu();
    void display_text_scroll_speed_x();
    void display_text_scroll_speed_y();


    void show_current_ip_address();
    void show_current_network();
    void show_git_commit_auther();
    void show_git_commit_msg();
    void show_git_commit_tag();

  public:

    Menu() {
      left_most_address_displaying_menu = (TOTAL_WIDTH / get_menu_width()) - 1; //  (256/64)-1 = 3 -> (256/65)-1 = 2.99... = 2 etc
    };
    void init_menu_tree();
    void display_menu();
    inline void set_menu_width(byte width) {
      menu_width = width;   //change the width of the menu on the screen
    }
    inline byte get_menu_width() {
      return menu_width;
    }

    inline byte get_selected_object() {
      return obj_selected;
    }

    inline byte get_current_menu() {
      return current_menu;
    }
    inline byte get_previous_menu() {
      return previous_menu;
    }

    inline bool is_all_system_menu(byte menu) { //return true if test menu is startup or default with previous menu being startup
      return (menu == STARTUP || (menu == DEFAULT_MENU && previous_menu == STARTUP));
    }

    inline void send_cailbration_data_to_megas(byte prefix, byte starting_address = 0, byte ending_address = NUM_SCREENS);

};


#endif  // Menu_H
