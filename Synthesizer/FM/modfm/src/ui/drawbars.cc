#include "ui/drawbars.h"

#include <GLFW/glfw3.h>
#include <absl/strings/str_format.h>
#include <nanogui/opengl.h>
#include <nanogui/vector.h>
#include <nanovg.h>

using namespace nanogui;

Drawbars::Drawbars(Widget *parent, std::array<GeneratorModel, kNumBars> &bars)
    : Widget(parent), x_padding_(4), bars_(bars) {}

Drawbars::ClickedCallbackFun Drawbars::clicked_bar_callback() const {
  return clicked_bar_cb_;
}

void Drawbars::set_clicked_bar_callback(ClickedCallbackFun cbf) {
  clicked_bar_cb_ = cbf;
}

std::function<void(int)> Drawbars::select_bar_callback() const {
  return select_bar_callback_;
}

void Drawbars::set_select_bar_callback(std::function<void(int)> sbf) {
  select_bar_callback_ = sbf;
}

float Drawbars::slot_width() const { return width() / bars_.size(); }

float Drawbars::bar_width() const { return slot_width() - x_padding_; }

float Drawbars::label_height() const { return 16; }

float Drawbars::toggle_height() const { return 16; }

float Drawbars::select_height() const { return 16; }

float Drawbars::bar_height() const {
  return height() - toggle_height() - select_height() - label_height();
}

bool Drawbars::mouse_motion_event(const Vector2i &p, const Vector2i &rel,
                                  int button, int modifiers) {
  if (m_enabled) {
    Vector2f rel_p = p - position();
    int bar_num = rel_p.x() / slot_width();
    hilighted_bar_ = bar_num;
  }
  return Widget::mouse_motion_event(p, rel, button, modifiers);
}

bool Drawbars::mouse_button_event(const Vector2i &p, int button, bool down,
                                  int modifiers) {
  if (!m_enabled) return Widget::mouse_button_event(p, button, down, modifiers);
  ;

  if (button != GLFW_MOUSE_BUTTON_1) {
    return Widget::mouse_button_event(p, button, down, modifiers);
    ;
  }

  if (down) {
    pushed_ = true;
    return true;
  }

  if (!pushed_) {
    return true;
  }

  Vector2f rel_p = p - position();
  if (rel_p.y() <= label_height())
    return Widget::mouse_button_event(p, button, down, modifiers);
  ;

  int bar_num = rel_p.x() / slot_width();
  GeneratorModel &bar = bars_[bar_num];

  // Adjust the position to ignore the label, makes the rest simpler.
  rel_p = rel_p - Vector2f(0, label_height());

  // Inside bar?
  // Find slot position and value
  if (rel_p.y() <= bar_height()) {
    float val = 1.0f - (rel_p.y() / bar_height());

    bar.on = true;
    bar.osc.A = val;

    if (clicked_bar_cb_) clicked_bar_cb_(bar.on, bar_num);

    return true;
  }

  // No, so inside toggle box?
  if (rel_p.y() <= bar_height() + toggle_height()) {
    bar_click(bar_num);
    return true;
  }

  // No, must be at the select button. Mark as selected.
  selected_bar_ = bar_num;

  // If the bar is not active, we should probably do that.
  if (!bar.on) {
    bar_click(bar_num);
  }
  if (select_bar_callback_) {
    select_bar_callback_(bar_num);
  }
  return true;
}

