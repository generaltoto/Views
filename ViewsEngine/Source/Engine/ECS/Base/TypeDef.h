#pragma once

#include <bitset>
#include <cstdint>

using InstanceID = std::uint32_t;
const InstanceID MAX_ENTITIES = 2000;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;

// The signature will be used to determine which components an entity has
using Signature = std::bitset<MAX_COMPONENTS>;
