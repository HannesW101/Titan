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

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class List_box
// ----------------------------------------------------------------------------

/** @brief Scrollable selectable list of text items (single or multi-select). */
class List_box final : public Widget {

public:
    /**
     * @brief Construct a List_box.
     * @param name Widget name (unique within its parent)
     */
    explicit List_box(std::string const& name);
    /**
     * @brief Set the items.
     * @param items New items
     */
    void set_items(std::vector<std::string> items);
    /**
     * @brief Add item.
     * @param s s
     */
    void add_item(std::string const& s);
    /**
     * @brief Clear items.
     */
    void clear_items();
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
     * @brief Get the selected text.
     * @return The selected text
     */
    std::string const& selected_text() const;
    /**
     * @brief Set the multi select.
     * @param on New multi select
     */
    void set_multi_select(bool on);
    /**
     * @brief Get the selected indices.
     * @return The selected indices
     */
    std::vector<std::size_t> const& selected_indices() const;
    /**
     * @brief Set the item height.
     * @param h New item height
     */
    void set_item_height(float h);
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
    std::string style_key() const override { return "list_box"; }

private:
    std::vector<std::string> _items;
    std::size_t              _sel      = 0;
    std::vector<std::size_t> _multi;
    bool                     _multisel = false;
    float                    _scroll   = 0.f;
    float                    _ih       = 24.f;

    std::function<void(std::size_t)> _on_changed;

    static std::string const _empty;

    std::uint64_t _click = 0;
    std::uint64_t _up    = 0;
    std::uint64_t _dn    = 0;
};
} // namespace titan::ui
