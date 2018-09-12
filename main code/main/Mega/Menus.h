

#ifndef MENUS_H
#define MENUS_H
#include "Local_Config.h"
#include "Arduino.h"
//#include <avr/pgmspace.h>



#define MENU_VISABLITIY_TIMOUT 15000  //15 seconds until menu will disapear/default to standard view
#define TIME_TO_DISPLAY_STARTUP 5000  // 5 second startup animation

/*
   menu numbers, these must be the same as on the due,
   menus number passed in menu frame, will be matched to these values
*/
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

#define SCROLL_SPEED_MENU_X           22
#define SCROLL_SPEED_MENU_Y           23

#define TEXT_OBJ_SELECTION_MENU       24
#define INTERNET_CONNECTION_MENU      25
#define GIT_SETTING_MENU              26

#define CURRENT_IP_MENU               27
#define CURRENT_NETWORK_MENU          28
#define GIT_AUTHER_MENU               29
#define GIT_COMMIT_MSG_MENU           30
#define GIT_COMMIT_TAG_MENU           31



//aditional menus to be printed on screen
#define NULL_STRING                   54
#define RETURN_MENU                   55
#define SCREEN_MODE_0                 56
#define SCREEN_MODE_1                 57
#define SCREEN_MODE_2                 58
#define SCREEN_MODE_3                 59
#define ENABLE_FAN                    60
#define DISABLE_FAN                   61
#define ENABLE_LED_STRIP              62
#define DISABLE_LED_STRIP             63
#define USE_RGB                       64
#define USE_HUE                       65
#define MOUNT_CARD1                   66
#define UNMOUNT_CARD1                 67
#define MOUNT_CARD2                   68
#define UNMOUNT_CARD2                 69


#define TEXT_OBJ_0                    100   //increase this to N objects
#define TEXT_OBJ_1                    101
#define TEXT_OBJ_2                    102
#define TEXT_OBJ_3                    103
#define TEXT_OBJ_4                    104






#if defined(USING_COLOUR_SET_888)

#define STARTUP_R 35   //expanding rings startup, blue/purple colour
#define STARTUP_G 0
#define STARTUP_B 180

#define MENU_OPTION_R 0 // menu option colour
#define MENU_OPTION_G 0   // NB highlighted colour for selected menu option is these values + COLOUR_LEVEL_STEP_SIZE, so values shouldnt be too close to colour limit
#define MENU_OPTION_B 255

#define MENU_TITLE_R 0
#define MENU_TITLE_G 0
#define MENU_TITLE_B 255

#define MENU_POINTER_COLOUR_R 0
#define MENU_POINTER_COLOUR_G 180
#define MENU_POINTER_COLOUR_B 180

#define MENU_TITLE_BACKGROUND_R 8
#define MENU_TITLE_BACKGROUND_G 0
#define MENU_TITLE_BACKGROUND_B 8

#elif defined(USING_COLOUR_SET_444)

#define STARTUP_R 2
#define STARTUP_G 0
#define STARTUP_B 10
#define MENU_OPTION_R 8
#define MENU_OPTION_G 0
#define MENU_OPTION_B 4
#define MENU_TITLE_R 10
#define MENU_TITLE_G 10
#define MENU_TITLE_B 10

#define MENU_POINTER_COLOUR_R 0
#define MENU_POINTER_COLOUR_G 8
#define MENU_POINTER_COLOUR_B 8

#define MENU_TITLE_BACKGROUND_R 1
#define MENU_TITLE_BACKGROUND_G 0
#define MENU_TITLE_BACKGROUND_B 2
#else

#define STARTUP_R 1
#define STARTUP_G 0
#define STARTUP_B 5

#define MENU_OPTION_R 4
#define MENU_OPTION_G 5
#define MENU_OPTION_B 5

#define MENU_TITLE_R 5
#define MENU_TITLE_G 7
#define MENU_TITLE_B 7

#define MENU_POINTER_COLOUR_R 0
#define MENU_POINTER_COLOUR_G 4
#define MENU_POINTER_COLOUR_B 4

#define MENU_TITLE_BACKGROUND_R 1
#define MENU_TITLE_BACKGROUND_G 0
#define MENU_TITLE_BACKGROUND_B 1
#endif



//triangle pointer co-ordinates
#define MENU_POINTER_X0 0   //NOTE: x coordinates are relative to left side of menu area
#define MENU_POINTER_Y0 16
#define MENU_POINTER_X1 0
#define MENU_POINTER_Y1 16+6
#define MENU_POINTER_X2 5
#define MENU_POINTER_Y2 16+3


