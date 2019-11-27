#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <array>
#include <vector>
#include <limits>
#include <map>
#include <dlfcn.h>
#include "../pistol-whip-hook/shared/utils/logging.h"
#include "../pistol-whip-hook/shared/inline-hook/inlineHook.h"
#include "../pistol-whip-hook/shared/utils/utils.h"
#include "../pistol-whip-hook/shared/utils/utils-functions.hpp"
#include "../pistol-whip-hook/shared/utils/il2cpp-functions.hpp"
#include "../pistol-whip-hook/shared/utils/il2cpp-utils.hpp"
#include "../pistol-whip-hook/shared/utils/typedefs.h"
//#define line_y_pos_for_line_layer_offset 0x4F5AC0

#define GunAmmoDisplayUpdateOffset 0x15E2798
#define PlayerActionManagerGameStartOffset 0x1724B54

static Il2CppObject* GameData;
MAKE_HOOK(PlayerActionManagerGameStart, PlayerActionManagerGameStartOffset, void, void* self, void* e)
{
    log(INFO, "Called PlayerActionManager GameStart Hook!");
    PlayerActionManagerGameStart(self, e);
    GameData = nullptr;
    GameData = il2cpp_utils::GetFieldValue(reinterpret_cast<Il2CppObject*>(self), "playerData");
    log(INFO, "Finished PlayerActionManager GameStart Hook!");
}
MAKE_HOOK(GunAmmoDisplayUpdate, GunAmmoDisplayUpdateOffset, void, void* self)
{
    GunAmmoDisplayUpdate(self);
    if(GameData == nullptr) return;
    Il2CppObject* displayTextObj = il2cpp_utils::GetFieldValue(reinterpret_cast<Il2CppObject*>(self), "displayText");
    Il2CppString* displayText = il2cpp_utils::GetFieldValue<Il2CppString*>(displayTextObj, "m_text");
    std::string text = to_utf8(csstrtostr(displayText));
    float accuracy = *(reinterpret_cast<float *>(il2cpp_functions::object_unbox(il2cpp_utils::GetFieldValue(GameData, "accuracy"))));
    text += " (" + std::to_string(accuracy) +")";
    il2cpp_utils::SetFieldValue(displayTextObj, "m_text", il2cpp_functions::string_new(text.data()));
}

__attribute__((constructor)) void lib_main()
{

    log(INFO, "Installing AccCounter Hooks!");
    INSTALL_HOOK(PlayerActionManagerGameStart);
    INSTALL_HOOK(GunAmmoDisplayUpdate);
    log(INFO, "Installed AccCounter Hooks!");
    log(INFO, "Initializing Il2Cpp Functions for AccCounter");
    il2cpp_functions::Init();
    log(INFO, "Initialized Il2Cpp Functions for AccCounter");
}