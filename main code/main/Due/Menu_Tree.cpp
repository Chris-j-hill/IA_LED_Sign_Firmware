#ifndef Menu_CPP
#define Menu_CPP

  int init_menu_tree(){
    
  }


  int change_menu(){
    if (MENU_VISABLITIY_TIMOUT>millis()-time_since_menu_last_changed){
      current_menu=1;
    }
    
    switch(current_menu)
    case 0: display_startup_sequence(); break;
    case 1: default_display(); break;
    case 2: display_menu_tree(); break;

    default: current_menu = 0;
  }


#endif  // Menu_CPP
