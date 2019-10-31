///
/// Copyright (C) 2012-2016, Dependable Systems Laboratory, EPFL
/// Copyright (C) 2015-2019, Cyberhaven
/// All rights reserved.
///
/// Licensed under the Cyberhaven Research License Agreement.
///

#ifndef S2E_PLUGINS_Vmi_H
#define S2E_PLUGINS_Vmi_H

#include <s2e/CorePlugin.h>
#include <s2e/Plugin.h>
#include <s2e/Plugins/OSMonitors/ModuleDescriptor.h>
#include <s2e/S2EExecutionState.h>

#include <llvm/ADT/DenseSet.h>
#include <llvm/Support/Path.h>
#include <vmi/ElfDwarf.h>
#include <vmi/ExecutableFile.h>
#include <vmi/PEFile.h>
#include <vmi/Vmi.h>

#include <vmi/FileProvider.h>
#include <vmi/RegisterProvider.h>

namespace s2e {

class ConfigFile;

namespace plugins {

class Vmi : public Plugin {
    S2E_PLUGIN
public:
    Vmi(S2E *s2e) : Plugin(s2e) {
    }

    ~Vmi() {
    }

    void initialize();

    static bool readGuestVirtual(void *opaque, uint64_t address, void *dest, unsigned size);
    static bool writeGuestVirtual(void *opaque, uint64_t address, const void *source, unsigned size);

    static bool readGuestPhysical(void *opaque, uint64_t address, void *dest, unsigned size);
    static bool writeGuestPhysical(void *opaque, uint64_t address, const void *source, unsigned size);
#if defined(TARGET_I386) || defined(TARGET_X86_64)
    static bool readX86Register(void *opaque, unsigned reg, void *value, unsigned size);
    static bool writeX86Register(void *opaque, unsigned reg, const void *value, unsigned size);
#elif defined(TARGET_ARM)
    static bool readARMRegister(void *opaque, unsigned reg, void *value, unsigned size);
    static bool writeARMRegister(void *opaque, unsigned reg, const void *value, unsigned size);
#else
#error Unsupported target architecture
#endif
    static std::string stripWindowsModulePath(const std::string &path);

    std::shared_ptr<vmi::ExecutableFile> getFromDisk(const std::string &modulePath, const std::string &moduleName,
                                                     bool caseInsensitive);

    bool readModuleData(const ModuleDescriptor &module, uint64_t addr, uint8_t &val);

    bool getResolvedImports(S2EExecutionState *state, const ModuleDescriptor &module, vmi::Imports &imports);

private:
    std::vector<std::string> m_baseDirectories;
    std::unordered_map<std::string /* guestfs path */, std::shared_ptr<vmi::ExecutableFile>> m_cache;

    bool findModule(const std::string &module, std::string &path);
    bool parseDirectories(ConfigFile *cfg, const std::string &baseDirsKey);

    bool getHostPathForModule(const std::string &modulePath, const std::string &moduleName, bool caseInsensitive,
                              std::string &hostPath);
    vmi::Imports resolveImports(S2EExecutionState *state, uint64_t loadBase, const vmi::Imports &imports);
};

} // namespace plugins
} // namespace s2e

#endif // S2E_PLUGINS_Vmi_H
