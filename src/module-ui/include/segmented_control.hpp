#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace titan::ui {

// ============================================================================
// Class Segmented_control
// ----------------------------------------------------------------------------

/** @brief Horizontal row of mutually-exclusive segments (like iOS segmented control). */
class Segmented_control final : public Widget {

public:
    /**
     * @brief Construct a Segmented_control.
     * @param name Widget name (unique within its parent)
     */
    explicit Segmented_control(std::string const& name);
    /**
     * @brief Set the segments.
     * @param segs New segments
     */
    void set_segments(std::vector<std::string> segs);
    /**
     * @brief Set the selected.
     * @param i New selected
     */
    void set_selected(std::size_t i);
    /**
     * @brief Get the selected.
     * @return The selected
     */
    std::size_t selected() const;
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void(std::size_t)> cb);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "dropdown"; }

private:
    std::vector<std::string> _segs; std::size_t _sel=0;
    std::function<void(std::size_t)> _on_changed;
    std::uint64_t _click=0;
};
} // namespace titan::ui
