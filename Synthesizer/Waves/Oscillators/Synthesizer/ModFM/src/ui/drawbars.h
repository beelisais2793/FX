#pragma once

#include <nanogui/widget.h>

#include <vector>

#include "model.h"

constexpr std::size_t kNumBars = 16;

class Drawbars : public nanogui::Widget {
 public:
  using ClickedCallbackFun = std::function<void(bool, int)>;

  Drawbars(Widget *parent, std::array<GeneratorModel, kNumBars> &bars);
  ClickedCallbackFun clicked_bar_callback() const;
  void set_clicked_bar_callback(ClickedCallbackFun cbf);
  std::function<void(int)> select_bar_callback() const;
  void set_select_bar_callback(std::function<void(int)> sbf);
  float slot_width() const;
  float bar_width() const;
  float label_height() const;
  float toggle_height() const;
  float select_height() const;
  float bar_height() const;
  bool mouse_button_event(const nanogui::Vector2i &p, int button, bool down,
                          int modifiers) override;
  void draw(NVGcontext *ctx) override;
  bool mouse_motion_event(const nanogui::Vector2i &p,
                          const nanogui::Vector2i &rel, int button,
                          int modifiers) override;

 private:
  void bar_click(int bar_num);
  ClickedCallbackFun clicked_bar_cb_;
  std::function<void(int)> select_bar_callback_;

  int x_padding_;

  std::array<GeneratorModel, kNumBars> &bars_;

  int hilighted_bar_ = -1;
  int selected_bar_ = -1;
  bool pushed_;
};
