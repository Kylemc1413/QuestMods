#include <array>
#include <limits>
#include <map>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapDataLoader.hpp"
#include "GlobalNamespace/BeatmapEventData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnMovementData.hpp"
#include "GlobalNamespace/BeatmapObjectType.hpp"
#include "GlobalNamespace/BeatmapSaveData_EventData.hpp"
#include "GlobalNamespace/BeatmapSaveData_NoteData.hpp"
#include "GlobalNamespace/BeatmapSaveData_ObstacleData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/NoteCutDirectionExtensions.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/NoteLineLayer.hpp"
#include "GlobalNamespace/NotesInTimeRowProcessor.hpp"
#include "GlobalNamespace/ObstacleController.hpp"
#include "GlobalNamespace/ObstacleData.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/SpawnRotationProcessor.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StretchableObstacle.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "System/Decimal.hpp"
#include "System/Single.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/Graphics.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
using namespace GlobalNamespace;

ModInfo modInfo;
const Logger& logger()
{
    static const Logger& logger(modInfo);
    return logger;
}

extern "C" void setup(ModInfo& info)
{
    info.id      = "MappingExtensions";
    info.version = "0.17.3";
    modInfo      = info;
    logger().info("Leaving setup!");
}

[[maybe_unused]] static void dump_real(int before, int after, void* ptr)
{
    logger().info("Dumping Immediate Pointer: %p: %lx", ptr, *reinterpret_cast<long*>(ptr));
    auto begin = static_cast<long*>(ptr) - before;
    auto end   = static_cast<long*>(ptr) + after;
    for (auto cur = begin; cur != end; ++cur) {
        logger().info("0x%lx: %lx", (long)cur - (long)ptr, *cur);
    }
}

// Normalized indices are faster to compute & reverse, and more accurate than, effective indices (see below).
// A "normalized" precision index is an effective index * 1000. So unlike normal precision indices, only 0 is 0.
int ToNormalizedPrecisionIndex(int index)
{
    if (index <= -1000) {
        return index + 1000;
    } else if (index >= 1000) {
        return index - 1000;
    } else {
        return index * 1000; // wasn't precision yet
    }
}
int FromNormalizedPrecisionIndex(int index)
{
    if (index % 1000 == 0) {
        return index / 1000;
    } else if (index > 0) {
        return index + 1000;
    } else {
        return index - 1000;
    }
}

// An effective index is a normal/extended index, but with decimal places that do what you'd expect.
float ToEffectiveIndex(int index)
{
    float effectiveIndex = index;
    if (effectiveIndex <= -1000) {
        effectiveIndex = effectiveIndex / 1000.0f + 1.0f;
    } else if (effectiveIndex >= 1000) {
        effectiveIndex = effectiveIndex / 1000.0f - 1.0f;
    }
    return effectiveIndex;
}

static IDifficultyBeatmap* storedDiffBeatmap = nullptr;
static BeatmapCharacteristicSO* storedBeatmapCharacteristicSO = nullptr;
MAKE_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent, void, StandardLevelDetailView* self)
{
    StandardLevelDetailView_RefreshContent(self);
    storedBeatmapCharacteristicSO = self->selectedDifficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic();
}
MAKE_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, void, Il2CppObject* self,
                     bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    storedBeatmapCharacteristicSO = nullptr;
    return MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

static bool skipWallRatings = false;
MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Init, void, Il2CppObject* self, float beatsPerMinute, int noteLinesCount,
    float noteJumpMovementSpeed, float noteJumpStartBeatOffset, float jumpOffsetY)
{
    if (storedDiffBeatmap) {
        float njs = storedDiffBeatmap->get_noteJumpMovementSpeed();
        if (njs < 0)
            noteJumpMovementSpeed = njs;
    }
    skipWallRatings = false;

    return BeatmapObjectSpawnController_Init(
        self, beatsPerMinute, noteLinesCount, noteJumpMovementSpeed, noteJumpStartBeatOffset, jumpOffsetY);
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectExecutionRatingsRecorder_HandleObstacleDidPassAvoidedMark, void, Il2CppObject* self,
    Il2CppObject* obstacleController)
{
    if (skipWallRatings) {
        return;
    } else {
        return BeatmapObjectExecutionRatingsRecorder_HandleObstacleDidPassAvoidedMark(self, obstacleController);
    }
}

