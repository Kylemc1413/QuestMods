#include <array>
#include <limits>
#include <map>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "../beatsaber-hook/shared/utils/utils.h"
#include "AssetImporter.hpp"

Il2CppObject* GetFirstObjectOfType(Il2CppClass* klass)
{
    auto* type = CRASH_UNLESS(il2cpp_utils::GetSystemType(klass));
    auto* objects
        = *CRASH_UNLESS(il2cpp_utils::RunMethod<Array<Il2CppObject*>*>("UnityEngine", "Resources", "FindObjectsOfTypeAll", type));
    RET_0_UNLESS(objects);
    return objects->values[0];
}

void ReplaceSaber(Il2CppObject* saber, Il2CppObject* customSaberObject)
{
    auto* saberGameObject = *CRASH_UNLESS(il2cpp_utils::GetPropertyValue(saber, "gameObject"));
    int saberType = *CRASH_UNLESS(il2cpp_utils::GetPropertyValue<int>(saber, "saberType"));

    auto* parentSaberTransform = *CRASH_UNLESS(il2cpp_utils::GetPropertyValue(saberGameObject, "transform"));
    Vector3 parentPos = *CRASH_UNLESS(il2cpp_utils::GetPropertyValue<Vector3>(parentSaberTransform, "position"));
    Quaternion parentRot = *CRASH_UNLESS(il2cpp_utils::GetPropertyValue<Quaternion>(parentSaberTransform, "rotation"));

    auto* customSaberObjectTransform = *CRASH_UNLESS(il2cpp_utils::RunMethod(customSaberObject, "get_transform"));
    Il2CppString* saberName = CRASH_UNLESS(il2cpp_utils::createcsstr((saberType == 0 ? "LeftSaber" : "RightSaber")));
    auto* childTransform = *CRASH_UNLESS(il2cpp_utils::RunMethod(customSaberObjectTransform, "Find", saberName));
    RET_V_UNLESS(childTransform);

    const bool kInactive = false;
    auto* tMeshFilter = CRASH_UNLESS(il2cpp_utils::GetSystemType("UnityEngine", "MeshFilter"));
    auto* meshFilters = *CRASH_UNLESS(
        il2cpp_utils::RunMethod<Array<Il2CppObject*>*>(parentSaberTransform, "GetComponentsInChildren", tMeshFilter, kInactive));
    for (int i = 0; i < meshFilters->Length(); i++) {
        auto* filterObject = *CRASH_UNLESS(il2cpp_utils::GetPropertyValue(meshFilters->values[i], "gameObject"));
        CRASH_UNLESS(il2cpp_utils::RunMethod(filterObject, "SetActive", kInactive));
    }
    log(INFO, "Disabled Original Saber Meshes");

    CRASH_UNLESS(il2cpp_utils::SetPropertyValue(childTransform, "parent", parentSaberTransform));
    CRASH_UNLESS(il2cpp_utils::SetPropertyValue(childTransform, "position", &parentPos));
    CRASH_UNLESS(il2cpp_utils::SetPropertyValue(childTransform, "rotation", &parentRot));
    log(INFO, "Placed Custom Saber");
    log(INFO, "Attempting to set colors of Custom Saber to colorManager Colors");
    auto* colorManager = GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "ColorManager"));

    RET_V_UNLESS(colorManager);
    Color colorForType = *CRASH_UNLESS(il2cpp_utils::RunMethod<Color>(colorManager, "ColorForSaberType", saberType));
    auto* tRenderer = CRASH_UNLESS(il2cpp_utils::GetSystemType("UnityEngine", "Renderer"));
    auto* renderers = *CRASH_UNLESS(
        il2cpp_utils::RunMethod<Array<Il2CppObject*>*>(childTransform, "GetComponentsInChildren", tRenderer, kInactive));

    Il2CppString* glowString = CRASH_UNLESS(il2cpp_utils::createcsstr("_Glow"));
    Il2CppString* bloomString = CRASH_UNLESS(il2cpp_utils::createcsstr("_Bloom"));
    Il2CppString* materialColor = CRASH_UNLESS(il2cpp_utils::createcsstr("_Color"));
    int glowInt = *CRASH_UNLESS(il2cpp_utils::RunMethod<int>("UnityEngine", "Shader", "PropertyToID", glowString));
    int bloomInt = *CRASH_UNLESS(il2cpp_utils::RunMethod<int>("UnityEngine", "Shader", "PropertyToID", bloomString));
    for (int i = 0; i < renderers->Length(); ++i) {
        auto* sharedMaterials
            = *CRASH_UNLESS(il2cpp_utils::GetPropertyValue<Array<Il2CppObject*>*>(renderers->values[i], "sharedMaterials"));

        for (int j = 0; j < sharedMaterials->Length(); ++j) {
            bool setColor = false;
            bool hasGlow = *CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(sharedMaterials->values[j], "HasProperty", glowInt));
            if (hasGlow) {
                float glowFloat = *CRASH_UNLESS(il2cpp_utils::RunMethod<float>(sharedMaterials->values[j], "GetFloat", glowInt));
                if (glowFloat > 0)
                    setColor = true;
            }
            if (!setColor) {
                bool hasBloom = *CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(sharedMaterials->values[j], "HasProperty", bloomInt));
                if (hasBloom) {
                    float bloomFloat
                        = *CRASH_UNLESS(il2cpp_utils::RunMethod<float>(sharedMaterials->values[j], "GetFloat", bloomInt));
                    if (bloomFloat > 0)
                        setColor = true;
                }
            }
            if (setColor) {
                CRASH_UNLESS(il2cpp_utils::RunMethod(sharedMaterials->values[j], "SetColor", materialColor, colorForType));
            }
        }
    }
    log(INFO, "ReplaceSaber complete.");
}