#define STARTUP_RING_EXPANSION_RATE 100
#define STARTUP_RING_MAX_RADIUS 150 // max number of pixels the ring will expand to, save time rendering if its definitely off the screen

#define MAIN_MENU_STRING                    "Main Menu"
#define RETURN_STRING                       "RETURN"
#define SCREEN_MODE_STRING                  "Screen Mode"
#define SCREEN_BRIGHTNESS_STRING            "Brightness"
#define TEXT_SETTINGS_STRING                "Text Settings"

#define FAN_SETTINGS_STRING                 "Fan Settings"
#define INTERNET_CONFIG_STRING              "Internet Config"
#define SD_CARDS_STRING                     "Sd Cards"
#define LED_STRIP_STRING                    "LED Strip"

#define BOTH_ON_STRING                      "Both on"
#define FRONT_SIDE_STRING                   "Front Side"
#define BACK_SIDE_STRING                    "Back Side"
#define BOTH_OFF_STRING                     "Both Off"

#define TEXT_SIZE_STRING                    "Text Size"
#define TEXT_COLOUR_STRING                  "Text Colour"
#define SCROLL_SPEED_STRING                 "Scroll Speed"
#define FLIP_DIR_STRING                     "Flip Direction"


#define FAN_SPEED_STRING                    "Fan Speed"
#define ENABLE_STRING                       "Enable"
#define DISABLE_STRING                      "Disable"
#define FAN_MIN_SPEED_STRING                "Minimum speed"

#define READ_FOLDERS_STRING                 "Read Folders"

#define STRIP_BRIGHTNESS_STRING             "Brightness"

#define RED_STRING                          "Red"
#define GREEN_STRING                        "Green"
#define BLUE_STRING                         "Blue"
#define HUE_STRING                          "Hue"
#define USE_HUE_STRING                      "Use Hue"
#define USE_RGB_STRING                      "Use RGB"

#define X_DIR_STRING                        "X Direction"
#define Y_DIR_STRING                        "Y Direction"


#define EMPTY_STRING                         " "
#define CONTENT_ERROR_STRING                "Error: no string found"// default in write_menu_option function
#define TITLE_ERROR_STRING                  "Error: no title found" // default in write_menu_option function


#define SIZEOF_MAIN_MENU_STRING               sizeof(MAIN_MENU_STRING)
#define SIZEOF_RETURN_STRING                  sizeof(RETURN_STRING)
#define SIZEOF_SCREEN_MODE_STRING             sizeof(SCREEN_MODE_STRING)
#define SIZEOF_SCREEN_BRIGHTNESS_STRING       sizeof(SCREEN_BRIGHTNESS_STRING)
#define SIZEOF_TEXT_SETTINGS_STRING           sizeof(TEXT_SETTINGS_STRING)

#define SIZEOF_FAN_SETTINGS_STRING            sizeof(FAN_SETTINGS_STRING)
#define SIZEOF_INTERNET_CONFIG_STRING         sizeof(INTERNET_CONFIG_STRING)
#define SIZEOF_SD_CARDS_STRING                sizeof(SD_CARDS_STRING)
#define SIZEOF_LED_STRIP_STRING               sizeof(LED_STRIP_STRING)

#define SIZEOF_BOTH_ON_STRING                 sizeof(BOTH_ON_STRING)
#define SIZEOF_FRONT_SIDE_STRING              sizeof(FRONT_SIDE_STRING)
#define SIZEOF_BACK_SIDE_STRING               sizeof(BACK_SIDE_STRING)
#define SIZEOF_BOTH_OFF_STRING                sizeof(BOTH_OFF_STRING)

#define SIZEOF_TEXT_SIZE_STRING               sizeof(TEXT_SIZE_STRING)
#define SIZEOF_TEXT_COLOUR_STRING             sizeof(TEXT_COLOUR_STRING)
#define SIZEOF_SCROLL_SPEED_STRING            sizeof(SCROLL_SPEED_STRING)
#define SIZEOF_FLIP_DIR_STRING                sizeof(FLIP_DIR_STRING)
#define SIZEOF_FAN_SPEED_STRING               sizeof(FAN_SPEED_STRING)
#define SIZEOF_ENABLE_STRING                  sizeof(ENABLE_STRING)
#define SIZEOF_DISABLE_STRING                 sizeof(DISABLE_STRING)
#define SIZEOF_FAN_MIN_SPEED_STRING           sizeof(FAN_MIN_SPEED_STRING)

#define SIZEOF_READ_FOLDERS_STRING            sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_STRIP_BRIGHTNESS_STRING        sizeof(FAN_MIN_SPEED_STRING)