/* PC version hooks */

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_GetNoteOffset, UnityEngine::Vector3, BeatmapObjectSpawnMovementData* self,
    int noteLineIndex, GlobalNamespace::NoteLineLayer noteLineLayer)
{
    if (noteLineIndex == 4839) {
        logger().info("lineIndex %i and lineLayer %i!", noteLineIndex, noteLineLayer.value);
    }
    auto __result = BeatmapObjectSpawnMovementData_GetNoteOffset(self, noteLineIndex, noteLineLayer);
    // if (!Plugin.active) return __result;

    if (noteLineIndex >= 1000 || noteLineIndex <= -1000) {
        if (noteLineIndex <= -1000)
            noteLineIndex += 2000;
        float num = -(self->noteLinesCount - 1.0f) * 0.5f;
        num += ((float)noteLineIndex * self->noteLinesDistance / 1000.0f);

        float yPos = self->LineYPosForLineLayer(noteLineLayer);
        __result   = UnityEngine::Vector3(num, yPos, 0.0f);
    }
    return __result;
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_HighestJumpPosYForLineLayer, float, BeatmapObjectSpawnMovementData* self,
    int lineLayer)
{
    float __result = BeatmapObjectSpawnMovementData_HighestJumpPosYForLineLayer(self, lineLayer);
    // if (!Plugin.active) return __result;
    float delta = (self->topLinesHighestJumpPosY - self->upperLinesHighestJumpPosY);

    if (lineLayer >= 1000 || lineLayer <= -1000) {
        __result = self->upperLinesHighestJumpPosY - delta - delta + self->jumpOffsetY + (lineLayer * (delta / 1000.0f));
    } else if (lineLayer > 2 || lineLayer < 0) {
        __result = self->upperLinesHighestJumpPosY - delta + self->jumpOffsetY + (lineLayer * delta);
    }
    if (__result > 2.9f) {
        logger().warning("Extreme note jump! lineLayer %i gave jump %f!", lineLayer, __result);
    }
    return __result;
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_LineYPosForLineLayer, float, BeatmapObjectSpawnMovementData* self,
    int lineLayer)
{
    float __result = BeatmapObjectSpawnMovementData_LineYPosForLineLayer(self, lineLayer);
    // if (!Plugin.active) return __result;
    float delta = (self->topLinesYPos - self->upperLinesYPos);

    if (lineLayer >= 1000 || lineLayer <= -1000) {
        __result = self->upperLinesYPos - delta - delta + (lineLayer * delta / 1000.0f);
    } else if (lineLayer > 2 || lineLayer < 0) {
        __result = self->upperLinesYPos - delta + (lineLayer * delta);
    }
    if (__result > 1.9f) {
        logger().warning("Extreme note position! lineLayer %i gave YPos %f!", lineLayer, __result);
    }
    return __result;
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_Get2DNoteOffset, UnityEngine::Vector2, BeatmapObjectSpawnMovementData* self,
    int noteLineIndex, GlobalNamespace::NoteLineLayer noteLineLayer)
{
    if (noteLineIndex == 4839) {
        logger().info("lineIndex %i and lineLayer %i!", noteLineIndex, noteLineLayer.value);
    }
    auto __result = BeatmapObjectSpawnMovementData_Get2DNoteOffset(self, noteLineIndex, noteLineLayer);
    // if (!Plugin.active) return __result;
    if (noteLineIndex >= 1000 || noteLineIndex <= -1000) {
        if (noteLineIndex <= -1000)
            noteLineIndex += 2000;
        float num = -(self->noteLinesCount - 1.0f) * 0.5f;
        float x = num + ((float)noteLineIndex * self->noteLinesDistance / 1000.0f);
        float y = self->LineYPosForLineLayer(noteLineLayer);
        __result = UnityEngine::Vector2(x, y);
    }
    return __result;
}

