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

using TYPEDEFS_H::Quaternion;

static void dump_real(int before, int after, void* ptr)
{
    log(INFO, "Dumping Immediate Pointer: %p: %lx", ptr, *reinterpret_cast<long*>(ptr));
    auto begin = static_cast<long*>(ptr) - before;
    auto end = static_cast<long*>(ptr) + after;
    for (auto cur = begin; cur != end; ++cur) {
        log(INFO, "0x%lx: %lx", (long)cur - (long)ptr, *cur);
    }
}

struct SaveDataNoteData : Il2CppObject {
    float time;
    int lineIndex;
    int lineLayer;
    int type;
    int cutDirection;
};

struct SaveDataObstacleData : Il2CppObject {
    float time;
    int lineIndex;
    int type;
    float duration;
    int width;
};

struct SaveDataEventData : Il2CppObject {
    float time;
    int eventType;
    int value;
};

struct BeatmapObjectData : Il2CppObject {
    int beatmapObjectType;
    float time;
    int lineIndex;
    int id;
};

struct NoteData : BeatmapObjectData {
    int noteType;
    int noteCutDirection;
    int noteLineLayer;
    int startNoteLineLayer;
    int flipLineIndex;
    float flipYSide;
    float timeToNextBasicNote;
    float timeToPrevBasicNote;

    NoteData(int i_id, float i_time, int i_lineIndex, int i_noteLineLayer, int i_startNoteLineLayer, int i_noteType,
        int i_cutDirection, float i_timeToNextBasicNote, float i_timeToPrevBasicNote)
    {
        id = i_id;
        time = i_time;
        lineIndex = i_lineIndex;
        noteLineLayer = i_noteLineLayer;
        startNoteLineLayer = i_startNoteLineLayer;
        noteType = i_noteType;
        noteCutDirection = i_cutDirection;
        timeToNextBasicNote = i_timeToNextBasicNote;
        timeToPrevBasicNote = i_timeToPrevBasicNote;
    }
};

struct ObstacleData : BeatmapObjectData {
    int obstacleType;
    float duration;
    int width;

    ObstacleData(int i_id, float i_time, int i_lineIndex, int i_obstacleType, float i_duration, int i_width)
    {
        id = i_id;
        time = i_time;
        lineIndex = i_lineIndex;
        obstacleType = i_obstacleType;
        duration = i_duration;
        width = i_width;
    }
};

struct BeatmapEventData : Il2CppObject {
    int eventType;
    float time;
    int value;

    BeatmapEventData(int i_eventType, float i_time, int i_value)
    {
        eventType = i_eventType;
        time = i_time;
        value = i_value;
    }
};

struct BeatmapLineData : Il2CppObject {
    Array<BeatmapObjectData*>* beatmapObjectData;
};

struct BeatmapData : Il2CppObject {
    Array<BeatmapLineData*>* beatmapLinesData;
    Array<BeatmapEventData*>* beatmapEventData;
    int notesCount;
    int obstaclesCount;
    int bombsCount;
    int spawnRotationEventsCount;
};

