#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstdint>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace titan::ui {

// ============================================================================
// Class Dropdown
// ----------------------------------------------------------------------------

/** @brief Drop-down selector. Expands an option list on click (UI_OVERLAY). */
class Dropdown final : public Widget {

public:
    /**
     * @brief Construct a Dropdown.
     * @param name Widget name (unique within its parent)
     */
    explicit Dropdown(std::string const& name);
    /**
     * @brief Set the options.
     * @param options New options
     */
    void set_options(std::vector<std::string> options);
    /**
     * @brief Set the selected.
     * @param index New selected
     */
    void set_selected(std::size_t index);
    /**
     * @brief Get the selected index.
     * @return The selected index
     */
    std::size_t selected_index() const;
    /**
     * @brief Get the selected text.
     * @return The selected text
     */
    std::string const& selected_text() const;
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(std::size_t,std::string const&)> cb);

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
    std::string style_key() const override { return "dropdown"; }

private:
    std::vector<std::string>                            _opts;
    std::size_t                                         _sel  = 0;
    bool                                                _open = false;
    std::function<void(std::size_t,std::string const&)> _on_changed;

    static std::string const _empty;
    std::uint64_t            _global_click = 0;
};
} // namespace titan::ui
