#ifndef Menu_H
#define Menu_H

#define MENU_VISABLITIY_TIMOUT 15000  //15 seconds until menu will disapear/default to standard view
int time_since_menu_last_changed=0;  //time variable updated from encoder

bool supress_frame_to_this_screen = false;  // use this to supress text frame to screens covered fully by menu
                                            // other screens will require a black box cover if menu visable

bool menu_width = 64;

class Menu{

private
  int current_menu = 0;    //use to keep track of where we are. use switch statement to navigate based on this value
  int previous_menu = 0;    //can be used if one menu option may be available from two locations in the tree
  int background_porcess = 1;
  // menu functions, these dont send menu option text, 
  //NB: menu items stored in LUT on both mega and due for display and feedback over com portnote 
  //    menu options sent back regardless of DEBUG, but rather dependant on NAVIGATE_MENU_OPTIONS_OVER_USB_SERIAL
  void display_startup_sequence(); 
  void default_display();
  void display_menu_tree();

public

  Menu(){};
  int init_menu_tree();
  int change_menu();
  int set_menu_width();   //change the width of the menu on the screen
  
  
}


#endif  // Menu_H