MAKE_HOOK_OFFSETLESS(FlyingScoreSpawner_SpawnFlyingScore, void, Il2CppObject* self, Il2CppObject* noteCutInfo, int noteLineIndex,
    int multiplier, UnityEngine::Vector3 pos, UnityEngine::Quaternion rotation, UnityEngine::Quaternion inverseRotation,
    UnityEngine::Color color)
{
    // if (!Plugin.active) {
        if (noteLineIndex < 0)
            noteLineIndex = 0;
        if (noteLineIndex > 3)
            noteLineIndex = 3;
    // }
    return FlyingScoreSpawner_SpawnFlyingScore(self, noteCutInfo, noteLineIndex, multiplier, pos, rotation, inverseRotation, color);
}

MAKE_HOOK_OFFSETLESS(NoteCutDirectionExtensions_RotationAngle, float, int cutDirection)
{
    float __result = NoteCutDirectionExtensions_RotationAngle(cutDirection);
    // if (!Plugin.active) return __result;
    if (cutDirection >= 1000 && cutDirection <= 1360) {
        __result = 1000 - cutDirection;
    } else if (cutDirection >= 2000 && cutDirection <= 2360) {
        __result = 2000 - cutDirection;
    }
    return __result;
}
MAKE_HOOK_OFFSETLESS(NoteCutDirectionExtensions_Direction, UnityEngine::Vector2, int cutDirection)
{
    UnityEngine::Vector2 __result = NoteCutDirectionExtensions_Direction(cutDirection);
    // if (!Plugin.active) return __result;
    if ((cutDirection >= 1000 && cutDirection <= 1360) ||
        (cutDirection >= 2000 && cutDirection <= 2360))
    {
        // uses RotationAngle hook indirectly
        auto quaternion = NoteCutDirectionExtensions::Rotation(cutDirection, 0.0f);
        static auto forward = UnityEngine::Vector3::get_forward();
        UnityEngine::Vector3 dir = quaternion * forward;
        __result = UnityEngine::Vector2(dir.x, dir.y);
        // logger().debug("NoteCutDirectionExtensions: {%f, %f}", dir.x, dir.y);
    }
    return __result;
}

bool MirrorPrecisionLineIndex(int& lineIndex, int lineCount)
{
    if (lineIndex >= 1000 || lineIndex <= -1000) {
        bool notVanillaRange = (lineIndex <= 0 || lineIndex > lineCount * 1000);

        int newIndex = (lineCount + 1) * 1000 - lineIndex;
        if (notVanillaRange)
            newIndex -= 2000; // this fixes the skip between 1000 and -1000 which happens once iff start or end is negative
        lineIndex = newIndex;
        return true;
    }
    return false;
}
MAKE_HOOK_OFFSETLESS(BeatmapObjectData_MirrorLineIndex, void, BeatmapObjectData* self, int lineCount)
{
    int lineIndex = self->lineIndex;
    BeatmapObjectData_MirrorLineIndex(self, lineCount);
    // if (!Plugin.active) return;
    if (MirrorPrecisionLineIndex(lineIndex, lineCount)) {
        self->lineIndex = lineIndex;
    }
}
MAKE_HOOK_OFFSETLESS(NoteData_MirrorLineIndex, void, NoteData* self, int lineCount)
{
    logger().debug("Mirroring note with time: %f, lineIndex: %i, lineLayer: %i, startNoteLineLayer: %i, cutDirection: %i",
            self->time, self->lineIndex, self->noteLineLayer, self->startNoteLineLayer, self->cutDirection);
    int lineIndex     = self->lineIndex;
    int flipLineIndex = self->flipLineIndex;
    NoteData_MirrorLineIndex(self, lineCount);
    // if (!Plugin.active) return;
    if (MirrorPrecisionLineIndex(lineIndex, lineCount)) {
        self->lineIndex = lineIndex;
    }
    if (MirrorPrecisionLineIndex(flipLineIndex, lineCount)) {
        self->flipLineIndex = flipLineIndex;
    }
}

