#ifndef Menu_CPP
#define Menu_CPP

#include "Menu_Tree.h"
#include "Config_Local.h"


Menu_tree_items menu_items;
int time_since_menu_last_changed=0;  //time variable updated from encoder

bool supress_frame_to_this_screen = false;  // use this to supress text frame to screens covered fully by menu
                                            // other screens will require a black box cover if menu visable
bool menu_width = 64;







int Menu::init_menu_tree(){
    
  }


int Menu::change_menu(){
    if (MENU_VISABLITIY_TIMOUT>millis()-time_since_menu_last_changed){
      current_menu=1;
    }
    
    switch(current_menu){
    case 0: {display_startup_sequence(); break;}
    case 1: {default_display(); break;}
    case 2: {display_menu_tree(); break;}

    default: current_menu = 0;
  }
}

void Menu::display_startup_sequence(){} 
void Menu::default_display(){}
void Menu::display_menu_tree(){}


#endif  // Menu_CPP
