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

#include "codegen/include/GlobalNamespace/BeatmapData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapDataLoader.hpp"
#include "codegen/include/GlobalNamespace/BeatmapDataLoader_NotesInTimeRowProcessor.hpp"
#include "codegen/include/GlobalNamespace/BeatmapEventData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapLineData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapObjectData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapObjectSpawnMovementData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapSaveData_EventData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapSaveData_NoteData.hpp"
#include "codegen/include/GlobalNamespace/BeatmapSaveData_ObstacleData.hpp"
#include "codegen/include/GlobalNamespace/ColorManager.hpp"
#include "codegen/include/GlobalNamespace/ColorScheme.hpp"
#include "codegen/include/GlobalNamespace/IDifficultyBeatmap.hpp"
#include "codegen/include/GlobalNamespace/NoteData.hpp"
#include "codegen/include/GlobalNamespace/ObstacleController.hpp"
#include "codegen/include/GlobalNamespace/ObstacleData.hpp"
#include "codegen/include/GlobalNamespace/StandardLevelDetailView.hpp"
#include "codegen/include/GlobalNamespace/StretchableObstacle.hpp"
#include "codegen/include/System/Collections/Generic/List_1.hpp"
#include "codegen/include/UnityEngine/Camera.hpp"
#include "codegen/include/UnityEngine/Color.hpp"
#include "codegen/include/UnityEngine/Graphics.hpp"
#include "codegen/include/UnityEngine/Resources.hpp"
#include "codegen/include/UnityEngine/Quaternion.hpp"
#include "codegen/include/UnityEngine/Vector3.hpp"
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
    info.version = "0.17.1";
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
// // This works but it allows float usage instead of ints
// int EffectiveIndexToIndex(float effectiveIndex)
// {
//     int newIndex = 1000.0f * effectiveIndex;
//     return FromNormalizedPrecisionIndex(newIndex);
// }

float LineYPosForLineLayer(BeatmapObjectSpawnMovementData* self, int noteLineLayer)
{
    float layer = ToEffectiveIndex(noteLineLayer);
    // Note: in 1.9, boths deltas are 0.6f by default - but this is safer.
    if (layer >= 1) {
        float delta = self->topLinesYPos - self->upperLinesYPos;
        return self->upperLinesYPos + delta * (layer - 1.0f);
    } else {
        float delta = self->upperLinesYPos - self->baseLinesYPos;
        return self->baseLinesYPos + delta * layer;
    }
}

float HighestJumpPosYForLineLayer(BeatmapObjectSpawnMovementData* self, int lineLayer)
{
    float layer = ToEffectiveIndex(lineLayer);
    // Note: in 1.9, these deltas are (by default) 0.5f vs 0.55f respectively
    if (layer >= 1) {
        float delta = self->topLinesHighestJumpPosY - self->upperLinesHighestJumpPosY;
        return self->jumpOffsetY + self->upperLinesHighestJumpPosY + delta * (layer - 1.0f);
    } else {
        float delta = self->upperLinesHighestJumpPosY - self->baseLinesHighestJumpPosY;
        return self->jumpOffsetY + self->baseLinesHighestJumpPosY + delta * layer;
    }
}

static bool skipWallRatings = false;
static IDifficultyBeatmap* storedDiffBeatmap = nullptr;
MAKE_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent, void, StandardLevelDetailView* self)
{
    StandardLevelDetailView_RefreshContent(self);
    storedDiffBeatmap = self->selectedDifficultyBeatmap;
    auto* arr = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Camera*>();
    logger().info("found %i objects!", arr->Length());
    logger().info("StandardLevelDetailView equals nullptr: %i", self->Equals(nullptr));
    // logger().info("StandardLevelDetailView equals nullptr: %i",
    //     CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(self, "Equals", (Il2CppObject*)nullptr)));
    UnityEngine::Graphics::Blit((UnityEngine::Texture*)nullptr, (UnityEngine::RenderTexture*)nullptr);
}
MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Init, void, Il2CppObject* self, float beatsPerMinute, int noteLinesCount,
    float noteJumpMovementSpeed, float noteJumpStartBeatOffset, bool disappearingArrows, bool ghostNotes, float jumpOffsetY)
{
    if (storedDiffBeatmap) {
        float njs = storedDiffBeatmap->get_noteJumpMovementSpeed();
        if (njs < 0)
            noteJumpMovementSpeed = njs;
    }
    skipWallRatings = false;

    return BeatmapObjectSpawnController_Init(self, beatsPerMinute, noteLinesCount, noteJumpMovementSpeed, noteJumpStartBeatOffset,
        disappearingArrows, ghostNotes, jumpOffsetY);
}
MAKE_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, void, Il2CppObject* self,
                     bool firstActivation, int activationType) {
    storedDiffBeatmap = nullptr;
    return MainMenuViewController_DidActivate(self, firstActivation, activationType);
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_NoteJumpGravityForLineLayer, float, BeatmapObjectSpawnMovementData* self,
    int lineLayer, int startLineLayer)
{
    float result = 2.0f * (HighestJumpPosYForLineLayer(self, lineLayer) - LineYPosForLineLayer(self, startLineLayer))
        / powf(self->jumpDistance / self->noteJumpMovementSpeed * 0.5f, 2.0f);

    return result;
}

