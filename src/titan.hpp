#pragma once

// ============================================================================
// Titan Engine - umbrella header
// ----------------------------------------------------------------------------
// Includes every public module of the engine. Include this for the whole API,
// or include an individual module header for just one area.
// ============================================================================

#include "module-utils/include/logger.hpp"
#include "module-utils/include/easing.hpp"
#include "module-utils/include/math.hpp"
#include "module-utils/include/random.hpp"
#include "module-utils/include/string.hpp"

#include "module-core/time/include/timer.hpp"
#include "module-core/time/include/time_manager.hpp"
#include "module-core/config/include/config.hpp"
#include "module-core/config/include/config_format.hpp"
#include "module-core/config/include/config_manager.hpp"
#include "module-core/events/include/event_data.hpp"
#include "module-core/events/include/event_listener.hpp"
#include "module-core/events/include/event_type.hpp"
#include "module-core/events/include/game_event_manager.hpp"
#include "module-core/events/include/sfml_event_manager.hpp"

#include "module-resources/include/animation.hpp"
#include "module-resources/include/resource_manager.hpp"
#include "module-resources/include/spritesheet.hpp"

#include "module-render/include/camera.hpp"
#include "module-render/include/debug_draw.hpp"
#include "module-render/include/renderer.hpp"
#include "module-render/include/render_layer.hpp"

#include "module-audio/include/audio_bus.hpp"
#include "module-audio/include/audio_system.hpp"
#include "module-audio/include/music_player.hpp"
#include "module-audio/include/sfx_options.hpp"
#include "module-audio/include/sfx_player.hpp"
#include "module-ui/include/ui.hpp"

#include "module-app/include/application_config.hpp"
#include "module-app/include/application.hpp"

#include "module-scene/include/scene.hpp"
#include "module-scene/include/scene_manager.hpp"

#include "module-game/include/game.hpp"
#include "module-scene/include/settings_scene.hpp"
#include "module-debug/include/debug.hpp"
