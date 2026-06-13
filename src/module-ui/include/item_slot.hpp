#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstdint>
#include <functional>
#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Item_slot
// ----------------------------------------------------------------------------

/** @brief An inventory slot holding an item id (>=0 = filled, -1 = empty),
 *  shown via an icon texture. Supports drag-out and drop-in using the
 *  UI_manager drag payload (carries the item id, drag_kind = "item"). */
class Item_slot final : public Widget {

public:
    static constexpr std::int64_t DRAG_KIND_ITEM = 1001;
    /**
     * @brief Construct a Item_slot.
     * @param name Widget name (unique within its parent)
     */
    explicit Item_slot(std::string const& name);
    /**
     * @brief Set the item.
     * @param item_id item id
     * @param icon_texture icon texture
     */
    void set_item(std::int64_t item_id, std::string const& icon_texture);
    /**
     * @brief Clear item.
     */
    void clear_item();
    /**
     * @brief Get the item id.
     * @return The item id
     */
    std::int64_t item_id() const;
    void set_count(int count);                 // stack size, 0 hides label
    /**
     * @brief Set the "on changed" callback.
     * @param cb Callback to invoke
     */
    void set_on_changed(std::function<void()> cb);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Handle the pressed event.
     */
    void on_pressed() override;
    /**
     * @brief Handle the released event.
     */
    void on_released() override;
    std::string style_key() const override { return "checkbox"; } // reuse box look

private:
    std::int64_t          _item  = -1;
    std::string           _icon;
    int                   _count = 0;
    std::function<void()> _on_changed;
};
} // namespace titan::ui
