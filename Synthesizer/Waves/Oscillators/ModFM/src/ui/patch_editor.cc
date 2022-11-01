#include "ui/patch_editor.h"

#include <absl/strings/str_format.h>
#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/slider.h>
#include <nanogui/textbox.h>

#include "patch.h"

using namespace nanogui;

GeneratorPatchEditor::GeneratorPatchEditor(nanogui::Widget *parent, int barnum,
                                           GeneratorModel *patch_model)
    : Widget(parent), barnum_(barnum), patch_model_(patch_model) {
  this->set_layout(new BoxLayout(Orientation::Vertical, Alignment::Fill));
  auto *osc_params = new Widget(this);
  osc_params->set_layout(
      new GridLayout(Orientation::Horizontal, 3, Alignment::Fill));
  NumericValueSliderEdit(osc_params, "C", &patch_model_->osc.C, 10);
  NumericValueSliderEdit(osc_params, "A", &patch_model_->osc.A, 1);
  NumericValueSliderEdit(osc_params, "M", &patch_model_->osc.M, 10);
  NumericValueSliderEdit(osc_params, "K", &patch_model_->osc.K, 10);
  new Label(this, "Carrier envelope");
  auto *a_env_params = new Widget(this);
  a_env_params->set_layout(
      new GridLayout(Orientation::Horizontal, 3, Alignment::Fill));
  NumericValueSliderEdit(a_env_params, "AR", &patch_model_->a_env.A_R, 5);
  NumericValueSliderEdit(a_env_params, "AL", &patch_model_->a_env.A_L, 1);
  NumericValueSliderEdit(a_env_params, "D", &patch_model_->a_env.D_R, 5);
  NumericValueSliderEdit(a_env_params, "S", &patch_model_->a_env.S_L, 1);
  NumericValueSliderEdit(a_env_params, "R", &patch_model_->a_env.R_R, 5);
  new Label(this, "Modulator envelope");
  auto *k_env_params = new Widget(this);
  k_env_params->set_layout(
      new GridLayout(Orientation::Horizontal, 3, Alignment::Fill));
  NumericValueSliderEdit(k_env_params, "AR", &patch_model_->k_env.A_R, 5);
  NumericValueSliderEdit(k_env_params, "AL", &patch_model_->k_env.A_L, 1);
  NumericValueSliderEdit(k_env_params, "D", &patch_model_->k_env.D_R, 5);
  NumericValueSliderEdit(k_env_params, "S", &patch_model_->k_env.S_L, 1);
  NumericValueSliderEdit(k_env_params, "R", &patch_model_->k_env.R_R, 5);
}

void GeneratorPatchEditor::set_changed_callback(
    ChangedCallbackFun changed_callback) {
  changed_callback_ = std::move(changed_callback);
}

GeneratorPatchEditor::ChangedCallbackFun
GeneratorPatchEditor::changed_callback() const {
  return changed_callback_;
}

int GeneratorPatchEditor::barnum() const { return barnum_; }

std::function<void(float)> GeneratorPatchEditor::MakeValueSetter(float *field) {
  return [field, this](float f) {
    *field = f;
    SlidersUpdate();

    if (changed_callback_) {
      changed_callback_(barnum_);
    }
  };
}

void GeneratorPatchEditor::NumericValueSliderEdit(Widget *parent,
                                                  const std::string &label,
                                                  float *field,
                                                  float max_range) {
  new Label(parent, label);
  auto slider = new Slider(parent);
  slider->set_range({0, max_range});
  slider->set_value(*field);
  slider->set_callback(MakeValueSetter(field));
  // TODO increment/decrement buttons
  auto *value_box = new TextBox(parent, absl::StrFormat("%.2f", *field));
  value_box->set_editable(true);
  value_box->set_format("[-]?[0-9]*\\.?[0-9]+");
  value_box->set_callback(
      [this, field, slider, max_range](const std::string &v) -> bool {
        try {
          float nval = std::stof(v);
          if (nval > max_range) nval = max_range;
          if (nval < 0) return false;
          *field = nval;
          slider->set_value(nval);
          SlidersUpdate();
          if (changed_callback_) {
            changed_callback_(barnum_);
          }
          return true;
        } catch (const std::exception &e) {
          return false;
        }
      });
  value_boxes_.push_back(std::make_pair(value_box, field));
}

void GeneratorPatchEditor::SlidersUpdate() {
  for (auto &field : value_boxes_) {
    field.first->set_value(absl::StrFormat("%.2f", *field.second));
  }
}