MAKE_HOOK_OFFSETLESS(NoteData_MirrorTransformCutDirection, void, NoteData* self)
{
    int state = self->cutDirection.value;
    NoteData_MirrorTransformCutDirection(self);
    if (state >= 1000) {
        int newdir         = 2360 - state;
        self->cutDirection = newdir;
    }
}

MAKE_HOOK_OFFSETLESS(ObstacleController_Init, void, ObstacleController* self, ObstacleData* obstacleData, float worldRotation,
    UnityEngine::Vector3 startPos, UnityEngine::Vector3 midPos, UnityEngine::Vector3 endPos, float move1Duration,
    float move2Duration, float singleLineWidth, float height)
{
    ObstacleController_Init(
        self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, height);
    // if (!Plugin.active) return;
    if ((obstacleData->obstacleType.value < 1000) && !(obstacleData->width >= 1000))
        return;
    // Either wall height or wall width are precision

    skipWallRatings = true;
    int mode        = (obstacleData->obstacleType.value >= 4001 && obstacleData->obstacleType.value <= 4100000) ? 1 : 0;
    int obsHeight;
    int startHeight = 0;
    if (mode == 1) {
        int value = obstacleData->obstacleType.value;
        value -= 4001;
        obsHeight = value / 1000;
        startHeight = value % 1000;
    } else {
        int value = obstacleData->obstacleType.value;
        obsHeight = value - 1000; // won't be used unless height is precision
    }

    float num = (float)obstacleData->width * singleLineWidth;
    if ((obstacleData->width >= 1000) || (mode == 1)) {
        if (obstacleData->width >= 1000) {
            float width              = (float)obstacleData->width - 1000.0f;
            float precisionLineWidth = singleLineWidth / 1000.0f;
            num                      = width * precisionLineWidth;
        }
        // Change y of b for start height
        UnityEngine::Vector3 b { b.x = (num - singleLineWidth) * 0.5f, b.y = 4 * ((float)startHeight / 1000), b.z = 0 };

        self->startPos = startPos + b;
        self->midPos   = midPos + b;
        self->endPos   = endPos + b;
    }

    float num2       = UnityEngine::Vector3::Distance(self->endPos, self->midPos) / move2Duration;
    float length     = num2 * obstacleData->duration;
    float multiplier = 1;
    if (obstacleData->obstacleType.value >= 1000) {
        multiplier = (float)obsHeight / 1000;
    }

    self->stretchableObstacle->SetSizeAndColor((num * 0.98f), (height * multiplier), length, self->color->color);
    self->bounds = self->stretchableObstacle->bounds;
}

MAKE_HOOK_OFFSETLESS(ObstacleData_MirrorLineIndex, void, ObstacleData* self, int lineCount)
{
    int __state         = self->lineIndex;
    bool precisionWidth = (self->width >= 1000);
    ObstacleData_MirrorLineIndex(self, lineCount);
    // if (!Plugin.active) return;

    logger().debug("lineCount: %i", lineCount);
    //   Console.WriteLine("Width: " + __instance.width);
    if (__state >= 1000 || __state <= -1000 || precisionWidth) {
        int normIndex = ToNormalizedPrecisionIndex(__state);
        int normWidth = ToNormalizedPrecisionIndex(self->width);

        // The vanilla formula * 1000
        int normNewIndex = lineCount * 1000 - normWidth - normIndex;

        self->lineIndex = FromNormalizedPrecisionIndex(normNewIndex);
        logger().debug("wall (of type %i) with lineIndex %i (norm %i) and width %i (norm %i) mirrored to %i (norm %i)",
            self->obstacleType, __state, normIndex, self->width, normWidth, self->lineIndex, normNewIndex);
    }
}

