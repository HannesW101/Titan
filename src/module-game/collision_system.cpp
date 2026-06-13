// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-game/include/collision_system.hpp"
#include "module-game/include/world.hpp"
#include "module-game/include/game_object.hpp"
#include "module-game/components/include/collider.hpp"

#include <algorithm>

// ============================================================================
// Namespaces
// ----------------------------------------------------------------------------

namespace titan::game {

// ============================================================================
// Class Collision_system
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
std::uint64_t Collision_system::_pair_key(
    std::uint64_t a,
    std::uint64_t b
    ) {

    if (a > b) { std::swap(a, b); }

    // 32 bits each, object ids comfortably fit for any realistic game.
    return (a << 32) | (b & 0xFFFFFFFFull);
}

// ----------------------------------------------------------------------------
std::vector<Collider*> Collision_system::_broad_phase(
    World& world
    ) const {

    std::vector<Collider*> result;

    for (auto& obj : world.objects()) {

        if (!obj->is_active() || obj->is_destroyed()) { continue; }

        if (auto* c = obj->get_component<Collider>()) {

            if (c->is_enabled()) { result.push_back(c); }
        }
    }

    return result;
}

// ----------------------------------------------------------------------------
void Collision_system::update(
    World& world
    ) {

    std::vector<Collider*> const colliders = _broad_phase(world);

    std::unordered_set<std::uint64_t> current_contacts;

    for (std::size_t i = 0u; i < colliders.size(); ++i) {

        for (std::size_t j = i + 1u; j < colliders.size(); ++j) {

            Collider* a = colliders[i];
            Collider* b = colliders[j];

            if (!a->overlaps(*b)) { continue; }

            std::uint64_t const key = _pair_key(a->owner().id(), b->owner().id());
            current_contacts.insert(key);

            bool const was_touching = _previous_contacts.contains(key);
            if (was_touching) {

                a->fire_stay(*b);
                b->fire_stay(*a);
            } else {

                a->fire_enter(*b);
                b->fire_enter(*a);
            }
        }
    }

    // Exits: pairs that were touching last step but aren't now. Both colliders
    // may still exist; if one was destroyed its callback simply won't refire.
    for (std::uint64_t const key : _previous_contacts) {

        if (!current_contacts.contains(key)) {

            std::uint64_t const id_a = key >> 32;
            std::uint64_t const id_b = key & 0xFFFFFFFFull;

            Game_object* oa = world.find_by_id(id_a);
            Game_object* ob = world.find_by_id(id_b);

            if (oa && ob) {

                auto* ca = oa->get_component<Collider>();
                auto* cb = ob->get_component<Collider>();

                if (ca && cb) {

                    ca->fire_exit(*cb);
                    cb->fire_exit(*ca);
                }
            }
        }
    }

    _previous_contacts = std::move(current_contacts);
}

// ----------------------------------------------------------------------------
void Collision_system::clear() {

    _previous_contacts.clear();
}

} // namespace titan::game