// just to make it harder to pass a wrong type of object
struct BeatmapObjectSpawnMovementData : Il2CppObject {
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
Vector3 ScaleVector(Vector3 v, float mult)
{
    Vector3 result;
    result.x = v.x * mult;
    result.y = v.y * mult;
    result.z = v.z * mult;
    return result;
}
float VectorMagnitude(Vector3 vector) { return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z); }

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
        return index - 1000;
    } else {
        return index + 1000;
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

// TODO this is an exact copy of the vanilla method. Replace all calls with Invoke?
float GetRealTimeFromBPMTime(float bmpTime, float beatsPerMinute, float shuffle, float shufflePeriod)
{
    float num = bmpTime;
    if (shufflePeriod > 0 && (int)(num * ((float)1 / shufflePeriod)) % 2 == 1) {
        num += shuffle * shufflePeriod;
    }
    if (beatsPerMinute > 0) {
        num = num / beatsPerMinute * 60.0f;
    }
    return num;
}

float LineYPosForLineLayer(BeatmapObjectSpawnMovementData* self, int noteLineLayer)
{
    float upperLinesYPos = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_upperLinesYPos"));
    float layer = ToEffectiveIndex(noteLineLayer);
    // Note: in 1.9, boths deltas are 0.6f by default - but this is safer.
    if (layer >= 1) {
        float topLinesYPos = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_topLinesYPos"));
        float delta = topLinesYPos - upperLinesYPos;
        return upperLinesYPos + delta * (layer - 1.0f);
    } else {
        float baseLinesYPos = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_baseLinesYPos"));
        float delta = upperLinesYPos - baseLinesYPos;
        return baseLinesYPos + delta * layer;
    }
}

float HighestJumpPosYForLineLayer(BeatmapObjectSpawnMovementData* self, int lineLayer)
{
    float upperLinesHighestJumpPosY = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_upperLinesHighestJumpPosY"));
    float jumpOffsetY = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_jumpOffsetY"));
    float layer = ToEffectiveIndex(lineLayer);
    // Note: in 1.9, these deltas are (by default) 0.5f vs 0.55f respectively
    if (layer >= 1) {
        float topLinesHighestJumpPosY = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_topLinesHighestJumpPosY"));
        float delta = topLinesHighestJumpPosY - upperLinesHighestJumpPosY;
        return jumpOffsetY + upperLinesHighestJumpPosY + delta * (layer - 1.0f);
    } else {
        float baseLinesHighestJumpPosY = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_baseLinesHighestJumpPosY"));
        float delta = upperLinesHighestJumpPosY - baseLinesHighestJumpPosY;
        return jumpOffsetY + baseLinesHighestJumpPosY + delta * layer;
    }
}

void SetNoteFlipToNote(NoteData* self, NoteData* targetNote)
{
    self->flipLineIndex = targetNote->lineIndex;
    self->flipYSide = (float)((self->lineIndex > targetNote->lineIndex) ? 1 : -1);
    if ((self->lineIndex > targetNote->lineIndex && self->noteLineLayer < targetNote->noteLineLayer)
        || (self->lineIndex < targetNote->lineIndex && self->noteLineLayer > targetNote->noteLineLayer)) {
        self->flipYSide *= -1.0f;
    }
}
void ProcessBasicNotesInTimeRow(std::vector<NoteData*> notes, float nextRowTime)
{
    if (notes.size() == 2) {
        NoteData* noteData = (notes[0]);
        NoteData* noteData2 = (notes[1]);
        if (noteData->noteType != noteData2->noteType
            && ((noteData->noteType == 0 && noteData->lineIndex > noteData2->lineIndex)
                || (noteData->noteType == 1 && noteData->lineIndex < noteData2->lineIndex))) {
            SetNoteFlipToNote(noteData, noteData2);
            SetNoteFlipToNote(noteData2, noteData);
        }
    }
    for (auto i = 0; i < notes.size(); i++) {
        notes[i]->timeToNextBasicNote = nextRowTime - notes[i]->time;
    }
}

static bool skipWallRatings = false;
static Il2CppObject* storedDiffBeatmap;
MAKE_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent, void, Il2CppObject* self)
{
    StandardLevelDetailView_RefreshContent(self);
    storedDiffBeatmap = *RET_V_UNLESS(il2cpp_utils::GetPropertyValue(self, "selectedDifficultyBeatmap"));
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Init, void, Il2CppObject* self, float beatsPerMinute, int noteLinesCount,
    float noteJumpMovementSpeed, float noteJumpStartBeatOffset, bool disappearingArrows, bool ghostNotes, float jumpOffsetY)
{
    float njs = *RET_V_UNLESS(il2cpp_utils::GetPropertyValue<float>(storedDiffBeatmap, "noteJumpMovementSpeed"));
    if (njs < 0)
        noteJumpMovementSpeed = njs;
    skipWallRatings = false;

    return BeatmapObjectSpawnController_Init(self, beatsPerMinute, noteLinesCount, noteJumpMovementSpeed, noteJumpStartBeatOffset,
        disappearingArrows, ghostNotes, jumpOffsetY);
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_NoteJumpGravityForLineLayer, float, BeatmapObjectSpawnMovementData* self,
    int lineLayer, int startLineLayer)
{
    float jumpDistance = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_jumpDistance"));
    float noteJumpMovementSpeed = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_noteJumpMovementSpeed"));

    float result = 2.0f * (HighestJumpPosYForLineLayer(self, lineLayer) - LineYPosForLineLayer(self, startLineLayer))
        / powf(jumpDistance / noteJumpMovementSpeed * 0.5f, 2.0f);

    return result;
}

