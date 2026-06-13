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
// Class Table
// ----------------------------------------------------------------------------

/** @brief Simple data table: column headers + rows of cell strings. */
class Table final : public Widget {

public:
    /**
     * @brief Construct a Table.
     * @param name Widget name (unique within its parent)
     */
    explicit Table(std::string const& name);
    /**
     * @brief Set the columns.
     * @param headers New columns
     */
    void set_columns(std::vector<std::string> headers);
    void set_column_widths(std::vector<float> widths);  ///< px per column
    /**
     * @brief Clear rows.
     */
    void clear_rows();
    /**
     * @brief Add row.
     * @param cells cells
     */
    void add_row(std::vector<std::string> cells);
    /**
     * @brief Get the row count.
     * @return The row count
     */
    std::size_t row_count() const;
    /**
     * @brief Set the row height.
     * @param h New row height
     */
    void set_row_height(float h);
    /**
     * @brief Set the "on row clicked" callback.
     * @param cb Callback to invoke
     */
    void set_on_row_clicked(std::function<void(std::size_t)> cb);

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
    std::string style_key() const override { return "table"; }

private:
    std::vector<std::string> _headers; std::vector<float> _widths;
    std::vector<std::vector<std::string>> _rows;
    float _row_h=24.f;
    std::function<void(std::size_t)> _on_row_clicked;
    std::uint64_t _click=0;
};
} // namespace titan::ui
