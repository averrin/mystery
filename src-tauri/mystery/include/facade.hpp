#pragma once

#define FMT_HEADER_ONLY
#include <cstdint>
#include "../../target/cxxbridge/src/common.rs.h"
#include "../../target/cxxbridge/rust/cxx.h"

City init(rust::String root,int32_t seed);