MAKE_HOOK_OFFSETLESS(SpawnRotationProcessor_RotationForEventValue, float, SpawnRotationProcessor* self, int index)
{
    float __result = SpawnRotationProcessor_RotationForEventValue(self, index);
    // if (!Plugin.active) return __result;
    if (!storedBeatmapCharacteristicSO->requires360Movement) return __result;
    if (index >= 1000 && index <= 1720)
        __result = index - 1360;
    return __result;
}

/* End of PC version hooks */

/* TODO: implement https://github.com/Kylemc1413/SongCore/blob/master/HarmonyPatches/ClampPatches.cs more closely */

int lineIndex;
MAKE_HOOK_OFFSETLESS(BeatmapData_AddBeatmapObjectData, void, BeatmapData* self, BeatmapObjectData* item)
{
    lineIndex = item->lineIndex;
    // Preprocess the lineIndex to be 0-3 (the real method is hard-coded to 4 lines), recording the info needed to reverse it
    if (lineIndex > 3) {
        item->lineIndex = 3;
    } else if (lineIndex < 0) {
        item->lineIndex = 0;
    }

    BeatmapData_AddBeatmapObjectData(self, item);
}
MAKE_HOOK_OFFSETLESS(BeatmapLineData_AddBeatmapObjectData, void, BeatmapLineData* self, BeatmapObjectData* item)
{
    item->lineIndex = lineIndex;
    BeatmapLineData_AddBeatmapObjectData(self, item);
}

MAKE_HOOK_OFFSETLESS(NotesInTimeRowProcessor_ProcessAllNotesInTimeRow, void,
    NotesInTimeRowProcessor* self, List<NoteData*>* notes)
{
    std::map<int, int> extendedLanesMap;
    for (int i = 0; i < notes->size; ++i) {
        auto* item = notes->items->values[i];
        if (item->lineIndex > 3) {
            extendedLanesMap[i] = item->lineIndex;
            item->lineIndex     = 3;
        } else if (item->lineIndex < 0) {
            extendedLanesMap[i] = item->lineIndex;
            item->lineIndex     = 0;
        }
    }

    // NotesInTimeRowProcessor_ProcessAllNotesInTimeRow(self, notes);
    // Instead, we have a reimplementation of the hooked method to deal with precision noteLineLayers:
    for (il2cpp_array_size_t i = 0; i < self->notesInColumns->Length(); i++) {
        self->notesInColumns->values[i]->Clear();
    }
    for (int j = 0; j < notes->size; j++) {
        auto* noteData = notes->items->values[j];
        auto* list = self->notesInColumns->values[noteData->lineIndex];

        bool flag = false;
        for (int k = 0; k < list->size; k++) {
            if (list->items->values[k]->noteLineLayer.value > noteData->noteLineLayer.value) {
                list->Insert(k, noteData);
                flag = true;
                break;
            }
        }
        if (!flag) {
            list->Add(noteData);
        }
    }
    for (il2cpp_array_size_t l = 0; l < self->notesInColumns->Length(); l++) {
        auto* list2 = self->notesInColumns->values[l];
        for (int m = 0; m < list2->size; m++) {
            auto* note = list2->items->values[m];
            if (note->noteLineLayer.value >= 0 && note->noteLineLayer.value <= 2) {
                note->SetNoteStartLineLayer(m);
            }
        }
    }

    for (int i = 0; i < notes->size; ++i) {
        if (extendedLanesMap.find(i) != extendedLanesMap.end()) {
            auto* item = notes->items->values[i];
            item->lineIndex = extendedLanesMap[i];
        }
    }
}

