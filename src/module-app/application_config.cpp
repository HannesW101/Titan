// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-app/include/application_config.hpp"

#include "module-core/config/include/config.hpp"

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::app {

// ============================================================================
// Constants
// ----------------------------------------------------------------------------

namespace {

    constexpr char const* SECTION = "application";
}

// ============================================================================
// Application_config
// ============================================================================

// ----------------------------------------------------------------------------
void Application_config::load_from_config(
    core::Config const& config
    ) {

    if (auto v = config.get<std::string>(SECTION, "title"                    )) { title                     = *v;                            }
    if (auto v = config.get<int>        (SECTION, "width"                    )) { width                     = static_cast<unsigned int>(*v); }
    if (auto v = config.get<int>        (SECTION, "height"                   )) { height                    = static_cast<unsigned int>(*v); }
    if (auto v = config.get<bool>       (SECTION, "fullscreen"               )) { fullscreen                = *v;                            }
    if (auto v = config.get<bool>       (SECTION, "resizable"                )) { resizable                 = *v;                            }
    if (auto v = config.get<int>        (SECTION, "antialiasing"             )) { antialiasing              = static_cast<unsigned int>(*v); }
    if (auto v = config.get<bool>       (SECTION, "vsync"                    )) { vsync                     = *v;                            }
    if (auto v = config.get<int>        (SECTION, "framerate_limit"          )) { framerate_limit           = static_cast<unsigned int>(*v); }
    if (auto v = config.get<float>      (SECTION, "fixed_timestep"           )) { fixed_timestep            = *v;                            }
    if (auto v = config.get<int>        (SECTION, "max_fixed_steps_per_frame")) { max_fixed_steps_per_frame = static_cast<unsigned int>(*v); }
}

// ----------------------------------------------------------------------------
void Application_config::save_to_config(
    core::Config& config
    ) const {

    config.set<std::string>(SECTION, "title",                     title                                      );
    config.set<int>        (SECTION, "width",                     static_cast<int>(width                    ));
    config.set<int>        (SECTION, "height",                    static_cast<int>(height                   ));
    config.set<bool>       (SECTION, "fullscreen",                fullscreen                                 );
    config.set<bool>       (SECTION, "resizable",                 resizable                                  );
    config.set<int>        (SECTION, "antialiasing",              static_cast<int>(antialiasing             ));
    config.set<bool>       (SECTION, "vsync",                     vsync                                      );
    config.set<int>        (SECTION, "framerate_limit",           static_cast<int>(framerate_limit          ));
    config.set<float>      (SECTION, "fixed_timestep",            fixed_timestep                             );
    config.set<int>        (SECTION, "max_fixed_steps_per_frame", static_cast<int>(max_fixed_steps_per_frame));
}

} // namespace titan::app
