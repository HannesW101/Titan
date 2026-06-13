#pragma once

// ============================================================================
// Titan Game Module - master include
// ----------------------------------------------------------------------------
// Component-based game objects: a World owns Game_objects, each a bag of
// Game_components. Include this for the whole system, or include individual
// component headers as needed.
// ============================================================================

// Core
#include "module-game/include/game_component.hpp"
#include "module-game/include/game_object.hpp"
#include "module-game/include/world.hpp"
#include "module-game/include/collision_system.hpp"

// Components
#include "module-game/components/include/transform.hpp"
#include "module-game/components/include/sprite_renderer.hpp"
#include "module-game/components/include/script.hpp"
#include "module-game/components/include/animator.hpp"
#include "module-game/components/include/collider.hpp"
#include "module-game/components/include/rigidbody.hpp"
#include "module-game/components/include/health.hpp"
#include "module-game/components/include/audio_emitter.hpp"
