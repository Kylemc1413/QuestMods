#include "AssetImporter.hpp"

// Note: if too much time passes since AssetComplete, asset becomes corrupted?
Il2CppObject* AssetImporter::InstantiateAsset(std::string_view nameSpace, std::string_view klass, std::string_view method) const {
    RET_0_UNLESS(loadedAsset);
    auto* cAsset = CRASH_UNLESS(il2cpp_functions::object_get_class(asset));
    if (cAsset == il2cpp_functions::defaults->string_class) {
        Il2CppString* str = reinterpret_cast<Il2CppString*>(asset);
        log(ERROR, "asset is string?! '%s'", to_utf8(csstrtostr(str)).c_str());
        return nullptr;
    } else {
        log(INFO, "asset is %s ?", il2cpp_utils::ClassStandardName(cAsset).c_str());
    }
    auto* ret = *RET_0_UNLESS(il2cpp_utils::RunMethod(nameSpace, klass, method, asset));
    log(INFO, "Instantiated Asset Object (%p)", ret);
    return ret;
}

bool AssetImporter::AssetComplete(AssetImporter* obj, Il2CppObject* asyncOp) {
    CRASH_UNLESS(il2cpp_utils::GetPropertyValue<bool>(asyncOp, "isDone").value_or(false));
    obj->asset = *RET_0_UNLESS(il2cpp_utils::RunMethod(asyncOp, "get_asset"));
    
    auto* cAsset = CRASH_UNLESS(il2cpp_functions::object_get_class(obj->asset));
    if (cAsset == il2cpp_functions::defaults->string_class) {
        Il2CppString* str = reinterpret_cast<Il2CppString*>(obj->asset);
        log(ERROR, "assetAsync returned string '%s'", to_utf8(csstrtostr(str)).c_str());
        return false;
    } else {
        log(INFO, "asset is %s ?", il2cpp_utils::ClassStandardName(cAsset).c_str());
    }
    obj->loadedAsset = true;
    obj->whenDone(obj);
    return true;
}

bool AssetImporter::LoadAsset() {
    log(INFO, "Loading asset '%s'", to_utf8(csstrtostr(assetName)).c_str());
    assetAsync = *RET_0_UNLESS(il2cpp_utils::RunMethod(assetBundle, "LoadAssetAsync", assetName, assetType));

    auto method = il2cpp_utils::FindMethodUnsafe(assetAsync, "add_completed", 1);
    auto action = il2cpp_utils::MakeAction(method, 0, this, AssetComplete);

    RET_0_UNLESS(il2cpp_utils::RunMethod(assetAsync, method, action));
    log(INFO, "Began loading asset async");
    return true;
}

bool AssetImporter::AssetBundleComplete(AssetImporter* obj, Il2CppObject* asyncOp) {
    obj->assetBundle = *RET_0_UNLESS(il2cpp_utils::RunMethod(asyncOp, "get_assetBundle"));
    obj->loadedAssetBundle = true;
    return obj->LoadAsset();
}

bool AssetImporter::LoadAssetBundle() {
    if (!pathExists) {
        pathExists = *RET_0_UNLESS(il2cpp_utils::RunMethod<bool>("System.IO", "File", "Exists", assetFilePath));
    }
    RET_0_UNLESS(pathExists);
    if (!bundleAsync) {
        bundleAsync = *RET_0_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "AssetBundle", "LoadFromFileAsync", assetFilePath));
        RET_0_UNLESS(il2cpp_utils::RunMethod(bundleAsync, "set_allowSceneActivation", true));

        auto method = RET_0_UNLESS(il2cpp_utils::FindMethodUnsafe(bundleAsync, "add_completed", 1));
        auto action = RET_0_UNLESS(il2cpp_utils::MakeAction(method, 0, this, AssetBundleComplete));

        RET_0_UNLESS(il2cpp_utils::RunMethod(bundleAsync, method, action));
        log(INFO, "Began loading bundle async");
    }
    return true;
}