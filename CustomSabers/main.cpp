#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <array>
#include <vector>
#include <limits>
#include <map>
#include "../beatsaber-hook/shared/inline-hook/inlineHook.h"
#include "../beatsaber-hook/shared/utils/utils.h"
//#undef log
//#define log(INFO,...) __android_log_print(ANDROID_LOG_INFO, "QuestHook", "[CustomSabers v0.0.1] " __VA_ARGS__)

//Hook offsets
#define set_active_scene_offset 0xD902B4
#define gameplay_core_scene_setup_start_offset 0x5268D0
#define saber_start_offset 0x538890
#define tutorial_controller_awake_offset 0x626A4C
#define MOD_ID "CustomSabers"
#define VERSION "0.0.1"
using il2cpp_utils::createcsstr;
using il2cpp_utils::GetClassFromName;
using il2cpp_utils::New;
using namespace il2cpp_functions;
template <class T>
struct List : Il2CppObject
{
    Array<T> *items;
    int size;
    int version;    // ?
    void *syncRoot; // ?
    uint padding2;  // ?
    uint padding3;  // ?

    //   auto static LIST_ADD_ITEM = reinterpret_cast<function_ptr_t<void, List<T> *, T>>(getRealOffset(0x12105A4));
};

struct UnityObject : Il2CppObject
{
    int *m_cachedPtr;
};
typedef struct
{
    float x;
    float y;
    float z;
    float w;
} Quaternion;

struct SaveDataNoteData : Il2CppObject
{
    float time;
    int lineIndex;
    int lineLayer;
    int type;
    int cutDirection;
};

struct Bounds : Il2CppObject
{
    Vector3 center;
    Vector3 extents;
};

struct Scene : Il2CppObject
{
    int handle;
};

Quaternion ToQuaternion(float pitch, float yaw, float roll) // yaw (Z), pitch (Y), roll (X)
{
    // Abbreviations for the various angular functions
    yaw *= 0.01745329251;
    pitch *= 0.01745329251;
    roll *= 0.01745329251;
    float rollOver2 = roll * 0.5f;
    float sinRollOver2 = sinf(rollOver2);
    float cosRollOver2 = cosf(rollOver2);
    float pitchOver2 = pitch * 0.5f;
    float sinPitchOver2 = sinf(pitchOver2);
    float cosPitchOver2 = cosf(pitchOver2);
    float yawOver2 = yaw * 0.5f;
    float sinYawOver2 = sinf(yawOver2);
    float cosYawOver2 = cosf(yawOver2);
    Quaternion result;
    result.w = cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;
    result.x = cosYawOver2 * sinPitchOver2 * cosRollOver2 + sinYawOver2 * cosPitchOver2 * sinRollOver2;
    result.y = sinYawOver2 * cosPitchOver2 * cosRollOver2 - cosYawOver2 * sinPitchOver2 * sinRollOver2;
    result.z = cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;

    return result;
}

