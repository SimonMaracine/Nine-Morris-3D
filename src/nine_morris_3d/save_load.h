#pragma once

#include <array>

// #include <entt/entt.hpp>

// namespace save_load {
//     struct Entities {
//         entt::entity board;
//         entt::entity camera;
//         std::array<entt::entity, 24> nodes;
//         std::array<entt::entity, 18> pieces;

//         template<typename Archive>
//         void serialize(Archive& archive) {
//             archive(board, camera, nodes, pieces);
//         }
//     };

//     Entities gather_entities(entt::entity board, entt::entity camera, entt::entity* nodes,
//                              entt::entity* pieces);
//     void reset_entities(const Entities& entities, entt::entity* board, entt::entity* camera,
//                         entt::entity* nodes, entt::entity* pieces);

//     void save_game(const entt::registry& registry, const Entities& entities);
//     void load_game(entt::registry& registry, Entities& entities);

//     bool save_files_exist();
// }