MAKE_HOOK_OFFSETLESS(NoteCutDirectionExtensions_Rotation, Quaternion, int cutDirection)
{
    Quaternion result1 = NoteCutDirectionExtensions_Rotation(cutDirection);
    // log(INFO,"Original result %f, %f, %f, %f", result1.x, result1.y, result1.z, result1.w);
    if (cutDirection >= 1000 && cutDirection <= 1360) {
        int angle = 1000 - cutDirection;
        // log(INFO,"Quaternion Method: %s", il2cpp_class_get_methods(quaternionClass, i)->name);
        Quaternion result = ToQuaternion(0, 0, angle);
        // log(INFO,"Altered result %f, %f, %f, %f", result.x, result.y, result.z, result.w);
        return result;
    } else {
        return result1;
    }
}

MAKE_HOOK_OFFSETLESS(NoteData_MirrorTransformCutDirection, void, NoteData* self)
{
    int state = self->noteCutDirection;
    if (state >= 1000 && state <= 1360) {
        int newdir = 2360 - state;
        self->noteCutDirection = newdir;
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
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int id = item->id;
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
        int id = item->id;
        if (item->lineIndex > 3) {
            // log(INFO,"Putting id: %i in map.", id);
            mirrorLanesMap[id] = item->lineIndex;
            item->lineIndex = 3;
        } else if (item->lineIndex < 0) {
            // log(INFO,"Putting id: %i in map.", id);
            mirrorLanesMap[id] = item->lineIndex;
            item->lineIndex = 0;
        }
    }
}

MAKE_HOOK_OFFSETLESS(BeatmapDataNoArrowsTransform_CreateTransformedData, BeatmapData*, BeatmapData* beatmapData, bool randomColors)
{
    std::map<int, int> extendedLanesMap;
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int id = item->id;
            if (item->lineIndex > 3) {
                // log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex = 3;
            } else if (item->lineIndex < 0) {
                // log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex = 0;
            }
        }
    }

    BeatmapData* result = BeatmapDataNoArrowsTransform_CreateTransformedData(beatmapData, randomColors);

    // Restore line indices in result AND input
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int id = item->id;
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // log(INFO,"Found id: %i in map. Time %f", id, item->time);
                item->lineIndex = extendedLanesMap[id];
            }
        }
    }
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int rid = item->id;
            if (extendedLanesMap.find(rid) != extendedLanesMap.end()) {
                // log(INFO,"Restoring Original data for id: %i in map.", rid);
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
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int id = item->id;
            if (item->lineIndex > 3) {
                // log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex = 3;
            } else if (item->lineIndex < 0) {
                // log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = item->lineIndex;
                item->lineIndex = 0;
            }
        }
    }
    BeatmapData* result = BeatmapDataObstaclesAndBombsTransform_CreateTransformedData(beatmapData, enabledObstaclesType, noBombs);
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int id = item->id;
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // log(INFO,"Found id: %i in map. Time %f", id, item->time);
                item->lineIndex = extendedLanesMap[id];
            }
        }
    }
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int rid = item->id;
            if (extendedLanesMap.find(rid) != extendedLanesMap.end()) {
                // log(INFO,"Restoring Original data for id: %i in map.", rid);
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
    int lineIndex = self->lineIndex;
    int flipLineIndex = self->flipLineIndex;
    NoteData_MirrorLineIndex(self, lineCount);
    int origLineIndex = lineIndex;
    if (MirrorPrecisionLineIndex(lineIndex, lineCount)) {
        self->lineIndex = lineIndex;
        // log(DEBUG, "mirrored %i to %i", origLineIndex, lineIndex);
    }
    if (MirrorPrecisionLineIndex(flipLineIndex, lineCount)) {
        self->flipLineIndex = flipLineIndex;
    }
}

