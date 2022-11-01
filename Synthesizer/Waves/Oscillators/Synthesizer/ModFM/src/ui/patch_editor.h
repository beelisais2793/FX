#pragma once

#include <nanogui/widget.h>

#include "model.h"

class GeneratorPatchEditor : public nanogui::Widget {
 public:
  GeneratorPatchEditor(Widget *parent, int barnum, GeneratorModel *patch_model);

  using ChangedCallbackFun = std::function<void(int)>;
  void set_changed_callback(ChangedCallbackFun changed_callback);

  ChangedCallbackFun changed_callback() const;

  int barnum() const;

 private:
  std::function<void(float)> MakeValueSetter(float *field);

  void NumericValueSliderEdit(nanogui::Widget *parent, const std::string &label,
                              float *field, float max_range = 1.0f);

  void SlidersUpdate();

  const int barnum_;
  ChangedCallbackFun changed_callback_;
  GeneratorModel *patch_model_;

  std::vector<std::pair<nanogui::TextBox *, float *>> value_boxes_;
};