MAKE_HOOK_OFFSETLESS(NoteCutDirectionExtensions_Rotation, UnityEngine::Quaternion, int cutDirection)
{
    UnityEngine::Quaternion result1 = NoteCutDirectionExtensions_Rotation(cutDirection);
    // logger().info("Original result %f, %f, %f, %f", result1.x, result1.y, result1.z, result1.w);
    if (cutDirection >= 1000 && cutDirection <= 1360) {
        int angle   = 1000 - cutDirection;
        auto result = UnityEngine::Quaternion::Euler(0, 0, angle);
        // logger().info("Altered result %f, %f, %f, %f", result.x, result.y, result.z, result.w);
        return result;
    } else {
        return result1;
    }
}

MAKE_HOOK_OFFSETLESS(NoteData_MirrorTransformCutDirection, void, NoteData* self)
{
    int state = self->cutDirection.value;
    if (state >= 1000 && state <= 1360) {
        int newdir         = 2360 - state;
        self->cutDirection = newdir;
        return;
    } else {
        return NoteData_MirrorTransformCutDirection(self);
    }
}

static std::map<int, int> mirrorLanesMap;
MAKE_HOOK_OFFSETLESS(BeatDataMirrorTransform_CreateTransformedData, BeatmapData*, BeatmapData* beatmapData)
{
    // This will call BeatDataMirrorTransform_MirrorTransformBeatmapObjects which will populate mirrorLanesMap for us
    auto result = BeatDataMirrorTransform_CreateTransformedData(beatmapData);

    // restore the line indices
    for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int id     = item->id;
            if (mirrorLanesMap.find(id) != mirrorLanesMap.end()) {
                item->lineIndex = mirrorLanesMap[id];
            }
        }
    }
    mirrorLanesMap.clear();
    return result;
}
MAKE_HOOK_OFFSETLESS(
    BeatDataMirrorTransform_MirrorTransformBeatmapObjects, void, List<BeatmapObjectData*>* beatmapObjects, int beatmapLineCount)
{
    BeatDataMirrorTransform_MirrorTransformBeatmapObjects(beatmapObjects, beatmapLineCount);
    // preprocess the flipped indices into the normal range
    for (int i = 0; i < beatmapObjects->size; ++i) {
        auto* item = beatmapObjects->items->values[i];
        int id     = item->id;
        if (item->lineIndex > 3) {
            // logger().info("Putting id: %i in map.", id);
            mirrorLanesMap[id] = item->lineIndex;
            item->lineIndex    = 3;
        } else if (item->lineIndex < 0) {
            // logger().info("Putting id: %i in map.", id);
            mirrorLanesMap[id] = item->lineIndex;
            item->lineIndex    = 0;
        }
    }
}