MAKE_HOOK_OFFSETLESS(ObstacleData_MirrorLineIndex, void, ObstacleData* self, int lineCount)
{
    int __state = self->lineIndex;
    bool precisionWidth = (self->width >= 1000);

    ObstacleData_MirrorLineIndex(self, lineCount);
    //   Console.WriteLine("Width: " + __instance.width);
    if (__state >= 1000 || __state <= -1000 || precisionWidth) {
        int normIndex = ToNormalizedPrecisionIndex(__state);
        int normWidth = ToNormalizedPrecisionIndex(self->width);

        // The vanilla formula * 1000
        int normNewIndex = lineCount * 1000 - normWidth - normIndex;

        self->lineIndex = FromNormalizedPrecisionIndex(normNewIndex);
        // log(DEBUG, "wall with lineIndex %i (norm %i) and width %i (norm %i) mirrored to %i (norm %i)", __state, normIndex, self->width, normWidth, self->lineIndex, normNewIndex);
    }
}

MAKE_HOOK_OFFSETLESS(FlyingScoreSpawner_SpawnFlyingScore, void, Il2CppObject* self, void* noteCutInfo, int noteLineIndex,
    int multiplier, Vector3 pos, Quaternion rotation, Quaternion inverseRotation, Color color)
{
    if (noteLineIndex > 3)
        noteLineIndex = 3;
    if (noteLineIndex < 0)
        noteLineIndex = 0;
    // Note: in 1.9 this method doesn't even use the line index...
    return FlyingScoreSpawner_SpawnFlyingScore(
        self, noteCutInfo, noteLineIndex, multiplier, pos, rotation, inverseRotation, color);
}

MAKE_HOOK_OFFSETLESS(
    BeatmapObjectSpawnMovementData_GetNoteOffset, Vector3, Il2CppObject* self, int noteLineIndex, int noteLineLayer)
{
    Vector3 __result = BeatmapObjectSpawnMovementData_GetNoteOffset(self, noteLineIndex, noteLineLayer);
    if (noteLineIndex >= 1000 || noteLineIndex <= -1000) { // override result for precision notes
        float ____noteLinesCount = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_noteLinesCount"));
        float ____noteLinesDistance = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<float>(self, "_noteLinesDistance"));
        Vector3 ____rightVec = *CRASH_UNLESS(il2cpp_utils::GetFieldValue<Vector3>(self, "_rightVec"));

        float lineIndex = ToEffectiveIndex(noteLineIndex);

        // From the vanilla code but they use an int line index there
        float num = -(____noteLinesCount - 1.0f) * 0.5f; // the lineIndex offset that makes the center 0
        num = (num + lineIndex) * ____noteLinesDistance; // the x position for this lineIndex

        float yPos = *CRASH_UNLESS(il2cpp_utils::RunMethod<float>(self, "LineYPosForLineLayer", noteLineLayer));
        __result = AddVectors(ScaleVector(____rightVec, num), Vector3 { 0.0f, yPos, 0.0f });
    }
    return __result;
}
MAKE_HOOK_OFFSETLESS(
    BeatmapObjectSpawnMovementData_Get2DNoteOffset, Vector2, Il2CppObject* self, int noteLineIndex, int noteLineLayer)
{
    auto vec3 = BeatmapObjectSpawnMovementData_GetNoteOffset(self, noteLineIndex, noteLineLayer);
    return { vec3.x, vec3.y };
}

