#pragma once
#include "path.h"


namespace FS {
    bool fileExists(const std::string & path);
    bool makeDirectory(const std::string & path);
}
