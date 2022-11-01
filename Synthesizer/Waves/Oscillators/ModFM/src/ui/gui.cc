#include "gui.h"

#include <GLFW/glfw3.h>
#include <absl/strings/str_format.h>
#include <glog/logging.h>
#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/opengl.h>
#include <nanogui/renderpass.h>
#include <nanogui/screen.h>
#include <nanogui/shader.h>
#include <nanogui/slider.h>
#include <nanogui/tabwidget.h>
#include <nanogui/vscrollpanel.h>

#include <functional>
#include <sigslot/signal.hpp>

#include "midi.h"
#include "patch.h"
#include "ui/device_selector.h"
#include "ui/drawbars.h"
#include "ui/patch_editor.h"

using namespace nanogui;

class PatchScreen : public nanogui::Screen {
 public:
  PatchScreen(Patch *patch, MIDIReceiver *midi_receiver)
      : Screen(nanogui::Vector2i{640, 480}, "Patch"), patch_(patch) {
    inc_ref();

    Widget *vertical_stack = new Widget(this);
    vertical_stack->set_layout(
        new BoxLayout(Orientation::Vertical, Alignment::Fill, 16));

    auto *top_panel = new Widget(vertical_stack);
    top_panel->set_layout(
        new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));

    new Label(top_panel, "Input device");
    new MIDIDeviceSelector(top_panel, midi_receiver);

    auto *tabs_widget = new TabWidget(vertical_stack);

    auto *editor_area = new Widget(tabs_widget);
    editor_area->set_layout(
        new BoxLayout(Orientation::Vertical, Alignment::Fill, 16));

    tabs_widget->append_tab("Harmonics", editor_area);

    float bar_values[]{
        1 / 8.0, 2 / 8.0, 3 / 8.0, 4 / 8.0, 5 / 8.0, 6 / 8.0, 7 / 8.0, 1.0,
        2.0,     3.0,     4.0,     5.0,     6.0,     7.0,     8.0,     16.0,
    };
    for (int i = 0; i < kNumBars; i++) {
      bars[i] = {false};
      bars[i].osc = GeneratorPatch::Osc{bar_values[i], 0.5, 1.0, 0, 1.0, 0.0};
      bars[i].a_env = kDefaultAmpEnvelope;
      bars[i].k_env = kDefaultCarEnvelope;
    }
    auto drawbars = new Drawbars(editor_area, bars);
    drawbars->set_height(200);
    drawbars->set_width(400);

    drawbars->set_clicked_bar_callback([this](bool on, int barnum) {
      auto it = bars_to_patches_.find(barnum);
      if (on) {
        if (it != bars_to_patches_.end()) {
          it->second->Update(bars[barnum].osc, bars[barnum].a_env,
                             bars[barnum].k_env);
        } else {
          GeneratorPatch *new_generator = patch_->AddGenerator();
          bars_to_patches_[barnum] = new_generator;
          new_generator->Update(bars[barnum].osc, bars[barnum].a_env,
                                bars[barnum].k_env);
        }
      } else {
        CHECK(it != bars_to_patches_.end());
        patch_->RmGenerator(it->second);
        bars_to_patches_.erase(it);
      }
    });
    drawbars->set_select_bar_callback([this, editor_area](int num) {
      if (generator_editor_ != nullptr) {
        // Already editing.
        if (generator_editor_->barnum() == num) return;

        // Remove existing editor view to replace with a new one.
        editor_area->remove_child(generator_editor_);
      }

      const auto &it = bars_to_patches_.find(num);
      CHECK(it != bars_to_patches_.end());

      generator_editor_ =
          new GeneratorPatchEditor(editor_area, num, &bars[num]);
      generator_editor_->set_changed_callback([this](int barnum) {
        auto *patch = bars_to_patches_[barnum];
        if (patch)
          patch->Update(bars[barnum].osc, bars[barnum].a_env,
                        bars[barnum].k_env);
      });
      perform_layout();
      redraw();
    });
    perform_layout();

    render_pass_ = new nanogui::RenderPass({this});
    shader_ = new nanogui::Shader(render_pass_, "modfm_shader",
                                  // Vertex shader
                                  R"(#version 330
            uniform mat4 mvp;
            in vec3 position;
            in vec3 color;
            out vec4 frag_color;
            void main() {
                frag_color = vec4(color, 1.0);
                gl_Position = mvp * vec4(position, 1.0);
            })",

                                  // Fragment shader
                                  R"(#version 330
            out vec4 color;
            in vec4 frag_color;
            void main() {
                color = frag_color;
            })");
  }

 private:
  Patch *patch_;
  std::array<GeneratorModel, kNumBars> bars;
  GeneratorPatchEditor *generator_editor_ = nullptr;
  std::unordered_map<int, GeneratorPatch *> bars_to_patches_;
  nanogui::RenderPass *render_pass_;
  nanogui::Shader *shader_;
};

GUI::GUI(Patch *patch, MIDIReceiver *midi_receiver)
    : patch_(patch), midi_receiver_(midi_receiver) {}

void GUI::Start() {
  StartUI();
  LOG(INFO) << "GUI started...";
}

void GUI::Stop() {
  if (nanogui::active()) {
    nanogui::shutdown();
  }
}

bool GUI::running() const { return nanogui::active(); }

GUI::~GUI() { Stop(); }

void GUI::StartUI() {
  try {
    nanogui::init();

    nanogui::ref<PatchScreen> patch_screen =
        new PatchScreen(patch_, midi_receiver_);
    patch_screen->draw_all();
    patch_screen->set_visible(true);
    nanogui::mainloop(1 / 60.f * 1000);
    nanogui::shutdown();
  } catch (const std::exception &e) {
    LOG(ERROR) << "Caught fatal error: " << e.what();
  }
}