Il2CppObject* leftSaber = nullptr;
Il2CppObject* rightSaber = nullptr;
void CustomSaberImportComplete(AssetImporter* importer)
{
    if (!importer->loadedAsset)
        return;
    if (leftSaber) {
        auto* inst = importer->InstantiateAsset();
        if (inst) {
            log(INFO, "Replacing left Saber with Custom Saber");
            ReplaceSaber(leftSaber, inst);
        }
        leftSaber = nullptr;
    }
    if (rightSaber) {
        auto* inst = importer->InstantiateAsset();
        if (inst) {
            log(INFO, "Replacing right Saber with Custom Saber");
            ReplaceSaber(rightSaber, inst);
        }
        rightSaber = nullptr;
    }
}

static AssetImporter* importer;
void BeginAssetImport()
{
    if (!importer) {
        log(DEBUG, "Making importer");
        importer = new AssetImporter("/sdcard/Android/data/com.beatgames.beatsaber/files/sabers/testSaber.qsaber", "_customsaber",
            CustomSaberImportComplete);
    }
}
MAKE_HOOK_OFFSETLESS(GameplayCoreSceneSetup_Start, void, Il2CppObject* self)
{
    log(INFO, "Called GameplayCoreSceneSetup_Start hook");
    GameplayCoreSceneSetup_Start(self);
    BeginAssetImport();
}
MAKE_HOOK_OFFSETLESS(TutorialController_Awake, void, Il2CppObject* self)
{
    log(INFO, "Called TutorialController_Awake hook");
    TutorialController_Awake(self);
    BeginAssetImport();
}

MAKE_HOOK_OFFSETLESS(SceneManager_SetActiveScene, bool, int scene)
{
    log(INFO, "Called SceneManager_SetActiveScene hook");
    bool result = SceneManager_SetActiveScene(scene);

    // Get Scene Name
    auto* csName
        = *CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>("UnityEngine.SceneManagement", "Scene", "GetNameInternal", scene));
    auto sceneName = to_utf8(csstrtostr(csName)).c_str();

    // Code to run if menuCore
    if (std::strncmp(sceneName, "MenuCore", 8) == 0) {
        log(INFO, "MenuCore Scene");
    }
    return result;
}

MAKE_HOOK_OFFSETLESS(Saber_Start, void, Il2CppObject* self)
{
    BeginAssetImport();
    Saber_Start(self);
    log(INFO, "Called Saber_Start hook");
    int saberType = *RET_V_UNLESS(il2cpp_utils::GetPropertyValue<int>(self, "saberType"));
    if (saberType == 0) {
        leftSaber = self;
    } else {
        rightSaber = self;
    }
    if (importer->loadedAsset) {
        CustomSaberImportComplete(importer);
    }
}

extern "C" void load()
{
    il2cpp_functions::Init();
    log(INFO, "Installing Custom Sabers Hooks!");
    INSTALL_HOOK_OFFSETLESS(SceneManager_SetActiveScene,
        il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "SetActiveScene", 1));
    // INSTALL_HOOK_OFFSETLESS(GameplayCoreSceneSetup_Start, il2cpp_utils::FindMethod("", "GameplayCoreSceneSetup", "Start"));
    // No longer exists
    // INSTALL_HOOK_OFFSETLESS(TutorialController_Awake, il2cpp_utils::FindMethod("", "TutorialController", "Awake"));
    INSTALL_HOOK_OFFSETLESS(Saber_Start, il2cpp_utils::FindMethod("", "Saber", "Start"));
    log(INFO, "Installed Custom Sabers Hooks!");
}