static int mirrorFound;
static std::map<Il2CppObject*, int> mirrorLanesMap;
MAKE_HOOK_OFFSETLESS(BeatmapDataMirrorTransform_CreateTransformedData, BeatmapData*, BeatmapData* beatmapData)
{
    // preprocess the flipped indices into the normal range
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (item->lineIndex > 3) {
                // logger().info("Putting item: %p in map.", item);
                mirrorLanesMap[item] = item->lineIndex;
                item->lineIndex = 3;
            } else if (item->lineIndex < 0) {
                // logger().info("Putting item: %p in map.", item);
                mirrorLanesMap[item] = item->lineIndex;
                item->lineIndex = 0;
            }
        }
    }
    mirrorFound = 0;

    // This will call the GetCopy's which will restore the proper indices to the copies
    auto result = BeatmapDataMirrorTransform_CreateTransformedData(beatmapData);

    logger().debug("BeatmapDataMirrorTransform.CreateTransformedData: found %llu out of %llu precision line indices!",
        mirrorFound, mirrorLanesMap.size());
    
    size_t restored = 0;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (mirrorLanesMap.find(item) != mirrorLanesMap.end()) {
                item->lineIndex = mirrorLanesMap[item];
                restored++;
            }
        }
    }
    logger().debug("BeatmapDataMirrorTransform.CreateTransformedData: restored %llu out of %llu precision line indices!",
        restored, mirrorLanesMap.size());
    
    mirrorLanesMap.clear();
    return result;
}
MAKE_HOOK_OFFSETLESS(NoteData_GetCopy, BeatmapObjectData*, NoteData* item)
{
    auto* result = NoteData_GetCopy(item);
    if (mirrorLanesMap.find(item) != mirrorLanesMap.end()) {
        result->lineIndex = mirrorLanesMap[item];
        mirrorFound++;
    }
    return result;
}
MAKE_HOOK_OFFSETLESS(ObstacleData_GetCopy, BeatmapObjectData*, ObstacleData* item)
{
    auto* result = ObstacleData_GetCopy(item);
    if (mirrorLanesMap.find(item) != mirrorLanesMap.end()) {
        result->lineIndex = mirrorLanesMap[item];
        mirrorFound++;
    }
    return result;
}

MAKE_HOOK_OFFSETLESS(BeatmapDataObstaclesAndBombsTransform_CreateTransformedData, BeatmapData*, BeatmapData* beatmapData,
    int enabledObstaclesType, bool noBombs)
{
    std::map<Il2CppObject*, int> extendedLanesMap;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            // int id     = item->id;
            if (item->lineIndex > 3) {
                // logger().info("Putting item: %p in map.", item);
                extendedLanesMap[item] = item->lineIndex;
                item->lineIndex      = 3;
            } else if (item->lineIndex < 0) {
                // logger().info("Putting item: %p in map.", item);
                extendedLanesMap[item] = item->lineIndex;
                item->lineIndex      = 0;
            }
        }
    }

    BeatmapData* result = BeatmapDataObstaclesAndBombsTransform_CreateTransformedData(
        beatmapData, enabledObstaclesType, noBombs);

    // Restore line indices in result AND input
    size_t found = 0;
    for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            // int id     = item->id;
            if (extendedLanesMap.find(item) != extendedLanesMap.end()) {
                // logger().info("Found item: %p in map. Time %f", item, item->time);
                item->lineIndex = extendedLanesMap[item];
                found++;
            }
        }
    }
    logger().debug("BeatmapDataObstaclesAndBombsTransform_CreateTransformedData: found %llu out of %llu precision line indices!",
        found, extendedLanesMap.size());

    size_t restored = 0;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            // int rid    = item->id;
            if (extendedLanesMap.find(item) != extendedLanesMap.end()) {
                // logger().info("Restoring Original data for item: %p in map.", item);
                item->lineIndex = extendedLanesMap[item];
                restored++;
            }
        }
    }
    logger().debug("BeatmapDataObstaclesAndBombsTransform_CreateTransformedData: restored %llu out of %llu precision line indices!",
        restored, extendedLanesMap.size());
    
    return result;
}

