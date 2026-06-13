#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace titan::ui {

class Radio_group;
// ============================================================================
// Class Radio_button
// ----------------------------------------------------------------------------

/** @brief One option in a Radio_group. */
class Radio_button final : public Widget {

public:
    /**
     * @brief Construct a Radio_button.
     * @param name Widget name (unique within its parent)
     */
    explicit Radio_button(std::string const& name);
    /**
     * @brief Set the label.
     * @param l New label
     */
    void set_label(std::string const& l);
    /**
     * @brief Check whether selected.
     * @return True if selected
     */
    bool is_selected() const;

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Handle the clicked event.
     */
    void on_clicked() override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "radio"; }

private:
    friend class Radio_group;
    std::string _label; bool _selected=false; Radio_group* _group=nullptr;
};
// ============================================================================
// Class Radio_group
// ----------------------------------------------------------------------------

/** @brief Manages mutual exclusion across a set of Radio_buttons. */
class Radio_group final {

public:
    /**
     * @brief Add.
     * @param parent parent
     * @param name Widget name (unique within its parent)
     * @param label label
     */
    Radio_button* add(Widget& parent, std::string const& name, std::string const& label);
    /**
     * @brief Select.
     * @param index index
     */
    void select(std::size_t index);
    /**
     * @brief Get the selected.
     * @return The selected
     */
    std::size_t selected() const { return _selected; }
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(std::size_t)> cb){_on_changed=std::move(cb);}

private:
    friend class Radio_button;
    std::vector<Radio_button*> _buttons; std::size_t _selected=0;
    std::function<void(std::size_t)> _on_changed;
    /**
     * @brief Notify.
     * @param b b
     */
    void _notify(Radio_button* b);
};
} // namespace titan::ui
