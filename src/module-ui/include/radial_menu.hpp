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
// Class Radial_menu
// ----------------------------------------------------------------------------

/** @brief Pie/wheel selection menu. show_at() opens it centered at a point;
 *  the hovered wedge highlights; release selects. */
class Radial_menu final : public Widget {

public:
    /**
     * @brief Construct a Radial_menu.
     * @param name Widget name (unique within its parent)
     */
    explicit Radial_menu(std::string const& name);
    /**
     * @brief Set the entries.
     * @param entries New entries
     */
    void set_entries(std::vector<std::string> entries);
    /**
     * @brief Set the radius.
     * @param inner inner
     * @param outer outer
     */
    void set_radius(float inner, float outer);
    /**
     * @brief Show at.
     * @param center center
     */
    void show_at(sf::Vector2f center);
    /**
     * @brief Hide.
     */
    void hide();
    /**
     * @brief Check whether shown.
     * @return True if shown
     */
    bool is_shown() const;
    /**
     * @brief Set the "on selected" callback.
     * @param cb Callback to invoke
     */
    void set_on_selected(std::function<void(std::size_t,std::string const&)> cb);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Whether the widget draws its own background.
     * @return True if it draws its own background
     */
    bool draws_own_background() const override { return true; }

private:
    std::vector<std::string> _entries; float _rin=40.f,_rout=110.f;
    bool _shown=false; sf::Vector2f _center{0,0};
    std::function<void(std::size_t,std::string const&)> _on_selected;
    std::uint64_t _release=0;
    /**
     * @brief Hover wedge.
     * @param m m
     */
    int _hover_wedge(sf::Vector2f m) const;
};
} // namespace titan::ui