MAKE_HOOK_OFFSETLESS(BeatmapDataNoArrowsTransform_CreateTransformedData, BeatmapData*, BeatmapData* beatmapData)
{
    int id = 0;
    std::map<int, int> extendedLanesMap;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (item->lineIndex > 3) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 3;
            } else if (item->lineIndex < 0) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 0;
            }
            id++;
        }
    }

    BeatmapData* result = BeatmapDataNoArrowsTransform_CreateTransformedData(beatmapData);
    id = 0;
    size_t found = 0;
    for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // logger().info("Found id: %i in map. Time %f", id, item->time);
                item->lineIndex = extendedLanesMap[id];
                found++;
            }
            id++;
        }
    }
    logger().debug("BeatmapDataNoArrowsTransform_CreateTransformedData: found %llu out of %llu precision line indices!",
        found, extendedLanesMap.size());
    
    id = 0;
    size_t restored = 0;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // logger().info("Restoring Original data for id: %i in map.", id);
                item->lineIndex = extendedLanesMap[id];
                restored++;
            }
            id++;
        }
    }
    logger().debug("BeatmapDataNoArrowsTransform_CreateTransformedData: restored %llu out of %llu precision line indices!",
        restored, extendedLanesMap.size());

    return result;
}

MAKE_HOOK_OFFSETLESS(BeatmapData_CopyBeatmapObjects, void, BeatmapData* beatmapData, BeatmapData* _, BeatmapData* result)
{
    int id = 0;
    std::map<int, int> extendedLanesMap;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (item->lineIndex > 3) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 3;
            } else if (item->lineIndex < 0) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 0;
            }
            id++;
        }
    }

    BeatmapData_CopyBeatmapObjects(beatmapData, _, result);
    id = 0;
    size_t found = 0;
    for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // logger().info("Found id: %i in map. Time %f", id, item->time);
                item->lineIndex = extendedLanesMap[id];
                found++;
            }
            id++;
        }
    }
    logger().debug("BeatmapData_CopyBeatmapObjects: found %llu out of %llu precision line indices!",
        found, extendedLanesMap.size());
    
    id = 0;
    size_t restored = 0;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->size; ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->items->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // logger().info("Restoring Original data for id: %i in map.", id);
                item->lineIndex = extendedLanesMap[id];
                restored++;
            }
            id++;
        }
    }
    logger().debug("BeatmapData_CopyBeatmapObjects: restored %llu out of %llu precision line indices!",
        restored, extendedLanesMap.size());
}

/* End of https://github.com/Kylemc1413/SongCore/blob/master/HarmonyPatches/ClampPatches.cs replacement */

