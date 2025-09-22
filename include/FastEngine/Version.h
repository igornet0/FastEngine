#pragma once

#define FASTENGINE_VERSION_MAJOR 1
#define FASTENGINE_VERSION_MINOR 0
#define FASTENGINE_VERSION_PATCH 0

#define FASTENGINE_VERSION_STRING "1.0.0"

#define FASTENGINE_VERSION_COMBINED \
    (FASTENGINE_VERSION_MAJOR * 10000 + \
     FASTENGINE_VERSION_MINOR * 100 + \
     FASTENGINE_VERSION_PATCH)

namespace FastEngine {
    struct Version {
        static constexpr int Major = FASTENGINE_VERSION_MAJOR;
        static constexpr int Minor = FASTENGINE_VERSION_MINOR;
        static constexpr int Patch = FASTENGINE_VERSION_PATCH;
        static constexpr const char* String = FASTENGINE_VERSION_STRING;
        static constexpr int Combined = FASTENGINE_VERSION_COMBINED;
    };
}
