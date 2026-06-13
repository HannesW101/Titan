#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace titan::ui {

// ============================================================================
// Class Tab_view
// ----------------------------------------------------------------------------

/** @brief Tabbed container; one child shown at a time, tab strip on top. */
class Tab_view final : public Widget {

public:
    /**
     * @brief Construct a Tab_view.
     * @param name Widget name (unique within its parent)
     */
    explicit Tab_view(std::string const& name);
    /**
     * @brief Add tab.
     * @param tab_name tab name
     * @param content content
     */
    void add_tab(std::string const& tab_name, std::unique_ptr<Widget> content);
    /**
     * @brief Set the active tab.
     * @param i New active tab
     */
    void set_active_tab(std::size_t i);
    /**
     * @brief Get the active tab.
     * @return The active tab
     */
    std::size_t active_tab() const;
    /**
     * @brief Set the tab height.
     * @param h New tab height
     */
    void set_tab_height(float h);
    /**
     * @brief Set the "on tab changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_tab_changed(std::function<void(std::size_t,std::string const&)> cb);

protected:
    /**
     * @brief Handle the layout event.
     */
    void on_layout() override;
    /**
     * @brief Handle the update event.
     * @param dt dt
     */
    void on_update(float dt) override;
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "panel"; }

private:
// ============================================================================
// Class Tab
// ----------------------------------------------------------------------------

    struct Tab { std::string name; std::unique_ptr<Widget> content; };
    std::vector<Tab> _tabs; std::size_t _active=0; float _tab_h=32.f;
    std::function<void(std::size_t,std::string const&)> _on_changed;
    std::uint64_t _click=0;
};
} // namespace titan::ui