void Drawbars::draw(NVGcontext *ctx) {
  Widget::draw(ctx);

  int x_pos = position().x();
  int y_pos = position().y();

  // Draw label alley.
  nvgBeginPath(ctx);
  nvgFillColor(ctx, theme()->m_window_fill_focused);
  nvgRect(ctx, x_pos, y_pos, width(), label_height());
  nvgFill(ctx);

  for (int bar_num = 0; bar_num < bars_.size(); bar_num++) {
    auto &bar = bars_[bar_num];

    // Draw the harmonic # label.
    nvgBeginPath(ctx);
    nvgTextAlign(ctx, NVG_ALIGN_CENTER);
    nvgFontFace(ctx, "mono");
    nvgFontSize(ctx, 10);
    nvgFillColor(ctx, theme()->m_text_color);
    nvgTextBox(ctx, x_pos, y_pos + label_height(), slot_width(),
               absl::StrFormat("%0.2f", bar.osc.C).c_str(), nullptr);

    // Draw drawbar background.
    int bar_y_start_pos = y_pos + label_height();
    NVGpaint bg = nvgLinearGradient(
        ctx, x_pos, bar_y_start_pos, slot_width(), bar_height(),
        Color(0, bar_num == hilighted_bar_ ? 32 : 10),
        Color(0, bar_num == hilighted_bar_ ? 128 : 210));

    nvgBeginPath(ctx);
    nvgRect(ctx, x_pos, bar_y_start_pos, slot_width(), bar_height());
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    // Draw the drawbar if enabled.
    if (bar.on) {
      float active_height = bar_height() * (1.0f - bar.osc.A);
      float active_start_y = bar_y_start_pos + active_height;
      nvgBeginPath(ctx);
      nvgRect(ctx, x_pos + (x_padding_ / 2), active_start_y, bar_width(),
              bar_height() - active_height);
      nvgFillColor(ctx, Color(255, 80, 80, 70));
      nvgFill(ctx);

      // Render the value inside the bar.
      nvgBeginPath(ctx);
      nvgTextAlign(ctx, NVG_ALIGN_CENTER);
      nvgFontFace(ctx, "mono");
      nvgFontSize(ctx, 10);
      nvgFillColor(ctx, theme()->m_text_color);
      nvgTextBox(ctx, x_pos, active_start_y, slot_width(),
                 absl::StrFormat("%.2f", bar.osc.A).c_str(), nullptr);
    }

    // Draw the toggle box under each bar.
    // Based on nanogui checkbox.cpp
    Vector2f c_pos{x_pos + x_padding_, bar_y_start_pos + bar_height()};
    Vector2f c_size{bar_width(), toggle_height()};
    NVGpaint cbg = nvgBoxGradient(ctx, c_pos.x() + 1.5f, c_pos.y() + 1.5f,
                                  c_size.y() - 2.0f, c_size.y() - 2.0f, 3, 3,
                                  bar.on ? Color(0, 100) : Color(0, 32),
                                  Color(0, 0, 0, 180));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, c_pos.x() + 1.0f, c_pos.y() + 1.0f, c_size.y() - 2.0f,
                   c_size.y() - 2.0f, 3);
    nvgFillPaint(ctx, cbg);
    nvgFill(ctx);

    if (bar.on) {
      nvgFontSize(ctx, icon_scale() * c_size.y());
      nvgFontFace(ctx, "icons");
      nvgFillColor(
          ctx, bar.on ? m_theme->m_icon_color : m_theme->m_disabled_text_color);
      nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
      nvgText(ctx, c_pos.x() + c_size.y() * 0.5f + 1,
              c_pos.y() + c_size.y() * 0.5f,
              utf8(m_theme->m_check_box_icon).data(), nullptr);
    }

    // Draw the select box under each bar that is either being moused over, or
    // is currently selected.
    if (bar_num == hilighted_bar_ || bar_num == selected_bar_) {
      Vector2f s_pos = c_pos + Vector2f{0, toggle_height()};
      Vector2f s_size{bar_width(), select_height()};
      nvgBeginPath(ctx);
      nvgRoundedRect(ctx, s_pos.x() + 1.0f, s_pos.y() + 1.0f, s_size.y() - 2.0f,
                     s_size.y() - 2.0f, 3);
      nvgFillPaint(ctx, cbg);
      nvgFill(ctx);
      nvgFontSize(ctx, icon_scale() * s_size.y());
      nvgFontFace(ctx, "icons");
      nvgFillColor(ctx, selected_bar_ == bar_num
                            ? m_theme->m_icon_color
                            : m_theme->m_disabled_text_color);
      nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
      nvgText(ctx, s_pos.x() + s_size.y() * 0.5f + 1,
              s_pos.y() + s_size.y() * 0.5f,
              utf8(m_theme->m_text_box_down_icon).data(), nullptr);
    }
    x_pos += slot_width();
  }
}

void Drawbars::bar_click(int bar_num) {
  auto &bar = bars_[bar_num];
  bar.on = !bar.on;
  if (bar.on && bar.osc.A == 0.0f) {
    bar.osc.A = 0.8f;
  }

  if (clicked_bar_cb_) clicked_bar_cb_(bar.on, bar_num);
}
