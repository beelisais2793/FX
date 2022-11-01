#ifndef CHANGEABLE_H
#define CHANGEABLE_H

#include <gtkmm.h>
#include <iostream>

class Changeable {
public:

  virtual float get_value() = 0;

  virtual void set_value(float val) = 0;

  virtual void connect(sigc::slot<void> slot) = 0;

  // TODO : is this necessary or should we use inheritance instance?
  virtual Gtk::Widget* get_widget() = 0;

};

#endif //CHANGEABLE_H