MAKE_HOOK_OFFSETLESS(BeatmapDataNoArrowsTransform_CreateTransformedData, BeatmapData*, BeatmapData* beatmapData, bool randomColors)
{
    std::map<int, int> extendedLanesMap;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int id     = item->id;
            if (item->lineIndex > 3) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 3;
            } else if (item->lineIndex < 0) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 0;
            }
        }
    }

    BeatmapData* result = BeatmapDataNoArrowsTransform_CreateTransformedData(beatmapData, randomColors);

    // Restore line indices in result AND input
    for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int id     = item->id;
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // logger().info("Found id: %i in map. Time %f", id, item->time);
                item->lineIndex = extendedLanesMap[id];
            }
        }
    }
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int rid    = item->id;
            if (extendedLanesMap.find(rid) != extendedLanesMap.end()) {
                // logger().info("Restoring Original data for id: %i in map.", rid);
                item->lineIndex = extendedLanesMap[rid];
            }
        }
    }
    return result;
}
MAKE_HOOK_OFFSETLESS(BeatmapDataObstaclesAndBombsTransform_CreateTransformedData, BeatmapData*, BeatmapData* beatmapData,
    int enabledObstaclesType, bool noBombs)
{
    std::map<int, int> extendedLanesMap;
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int id     = item->id;
            if (item->lineIndex > 3) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 3;
            } else if (item->lineIndex < 0) {
                // logger().info("Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex      = 0;
            }
        }
    }
    BeatmapData* result = BeatmapDataObstaclesAndBombsTransform_CreateTransformedData(beatmapData, enabledObstaclesType, noBombs);
    for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int id     = item->id;
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // logger().info("Found id: %i in map. Time %f", id, item->time);
                item->lineIndex = extendedLanesMap[id];
            }
        }
    }
    for (il2cpp_array_size_t i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int rid    = item->id;
            if (extendedLanesMap.find(rid) != extendedLanesMap.end()) {
                // logger().info("Restoring Original data for id: %i in map.", rid);
                item->lineIndex = extendedLanesMap[rid];
            }
        }
    }
    return result;
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
MAKE_HOOK_OFFSETLESS(NoteData_MirrorLineIndex, void, NoteData* self, int lineCount)
{
    int lineIndex     = self->lineIndex;
    int flipLineIndex = self->flipLineIndex;
    NoteData_MirrorLineIndex(self, lineCount);
    // int origLineIndex = lineIndex;
    if (MirrorPrecisionLineIndex(lineIndex, lineCount)) {
        self->lineIndex = lineIndex;
        // logger().debug("mirrored %i to %i", origLineIndex, lineIndex);
    }
    if (MirrorPrecisionLineIndex(flipLineIndex, lineCount)) {
        self->flipLineIndex = flipLineIndex;
    }
}

MAKE_HOOK_OFFSETLESS(ObstacleData_MirrorLineIndex, void, ObstacleData* self, int lineCount)
{
    int __state         = self->lineIndex;
    bool precisionWidth = (self->width >= 1000);

    ObstacleData_MirrorLineIndex(self, lineCount);
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

MAKE_HOOK_OFFSETLESS(FlyingScoreSpawner_SpawnFlyingScore, void, Il2CppObject* self, void* noteCutInfo, int noteLineIndex,
    int multiplier, UnityEngine::Vector3 pos, UnityEngine::Quaternion rotation, UnityEngine::Quaternion inverseRotation,
    UnityEngine::Color color)
{
    if (noteLineIndex > 3)
        noteLineIndex = 3;
    if (noteLineIndex < 0)
        noteLineIndex = 0;
    // Note: in 1.9 this method doesn't even use the line index...
    return FlyingScoreSpawner_SpawnFlyingScore(
        self, noteCutInfo, noteLineIndex, multiplier, pos, rotation, inverseRotation, color);
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_GetNoteOffset, UnityEngine::Vector3, BeatmapObjectSpawnMovementData* self,
    int noteLineIndex, GlobalNamespace::NoteLineLayer noteLineLayer)
{
    UnityEngine::Vector3 __result = BeatmapObjectSpawnMovementData_GetNoteOffset(self, noteLineIndex, noteLineLayer);
    if (noteLineIndex >= 1000 || noteLineIndex <= -1000) { // override result for precision notes
        float lineIndex = ToEffectiveIndex(noteLineIndex);

        // From the vanilla code but they use an int line index there
        float num = -(self->noteLinesCount - 1.0f) * 0.5f;       // the lineIndex offset that makes the center 0
        num       = (num + lineIndex) * self->noteLinesDistance; // the x position for this lineIndex

        float yPos = self->LineYPosForLineLayer(noteLineLayer);
        __result   = (self->rightVec * num) + UnityEngine::Vector3(0.0f, yPos, 0.0f);
    }
    return __result;
}
MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_Get2DNoteOffset, Vector2, BeatmapObjectSpawnMovementData* self,
    int noteLineIndex, GlobalNamespace::NoteLineLayer noteLineLayer)
{
    auto vec3 = BeatmapObjectSpawnMovementData_GetNoteOffset(self, noteLineIndex, noteLineLayer);
    return { vec3.x, vec3.y };
}

static UnityEngine::Color obstacleColor;
MAKE_HOOK_OFFSETLESS(ColorManager_Start, void, ColorManager* self)
{
    ColorManager_Start(self);
    obstacleColor = self->colorScheme->obstaclesColor;
}
void SetStrechableObstacleSize(GlobalNamespace::StretchableObstacle* object, float paramOne, float paramTwo, float paramThree)
{
    il2cpp_utils::GetFieldValue<Il2CppArray*>(object, "blah");
    object->SetSizeAndColor(paramOne, paramTwo, paramThree, obstacleColor);
}

