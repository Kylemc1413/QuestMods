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
#include "../beatsaber-hook/shared/libil2cpp/il2cpp-api.h"
#include "../beatsaber-hook/shared/libil2cpp/il2cpp-object-internals.h"
//#undef log
//#define log(...) __android_log_print(ANDROID_LOG_INFO, "QuestHook", "[CustomSabers v0.0.1] " __VA_ARGS__)

//Hook offsets
#define set_active_scene_offset 0xD902B4
#define gameplay_core_scene_setup_start_offset 0x5268D0
#define saber_start_offset 0x538890

#define MOD_ID "CustomSabers"
#define VERSION "0.0.1"
using il2cpp_utils::createcsstr;
using il2cpp_utils::GetClassFromName;
static const MethodInfo *(*get_method_from_name)(Il2CppClass *, const char *, int) = nullptr;
static Il2CppObject *(*runtime_invoke)(const MethodInfo *, void *, void **, Il2CppException **) = nullptr;
static const Il2CppType *(*class_get_type)(Il2CppClass *) = nullptr;
static Il2CppObject *(*type_get_object)(const Il2CppType *) = nullptr;
static void *(*object_unbox)(Il2CppObject *) = nullptr;
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
static void *asyncBundle;
void GrabMethods();
MAKE_HOOK(set_active_scene, set_active_scene_offset, bool, int scene)
{
    log("Called set_active_scene hook");
    bool result = set_active_scene(scene);

    if (sceneClass == nullptr)
        sceneClass = GetClassFromName("UnityEngine.SceneManagement", "Scene");
    //Get scene name method
    if (sceneNameMethodInfo == nullptr)
        sceneNameMethodInfo = get_method_from_name(sceneClass, "GetNameInternal", 1);

    //Get Scene Name

    Il2CppException *exception = nullptr;
    void *sceneNameparams[] = {&scene};
    auto nameResult = runtime_invoke(sceneNameMethodInfo, nullptr, sceneNameparams, &exception);
    cs_string *csName = reinterpret_cast<cs_string *>(nameResult);
    auto sceneName = to_utf8(csstrtostr(csName)).c_str();

    //Code to run if menuCore
    if (std::strncmp(sceneName, "MenuCore", 8) == 0)
    {
        log("MenuCore Scene");
    }
    log("End set_active_scene hook");
    return result;
}

void *customSaberAssetBundle;
void *customSaberGameObject;

MAKE_HOOK(gameplay_core_scene_setup_start, gameplay_core_scene_setup_start_offset, void, void *self)
{
    log("Called gameplay_core_scene_setup_start hook");
    gameplay_core_scene_setup_start(self);

    GrabMethods();
    if (asyncBundle == nullptr)
    {
        Il2CppException *exception;
        cs_string *assetFilePath = createcsstr("/sdcard/Android/data/com.beatgames.beatsaber/files/sabers/testSaber.qsaber");
        void *fromFileParams[] = {assetFilePath};
        asyncBundle = runtime_invoke(assetBundleFromFileAsync, nullptr, fromFileParams, &exception);
        bool sceneActivationValue = true;
        void *setSceneActivationParams[] = {&sceneActivationValue};
        runtime_invoke(asyncOperationSetAllowSceneActivation, asyncBundle, setSceneActivationParams, &exception);
        log("Loaded Async Bundle");
    }
    customSaberGameObject = nullptr;
}