Vector3 AddVectors(Vector3 left, Vector3 right)
{

    Vector3 result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.z = left.z + right.z;
    return result;
}
Vector3 SubtractVectors(Vector3 left, Vector3 right)
{
    Vector3 result;
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    result.z = left.z - right.z;
    return result;
}
float VectorMagnitude(Vector3 vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}
static Il2CppClass *sceneClass;
static Il2CppClass *assetBundleClass;
static Il2CppClass *assetBundleCreateRequestClass;
static Il2CppClass *assetBundleRequestClass;
static Il2CppClass *gameObjectClass;
static Il2CppClass *objectClass;
static Il2CppClass *transformClass;
static Il2CppClass *asyncOperationClass;
static Il2CppClass *saberClass;
static Il2CppClass *componentClass;
static Il2CppClass *meshFilterClass;
static Il2CppClass *resourcesClass;
static Il2CppClass *colorManagerClass;
static Il2CppClass *rendererClass;
static Il2CppClass *materialClass;
static Il2CppClass *shaderClass;
static Il2CppClass *beatmapObjectSpawnControllerClass;
static Il2CppClass *actionThree;
static const MethodInfo *sceneNameMethodInfo;
static const MethodInfo *assetBundleFromFileAsync;
static const MethodInfo *assetBundleFromAsync;
static const MethodInfo *loadAssetAsync;
static const MethodInfo *asyncOperationGetIsDone;
static const MethodInfo *asyncOperationSetAllowSceneActivation;
static const MethodInfo *getAsset;
static const MethodInfo *objectInstantiate;
static const MethodInfo *findGameObject;
static const MethodInfo *getGameObjectTransform;
static const MethodInfo *findTransform;
static const MethodInfo *transformPosGet;
static const MethodInfo *transformLocalPosGet;
static const MethodInfo *transformPosSet;
static const MethodInfo *transformLocalPosSet;
static const MethodInfo *transformEulerSet;
static const MethodInfo *transformEulerGet;
static const MethodInfo *transformLocalEulerSet;
static const MethodInfo *transformLocalEulerGet;
static const MethodInfo *transformParentGet;
static const MethodInfo *transformParentSet;
static const MethodInfo *saberTypeGet;
static const MethodInfo *componentGetGameObject;
static const MethodInfo *componentGetComponentsInChildren;
static const MethodInfo *gameObjectSetActive;
static const MethodInfo *resourcesFindObjectsOfTypeAll;
static const MethodInfo *rendererGetSharedMaterials;
static const MethodInfo *colorManagerColorForSaberType;
static const MethodInfo *materialGetFloat;
static const MethodInfo *materialHasProperty;
static const MethodInfo *materialSetColor;
static const MethodInfo *shaderPropertyToID;
static const MethodInfo *addSpawnControllerNoteWasCut;
static void *asyncBundle;
void GrabMethods();
MAKE_HOOK(set_active_scene, set_active_scene_offset, bool, int scene)
{
    log(INFO, "Called set_active_scene hook");
    bool result = set_active_scene(scene);

    if (sceneClass == nullptr)
        sceneClass = GetClassFromName("UnityEngine.SceneManagement", "Scene");
    //Get scene name method
    if (sceneNameMethodInfo == nullptr)
        sceneNameMethodInfo = class_get_method_from_name(sceneClass, "GetNameInternal", 1);

    //Get Scene Name

    Il2CppException *exception = nullptr;
    void *sceneNameparams[] = {&scene};
    auto nameResult = runtime_invoke(sceneNameMethodInfo, nullptr, sceneNameparams, &exception);
    Il2CppString *csName = reinterpret_cast<Il2CppString *>(nameResult);
    auto sceneName = to_utf8(csstrtostr(csName)).c_str();

    //Code to run if menuCore
    if (std::strncmp(sceneName, "MenuCore", 8) == 0)
    {
        log(INFO, "MenuCore Scene");
    }
    log(INFO, "End set_active_scene hook");
    return result;
}

void *customSaberAssetBundle;
void *customSaberGameObject;