MAKE_HOOK_OFFSETLESS(ObstacleController_Init, void, ObstacleController* self, ObstacleData* obstacleData, float worldRotation,
    UnityEngine::Vector3 startPos, UnityEngine::Vector3 midPos, UnityEngine::Vector3 endPos, float move1Duration,
    float move2Duration, float singleLineWidth, float obsHeight)
{
    self->obstacleData = obstacleData;

    ObstacleController_Init(
        self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, obsHeight);
    if ((obstacleData->obstacleType.value <= 1) && !(obstacleData->width >= 1000))
        return;
    // Either wall height or wall width are precision

    skipWallRatings = true;
    int mode        = (obstacleData->obstacleType.value >= 4001 && obstacleData->obstacleType.value <= 4100000) ? 1 : 0;
    int startHeight = 0;
    int height;
    if (mode == 1) {
        int value = obstacleData->obstacleType.value;
        value -= 4001;
        startHeight = value % 1000;
        height      = value / 1000;
    } else {
        int value = obstacleData->obstacleType.value;
        height    = value - 1000; // won't be used unless height is precision
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
    if ((int)obstacleData->obstacleType.value >= 1000) {
        multiplier = (float)height / 1000;
    }

    SetStrechableObstacleSize(self->stretchableObstacle, (num * 0.98f), (obsHeight * multiplier), (length));
    //  dump_real(0, 50, self->stretchableObstacle);

    self->bounds = self->stretchableObstacle->bounds;
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

MAKE_HOOK_OFFSETLESS(BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData, BeatmapData*, BeatmapDataLoader* self,
    List<BeatmapSaveData::NoteData*>* noteSaveData, List<BeatmapSaveData::ObstacleData*>* obstaclesSaveData,
    List<BeatmapSaveData::EventData*>* eventsSaveData, float beatsPerMinute, float shuffle, float shufflePeriod)
{
    // Preprocess the lineIndex's to be 0-3 (the real method is hard-coded to 4 lines), recording the info needed to reverse it
    std::map<int, int> extendedLanesMap;
    int num = 0;
    // logger().debug("noteSaveData size: %i", noteSaveData->size);
    for (int i = 0; i < noteSaveData->size; ++i) {
        auto* item = noteSaveData->items->values[i];
        // logger().info("id: %i, lineIndex: %i", num, item->_lineIndex);
        if (item->lineIndex > 3) {
            // logger().info("Putting id: %i in map.", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex       = 3;
        } else if (item->lineIndex < 0) {
            // logger().info("Putting id: %i in map.", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex       = 0;
        }
        num++;
    }

    // logger().debug("obstaclesSaveData size: %i", obstaclesSaveData->size);
    for (int i = 0; i < obstaclesSaveData->size; ++i) {
        auto* item = obstaclesSaveData->items->values[i];
        if (item->lineIndex > 3) {
            // logger().info("Putting id: %i in map", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex       = 3;
        } else if (obstaclesSaveData->items->values[i]->lineIndex < 0) {
            // logger().info("Putting id: %i in map", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex       = 0;
        }
        num++;
    }
    // logger().debug("num: %i", num);

    BeatmapData* result = BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData(
        self, noteSaveData, obstaclesSaveData, eventsSaveData, beatsPerMinute, shuffle, shufflePeriod);

    // Reverse the lineIndex changes
    for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (il2cpp_array_size_t j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
            int id     = item->id;
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // logger().info("Found id: %i in map.", id);
                item->lineIndex = extendedLanesMap[id];
            }
        }
    }

    if (extendedLanesMap.size() > 0) {
        logger().info("Attempting to correct extended lanes flipping when they shouldn't.");
        std::vector<NoteData*> allnotes;
        // Get all the notes
        for (il2cpp_array_size_t i = 0; i < result->beatmapLinesData->Length(); ++i) {
            for (il2cpp_array_size_t j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectsData->Length(); ++j) {
                auto* item = result->beatmapLinesData->values[i]->beatmapObjectsData->values[j];
                if (item->beatmapObjectType.value == BeatmapObjectType::Note) // basic note
                    allnotes.push_back(reinterpret_cast<NoteData*>(item));
            }
        }
        auto* list2 = CRASH_UNLESS(System::Collections::Generic::List_1<NoteData*>::New_ctor(4));
        for (il2cpp_array_size_t i = 0; i < allnotes.size(); ++i) {
            allnotes[i]->flipLineIndex = allnotes[i]->lineIndex;
            allnotes[i]->flipYSide     = 0;
            float realTimeFromBPMTime  = self->GetRealTimeFromBPMTime(allnotes[i]->time, beatsPerMinute, shuffle, shufflePeriod);

            int type = allnotes[i]->noteType.value;
            if (type == NoteType::NoteA || type == NoteType::NoteB) {
                if (list2->size > 0 && list2->items->values[0]->time < realTimeFromBPMTime - 0.001f) {
                    self->notesInTimeRowProcessor->ProcessBasicNotesInTimeRow(list2, realTimeFromBPMTime);
                    list2->Clear();
                }
                list2->Add(allnotes[i]);
            }
        }
        self->notesInTimeRowProcessor->ProcessBasicNotesInTimeRow(list2, std::numeric_limits<float>::max());
    }

    return result;
}