void ReplaceSaber(void *, void *);
MAKE_HOOK(saber_start, saber_start_offset, void, void *self)
{
    saber_start(self);
    log("Called saber_start hook");
    //Load Custom Saber Objects if not loaded
    Il2CppException *exception = nullptr;
    if (customSaberAssetBundle == nullptr)
    {
        customSaberAssetBundle = runtime_invoke(assetBundleFromAsync, asyncBundle, nullptr, &exception);
        log("Grabbed Asset bundle");
        asyncBundle = nullptr;
    }
    if (customSaberGameObject == nullptr && customSaberAssetBundle != nullptr)
    {
        cs_string *assetPath = createcsstr("_customsaber");
        void *assetPathParams[] = {assetPath, type_get_object(class_get_type(gameObjectClass))};
        void *assetAsync = runtime_invoke(loadAssetAsync, customSaberAssetBundle, assetPathParams, &exception);
        if (exception != nullptr)
        {
            const MethodInfo *exceptionToString = get_method_from_name(exception->klass, "ToString", 0);
            void *exceptionString = runtime_invoke(exceptionToString, exception, nullptr, &exception);
            cs_string *message = reinterpret_cast<cs_string *>(exceptionString);
            log("Exception: %s", to_utf8(csstrtostr(message)).c_str());
        }
        log("Grabbed Asset Async Request");

        void *customSaberObject = runtime_invoke(getAsset, assetAsync, nullptr, &exception);
        if (exception != nullptr)
        {
            const MethodInfo *exceptionToString = get_method_from_name(exception->klass, "ToString", 0);
            void *exceptionString = runtime_invoke(exceptionToString, exception, nullptr, &exception);
            cs_string *message = reinterpret_cast<cs_string *>(exceptionString);
            log("Exception: %s", to_utf8(csstrtostr(message)).c_str());
        }
        log("Grabbed Asset Object");

        //Attempt to Instaniate GameObject
        void *instantiateParams[] = {customSaberObject};
        customSaberGameObject = runtime_invoke(objectInstantiate, nullptr, instantiateParams, &exception);
        log("Instantiated Asset Object");
    }

    if (customSaberGameObject != nullptr)
    {
        log("Replacing Saber with Custom Saber");
        ReplaceSaber(self, customSaberGameObject);
    }
}
__attribute__((constructor)) void lib_main()
{

    log("Installing Custom Sabers Hooks!");
    INSTALL_HOOK(set_active_scene);
    INSTALL_HOOK(gameplay_core_scene_setup_start);
    INSTALL_HOOK(saber_start);
    log("Installed Custom Sabers Hooks!");
    log("Getting il2cpp api functions for Custom Sabers.");
    if (get_method_from_name == nullptr || runtime_invoke == nullptr)
    {
        void *imagehandle = dlopen("/data/app/com.beatgames.beatsaber-1/lib/arm/libil2cpp.so", 1);
        *(void **)(&get_method_from_name) = dlsym(imagehandle, "il2cpp_class_get_method_from_name");
        *(void **)(&runtime_invoke) = dlsym(imagehandle, "il2cpp_runtime_invoke");
        *(void **)(&class_get_type) = dlsym(imagehandle, "il2cpp_class_get_type");
        *(void **)(&type_get_object) = dlsym(imagehandle, "il2cpp_type_get_object");
        *(void **)(&object_unbox) = dlsym(imagehandle, "il2cpp_object_unbox");
        dlclose(imagehandle);
    }
    log("Got il2cpp api functions for Custom Sabers.");
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

    if (assetBundleFromFileAsync == nullptr)
        assetBundleFromFileAsync = get_method_from_name(assetBundleClass, "LoadFromFileAsync", 1);
    if (assetBundleFromAsync == nullptr)
        assetBundleFromAsync = get_method_from_name(assetBundleCreateRequestClass, "get_assetBundle", 0);
    if (loadAssetAsync == nullptr)
        loadAssetAsync = get_method_from_name(assetBundleClass, "LoadAssetAsync", 2);
    if (getAsset == nullptr)
        getAsset = get_method_from_name(assetBundleRequestClass, "get_asset", 0);
    if (objectInstantiate == nullptr)
        objectInstantiate = get_method_from_name(objectClass, "Instantiate", 1);

    if (findGameObject == nullptr)
        findGameObject = get_method_from_name(gameObjectClass, "Find", 1);
    if (getGameObjectTransform == nullptr)
        getGameObjectTransform = get_method_from_name(gameObjectClass, "get_transform", 0);
    if (gameObjectSetActive == nullptr)
        gameObjectSetActive = get_method_from_name(gameObjectClass, "SetActive", 1);
    if (findTransform == nullptr)
        findTransform = get_method_from_name(transformClass, "Find", 1);
    if (transformPosGet == nullptr)

        transformPosGet = get_method_from_name(transformClass, "get_position", 0);
    if (transformPosSet == nullptr)
        transformPosSet = get_method_from_name(transformClass, "set_position", 1);
    if (transformLocalPosGet == nullptr)
        transformLocalPosGet = get_method_from_name(transformClass, "get_localPosition", 0);
    if (transformLocalPosSet == nullptr)
        transformLocalPosSet = get_method_from_name(transformClass, "set_localPosition", 1);
    if (transformEulerGet == nullptr)
        transformEulerGet = get_method_from_name(transformClass, "get_eulerAngles", 0);
    if (transformEulerSet == nullptr)
        transformEulerSet = get_method_from_name(transformClass, "set_eulerAngles", 1);
    if (transformLocalEulerGet == nullptr)
        transformLocalEulerGet = get_method_from_name(transformClass, "get_localEulerAngles", 0);
    if (transformLocalEulerSet == nullptr)
        transformLocalEulerSet = get_method_from_name(transformClass, "set_localEulerAngles", 1);
    if (transformParentGet == nullptr)
        transformParentGet = get_method_from_name(transformClass, "get_parent", 0);
    if (transformParentSet == nullptr)
        transformParentSet = get_method_from_name(transformClass, "set_parent", 1);

    if (asyncOperationSetAllowSceneActivation == nullptr)
        asyncOperationSetAllowSceneActivation = get_method_from_name(asyncOperationClass, "set_allowSceneActivation", 1);
    if (asyncOperationGetIsDone == nullptr)
        asyncOperationGetIsDone = get_method_from_name(asyncOperationClass, "get_isDone", 0);

    if (saberTypeGet == nullptr)
        saberTypeGet = get_method_from_name(saberClass, "get_saberType", 0);

    if (componentGetGameObject == nullptr)
        componentGetGameObject = get_method_from_name(componentClass, "get_gameObject", 0);
    if (componentGetComponentsInChildren == nullptr)
        componentGetComponentsInChildren = get_method_from_name(componentClass, "GetComponentsInChildren", 2);
    if (resourcesFindObjectsOfTypeAll == nullptr)
        resourcesFindObjectsOfTypeAll = get_method_from_name(resourcesClass, "FindObjectsOfTypeAll", 1);

    if (materialGetFloat == nullptr)
        materialGetFloat = get_method_from_name(materialClass, "GetFloat", 1);
    if (materialHasProperty == nullptr)
        materialHasProperty = get_method_from_name(materialClass, "HasProperty", 1);
    if (materialSetColor == nullptr)
        materialSetColor = get_method_from_name(materialClass, "SetColor", 2);
    if (rendererGetSharedMaterials == nullptr)
        rendererGetSharedMaterials = get_method_from_name(rendererClass, "get_sharedMaterials", 0);
    if (shaderPropertyToID == nullptr)
        shaderPropertyToID = get_method_from_name(shaderClass, "PropertyToID", 1);

    if (colorManagerColorForSaberType == nullptr)
        colorManagerColorForSaberType = get_method_from_name(colorManagerClass, "ColorForSaberType", 1);
}

