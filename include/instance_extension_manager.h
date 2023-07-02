/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <pch.h>

namespace rvr {
class InstanceExtensionManager {
public:
    InstanceExtensionManager();
    ~InstanceExtensionManager();
    std::vector<const char*> GetExtensions();
private:
    void ConvertToCharPointers();
    void PushBackExtensions(const std::vector<std::string>& extensionsToPush);
    void EnsureExtensionsAvailable();
    std::vector<std::string> extensions_;
    std::vector<const char*> extensionsAsCharPointers_;

};
}