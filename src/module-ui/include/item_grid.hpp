#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace titan::ui {

class Item_slot;
// ============================================================================
// Class Item_grid
// ----------------------------------------------------------------------------

/** @brief A grid of Item_slots with drag-drop between them built in. */
class Item_grid final : public Widget {

public:
    /**
     * @brief Construct a Item_grid.
     * @param name Widget name (unique within its parent)
     */
    explicit Item_grid(std::string const& name);
    /**
     * @brief Configure.
     * @param columns columns
     * @param rows rows
     * @param slot_size slot size
     * @param gap gap
     */
    void configure(std::size_t columns, std::size_t rows, float slot_size, float gap);
    /**
     * @brief Slot.
     * @param index index
     */
    Item_slot* slot(std::size_t index);
    /**
     * @brief Get the slot count.
     * @return The slot count
     */
    std::size_t slot_count() const;

protected:
    /**
     * @brief Handle the layout event.
     */
    void on_layout() override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "panel"; }

private:
    std::size_t             _cols  = 4u;
    std::size_t             _rows  = 4u;
    float                   _slot  = 48.0f;
    float                   _gap   = 4.0f;
    bool                    _built = false;
    std::vector<Item_slot*> _slots;
    /**
     * @brief Build.
     */
    void _build();
};
} // namespace titan::ui