static Color obstacleColor;
MAKE_HOOK_OFFSETLESS(ColorManager_Start, void, Il2CppObject* self)
{
    ColorManager_Start(self);
    auto* colorScheme = *RET_V_UNLESS(il2cpp_utils::GetFieldValue(self, "_colorScheme"));
    obstacleColor = *RET_V_UNLESS(il2cpp_utils::GetPropertyValue<Color>(colorScheme, "obstaclesColor"));
}
void SetStrechableObstacleSize(Il2CppObject* object, float paramOne, float paramTwo, float paramThree)
{
    il2cpp_utils::RunMethod(object, "SetSizeAndColor", paramOne, paramTwo, paramThree, obstacleColor);
}

MAKE_HOOK_OFFSETLESS(ObstacleController_Init, void, Il2CppObject* self, ObstacleData* obstacleData, float worldRotation,
    Vector3 startPos, Vector3 midPos, Vector3 endPos, float move1Duration, float move2Duration, float singleLineWidth,
    float obsHeight)
{
    il2cpp_utils::SetFieldValue(self, "_obstacleData", obstacleData);

    ObstacleController_Init(
        self, obstacleData, worldRotation, startPos, midPos, endPos, move1Duration, move2Duration, singleLineWidth, obsHeight);
    if ((obstacleData->obstacleType == 0 || obstacleData->obstacleType == 1) && !(obstacleData->width >= 1000))
        return;
    // Either wall height or wall width are precision

    skipWallRatings = true;
    int mode = (obstacleData->obstacleType >= 4001 && obstacleData->obstacleType <= 4100000) ? 1 : 0;
    int startHeight = 0;
    int height;
    if (mode == 1) {
        int value = obstacleData->obstacleType;
        value -= 4001;
        startHeight = value % 1000;
        height = value / 1000;
    } else {
        int value = obstacleData->obstacleType;
        height = value - 1000; // won't be used unless height is precision
    }

    float num = (float)obstacleData->width * singleLineWidth;
    if ((obstacleData->width >= 1000) || (mode == 1)) {
        if (obstacleData->width >= 1000) {
            float width = (float)obstacleData->width - 1000.0f;
            float precisionLineWidth = singleLineWidth / 1000.0f;
            num = width * precisionLineWidth;
        }
        // Change y of b for start height
        Vector3 b { b.x = (num - singleLineWidth) * 0.5f, b.y = 4 * ((float)startHeight / 1000), b.z = 0 };

        Vector3 newStartPos = AddVectors(startPos, b);
        Vector3 newMidPos = AddVectors(midPos, b);
        Vector3 newEndPos = AddVectors(endPos, b);
        RET_V_UNLESS(il2cpp_utils::SetFieldValue(self, "_startPos", &newStartPos));
        RET_V_UNLESS(il2cpp_utils::SetFieldValue(self, "_midPos", &newMidPos));
        RET_V_UNLESS(il2cpp_utils::SetFieldValue(self, "_endPos", &newEndPos));
    }

    float num2 = VectorMagnitude(SubtractVectors(*RET_V_UNLESS(il2cpp_utils::GetFieldValue<Vector3>(self, "_endPos")),
                     *RET_V_UNLESS(il2cpp_utils::GetFieldValue<Vector3>(self, "_midPos"))))
        / move2Duration;
    float length = num2 * obstacleData->duration;
    float multiplier = 1;
    if ((int)obstacleData->obstacleType >= 1000) {
        multiplier = (float)height / 1000;
    }

    auto* stretchableObstacle = *RET_V_UNLESS(il2cpp_utils::GetFieldValue(self, "_stretchableObstacle"));
    SetStrechableObstacleSize(stretchableObstacle, (num * 0.98f), (obsHeight * multiplier), (length));
    //  dump_real(0, 50, self->stretchableObstacle);

    auto* stretchableObstacleBounds = *RET_V_UNLESS(il2cpp_utils::GetPropertyValue(stretchableObstacle, "bounds"));
    il2cpp_utils::SetFieldValue(self, "_bounds", stretchableObstacleBounds);
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

MAKE_HOOK_OFFSETLESS(BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData, BeatmapData*, Il2CppObject* self,
    List<SaveDataNoteData*>* noteSaveData, List<SaveDataObstacleData*>* obstaclesSaveData,
    List<SaveDataEventData*>* eventsSaveData, float beatsPerMinute, float shuffle, float shufflePeriod)
{
    // Preprocess the lineIndex's to be 0-3 (the real method is hard-coded to 4 lines), recording the info needed to reverse it
    std::map<int, int> extendedLanesMap;
    int num = 0;
    // log(DEBUG, "noteSaveData size: %i", noteSaveData->size);
    for (int i = 0; i < noteSaveData->size; ++i) {
        auto* item = noteSaveData->items->values[i];
        if (item->lineIndex > 3) {
            // log(INFO,"Putting id: %i in map.", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex = 3;
        } else if (item->lineIndex < 0) {
            // log(INFO,"Putting id: %i in map.", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex = 0;
        }
        num++;
    }

    // log(DEBUG, "obstaclesSaveData size: %i", obstaclesSaveData->size);
    for (int i = 0; i < obstaclesSaveData->size; ++i) {
        auto* item = obstaclesSaveData->items->values[i];
        if (item->lineIndex > 3) {
            // log(INFO,"Putting id: %i in map", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex = 3;
        } else if (obstaclesSaveData->items->values[i]->lineIndex < 0) {
            // log(INFO,"Putting id: %i in map", num);
            extendedLanesMap[num] = item->lineIndex;
            item->lineIndex = 0;
        }
        num++;
    }
    // log(DEBUG, "num: %i", num);

    BeatmapData* result = BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData(
        self, noteSaveData, obstaclesSaveData, eventsSaveData, beatsPerMinute, shuffle, shufflePeriod);

    // Reverse the lineIndex changes
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i) {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
            auto* item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            int id = item->id;
            if (extendedLanesMap.find(id) != extendedLanesMap.end()) {
                // log(INFO,"Found id: %i in map.", id);
                item->lineIndex = extendedLanesMap[id];
            }
        }
    }

    if (extendedLanesMap.size() > 0) {
        log(INFO, "Attempting to correct extended lanes flipping when they shouldn't.");
        std::vector<NoteData*> allnotes;
        // Get all the notes
        for (int i = 0; i < result->beatmapLinesData->Length(); ++i) {
            for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j) {
                auto* item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
                if (item->beatmapObjectType == 0) // basic note
                    allnotes.push_back(reinterpret_cast<NoteData*>(item));
            }
        }
        std::vector<NoteData*> list2;
        list2.reserve(4);
        for (int i = 0; i < allnotes.size(); ++i) {
            allnotes[i]->flipLineIndex = allnotes[i]->lineIndex;
            allnotes[i]->flipYSide = 0;
            float realTimeFromBPMTime = GetRealTimeFromBPMTime(allnotes[i]->time, beatsPerMinute, shuffle, shufflePeriod);

            int type = allnotes[i]->noteType;
            if (type == 0 || type == 1) {
                if (list2.size() > 0 && list2[0]->time < realTimeFromBPMTime - 0.001f) {
                    ProcessBasicNotesInTimeRow(list2, realTimeFromBPMTime);
                    list2.clear();
                }
                list2.push_back(allnotes[i]);
            }
        }
        ProcessBasicNotesInTimeRow(list2, std::numeric_limits<float>::max());
    }

    return result;
}

extern "C" void load()
{
    log(INFO, "Installing Mapping Extensions Hooks!");

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

    INSTALL_HOOK_OFFSETLESS(BeatmapObjectExecutionRatingsRecorder_HandleObstacleDidPassAvoidedMark,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectExecutionRatingsRecorder", "HandleObstacleDidPassAvoidedMark", 1));

    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnMovementData_NoteJumpGravityForLineLayer,
        il2cpp_utils::FindMethodUnsafe("", "BeatmapObjectSpawnMovementData", "NoteJumpGravityForLineLayer", 2));

    log(INFO, "Installed  Mapping Extensions Hooks!");
}