MAKE_HOOK(gameplay_core_scene_setup_start, gameplay_core_scene_setup_start_offset, void, void *self)
{
    log(INFO, "Called gameplay_core_scene_setup_start hook");
    gameplay_core_scene_setup_start(self);

    GrabMethods();
    if (asyncBundle == nullptr)
    {
        Il2CppException *exception;
        Il2CppString *assetFilePath = createcsstr("/sdcard/Android/data/com.beatgames.beatsaber/files/sabers/testSaber.qsaber");
        void *fromFileParams[] = {assetFilePath};
        asyncBundle = runtime_invoke(assetBundleFromFileAsync, nullptr, fromFileParams, &exception);
        bool sceneActivationValue = true;
        void *setSceneActivationParams[] = {&sceneActivationValue};
        runtime_invoke(asyncOperationSetAllowSceneActivation, asyncBundle, setSceneActivationParams, &exception);
        log(INFO, "Loaded Async Bundle");
    }
    customSaberGameObject = nullptr;
}
MAKE_HOOK(tutorial_controller_awake, tutorial_controller_awake_offset, void, void *self)
{
    log(INFO, "Called tutorial_controller_awake hook");
    tutorial_controller_awake(self);
    GrabMethods();
    if (asyncBundle == nullptr)
    {
        Il2CppException *exception;
        Il2CppString *assetFilePath = createcsstr("/sdcard/Android/data/com.beatgames.beatsaber/files/sabers/testSaber.qsaber");
        void *fromFileParams[] = {assetFilePath};
        asyncBundle = runtime_invoke(assetBundleFromFileAsync, nullptr, fromFileParams, &exception);
        bool sceneActivationValue = true;
        void *setSceneActivationParams[] = {&sceneActivationValue};
        runtime_invoke(asyncOperationSetAllowSceneActivation, asyncBundle, setSceneActivationParams, &exception);
        log(INFO, "Loaded Async Bundle");
    }
    customSaberGameObject = nullptr;
}
void ReplaceSaber(void *, void *);
MAKE_HOOK(saber_start, saber_start_offset, void, void *self)
{
    saber_start(self);
    log(INFO, "Called saber_start hook");
    //Load Custom Saber Objects if not loaded
    Il2CppException *exception = nullptr;
    if (customSaberAssetBundle == nullptr)
    {
        customSaberAssetBundle = runtime_invoke(assetBundleFromAsync, asyncBundle, nullptr, &exception);
        log(INFO, "Grabbed Asset bundle");
        asyncBundle = nullptr;
    }
    if (customSaberGameObject == nullptr && customSaberAssetBundle != nullptr)
    {
        Il2CppString *assetPath = createcsstr("_customsaber");
        void *assetPathParams[] = {assetPath, type_get_object(class_get_type(gameObjectClass))};
        void *assetAsync = runtime_invoke(loadAssetAsync, customSaberAssetBundle, assetPathParams, &exception);
        if (exception != nullptr)
        {
            const MethodInfo *exceptionToString = class_get_method_from_name(exception->klass, "ToString", 0);
            void *exceptionString = runtime_invoke(exceptionToString, exception, nullptr, &exception);
            Il2CppString *message = reinterpret_cast<Il2CppString *>(exceptionString);
            log(INFO, "Exception: %s", to_utf8(csstrtostr(message)).c_str());
        }
        log(INFO, "Grabbed Asset Async Request");

        void *customSaberObject = runtime_invoke(getAsset, assetAsync, nullptr, &exception);
        if (exception != nullptr)
        {
            const MethodInfo *exceptionToString = class_get_method_from_name(exception->klass, "ToString", 0);
            void *exceptionString = runtime_invoke(exceptionToString, exception, nullptr, &exception);
            Il2CppString *message = reinterpret_cast<Il2CppString *>(exceptionString);
            log(INFO, "Exception: %s", to_utf8(csstrtostr(message)).c_str());
        }
        log(INFO, "Grabbed Asset Object");

        //Attempt to Instaniate GameObject
        void *instantiateParams[] = {customSaberObject};
        customSaberGameObject = runtime_invoke(objectInstantiate, nullptr, instantiateParams, &exception);
        log(INFO, "Instantiated Asset Object");
    }

    if (customSaberGameObject != nullptr)
    {
        log(INFO, "Replacing Saber with Custom Saber");
        ReplaceSaber(self, customSaberGameObject);
    }
}
__attribute__((constructor)) void lib_main()
{

    log(INFO, "Installing Custom Sabers Hooks!");
    INSTALL_HOOK(set_active_scene);
    INSTALL_HOOK(gameplay_core_scene_setup_start);
    INSTALL_HOOK(saber_start);
    INSTALL_HOOK(tutorial_controller_awake);
    log(INFO, "Installed Custom Sabers Hooks!");
    log(INFO, "Initializing il2cpp api functions for Custom Sabers.");
    Init();
    log(INFO, "Initialized il2cpp api functions for Custom Sabers.");
}

