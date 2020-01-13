#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <array>
#include <vector>
#include <limits>
#include <map>
#include "../beatsaber-hook/shared/utils/utils.h"

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
static const MethodInfo *transformRotationSet;
static const MethodInfo *transformRotationGet;
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
MAKE_HOOK_OFFSETLESS(SceneManager_SetActiveScene, bool, int scene)
{
    log(INFO, "Called SceneManager_SetActiveScene hook");
    bool result = SceneManager_SetActiveScene(scene);

    if (sceneClass == nullptr)
        sceneClass = il2cpp_utils::GetClassFromName("UnityEngine.SceneManagement", "Scene");
    //Get scene name method
    if (sceneNameMethodInfo == nullptr)
        sceneNameMethodInfo = il2cpp_functions::class_get_method_from_name(sceneClass, "GetNameInternal", 1);

    //Get Scene Name

    Il2CppException *exception = nullptr;
    void *sceneNameparams[] = {&scene};
    auto nameResult = il2cpp_functions::runtime_invoke(sceneNameMethodInfo, nullptr, sceneNameparams, &exception);
    Il2CppString *csName = reinterpret_cast<Il2CppString *>(nameResult);
    auto sceneName = to_utf8(csstrtostr(csName)).c_str();

    //Code to run if menuCore
    if (std::strncmp(sceneName, "MenuCore", 8) == 0)
    {
        log(INFO, "MenuCore Scene");
    }
    return result;
}

void *customSaberAssetBundle;
void *customSaberGameObject;

