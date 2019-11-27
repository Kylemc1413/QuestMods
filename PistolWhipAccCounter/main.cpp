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

#define GunAmmoDisplayUpdateOffset 0x15E2798
#define PlayerActionManagerGameStartOffset 0x1724B54

static Il2CppObject* GameData;
MAKE_HOOK(PlayerActionManagerGameStart, PlayerActionManagerGameStartOffset, void, void* self, void* e)
{
    log(INFO, "Called PlayerActionManager GameStart Hook!");
    PlayerActionManagerGameStart(self, e);
    GameData = nullptr;
    GameData = il2cpp_utils::GetFieldValue(reinterpret_cast<Il2CppObject*>(self), "playerData");
    log(INFO, "GameData: %p", GameData);
    log(INFO, "Finished PlayerActionManager GameStart Hook!");
}

static float lastAcc;
MAKE_HOOK(GunAmmoDisplayUpdate, GunAmmoDisplayUpdateOffset, void, void* self)
{
    GunAmmoDisplayUpdate(self);
    if(GameData == nullptr) {
        return;
    }
    float accuracy;
    il2cpp_utils::GetFieldValue(&accuracy, GameData, "accuracy");
    if (lastAcc == accuracy) return; // No shot fired
    Il2CppObject* displayTextObj = il2cpp_utils::GetFieldValue(reinterpret_cast<Il2CppObject*>(self), "displayText");
    auto tmpclass = il2cpp_utils::GetClassFromName("TMPro", "TMP_Text");
    auto getter = il2cpp_utils::GetMethod(tmpclass, "get_text", 0);
    auto setter = il2cpp_utils::GetMethod(tmpclass, "set_text", 1);
    log(INFO, "displayTextObj: %p", displayTextObj);
    Il2CppString* displayText;
    il2cpp_utils::RunMethod(&displayText, displayTextObj, getter);
    log(INFO, "displayText: %p", displayText);
    std::string text = to_utf8(csstrtostr(displayText));
    log(INFO, "displayText text: %s", text.data());
    
    // AccuracyBox is actually the literal float
    // log(INFO, "accuracyBox: %f", accuracy);
    // float accuracy = *(reinterpret_cast<float *>(il2cpp_functions::object_unbox(accuracyBox)));
    log(INFO, "Accuracy: %.2f", accuracy);
    char buffer[20];
    sprintf(buffer, "%.2f", accuracy);
    auto s = std::string(buffer);
    text += " (" + s +")";
    log(INFO, "Updated text: %s", text.data());
    il2cpp_utils::RunMethod(displayTextObj, setter, il2cpp_functions::string_new(text.data()));
    lastAcc = accuracy;
}

__attribute__((constructor)) void lib_main()
{
    log(INFO, "Installing AccCounter Hooks!");
    INSTALL_HOOK(PlayerActionManagerGameStart);
    INSTALL_HOOK(GunAmmoDisplayUpdate);
    log(INFO, "Installed AccCounter Hooks!");
}

void load() {
    log(INFO, "Loaded AccCounter!");
}