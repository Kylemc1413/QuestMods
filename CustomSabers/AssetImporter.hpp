#include "../beatsaber-hook/shared/utils/utils.h"
#include <functional>

class AssetImporter : public Il2CppObject {
public:
    Il2CppString* assetFilePath;
    Il2CppString* assetName;
    Il2CppObject* assetType;
    std::function<void(AssetImporter*)> whenDone;

    bool pathExists = false;
    bool loadedAssetBundle = false;
    bool loadedAsset = false;

    AssetImporter(std::string_view path, std::string_view assetS, decltype(whenDone) callback,
        std::string_view assetNamespace = "UnityEngine", std::string_view assetClassName = "GameObject")
    {
        assetFilePath = CRASH_UNLESS(il2cpp_utils::createcsstr(path));
        assetName = CRASH_UNLESS(il2cpp_utils::createcsstr(assetS));
        assetType = CRASH_UNLESS(il2cpp_utils::GetSystemType(assetNamespace, assetClassName));
        whenDone = callback;

        pathExists = *CRASH_UNLESS(il2cpp_utils::RunMethod<bool>("System.IO", "File", "Exists", assetFilePath));

        this->LoadAssetBundle();
    }
    Il2CppObject* InstantiateAsset(std::string_view nameSpace = "UnityEngine", std::string_view klass = "GameObject",
        std::string_view method = "Instantiate") const;

private:
    Il2CppObject* bundleAsync = nullptr;
    Il2CppObject* assetBundle = nullptr;
    Il2CppObject* assetAsync = nullptr;
    Il2CppObject* asset = nullptr;

    bool LoadAssetBundle();
    bool LoadAsset();

    static bool AssetBundleComplete(AssetImporter* obj, Il2CppObject* asyncOp);
    static bool AssetComplete(AssetImporter* obj, Il2CppObject* asyncOp);
};