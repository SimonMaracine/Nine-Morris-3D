#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <fstream>
#include <unordered_map>
#include <map>
#include <cstddef>
#include <functional>
#include <string_view>
#include <cmath>
#include <iterator>
#include <iomanip>
#include <thread>
#include <atomic>
#include <cassert>
#include <optional>
#include <any>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/base_class.hpp>

#include <entt/entt.hpp>

using namespace entt::literals;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