extern "C" void load()
{
    logger().info("Installing MappingExtensions Hooks!");
    il2cpp_functions::Init();

    INSTALL_HOOK_OFFSETLESS(
        NoteCutDirectionExtensions_Rotation, il2cpp_utils::FindMethodUnsafe("", "NoteCutDirectionExtensions", "Rotation", 1));

    INSTALL_HOOK_OFFSETLESS(NoteData_MirrorLineIndex, il2cpp_utils::FindMethodUnsafe("", "NoteData", "MirrorLineIndex", 1));

    INSTALL_HOOK_OFFSETLESS(
        NoteData_MirrorTransformCutDirection, il2cpp_utils::FindMethod("", "NoteData", "MirrorTransformCutDirection"));

    INSTALL_HOOK_OFFSETLESS(
        ObstacleData_MirrorLineIndex, il2cpp_utils::FindMethodUnsafe("", "ObstacleData", "MirrorLineIndex", 1));

    INSTALL_HOOK_OFFSETLESS(
        FlyingScoreSpawner_SpawnFlyingScore, il2cpp_utils::FindMethodUnsafe("", "FlyingScoreSpawner", "SpawnFlyingScore", 7));

    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_GetNoteOffset,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "GetNoteOffset", 2));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_Get2DNoteOffset,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "Get2DNoteOffset", 2));

    INSTALL_HOOK_OFFSETLESS(BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapDataLoader", "GetBeatmapDataFromBeatmapSaveData", 6));

    // These hooks work together to do the job of the CreateTransformedData hook in the PC version
    INSTALL_HOOK_OFFSETLESS(BeatDataMirrorTransform_CreateTransformedData,
        il2cpp_utils::FindMethodUnsafe("", "BeatDataMirrorTransform", "CreateTransformedData", 1));
    INSTALL_HOOK_OFFSETLESS(BeatDataMirrorTransform_MirrorTransformBeatmapObjects,
        il2cpp_utils::FindMethodUnsafe("", "BeatDataMirrorTransform", "MirrorTransformBeatmapObjects", 2));

    INSTALL_HOOK_OFFSETLESS(BeatmapDataNoArrowsTransform_CreateTransformedData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapDataNoArrowsTransform", "CreateTransformedData", 2));

    INSTALL_HOOK_OFFSETLESS(BeatmapDataObstaclesAndBombsTransform_CreateTransformedData,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapDataObstaclesAndBombsTransform", "CreateTransformedData", 3));

    INSTALL_HOOK_OFFSETLESS(ObstacleController_Init, il2cpp_utils::FindMethodUnsafe("", "ObstacleController", "Init", 9));

    INSTALL_HOOK_OFFSETLESS(ColorManager_Start, il2cpp_utils::FindMethod("", "ColorManager", "Start"));

    INSTALL_HOOK_OFFSETLESS(
        StandardLevelDetailView_RefreshContent, il2cpp_utils::FindMethod("", "StandardLevelDetailView", "RefreshContent"));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Init,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnController/InitData", ".ctor", 7));
    INSTALL_HOOK_OFFSETLESS(MainMenuViewController_DidActivate,
        il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 2));

    INSTALL_HOOK_OFFSETLESS(BeatmapObjectExecutionRatingsRecorder_HandleObstacleDidPassAvoidedMark,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectExecutionRatingsRecorder", "HandleObstacleDidPassAvoidedMark", 1));

    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_NoteJumpGravityForLineLayer,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "NoteJumpGravityForLineLayer", 2));

    logger().info("Installed MappingExtensions Hooks!");
}
