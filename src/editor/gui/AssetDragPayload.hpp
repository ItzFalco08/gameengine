#pragma once

#include "../../core/assets/meta/AssetMeta.hpp"

// ImGui copies drag payloads as raw bytes, so keep this trivially copyable.
struct AssetDragPayload {
    char uuid[33] = {};
    AssetType type{};
};