void ReplaceSaber(void *saber, void *customSaberObject)
{
    Il2CppException *exception;
    //Get GameObject for Saber
    void *saberGameObject = runtime_invoke(componentGetGameObject, saber, nullptr, &exception);
    log("Got Saber GameObject");
    //Get Transform for CustomSaberObject
    void *customSaberGameObjectTransform = runtime_invoke(getGameObjectTransform, customSaberGameObject, nullptr, &exception);
    log("Got CustomSaberObject Transform");
    //Check Type of Saber
    int saberType = *(reinterpret_cast<int *>(object_unbox(runtime_invoke(saberTypeGet, saber, nullptr, &exception))));
    cs_string *saberName = createcsstr((saberType == 0 ? "LeftSaber" : "RightSaber"));
    void *saberChildParams[] = {saberName};
    void *childTransform = runtime_invoke(findTransform, customSaberGameObjectTransform, saberChildParams, &exception);
    log("Got Child Saber Transform");
    void *parentSaberTransform = runtime_invoke(getGameObjectTransform, saberGameObject, nullptr, &exception);
    log("Got Parent Saber Transform");
    void *parentPos = object_unbox(runtime_invoke(transformPosGet, parentSaberTransform, nullptr, &exception));
    void *parentRot = object_unbox(runtime_invoke(transformEulerGet, parentSaberTransform, nullptr, &exception));
    log("Got Parent Saber Transform Position and Rotation");
    //   Vector3 *ParentPos = reinterpret_cast<Vector3 *>(object_unbox(parentRot));
    //  log("Parent rotation: %f %f %f", ParentPos->x, ParentPos->y, ParentPos->z);
    //Disable Original Saber Mesh
    log("Disabling Original Saber Meshes");
    bool getInactive = false;
    void *getMeshFiltersParams[] = {type_get_object(class_get_type(meshFilterClass)), &getInactive};
    Array<void *> *meshfilters = reinterpret_cast<Array<void *> *>(runtime_invoke(componentGetComponentsInChildren, parentSaberTransform, getMeshFiltersParams, &exception));
    for (int i = 0; i < meshfilters->Length(); i++)
    {
        log("Getting Filter Gameobject");
        void *filterObject = runtime_invoke(componentGetGameObject, meshfilters->values[i], nullptr, &exception);
        log("Disabling Filter");
        void *disableParam[] = {&getInactive};
        runtime_invoke(gameObjectSetActive, filterObject, disableParam, &exception);
    }
    log("Disabled Original Saber Meshes");
    //Place Custom Sabers
    runtime_invoke(transformParentSet, childTransform, &parentSaberTransform, &exception);
    log("Set Child Parent");
    runtime_invoke(transformPosSet, childTransform, &parentPos, &exception);
    log("Set Child Pos");
    runtime_invoke(transformEulerSet, childTransform, &parentRot, &exception);
    log("Set Child Rot");

    //Match ColorManager Colors
    log("Attempting to set colors of saber to colorManager Colors");
    void *allcolorManagersParams[] = {type_get_object(class_get_type(colorManagerClass))};
    Array<void *> *colorManagers = reinterpret_cast<Array<void *> *>(runtime_invoke(resourcesFindObjectsOfTypeAll, nullptr, allcolorManagersParams, &exception));
    if (colorManagers != nullptr)
    {
        log("Getting Color Manager");
        void *colorManager = colorManagers->values[0];
        if (colorManager != nullptr)
        {
            log("Got Color Manager");
            void *colorForSaberTypeParams[] = {&saberType};
            Color colorForType = *(reinterpret_cast<Color *>(object_unbox(runtime_invoke(colorManagerColorForSaberType, colorManager, colorForSaberTypeParams, &exception))));
            log("Got Color for type");
            void *getRendererParams[] = {type_get_object(class_get_type(rendererClass)), &getInactive};
            Array<void *> *renderers = reinterpret_cast<Array<void *> *>(runtime_invoke(componentGetComponentsInChildren, childTransform, getRendererParams, &exception));
            log("Got Renderers %s", renderers == nullptr ? "False" : "true");
            for (int i = 0; i < renderers->Length(); ++i)
            {
                log("Checking Renderer");
                Array<void *> *sharedMaterials = reinterpret_cast<Array<void *> *>(runtime_invoke(rendererGetSharedMaterials, renderers->values[i], nullptr, &exception));
                for (int j = 0; j < sharedMaterials->Length(); ++j)
                {
                    log("Checking Material");
                    cs_string *glowString = createcsstr("_Glow");
                    cs_string *bloomString = createcsstr("_Bloom");
                    cs_string *materialColor = createcsstr("_Color");
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
                        log("Has Glow, getting float");
                        float glowFloat = *(reinterpret_cast<float *>(object_unbox(runtime_invoke(materialGetFloat, sharedMaterials->values[j], glowIntParams, &exception))));
                        log("Glow Float %f", glowFloat);
                        if (glowFloat > 0)
                            setColor = true;
                    }
                    if (!setColor)
                    {
                        bool hasBloom = runtime_invoke(materialHasProperty, sharedMaterials->values[j], bloomIntParams, &exception);
                        if (hasBloom)
                        {
                            log("Has Bloom, getting float");
                            float bloomFloat = *(reinterpret_cast<float *>(object_unbox(runtime_invoke(materialGetFloat, sharedMaterials->values[j], bloomIntParams, &exception))));
                            log("Bloom Float %f", bloomFloat);
                            if (bloomFloat > 0)
                                setColor = true;
                        }
                    }
                    if (setColor)
                    {
                        log("Setting Color");
                        runtime_invoke(materialSetColor, sharedMaterials->values[j], materialColorParams, &exception);
                    }
                }
            }
        }
        else
        {
            log("null colorManager");
        }
    }
    else
    {
        log("null colorManagers");
    }
}