MAKE_HOOK_OFFSETLESS(GameplayCoreSceneSetup_Start, void, void *self)
{
    log(INFO, "Called GameplayCoreSceneSetup_Start hook");
    GameplayCoreSceneSetup_Start(self);

    GrabMethods();
    if (asyncBundle == nullptr)
    {
        Il2CppException *exception;
        Il2CppString *assetFilePath = il2cpp_utils::createcsstr("/sdcard/Android/data/com.beatgames.beatsaber/files/sabers/testSaber.qsaber");
        void *fromFileParams[] = {assetFilePath};
        asyncBundle = il2cpp_functions::runtime_invoke(assetBundleFromFileAsync, nullptr, fromFileParams, &exception);
        bool sceneActivationValue = true;
        void *setSceneActivationParams[] = {&sceneActivationValue};
        il2cpp_functions::runtime_invoke(asyncOperationSetAllowSceneActivation, asyncBundle, setSceneActivationParams, &exception);
        log(INFO, "Loaded Async Bundle");
    }
    customSaberGameObject = nullptr;
}
MAKE_HOOK_OFFSETLESS(TutorialController_Awake, void, void *self)
{
    log(INFO, "Called TutorialController_Awake hook");
    TutorialController_Awake(self);
    GrabMethods();
    if (asyncBundle == nullptr)
    {
        Il2CppException *exception;
        Il2CppString *assetFilePath = il2cpp_utils::createcsstr("/sdcard/Android/data/com.beatgames.beatsaber/files/sabers/testSaber.qsaber");
        void *fromFileParams[] = {assetFilePath};
        asyncBundle = il2cpp_functions::runtime_invoke(assetBundleFromFileAsync, nullptr, fromFileParams, &exception);
        bool sceneActivationValue = true;
        void *setSceneActivationParams[] = {&sceneActivationValue};
        il2cpp_functions::runtime_invoke(asyncOperationSetAllowSceneActivation, asyncBundle, setSceneActivationParams, &exception);
        log(INFO, "Loaded Async Bundle");
    }
    customSaberGameObject = nullptr;
}
void ReplaceSaber(void *, void *);
MAKE_HOOK_OFFSETLESS(Saber_Start, void, void *self)
{
    Saber_Start(self);
    log(INFO, "Called Saber_Start hook");
    //Load Custom Saber Objects if not loaded
    Il2CppException *exception = nullptr;
    if (customSaberAssetBundle == nullptr)
    {
        customSaberAssetBundle = il2cpp_functions::runtime_invoke(assetBundleFromAsync, asyncBundle, nullptr, &exception);
        log(INFO, "Grabbed Asset bundle");
        asyncBundle = nullptr;
    }
    if (customSaberGameObject == nullptr && customSaberAssetBundle != nullptr)
    {
        Il2CppString *assetPath = il2cpp_utils::createcsstr("_customsaber");
        void *assetPathParams[] = {assetPath, il2cpp_functions::type_get_object(il2cpp_functions::class_get_type(gameObjectClass))};
        void *assetAsync = il2cpp_functions::runtime_invoke(loadAssetAsync, customSaberAssetBundle, assetPathParams, &exception);
        if (exception != nullptr)
        {
            const MethodInfo *exceptionToString = il2cpp_functions::class_get_method_from_name(exception->klass, "ToString", 0);
            void *exceptionString = il2cpp_functions::runtime_invoke(exceptionToString, exception, nullptr, &exception);
            Il2CppString *message = reinterpret_cast<Il2CppString *>(exceptionString);
            log(INFO, "Exception: %s", to_utf8(csstrtostr(message)).c_str());
        }
        log(INFO, "Grabbed Asset Async Request");

        void *customSaberObject = il2cpp_functions::runtime_invoke(getAsset, assetAsync, nullptr, &exception);
        if (exception != nullptr)
        {
            const MethodInfo *exceptionToString = il2cpp_functions::class_get_method_from_name(exception->klass, "ToString", 0);
            void *exceptionString = il2cpp_functions::runtime_invoke(exceptionToString, exception, nullptr, &exception);
            Il2CppString *message = reinterpret_cast<Il2CppString *>(exceptionString);
            log(INFO, "Exception: %s", to_utf8(csstrtostr(message)).c_str());
        }
        log(INFO, "Grabbed Asset Object");

        //Attempt to Instaniate GameObject
        void *instantiateParams[] = {customSaberObject};
        customSaberGameObject = il2cpp_functions::runtime_invoke(objectInstantiate, nullptr, instantiateParams, &exception);
        log(INFO, "Instantiated Asset Object");
		log(INFO, "Pointer?%p", customSaberGameObject);
    }

    if (customSaberGameObject != nullptr)
    {
        log(INFO, "Replacing Saber with Custom Saber");
        ReplaceSaber(self, customSaberGameObject);
    }
}
extern "C" void load()
{
    il2cpp_functions::Init();
    log(INFO, "Installing Custom Sabers Hooks!");
    INSTALL_HOOK_OFFSETLESS(SceneManager_SetActiveScene, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("UnityEngine.SceneManagement", "SceneManager"), "SetActiveScene", 1));
    INSTALL_HOOK_OFFSETLESS(GameplayCoreSceneSetup_Start, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "GameplayCoreSceneSetup"), "Start", 0));
    INSTALL_HOOK_OFFSETLESS(TutorialController_Awake, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "TutorialController"), "Awake", 0));
    INSTALL_HOOK_OFFSETLESS(Saber_Start, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "Saber"), "Start", 0));
    log(INFO, "Installed Custom Sabers Hooks!");
}