#define SIZEOF_RED_STRING                     sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_GREEN_STRING                   sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_BLUE_STRING                    sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_HUE_STRING                     sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_USE_HUE_STRING                 sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_USE_RGB_STRING                 sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_X_DIR_STRING                   sizeof(FAN_MIN_SPEED_STRING)
#define SIZEOF_Y_DIR_STRING                   sizeof(FAN_MIN_SPEED_STRING)

#define SIZEOF_EMPTY_STRING                   sizeof(EMPTY_STRING)
#define SIZEOF_CONTENT_ERROR_STRING           sizeof(CONTENT_ERROR_STRING)
#define SIZEOF_TITLE_ERROR_STRING             sizeof(TITLE_ERROR_STRING)


#define SCROLL_UP 0
#define SCROLL_DOWN 1

struct Menu_tree_items {

  //level 1 menu items
  const char main_menu[]                PROGMEM           = MAIN_MENU_STRING;
  const char RETURN[]                   PROGMEM           = RETURN_STRING;
  const char screen_mode[]              PROGMEM           = SCREEN_MODE_STRING;
  const char brightness[]               PROGMEM           = SCREEN_BRIGHTNESS_STRING;
  const char text_settings[]            PROGMEM           = TEXT_SETTINGS_STRING;
  const char fan_settings[]             PROGMEM           = FAN_SETTINGS_STRING;
  const char internet_settings[]        PROGMEM           = INTERNET_CONFIG_STRING;
  const char sd_card_settings[]         PROGMEM           = SD_CARDS_STRING;
  const char led_strip_settings[]       PROGMEM           = LED_STRIP_STRING;


  //level 2 menu items
  //change mode folder
  const char screen_mode0[]             PROGMEM           = BOTH_ON_STRING;
  const char screen_mode1[]             PROGMEM           = FRONT_SIDE_STRING;
  const char screen_mode3[]             PROGMEM           = BACK_SIDE_STRING;
  const char screen_mode2[]             PROGMEM           = BOTH_OFF_STRING;

  //screen brightness folder
  const char brightness_menu[]          PROGMEM           = SCREEN_BRIGHTNESS_STRING;

  //text adjustment folder
  const char text_settings_menu[]       PROGMEM           = TEXT_SETTINGS_STRING;
  const char text_size_settings[]       PROGMEM           = TEXT_SIZE_STRING;
  const char text_colour_settings[]     PROGMEM           = TEXT_COLOUR_STRING;
  const char scroll_speed_settings[]    PROGMEM           = SCROLL_SPEED_STRING;
  const char flip_dir_settings[]        PROGMEM           = FLIP_DIR_STRING;

  //fan settings folder
  const char fan_settings_menu[]        PROGMEM           = FAN_SETTINGS_STRING;
  const char fan_speed_settings[]       PROGMEM           = FAN_SPEED_STRING;
  const char fan_enable[]               PROGMEM           = ENABLE_STRING;
  const char fan_disable[]              PROGMEM           = DISABLE_STRING;
  const char minimum_rotating_speed[]   PROGMEM           = FAN_MIN_SPEED_STRING;

  // internet configuration folder
  const char internet_config_menu[]     PROGMEM           = "Internet";
  const char select_network_manually[]  PROGMEM           = "Connect To...";
  const char ethernet_enable[]          PROGMEM           = "Enable Eth";
  const char ethernet_disable[]         PROGMEM           = "Disable Eth";
  const char wifi_enable[]              PROGMEM           = "Enable Wifi";
  const char wifi_disable[]             PROGMEM           = "Disable Wifi";

  // SD cards folder
  const char sd_cards_menu[]            PROGMEM           = SD_CARDS_STRING;
  const char enable_ext_card[]          PROGMEM           = ENABLE_STRING;
  const char disable_ext_card[]         PROGMEM           = DISABLE_STRING;
  const char sd_card_folders[]          PROGMEM           = READ_FOLDERS_STRING;

  // Led Strip folder
  const char led_strip_menu[]           PROGMEM           = LED_STRIP_STRING;
  const char enable_led_strip[]         PROGMEM           = ENABLE_STRING;
  const char disable_led_strip[]        PROGMEM           = DISABLE_STRING;
  const char led_strip_brightness[]     PROGMEM           = STRIP_BRIGHTNESS_STRING;


  //level 3 folders
  //text size
  const char text_size_menu[]           PROGMEM           = TEXT_SIZE_STRING;