void GrabMethods()
{
    if (assetBundleClass == nullptr)
        assetBundleClass = GetClassFromName("UnityEngine", "AssetBundle");
    if (assetBundleCreateRequestClass == nullptr)
        assetBundleCreateRequestClass = GetClassFromName("UnityEngine", "AssetBundleCreateRequest");
    if (assetBundleRequestClass == nullptr)
        assetBundleRequestClass = GetClassFromName("UnityEngine", "AssetBundleRequest");
    if (gameObjectClass == nullptr)
        gameObjectClass = GetClassFromName("UnityEngine", "GameObject");
    if (objectClass == nullptr)
        objectClass = GetClassFromName("UnityEngine", "Object");
    if (transformClass == nullptr)
        transformClass = GetClassFromName("UnityEngine", "Transform");
    if (asyncOperationClass == nullptr)
        asyncOperationClass = GetClassFromName("UnityEngine", "AsyncOperation");
    if (saberClass == nullptr)
        saberClass = GetClassFromName("", "Saber");
    if (componentClass == nullptr)
        componentClass = GetClassFromName("UnityEngine", "Component");
    if (meshFilterClass == nullptr)
        meshFilterClass = GetClassFromName("UnityEngine", "MeshFilter");
    if (resourcesClass == nullptr)
        resourcesClass = GetClassFromName("UnityEngine", "Resources");
    if (colorManagerClass == nullptr)
        colorManagerClass = GetClassFromName("", "ColorManager");
    if (rendererClass == nullptr)
        rendererClass = GetClassFromName("UnityEngine", "Renderer");
    if (materialClass == nullptr)
        materialClass = GetClassFromName("UnityEngine", "Material");
    if (shaderClass == nullptr)
        shaderClass = GetClassFromName("UnityEngine", "Shader");
    if (beatmapObjectSpawnControllerClass == nullptr)
        beatmapObjectSpawnControllerClass = GetClassFromName("", "BeatmapObjectSpawnController");
    if (actionThree == nullptr)
        actionThree = GetClassFromName("System", "Action`3");

    if (assetBundleFromFileAsync == nullptr)
        assetBundleFromFileAsync = class_get_method_from_name(assetBundleClass, "LoadFromFileAsync", 1);
    if (assetBundleFromAsync == nullptr)
        assetBundleFromAsync = class_get_method_from_name(assetBundleCreateRequestClass, "get_assetBundle", 0);
    if (loadAssetAsync == nullptr)
        loadAssetAsync = class_get_method_from_name(assetBundleClass, "LoadAssetAsync", 2);
    if (getAsset == nullptr)
        getAsset = class_get_method_from_name(assetBundleRequestClass, "get_asset", 0);
    if (objectInstantiate == nullptr)
        objectInstantiate = class_get_method_from_name(objectClass, "Instantiate", 1);

    if (findGameObject == nullptr)
        findGameObject = class_get_method_from_name(gameObjectClass, "Find", 1);
    if (getGameObjectTransform == nullptr)
        getGameObjectTransform = class_get_method_from_name(gameObjectClass, "get_transform", 0);
    if (gameObjectSetActive == nullptr)
        gameObjectSetActive = class_get_method_from_name(gameObjectClass, "SetActive", 1);
    if (findTransform == nullptr)
        findTransform = class_get_method_from_name(transformClass, "Find", 1);
    if (transformPosGet == nullptr)
        transformPosGet = class_get_method_from_name(transformClass, "get_position", 0);
    if (transformPosSet == nullptr)
        transformPosSet = class_get_method_from_name(transformClass, "set_position", 1);
    if (transformLocalPosGet == nullptr)
        transformLocalPosGet = class_get_method_from_name(transformClass, "get_localPosition", 0);
    if (transformLocalPosSet == nullptr)
        transformLocalPosSet = class_get_method_from_name(transformClass, "set_localPosition", 1);
    if (transformEulerGet == nullptr)
        transformEulerGet = class_get_method_from_name(transformClass, "get_eulerAngles", 0);
    if (transformEulerSet == nullptr)
        transformEulerSet = class_get_method_from_name(transformClass, "set_eulerAngles", 1);
    if (transformLocalEulerGet == nullptr)
        transformLocalEulerGet = class_get_method_from_name(transformClass, "get_localEulerAngles", 0);
    if (transformLocalEulerSet == nullptr)
        transformLocalEulerSet = class_get_method_from_name(transformClass, "set_localEulerAngles", 1);
    if (transformParentGet == nullptr)
        transformParentGet = class_get_method_from_name(transformClass, "get_parent", 0);
    if (transformParentSet == nullptr)
        transformParentSet = class_get_method_from_name(transformClass, "set_parent", 1);

    if (asyncOperationSetAllowSceneActivation == nullptr)
        asyncOperationSetAllowSceneActivation = class_get_method_from_name(asyncOperationClass, "set_allowSceneActivation", 1);
    if (asyncOperationGetIsDone == nullptr)
        asyncOperationGetIsDone = class_get_method_from_name(asyncOperationClass, "get_isDone", 0);

    if (saberTypeGet == nullptr)
        saberTypeGet = class_get_method_from_name(saberClass, "get_saberType", 0);

    if (componentGetGameObject == nullptr)
        componentGetGameObject = class_get_method_from_name(componentClass, "get_gameObject", 0);
    if (componentGetComponentsInChildren == nullptr)
        componentGetComponentsInChildren = class_get_method_from_name(componentClass, "GetComponentsInChildren", 2);
    if (resourcesFindObjectsOfTypeAll == nullptr)
        resourcesFindObjectsOfTypeAll = class_get_method_from_name(resourcesClass, "FindObjectsOfTypeAll", 1);

    if (materialGetFloat == nullptr)
        materialGetFloat = class_get_method_from_name(materialClass, "GetFloat", 1);
    if (materialHasProperty == nullptr)
        materialHasProperty = class_get_method_from_name(materialClass, "HasProperty", 1);
    if (materialSetColor == nullptr)
        materialSetColor = class_get_method_from_name(materialClass, "SetColor", 2);
    if (rendererGetSharedMaterials == nullptr)
        rendererGetSharedMaterials = class_get_method_from_name(rendererClass, "get_sharedMaterials", 0);
    if (shaderPropertyToID == nullptr)
        shaderPropertyToID = class_get_method_from_name(shaderClass, "PropertyToID", 1);

    if (colorManagerColorForSaberType == nullptr)
        colorManagerColorForSaberType = class_get_method_from_name(colorManagerClass, "ColorForSaberType", 1);
    if (addSpawnControllerNoteWasCut == nullptr)
        addSpawnControllerNoteWasCut = class_get_method_from_name(beatmapObjectSpawnControllerClass, "add_noteWasCutEvent", 1);
}
void *GetFirstObjectOfType(Il2CppClass *);
void SpawnControllerNoteWasCut(void *BeatmapObjectSpawnController, void *NoteController, void *NoteCutInfo);
void ReplaceSaber(void *saber, void *customSaberObject)
{
    Il2CppException *exception;
    void *saberGameObject = runtime_invoke(componentGetGameObject, saber, nullptr, &exception);
    void *customSaberGameObjectTransform = runtime_invoke(getGameObjectTransform, customSaberGameObject, nullptr, &exception);
    int saberType = *(reinterpret_cast<int *>(object_unbox(runtime_invoke(saberTypeGet, saber, nullptr, &exception))));
    Il2CppString *saberName = createcsstr((saberType == 0 ? "LeftSaber" : "RightSaber"));
    void *saberChildParams[] = {saberName};
    void *childTransform = runtime_invoke(findTransform, customSaberGameObjectTransform, saberChildParams, &exception);
    void *parentSaberTransform = runtime_invoke(getGameObjectTransform, saberGameObject, nullptr, &exception);
    void *parentPos = object_unbox(runtime_invoke(transformPosGet, parentSaberTransform, nullptr, &exception));
    void *parentRot = object_unbox(runtime_invoke(transformEulerGet, parentSaberTransform, nullptr, &exception));

    bool getInactive = false;
    void *getMeshFiltersParams[] = {type_get_object(class_get_type(meshFilterClass)), &getInactive};
    Array<void *> *meshfilters = reinterpret_cast<Array<void *> *>(runtime_invoke(componentGetComponentsInChildren, parentSaberTransform, getMeshFiltersParams, &exception));
    for (int i = 0; i < meshfilters->Length(); i++)
    {
        void *filterObject = runtime_invoke(componentGetGameObject, meshfilters->values[i], nullptr, &exception);
        void *disableParam[] = {&getInactive};
        runtime_invoke(gameObjectSetActive, filterObject, disableParam, &exception);
    }
    log(INFO, "Disabled Original Saber Meshes");

    runtime_invoke(transformParentSet, childTransform, &parentSaberTransform, &exception);
    runtime_invoke(transformPosSet, childTransform, &parentPos, &exception);
    runtime_invoke(transformEulerSet, childTransform, &parentRot, &exception);
    log(INFO, "Placed Custom Saber");
    log(INFO, "Attempting to set colors of Custom Saber to colorManager Colors");
    void *colorManager = GetFirstObjectOfType(colorManagerClass);
    if (colorManager != nullptr)
    {
        void *colorForSaberTypeParams[] = {&saberType};
        Color colorForType = *(reinterpret_cast<Color *>(object_unbox(runtime_invoke(colorManagerColorForSaberType, colorManager, colorForSaberTypeParams, &exception))));
        void *getRendererParams[] = {type_get_object(class_get_type(rendererClass)), &getInactive};
        Array<void *> *renderers = reinterpret_cast<Array<void *> *>(runtime_invoke(componentGetComponentsInChildren, childTransform, getRendererParams, &exception));
        for (int i = 0; i < renderers->Length(); ++i)
        {
            Array<void *> *sharedMaterials = reinterpret_cast<Array<void *> *>(runtime_invoke(rendererGetSharedMaterials, renderers->values[i], nullptr, &exception));
            for (int j = 0; j < sharedMaterials->Length(); ++j)
            {
                Il2CppString *glowString = createcsstr("_Glow");
                Il2CppString *bloomString = createcsstr("_Bloom");
                Il2CppString *materialColor = createcsstr("_Color");
                void *glowStringParams[] = {glowString};
                void *bloomStringParams[] = {bloomString};
                int glowInt = *(reinterpret_cast<int *>(object_unbox(runtime_invoke(shaderPropertyToID, nullptr, glowStringParams, &exception))));
                int bloomInt = *(reinterpret_cast<int *>(object_unbox(runtime_invoke(shaderPropertyToID, nullptr, bloomStringParams, &exception))));
                void *glowIntParams[] = {&glowInt};
                void *bloomIntParams[] = {&bloomInt};
                void *materialColorParams[] = {materialColor, &colorForType};
                bool setColor = false;
                bool hasGlow = runtime_invoke(materialHasProperty, sharedMaterials->values[j], glowIntParams, &exception);
                if (hasGlow)
                {
                    float glowFloat = *(reinterpret_cast<float *>(object_unbox(runtime_invoke(materialGetFloat, sharedMaterials->values[j], glowIntParams, &exception))));
                    if (glowFloat > 0)
                        setColor = true;
                }
                if (!setColor)
                {
                    bool hasBloom = runtime_invoke(materialHasProperty, sharedMaterials->values[j], bloomIntParams, &exception);
                    if (hasBloom)
                    {
                        float bloomFloat = *(reinterpret_cast<float *>(object_unbox(runtime_invoke(materialGetFloat, sharedMaterials->values[j], bloomIntParams, &exception))));
                        if (bloomFloat > 0)
                            setColor = true;
                    }
                }
                if (setColor)
                {
                    runtime_invoke(materialSetColor, sharedMaterials->values[j], materialColorParams, &exception);
                }
            }
        }
    }
    else
    {
        log(INFO, "null colorManager");
    }

    log(INFO, "Finished With Saber");
}

void SpawnControllerNoteWasCut(void *BeatmapObjectSpawnController, void *NoteController, void *NoteCutInfo)
{
    log(INFO, "Note Was Cut Callback");
}

void *GetFirstObjectOfType(Il2CppClass *klass)
{
    Il2CppException *exception;
    void *params[] = {type_get_object(class_get_type(klass))};
    Array<void *> *objects = reinterpret_cast<Array<void *> *>(runtime_invoke(resourcesFindObjectsOfTypeAll, nullptr, params, &exception));
    if (objects != nullptr)
    {
        return objects->values[0];
    }
    else
    {
        return nullptr;
    }
}