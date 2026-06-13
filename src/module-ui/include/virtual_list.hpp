#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

namespace titan::ui {

// ============================================================================
// Class Virtual_list
// ----------------------------------------------------------------------------

/** @brief Virtualized list: only visible rows are built each frame, so it
 *  handles millions of items. Provide a count and a row-text provider. */
class Virtual_list final : public Widget {

public:
    /**
     * @brief Construct a Virtual_list.
     * @param name Widget name (unique within its parent)
     */
    explicit Virtual_list(std::string const& name);
    /**
     * @brief Set the item count.
     * @param n New item count
     */
    void set_item_count(std::size_t n);
    /**
     * @brief Set the item height.
     * @param h New item height
     */
    void set_item_height(float h);
    /**
     * @brief Set the text provider.
     * @param size_t New text provider
     */
    void set_text_provider(std::function<std::string(std::size_t)> fn);
    /**
     * @brief Set the "on clicked" callback.
     * @param cb Callback to invoke
     */
    void set_on_clicked(std::function<void(std::size_t)> cb);
    /**
     * @brief Get the item count.
     * @return The item count
     */
    std::size_t item_count() const;

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
    std::string style_key() const override { return "list_box"; }

private:
    std::size_t _count=0; float _ih=24.f,_scroll=0.f;
    std::function<std::string(std::size_t)> _provider;
    std::function<void(std::size_t)> _on_clicked;
    std::uint64_t _click=0,_up=0,_dn=0;
};
} // namespace titan::ui