  //text colour
  const char text_colour_menu[]         PROGMEM           = TEXT_COLOUR_STRING;
  const char text_colour_red[]          PROGMEM           = RED_STRING;
  const char text_colour_green[]        PROGMEM           = GREEN_STRING;
  const char text_colour_blue[]         PROGMEM           = BLUE_STRING;
  const char text_colour_hue[]          PROGMEM           = HUE_STRING;
  const char text_colour_use_hue[]      PROGMEM           = USE_HUE_STRING;
  const char text_colour_use_rgb[]      PROGMEM           = USE_RGB_STRING;

  // scroll speed
  const char scroll_speed_menu[]        PROGMEM           = SCROLL_SPEED_STRING;
  const char scroll_speed_x[]           PROGMEM           = X_DIR_STRING;
  const char scroll_speed_y[]           PROGMEM           = Y_DIR_STRING;

  //set fan speed
  const char fan_speed_menu[]           PROGMEM           = FAN_SPEED_STRING;

  // fan minimum speed
  const char minimum_fan_speed_menu[]   PROGMEM           = FAN_MIN_SPEED_STRING;

  // read sd card folders
  const char SD_card_folders_menu[]     PROGMEM           = READ_FOLDERS_STRING;

  // led brightness menu
  const char led_strip_brightness_menu[]   PROGMEM        = STRIP_BRIGHTNESS_STRING;

  //level 4
  //text_colour_red
  const char text_colour_red_menu[]     PROGMEM           = RED_STRING;

  //text_colour_green
  const char text_colour_green_menu[]     PROGMEM         = GREEN_STRING;

  //text_colour_blue
  const char text_colour_blue_menu[]     PROGMEM          = BLUE_STRING;

  //text_colour_hue
  const char text_colour_hue_menu[]     PROGMEM           = HUE_STRING;

  //dummy string
  const char null_string[]              PROGMEM           = EMPTY_STRING;
  const char default_string[]           PROGMEM           = CONTENT_ERROR_STRING; // default in write_menu_option function
  const char default_title[]            PROGMEM           = TITLE_ERROR_STRING; // default in write_menu_option function

};

struct Menu_Tree_Item_Lengths {
  byte main_menu                PROGMEM           = SIZEOF_MAIN_MENU_STRING;
  byte RETURN                   PROGMEM           = SIZEOF_RETURN_STRING;
  byte screen_mode              PROGMEM           = SIZEOF_SCREEN_MODE_STRING;
  byte brightness               PROGMEM           = SIZEOF_SCREEN_BRIGHTNESS_STRING;
  byte text_settings            PROGMEM           = SIZEOF_TEXT_SETTINGS_STRING;
  byte fan_settings             PROGMEM           = SIZEOF_FAN_SETTINGS_STRING;
  byte internet_settings        PROGMEM           = SIZEOF_INTERNET_CONFIG_STRING;
  byte sd_card_settings         PROGMEM           = SIZEOF_SD_CARDS_STRING;
  byte led_strip_settings       PROGMEM           = SIZEOF_LED_STRIP_STRING;

  byte screen_mode0             PROGMEM           = SIZEOF_BOTH_ON_STRING;
  byte screen_mode1             PROGMEM           = SIZEOF_FRONT_SIDE_STRING;
  byte screen_mode3             PROGMEM           = SIZEOF_BACK_SIDE_STRING;
  byte screen_mode2             PROGMEM           = SIZEOF_BOTH_OFF_STRING;
  byte text_size_settings       PROGMEM           = SIZEOF_TEXT_SIZE_STRING;
  byte text_colour_settings     PROGMEM           = SIZEOF_TEXT_COLOUR_STRING;
  byte scroll_speed_settings    PROGMEM           = SIZEOF_SCROLL_SPEED_STRING;
  byte flip_dir_settings        PROGMEM           = SIZEOF_FLIP_DIR_STRING;
  byte fan_speed_settings       PROGMEM           = SIZEOF_FAN_SPEED_STRING;
  byte fan_enable               PROGMEM           = SIZEOF_ENABLE_STRING;
  byte fan_disable              PROGMEM           = SIZEOF_DISABLE_STRING;
  byte minimum_rotating_speed   PROGMEM           = SIZEOF_FAN_MIN_SPEED_STRING;


