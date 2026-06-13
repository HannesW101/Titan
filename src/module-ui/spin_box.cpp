// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/spin_box.hpp"

#include "module-ui/include/button.hpp"
#include "module-ui/include/label.hpp"
#include "module-utils/include/math.hpp"

#include <iomanip>
#include <sstream>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

using namespace titan::utils;

Spin_box::Spin_box(std::string const& name) : Widget(name) { set_focusable(true); }

std::string Spin_box::_fmt(float v) const {
    std::ostringstream o; o << std::fixed << std::setprecision(_decimals) << v; return o.str();
}
void Spin_box::_refresh() { if (_value_lbl) { _value_lbl->set_text(_fmt(_val)); } }

void Spin_box::set_range(float a, float b) { _min=a; _max=b; _val=clamp(_val,_min,_max); _refresh(); }
void Spin_box::set_value(float v) { _val=clamp(v,_min,_max); _refresh(); }
float Spin_box::value() const { return _val; }
void Spin_box::set_step(float s) { _step=s; }
void Spin_box::set_decimals(int d) { _decimals=d; _refresh(); }
void Spin_box::set_value_width(UI_length w) { _value_w=w; }
void Spin_box::set_on_changed(std::function<void(float)> cb) { _on_changed=std::move(cb); }
Button* Spin_box::minus_button() { return _minus; }
Button* Spin_box::plus_button()  { return _plus; }
Label*  Spin_box::value_field()  { return _value_lbl; }

void Spin_box::_step_by(float dir) {
    _val = clamp(_val + dir*_step, _min, _max);
    _refresh();
    if (_on_changed) { _on_changed(_val); }
}

void Spin_box::on_layout() { if (!_built) { _build(); _built=true; } }

void Spin_box::_build() {
    set_layout_mode(Layout_mode::HORIZONTAL);
    set_spacing(2.f);
    float const h = rect().size.y;

    auto minus = std::make_unique<Button>(name()+"_minus");
    minus->set_text("-");
    minus->set_size(UI_length::px(h), UI_length::px(h));
    minus->set_on_click([this]{ _step_by(-1.f); });
    _minus = static_cast<Button*>(add_child(std::move(minus)));

    auto val = std::make_unique<Label>(name()+"_value");
    val->set_text(_fmt(_val));
    val->set_align(Text_align::CENTER);
    val->set_size(_value_w, UI_length::px(h));
    _value_lbl = static_cast<Label*>(add_child(std::move(val)));

    auto plus = std::make_unique<Button>(name()+"_plus");
    plus->set_text("+");
    plus->set_size(UI_length::px(h), UI_length::px(h));
    plus->set_on_click([this]{ _step_by(1.f); });
    _plus = static_cast<Button*>(add_child(std::move(plus)));
}

} // namespace titan::ui