extern "C" void load()
{
    logger().info("Installing MappingExtensions Hooks!");
    il2cpp_functions::Init();

    // hooks to help us get the selected beatmap info
    INSTALL_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent,
        il2cpp_utils::FindMethod("", "StandardLevelDetailView", "RefreshContent"));
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate,
        il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 3));

    // PC version hooks
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectData_MirrorLineIndex,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectData", "MirrorLineIndex", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_GetNoteOffset,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "GetNoteOffset", 2));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_HighestJumpPosYForLineLayer,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "HighestJumpPosYForLineLayer", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_LineYPosForLineLayer,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "LineYPosForLineLayer", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_Get2DNoteOffset,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "Get2DNoteOffset", 2));
    INSTALL_HOOK_OFFSETLESS(FlyingScoreSpawner_SpawnFlyingScore,
        il2cpp_utils::FindMethodUnsafe("", "FlyingScoreSpawner", "SpawnFlyingScore", 7));
    INSTALL_HOOK_OFFSETLESS(NoteCutDirectionExtensions_RotationAngle,
        il2cpp_utils::FindMethodUnsafe("", "NoteCutDirectionExtensions", "RotationAngle", 1));
    INSTALL_HOOK_OFFSETLESS(NoteCutDirectionExtensions_Direction,
        il2cpp_utils::FindMethodUnsafe("", "NoteCutDirectionExtensions", "Direction", 1));
    INSTALL_HOOK_OFFSETLESS(NoteData_MirrorLineIndex,
        il2cpp_utils::FindMethodUnsafe("", "NoteData", "MirrorLineIndex", 1));
    INSTALL_HOOK_OFFSETLESS(NoteData_MirrorTransformCutDirection,
        il2cpp_utils::FindMethod("", "NoteData", "MirrorTransformCutDirection"));
    INSTALL_HOOK_OFFSETLESS(ObstacleController_Init,
        il2cpp_utils::FindMethodUnsafe("", "ObstacleController", "Init", 9));
    INSTALL_HOOK_OFFSETLESS(ObstacleData_MirrorLineIndex,
        il2cpp_utils::FindMethodUnsafe("", "ObstacleData", "MirrorLineIndex", 1));
    INSTALL_HOOK_OFFSETLESS(SpawnRotationProcessor_RotationForEventValue,
        il2cpp_utils::FindMethodUnsafe("", "SpawnRotationProcessor", "RotationForEventValue", 1));

    // Clampers
    // These work together to do the job of https://github.com/Kylemc1413/SongCore/blob/4f7dd66e022cf3f8296e26ea81e39ac1be3cc461/HarmonyPatches/ClampPatches.cs#L10-L50
    INSTALL_HOOK_OFFSETLESS(BeatmapData_AddBeatmapObjectData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapData", "AddBeatmapObjectData", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapLineData_AddBeatmapObjectData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapLineData", "AddBeatmapObjectData", 1));

    INSTALL_HOOK_OFFSETLESS(NotesInTimeRowProcessor_ProcessAllNotesInTimeRow,
        il2cpp_utils::FindMethodUnsafe("", "NotesInTimeRowProcessor", "ProcessAllNotesInTimeRow", 1));

    // The next 3 hooks work together to do the job of the CreateTransformedData hook in the PC version
    INSTALL_HOOK_OFFSETLESS(NoteData_GetCopy, il2cpp_utils::FindMethod("", "NoteData", "GetCopy"));
    INSTALL_HOOK_OFFSETLESS(ObstacleData_GetCopy, il2cpp_utils::FindMethod("", "ObstacleData", "GetCopy"));
    // The next 4 hooks are the users of BeatmapData.beatmapObjectsData's getter, which uses the lineIndex on arrays.
    INSTALL_HOOK_OFFSETLESS(BeatmapDataMirrorTransform_CreateTransformedData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapDataMirrorTransform", "CreateTransformedData", 1));

    INSTALL_HOOK_OFFSETLESS(BeatmapDataObstaclesAndBombsTransform_CreateTransformedData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapDataObstaclesAndBombsTransform", "CreateTransformedData", 3));

    INSTALL_HOOK_OFFSETLESS(BeatmapData_CopyBeatmapObjects,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapData", "CopyBeatmapObjects", 2));

    INSTALL_HOOK_OFFSETLESS(BeatmapDataNoArrowsTransform_CreateTransformedData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapDataNoArrowsTransform", "CreateTransformedData", 1));
    // end of clampers

    // ???
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Init,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController/InitData", ".ctor", 5));

    INSTALL_HOOK_OFFSETLESS(BeatmapObjectExecutionRatingsRecorder_HandleObstacleDidPassAvoidedMark,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectExecutionRatingsRecorder", "HandleObstacleDidPassAvoidedMark", 1));

    logger().info("Installed MappingExtensions Hooks!");
}