  byte sd_cards_menu            PROGMEM           = SIZEOF_SD_CARDS_STRING;
  byte enable_ext_card          PROGMEM           = SIZEOF_ENABLE_STRING;
  byte disable_ext_card         PROGMEM           = SIZEOF_ENABLE_STRING;
  byte sd_card_folders          PROGMEM           = SIZEOF_READ_FOLDERS_STRING;
  byte led_strip_menu           PROGMEM           = SIZEOF_LED_STRIP_STRING;
  byte led_strip_brightness     PROGMEM           = SIZEOF_STRIP_BRIGHTNESS_STRING;
  byte led_strip_enable         PROGMEM           = SIZEOF_ENABLE_STRING;
  byte led_strip_disable        PROGMEM           = SIZEOF_DISABLE_STRING;  
  byte text_colour_red          PROGMEM           = SIZEOF_RED_STRING;
  byte text_colour_green        PROGMEM           = SIZEOF_GREEN_STRING;
  byte text_colour_blue         PROGMEM           = SIZEOF_BLUE_STRING;
  byte text_colour_hue          PROGMEM           = SIZEOF_HUE_STRING;
  byte text_colour_use_hue      PROGMEM           = SIZEOF_USE_HUE_STRING;
  byte text_colour_use_rgb      PROGMEM           = SIZEOF_USE_RGB_STRING;
  byte scroll_speed_x           PROGMEM           = SIZEOF_X_DIR_STRING;
  byte scroll_speed_y           PROGMEM           = SIZEOF_Y_DIR_STRING;


  byte null_string              PROGMEM           = SIZEOF_EMPTY_STRING;
  byte default_string           PROGMEM           = SIZEOF_CONTENT_ERROR_STRING; // default in write_menu_option function
  byte default_title            PROGMEM           = SIZEOF_TITLE_ERROR_STRING; // default in write_menu_option function


};





//struct for menu items that need to be displayable but not directly used by the megas
struct Menu_Struct {

  byte current1 = 0;
  byte current2 = 0;

  byte light1 = 0;
  byte light2 = 0;

  byte temp1 = 0;
  byte temp2 = 0;
  byte temp3 = 0;

  byte fan_speed = 0;
  byte min_fan_speed = 0;
  bool fan_enabled = false;

  byte led_strip_brightness = 0;
  bool led_strip_enabled = false;

  bool sd_card1_detected = false;
  bool sd_card2_detected = false;

  bool ethernet_connected = false;
  bool wifi_connected = false;

  bool sd_card1_mounted = false;
  bool sd_card2_mounted = false;

  int encoder_position = 0;
  int last_encoder_pos = -10000; //random num, so wont equal encoder pos on startup

  byte obj_selected = 0;

};


struct Menu_colour_struct {
  byte red = DEFAULT_TEXT_RED_BRIGHTNESS;    //default is bright white, set to correct colour on initialisation
  byte green = DEFAULT_TEXT_GREEN_BRIGHTNESS;
  byte blue = DEFAULT_TEXT_BLUE_BRIGHTNESS;
};


class Menu {

  private:

    byte current_menu = DEFAULT_MENU;
    byte previous_menu = current_menu + 1;

    void clear_background_text(); //clear area for menu

    // all menus containing sub menu items (ie not adjustment menus)

    //level 1
    void display_startup_sequence();
    void default_display();
    void display_main_menu();

    //level 2
    void display_screen_mode_menu();
    void display_screen_brightness_menu();
    void display_fan_settings_menu();
    void display_internet_config_menu(){}
    void display_SD_cards_menu();
    void display_led_strip_menu();
    void display_text_obj_selection_menu();

    //level 3
    void display_text_settings_menu();
    void display_sd_folder_menu() {}

    //level 4
    void display_text_colour_menu();
    void display_scroll_speed_menu();

    // takes care of all adjustment menus
    inline void display_adjustment_menu(byte menu);


    void scroll_main_menu(byte dir);
    void scroll_screen_mode_menu(byte dir);
    void scroll_text_settings_menu(byte dir);
    void scroll_fan_settings_menu(byte dir);
    void scroll_internet_config_menu(byte dir){}
    void scroll_SD_cards_menu(byte dir);
    void scroll_led_strip_menu(byte dir);
    void scroll_text_colour_menu(byte dir);
    void scroll_scroll_speed_menu(byte dir);
    void scroll_sd_folder_menu(byte dir){}
    void scroll_text_obj_selection_menu(byte dir);

    inline bool menu_is_adjustment_menu(byte test_menu);


  public:

    int current_menu_limit_min = 0;        // <- set from most recent menu frame, save ram by not hard coding these for all atributes
    int current_menu_limit_max = 0;

    Menu() {}

    void init_menu_tree();
    void display_menu();

    byte get_current_menu() {
      return current_menu;
    }
    void set_current_menu(byte cur_menu) {
      if (cur_menu != current_menu) previous_menu = current_menu;
      current_menu = cur_menu;
    }
    byte get_previous_menu() {
      return previous_menu;
    }



};

#endif // MENUS_H
