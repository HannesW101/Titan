#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"
#include "SFML/Graphics/Color.hpp"

#include <string>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::ui {

// ============================================================================
// Class Gauge
// ----------------------------------------------------------------------------

/** @brief Radial/arc meter for cooldowns, speed, health rings, etc.
 *  Value in [0,1]; sweeps from start_angle clockwise by up to sweep degrees. */
class Gauge final : public Widget {

public:
    /**
     * @brief Construct a Gauge.
     * @param name Widget name (unique within its parent)
     */
    explicit Gauge(std::string const& name);
    /**
     * @brief Set the value.
     * @param v01 New value
     */
    void set_value(float v01);
    /**
     * @brief Get the value.
     * @return The value
     */
    float value() const;
    /**
     * @brief Set the angles.
     * @param start_deg start deg
     * @param sweep_deg sweep deg
     */
    void set_angles(float start_deg, float sweep_deg);
    /**
     * @brief Set the thickness.
     * @param px New thickness
     */
    void set_thickness(float px);
    /**
     * @brief Set the colors.
     * @param track track
     * @param fill fill
     */
    void set_colors(sf::Color track, sf::Color fill);
    /**
     * @brief Set the show text.
     * @param on New show text
     */
    void set_show_text(bool on);

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
    float _v     = 0.0f;
    float _start = -90.0f;
    float _sweep = 360.0f;
    float _thick = 8.0f;

    sf::Color _track{60,60,90};
    sf::Color _fill{80,140,230};

    bool _text = false;
};

} // namespace titan::ui