void GrabMethods()
{
    if (assetBundleClass == nullptr)
        assetBundleClass = il2cpp_utils::GetClassFromName("UnityEngine", "AssetBundle");
    if (assetBundleCreateRequestClass == nullptr)
        assetBundleCreateRequestClass = il2cpp_utils::GetClassFromName("UnityEngine", "AssetBundleCreateRequest");
    if (assetBundleRequestClass == nullptr)
        assetBundleRequestClass = il2cpp_utils::GetClassFromName("UnityEngine", "AssetBundleRequest");
    if (gameObjectClass == nullptr)
        gameObjectClass = il2cpp_utils::GetClassFromName("UnityEngine", "GameObject");
    if (objectClass == nullptr)
        objectClass = il2cpp_utils::GetClassFromName("UnityEngine", "Object");
    if (transformClass == nullptr)
        transformClass = il2cpp_utils::GetClassFromName("UnityEngine", "Transform");
    if (asyncOperationClass == nullptr)
        asyncOperationClass = il2cpp_utils::GetClassFromName("UnityEngine", "AsyncOperation");
    if (saberClass == nullptr)
        saberClass = il2cpp_utils::GetClassFromName("", "Saber");
    if (componentClass == nullptr)
        componentClass = il2cpp_utils::GetClassFromName("UnityEngine", "Component");
    if (meshFilterClass == nullptr)
        meshFilterClass = il2cpp_utils::GetClassFromName("UnityEngine", "MeshFilter");
    if (resourcesClass == nullptr)
        resourcesClass = il2cpp_utils::GetClassFromName("UnityEngine", "Resources");
    if (colorManagerClass == nullptr)
        colorManagerClass = il2cpp_utils::GetClassFromName("", "ColorManager");
    if (rendererClass == nullptr)
        rendererClass = il2cpp_utils::GetClassFromName("UnityEngine", "Renderer");
    if (materialClass == nullptr)
        materialClass = il2cpp_utils::GetClassFromName("UnityEngine", "Material");
    if (shaderClass == nullptr)
        shaderClass = il2cpp_utils::GetClassFromName("UnityEngine", "Shader");
    if (beatmapObjectSpawnControllerClass == nullptr)
        beatmapObjectSpawnControllerClass = il2cpp_utils::GetClassFromName("", "BeatmapObjectSpawnController");
    if (actionThree == nullptr)
        actionThree = il2cpp_utils::GetClassFromName("System", "Action`3");

    if (assetBundleFromFileAsync == nullptr)
        assetBundleFromFileAsync = il2cpp_functions::class_get_method_from_name(assetBundleClass, "LoadFromFileAsync", 1);
    if (assetBundleFromAsync == nullptr)
        assetBundleFromAsync = il2cpp_functions::class_get_method_from_name(assetBundleCreateRequestClass, "get_assetBundle", 0);
    if (loadAssetAsync == nullptr)
        loadAssetAsync = il2cpp_functions::class_get_method_from_name(assetBundleClass, "LoadAssetAsync", 2);
    if (getAsset == nullptr)
        getAsset = il2cpp_functions::class_get_method_from_name(assetBundleRequestClass, "get_asset", 0);
    if (objectInstantiate == nullptr)
        objectInstantiate = il2cpp_functions::class_get_method_from_name(objectClass, "Instantiate", 1);

    if (findGameObject == nullptr)
        findGameObject = il2cpp_functions::class_get_method_from_name(gameObjectClass, "Find", 1);
    if (getGameObjectTransform == nullptr)
        getGameObjectTransform = il2cpp_functions::class_get_method_from_name(gameObjectClass, "get_transform", 0);
    if (gameObjectSetActive == nullptr)
        gameObjectSetActive = il2cpp_functions::class_get_method_from_name(gameObjectClass, "SetActive", 1);
    if (findTransform == nullptr)
        findTransform = il2cpp_functions::class_get_method_from_name(transformClass, "Find", 1);
    if (transformPosGet == nullptr)
        transformPosGet = il2cpp_functions::class_get_method_from_name(transformClass, "get_position", 0);
    if (transformPosSet == nullptr)
        transformPosSet = il2cpp_functions::class_get_method_from_name(transformClass, "set_position", 1);
    if (transformLocalPosGet == nullptr)
        transformLocalPosGet = il2cpp_functions::class_get_method_from_name(transformClass, "get_localPosition", 0);
    if (transformLocalPosSet == nullptr)
        transformLocalPosSet = il2cpp_functions::class_get_method_from_name(transformClass, "set_localPosition", 1);
    if (transformRotationGet == nullptr)
        transformRotationGet = il2cpp_functions::class_get_method_from_name(transformClass, "get_rotation", 0);
    if (transformRotationSet == nullptr)
        transformRotationSet = il2cpp_functions::class_get_method_from_name(transformClass, "set_rotation", 1);
    if (transformLocalEulerGet == nullptr)
        transformLocalEulerGet = il2cpp_functions::class_get_method_from_name(transformClass, "get_localEulerAngles", 0);
    if (transformLocalEulerSet == nullptr)
        transformLocalEulerSet = il2cpp_functions::class_get_method_from_name(transformClass, "set_localEulerAngles", 1);
    if (transformParentGet == nullptr)
        transformParentGet = il2cpp_functions::class_get_method_from_name(transformClass, "get_parent", 0);
    if (transformParentSet == nullptr)
        transformParentSet = il2cpp_functions::class_get_method_from_name(transformClass, "set_parent", 1);

    if (asyncOperationSetAllowSceneActivation == nullptr)
        asyncOperationSetAllowSceneActivation = il2cpp_functions::class_get_method_from_name(asyncOperationClass, "set_allowSceneActivation", 1);
    if (asyncOperationGetIsDone == nullptr)
        asyncOperationGetIsDone = il2cpp_functions::class_get_method_from_name(asyncOperationClass, "get_isDone", 0);

    if (saberTypeGet == nullptr)
        saberTypeGet = il2cpp_functions::class_get_method_from_name(saberClass, "get_saberType", 0);

    if (componentGetGameObject == nullptr)
        componentGetGameObject = il2cpp_functions::class_get_method_from_name(componentClass, "get_gameObject", 0);
    if (componentGetComponentsInChildren == nullptr)
        componentGetComponentsInChildren = il2cpp_functions::class_get_method_from_name(componentClass, "GetComponentsInChildren", 2);
    if (resourcesFindObjectsOfTypeAll == nullptr)
        resourcesFindObjectsOfTypeAll = il2cpp_functions::class_get_method_from_name(resourcesClass, "FindObjectsOfTypeAll", 1);

    if (materialGetFloat == nullptr)
        materialGetFloat = il2cpp_functions::class_get_method_from_name(materialClass, "GetFloat", 1);
    if (materialHasProperty == nullptr)
        materialHasProperty = il2cpp_functions::class_get_method_from_name(materialClass, "HasProperty", 1);
    if (materialSetColor == nullptr)
        materialSetColor = il2cpp_functions::class_get_method_from_name(materialClass, "SetColor", 2);
    if (rendererGetSharedMaterials == nullptr)
        rendererGetSharedMaterials = il2cpp_functions::class_get_method_from_name(rendererClass, "get_sharedMaterials", 0);
    if (shaderPropertyToID == nullptr)
        shaderPropertyToID = il2cpp_functions::class_get_method_from_name(shaderClass, "PropertyToID", 1);

    if (colorManagerColorForSaberType == nullptr)
        colorManagerColorForSaberType = il2cpp_functions::class_get_method_from_name(colorManagerClass, "ColorForSaberType", 1);
    if (addSpawnControllerNoteWasCut == nullptr)
        addSpawnControllerNoteWasCut = il2cpp_functions::class_get_method_from_name(beatmapObjectSpawnControllerClass, "add_noteWasCutEvent", 1);
}
void *GetFirstObjectOfType(Il2CppClass *);
void SpawnControllerNoteWasCut(void *BeatmapObjectSpawnController, void *NoteController, void *NoteCutInfo);
void ReplaceSaber(void *saber, void *customSaberObject)
{
    Il2CppException *exception;
    void *saberGameObject = il2cpp_functions::runtime_invoke(componentGetGameObject, saber, nullptr, &exception);
    void *customSaberGameObjectTransform = il2cpp_functions::runtime_invoke(getGameObjectTransform, customSaberGameObject, nullptr, &exception);
    int saberType = *(reinterpret_cast<int *>(il2cpp_functions::il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(saberTypeGet, saber, nullptr, &exception))));
    Il2CppString *saberName = il2cpp_utils::createcsstr((saberType == 0 ? "LeftSaber" : "RightSaber"));
    void *saberChildParams[] = {saberName};
    void *childTransform = il2cpp_functions::runtime_invoke(findTransform, customSaberGameObjectTransform, saberChildParams, &exception);
    void *parentSaberTransform = il2cpp_functions::runtime_invoke(getGameObjectTransform, saberGameObject, nullptr, &exception);
    void *parentPos = il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(transformPosGet, parentSaberTransform, nullptr, &exception));
    void *parentRot = il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(transformRotationGet, parentSaberTransform, nullptr, &exception));
    bool getInactive = false;
    void *getMeshFiltersParams[] = {il2cpp_functions::type_get_object(il2cpp_functions::class_get_type(meshFilterClass)), &getInactive};
    Array<void *> *meshfilters = reinterpret_cast<Array<void *> *>(il2cpp_functions::runtime_invoke(componentGetComponentsInChildren, parentSaberTransform, getMeshFiltersParams, &exception));
    for (int i = 0; i < meshfilters->Length(); i++)
    {
        void *filterObject = il2cpp_functions::runtime_invoke(componentGetGameObject, meshfilters->values[i], nullptr, &exception);
        void *disableParam[] = {&getInactive};
        il2cpp_functions::runtime_invoke(gameObjectSetActive, filterObject, disableParam, &exception);
    }
    log(INFO, "Disabled Original Saber Meshes");

    il2cpp_functions::runtime_invoke(transformParentSet, childTransform, &parentSaberTransform, &exception);
    il2cpp_functions::runtime_invoke(transformPosSet, childTransform, &parentPos, &exception);
    il2cpp_functions::runtime_invoke(transformRotationSet, childTransform, &parentRot, &exception);
    log(INFO, "Placed Custom Saber");
    log(INFO, "Attempting to set colors of Custom Saber to colorManager Colors");
    void *colorManager = GetFirstObjectOfType(colorManagerClass);
    if (colorManager != nullptr)
    {
        void *colorForSaberTypeParams[] = {&saberType};
        Color colorForType = *(reinterpret_cast<Color *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(colorManagerColorForSaberType, colorManager, colorForSaberTypeParams, &exception))));
        void *getRendererParams[] = {il2cpp_functions::type_get_object(il2cpp_functions::class_get_type(rendererClass)), &getInactive};
        Array<void *> *renderers = reinterpret_cast<Array<void *> *>(il2cpp_functions::runtime_invoke(componentGetComponentsInChildren, childTransform, getRendererParams, &exception));
        for (int i = 0; i < renderers->Length(); ++i)
        {
            Array<void *> *sharedMaterials = reinterpret_cast<Array<void *> *>(il2cpp_functions::runtime_invoke(rendererGetSharedMaterials, renderers->values[i], nullptr, &exception));
            for (int j = 0; j < sharedMaterials->Length(); ++j)
            {
                Il2CppString *glowString = il2cpp_utils::createcsstr("_Glow");
                Il2CppString *bloomString = il2cpp_utils::createcsstr("_Bloom");
                Il2CppString *materialColor = il2cpp_utils::createcsstr("_Color");
                void *glowStringParams[] = {glowString};
                void *bloomStringParams[] = {bloomString};
                int glowInt = *(reinterpret_cast<int *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(shaderPropertyToID, nullptr, glowStringParams, &exception))));
                int bloomInt = *(reinterpret_cast<int *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(shaderPropertyToID, nullptr, bloomStringParams, &exception))));
                void *glowIntParams[] = {&glowInt};
                void *bloomIntParams[] = {&bloomInt};
                void *materialColorParams[] = {materialColor, &colorForType};
                bool setColor = false;
                bool hasGlow = il2cpp_functions::runtime_invoke(materialHasProperty, sharedMaterials->values[j], glowIntParams, &exception);
                if (hasGlow)
                {
                    float glowFloat = *(reinterpret_cast<float *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(materialGetFloat, sharedMaterials->values[j], glowIntParams, &exception))));
                    if (glowFloat > 0)
                        setColor = true;
                }
                if (!setColor)
                {
                    bool hasBloom = il2cpp_functions::runtime_invoke(materialHasProperty, sharedMaterials->values[j], bloomIntParams, &exception);
                    if (hasBloom)
                    {
                        float bloomFloat = *(reinterpret_cast<float *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(materialGetFloat, sharedMaterials->values[j], bloomIntParams, &exception))));
                        if (bloomFloat > 0)
                            setColor = true;
                    }
                }
                if (setColor)
                {
                    il2cpp_functions::runtime_invoke(materialSetColor, sharedMaterials->values[j], materialColorParams, &exception);
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
    void *params[] = {il2cpp_functions::type_get_object(il2cpp_functions::class_get_type(klass))};
    Array<void *> *objects = reinterpret_cast<Array<void *> *>(il2cpp_functions::runtime_invoke(resourcesFindObjectsOfTypeAll, nullptr, params, &exception));
    if (objects != nullptr)
    {
        return objects->values[0];
    }
    else
    {
        return nullptr;
    }
}