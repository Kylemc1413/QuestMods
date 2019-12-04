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

#define MOD_ID "MappingExtenions"
#define VERSION "0.14.2"

using TYPEDEFS_H::Quaternion;

static void dump_real(int before, int after, void *ptr)
{
    log(INFO, "Dumping Immediate Pointer: %p: %lx", ptr, *reinterpret_cast<long *>(ptr));
    auto begin = static_cast<long *>(ptr) - before;
    auto end = static_cast<long *>(ptr) + after;
    for (auto cur = begin; cur != end; ++cur)
    {
        log(INFO, "0x%lx: %lx", (long)cur - (long)ptr, *cur);
    }
}

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

struct SaveDataObstacleData : Il2CppObject
{
    float time;
    int lineIndex;
    int type;
    float duration;
    int width;
};

struct SaveDataEventData : Il2CppObject
{
    float time;
    int eventType;
    int value;
};

struct BeatmapObjectData : Il2CppObject
{
    int beatmapObjectType;
    float time;
    int lineIndex;
    int id;
};

struct NoteData : BeatmapObjectData
{
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

struct ObstacleData : BeatmapObjectData
{
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

struct BeatmapEventData : Il2CppObject
{
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

struct BeatmapLineData : Il2CppObject
{
    Array<BeatmapObjectData *> *beatmapObjectData;
};

struct Bounds : Il2CppObject
{
    Vector3 center;
    Vector3 extents;
};

struct StretchableObstacle : UnityObject
{
    float edgeSize;
    float coreOffset;
    float addColorMultiplier;
    int32_t pad1;
    void *obstacleCore;
    void *stretchableCore;
    void *obstacleFrame;
    void *obstacleFakeGlow;
    void *addColorSetters;
    void *tintcolorSetters;
    void *bounds;
};

struct ObstacleController : UnityObject
{
    void *activeObstaclesManager;
    StretchableObstacle *stretchableObstacle;
    void *color;
    float endDistanceOffset;
    int32_t pad1;
    void *visualWrappers;
    void *playerController;
    void *audioTimeSyncController;
    void *didInitEvent;
    void *finishedMovementEvent;
    void *passedThreeQuartersOfMove2Event;
    void *passedAvoidedMarkEvent;
    void *didStartDissolvingEvent;
    void *didDissolveEvent;
    Vector3 startPos;
    Vector3 midPos;
    Vector3 endPos;
    float move1Duration;
    float move2Duration;
    float startTimeOffset;
    float obstacleDuration;
    bool passedThreeQuartersOfMove2Reported;
    bool passedAvoidedMarkReported;
    int16_t pad2;
    float passedAvoidedMarkTime;
    float finishMovementTime;
    bool initialized;
    int16_t pad3;
    bool pad4;
    void *bounds;
    bool dissolving;
    int16_t pad5;
    bool pad6;
    ObstacleData *obstacleData;
};

struct BeatmapData : Il2CppObject
{
    Array<BeatmapLineData *> *beatmapLinesData;
    Array<BeatmapEventData *> *beatmapEventData;
    int notesCount;
    int obstaclesCount;
    int bombsCount;
    int spawnRotationEventsCount;
};

struct BeatmapObjectSpawnController : UnityObject
{
    float noteLinesDistance;
    float globalYJumpOffset;
    float maxHalfJumpDistance;
    float halfJumpDurationInBeats;
    float baseLinesHighestJumpPosY;
    float upperLinesHighestJumpPosY;
    float topLinesHighestJumpPosY;
    float topLinesZPosOffset;
    float moveSpeed;
    float moveDurationInBeats;
    float baseLinesYPos;
    float upperLinesYPos;
    float topLinesYPos;
    float verticleObstaclePosY;
    float topObstaclePosY;
    void *beatmapObjectCallbackController;
    void *noteAPool;
    void *noteBPool;
    void *bombNotePool;
    void *fullHeightObstaclePool;
    void *topObstaclePool;
    void *noteLineConnectionPool;
    void *beatmapObjectWasSpawnedEvent;
    void *noteWasMissedEvent;
    void *noteWasCutEvent;
    void *noteDidStartJumpEvent;
    void *obstacleDidStartMovementEvent;
    void *obstacleDidFinishMovementEvent;
    void *obstacleDidPassThreeQuartersOfMove2Event;
    void *obstacleDidPassAvoidedMarkEvent;
    float spawnAheadTime;
    float jumpDistance;
    float moveDistance;
    bool disableSpawning;
    int beatmapObjectCallbackId;
    int lateEventCallbackId;
    int earlyEventCallbackId;
    float beatsPerMinute;
    float noteLinesCount;
    float noteJumpMovementSpeed;
    bool disappearingArrows;
    bool ghostNotes;
    void *prevSpawnedNormalNoteController;
    int numberOfSpawnedBasicNotes;
    float firstBasicNoteTime;
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

float GetRealTimeFromBPMTime(float bmpTime, float beatsPerMinute, float shuffle, float shufflePeriod)
{
    float num = bmpTime;
    if (shufflePeriod > 0 && (int)(num * ((float)1 / shufflePeriod)) % 2 == 1)
    {
        num += shuffle * shufflePeriod;
    }
    if (beatsPerMinute > 0)
    {
        num = num / beatsPerMinute * (float)60;
    }
    return num;
}

float LineYPosForLineLayer(int noteLineLayer, float topLinesYPos, float upperLinesYPos, float baseLinesYPos)
{
    float result;
    float delta = topLinesYPos - upperLinesYPos;
    switch (noteLineLayer)
    {
    case 0:
        result = baseLinesYPos;
        break;
    case 1:
        result = upperLinesYPos;
        break;
    case 2:
        result = topLinesYPos;
        break;
    default:
        if (noteLineLayer >= 1000 || noteLineLayer <= -1000)
            result = upperLinesYPos - delta - delta + ((float)(noteLineLayer * (delta / 1000)));
        else if (noteLineLayer > 2 || noteLineLayer < 0)
            result = upperLinesYPos - delta + (noteLineLayer * delta);
    }
    return result;
}

float HighestJumpPosYForLineLayer(int lineLayer, float topLinesHighestJumpPosY, float upperLinesHighestJumpPosY, float baseLinesHighestJumpPosY, float globalYJumpOffset)
{
    float delta = topLinesHighestJumpPosY - upperLinesHighestJumpPosY;
    //  log(INFO,"Global offset: %f", globalYJumpOffset);
    float result;
    switch (lineLayer)
    {
    case 0:
        result = baseLinesHighestJumpPosY + globalYJumpOffset;
        break;
    case 1:
        result = upperLinesHighestJumpPosY + globalYJumpOffset;
        break;
    case 2:
        result = topLinesHighestJumpPosY + globalYJumpOffset;
        break;
    default:
        if (lineLayer >= 1000 || lineLayer <= -1000)
            result = upperLinesHighestJumpPosY - delta - delta + globalYJumpOffset + (float)(lineLayer * (delta / 1000));
        else if (lineLayer > 2 || lineLayer < 0)
            result = upperLinesHighestJumpPosY - delta + globalYJumpOffset + (float)(lineLayer * delta);
    }
    return result;
}

void ProcessBasicNotesInTimeRow(std::vector<NoteData *> notes, float nextRowTime)
{
    if (notes.size() == 2)
    {
        NoteData *noteData = (notes[0]);
        NoteData *noteData2 = (notes[1]);
        if (noteData->noteType != noteData2->noteType && ((noteData->noteType == 0 && noteData->lineIndex > noteData2->lineIndex) || (noteData->noteType == 1 && noteData->lineIndex < noteData2->lineIndex)))
        {
            {
                noteData->flipLineIndex = noteData2->lineIndex;
                noteData->flipYSide = (float)((noteData->lineIndex > noteData2->lineIndex) ? 1 : -1);
                if ((noteData->lineIndex > noteData2->lineIndex && noteData->noteLineLayer < noteData2->noteLineLayer) || (noteData->lineIndex < noteData2->lineIndex && noteData->noteLineLayer > noteData2->noteLineLayer))
                {
                    noteData->flipYSide *= -1;
                }
            }
            {
                noteData2->flipLineIndex = noteData->lineIndex;
                noteData2->flipYSide = (float)((noteData2->lineIndex > noteData->lineIndex) ? 1 : -1);
                if ((noteData2->lineIndex > noteData->lineIndex && noteData2->noteLineLayer < noteData->noteLineLayer) || (noteData2->lineIndex < noteData->lineIndex && noteData2->noteLineLayer > noteData->noteLineLayer))
                {
                    noteData2->flipYSide *= -1;
                }
            }
        }
    }
    for (auto i = 0; i < notes.size(); i++)
    {
        notes[i]->timeToNextBasicNote = nextRowTime - notes[i]->time;
    }
}

bool CompareBeatmapObjects(BeatmapObjectData *x, BeatmapObjectData *y)
{
    if (x->time == y->time)
    {
        return 0;
    }
    return (x->time <= y->time) ? -1 : 1;
}
static bool skipWallRatings = false;
static void *storedDiffBeatmap;
static Il2CppClass *difficultyBeatmapClass;
static Il2CppClass *standardLevelDetailViewClass;
static const MethodInfo *getDiffBeatmapInfo;
static const MethodInfo *getNjsMethodInfo;
MAKE_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent, void, void *self)
{
    StandardLevelDetailView_RefreshContent(self);
    if (standardLevelDetailViewClass == nullptr)
        standardLevelDetailViewClass = il2cpp_utils::GetClassFromName("", "StandardLevelDetailView");
    if (standardLevelDetailViewClass != nullptr && getDiffBeatmapInfo == nullptr)
    {
        getDiffBeatmapInfo = il2cpp_functions::class_get_method_from_name(standardLevelDetailViewClass, "get_selectedDifficultyBeatmap", 0);
    }
    if (getDiffBeatmapInfo != nullptr)
    {
        Il2CppException *exception = nullptr;
        storedDiffBeatmap = (il2cpp_functions::runtime_invoke(getDiffBeatmapInfo, self, nullptr, &exception));
    }
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Init, void, void *self, float beatsPerMinute, int noteLinesCount,
          float noteJumpMovementSpeed, float noteJumpStartBeatOffset, bool disappearingArrows, bool ghostNotes)
{

    skipWallRatings = false;
    float njs = 0;
    if (difficultyBeatmapClass == nullptr)
        difficultyBeatmapClass = il2cpp_utils::GetClassFromName("", "BeatmapLevelSO/DifficultyBeatmap");
    if (difficultyBeatmapClass != nullptr && getNjsMethodInfo == nullptr)
    {
        getNjsMethodInfo = il2cpp_functions::class_get_method_from_name(difficultyBeatmapClass, "get_noteJumpMovementSpeed", 0);
    }
    else
    {
        log(INFO, "Difficulty Beatmap Class null");
    }
    if (getNjsMethodInfo != nullptr && storedDiffBeatmap != nullptr)
    {
        Il2CppException *exception = nullptr;
        njs = *(reinterpret_cast<float *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(getNjsMethodInfo, storedDiffBeatmap, nullptr, &exception))));
    }
    else
    {
        if (storedDiffBeatmap == nullptr)
            log(INFO, "Beatmap null");
        else
            log(INFO, "Get NJS Method info null");
    }
    if (njs < 0)
        noteJumpMovementSpeed = njs;

    return BeatmapObjectSpawnController_Init(self, beatsPerMinute, noteLinesCount, noteJumpMovementSpeed, noteJumpStartBeatOffset, disappearingArrows, ghostNotes);
}

MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_JumpGravityForLineLayer, float, BeatmapObjectSpawnController *self, int lineLayer, int startLineLayer)
{

    float original = BeatmapObjectSpawnController_JumpGravityForLineLayer(self, lineLayer, startLineLayer);

    float result = (float)2 * (HighestJumpPosYForLineLayer(lineLayer, self->topLinesHighestJumpPosY, self->upperLinesHighestJumpPosY, self->baseLinesHighestJumpPosY, self->globalYJumpOffset) - LineYPosForLineLayer(startLineLayer, self->topLinesYPos, self->upperLinesYPos, self->baseLinesYPos)) / powf(self->jumpDistance / self->noteJumpMovementSpeed * 0.5, 2);

    return result;
}
MAKE_HOOK_OFFSETLESS(NoteCutDirectionExtensions_Rotation, Quaternion, int cutDirection)
{
    Quaternion result1 = NoteCutDirectionExtensions_Rotation(cutDirection);
    //  log(INFO,"Original result %f, %f, %f, %f", result1.x, result1.y, result1.z, result1.w);
    if (cutDirection >= 1000 && cutDirection <= 1360)
    {
        int angle = 1000 - cutDirection;
        //    log(INFO,"Quaternion Method: %s", il2cpp_class_get_methods(quaternionClass, i)->name);
        // il2cpp_runtime_invoke()
        Quaternion result = ToQuaternion(0, 0, angle);
        //       log(INFO,"Altered result %f, %f, %f, %f", result.x, result.y, result.z, result.w);
        return result;
    }
    else
    {
        return result1;
    }
}

MAKE_HOOK_OFFSETLESS(NoteData_MirrorTransformCutDirection, void, NoteData *self)
{
    int state = self->noteCutDirection;
    if (state >= 1000 && state <= 1360)
    {
        int newdir = 2360 - state;
        self->noteCutDirection = newdir;
        return;
    }
    else
    {
        return NoteData_MirrorTransformCutDirection(self);
    }
}

void MirrorLineIndex(BeatmapObjectData *object, int type, int lineIndex)
{
    if (type == 0)
    {
        //Note Index Mirror
        NoteData *note = static_cast<NoteData *>(object);
        int lineIndex = note->lineIndex;
        int flipLineIndex = note->flipLineIndex;
        if (lineIndex > 3 || lineIndex < 0)
        {
            if (lineIndex >= 1000 || lineIndex <= -1000)
            {
                int newIndex = lineIndex;
                bool leftSide = false;
                if (newIndex <= -1000)
                {
                    newIndex += 2000;
                }

                if (newIndex >= 4000)
                    leftSide = true;

                newIndex = 5000 - newIndex;
                if (leftSide)
                    newIndex -= 2000;
                note->lineIndex = newIndex;
            }
            else if (lineIndex > 3)
            {
                int diff = ((lineIndex - 3) * 2);
                int newlaneCount = 4 + diff;
                note->lineIndex = newlaneCount - diff - 1 - lineIndex;
            }
            else if (lineIndex < 0)
            {
                int diff = ((0 - lineIndex) * 2);
                int newlaneCount = 4 + diff;
                note->lineIndex = newlaneCount - diff - 1 - lineIndex;
            }
        }

        if (flipLineIndex > 3 || flipLineIndex < 0)
        {
            if (flipLineIndex >= 1000 || flipLineIndex <= -1000)
            {
                int newIndex = flipLineIndex;
                bool leftSide = false;
                if (newIndex <= -1000)
                {
                    newIndex += 2000;
                }

                if (newIndex >= 4000)
                    leftSide = true;

                newIndex = 5000 - newIndex;
                if (leftSide)
                    newIndex -= 2000;
                note->flipLineIndex = newIndex;
            }

            else if (flipLineIndex > 3)
            {
                int diff = ((flipLineIndex - 3) * 2);
                int newlaneCount = 4 + diff;
                note->flipLineIndex = newlaneCount - diff - 1 - flipLineIndex;
            }
            else if (flipLineIndex < 0)
            {
                int diff = ((0 - flipLineIndex) * 2);
                int newlaneCount = 4 + diff;
                note->flipLineIndex = newlaneCount - diff - 1 - flipLineIndex;
            }
        }
    }
    if (type == 1)
    {
        //Obstacle Index Mirror
        ObstacleData *obstacle = static_cast<ObstacleData *>(object);
        int __state = obstacle->lineIndex;

        bool precisionWidth = obstacle->width >= 1000;
        //   Console.WriteLine("Width: " + __instance.width);
        if (__state > 3 || __state < 0 || precisionWidth)
        {
            if (__state >= 1000 || __state <= -1000 || precisionWidth) // precision lineIndex
            {
                int newIndex = __state;
                if (newIndex <= -1000) // normalize index values, we'll fix them later
                {
                    newIndex += 1000;
                }
                else if (newIndex >= 1000)
                {
                    newIndex += -1000;
                }
                else
                {
                    newIndex = newIndex * 1000; //convert lineIndex to precision if not already
                }
                newIndex = (((newIndex - 2000) * -1) + 2000); //flip lineIndex

                int newWidth = obstacle->width; //normalize wall width
                if (newWidth < 1000)
                {
                    newWidth = newWidth * 1000;
                }
                else
                {
                    newWidth -= 1000;
                }
                newIndex = newIndex - newWidth;

                if (newIndex < 0)
                { //this is where we fix them
                    newIndex -= 1000;
                }
                else
                {
                    newIndex += 1000;
                }
                obstacle->lineIndex = newIndex;
            }
            else // state > -1000 || state < 1000 assumes no precision width
            {
                int mirrorLane = (((__state - 2) * -1) + 2);        //flip lineIndex
                obstacle->lineIndex = mirrorLane - obstacle->width; //adjust for wall width
            }
        }
    }
}
MAKE_HOOK_OFFSETLESS(BeatDataMirrorTransform_CreateTransformedData, BeatmapData *, BeatmapData *beatmapData)
{
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int index = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
            if (index > 3 || index < 0)
            {
                log(INFO, "Non-Standard Line indicies detected. Returning original data");
                return beatmapData;
            }
        }
    }
    return BeatDataMirrorTransform_CreateTransformedData(beatmapData);
}

MAKE_HOOK_OFFSETLESS(BeatmapDataNoArrowsTransform_CreateTransformedData, BeatmapData *, BeatmapData *beatmapData, bool randomColors)
{
    std::map<int, int> extendedLanesMap;
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex > 3)
            {
                //               log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 3;
            }
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex < 0)
            {
                //             log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 0;
            }
        }
    }
    BeatmapData *result = BeatmapDataNoArrowsTransform_CreateTransformedData(beatmapData, randomColors);
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            BeatmapObjectData *value = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end())
            {
                auto item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];

                //          log(INFO,"Found id: %i in map. Time %f", id, item->time);
                result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[id];
            }
        }
    }
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int rid = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            if (extendedLanesMap.find(rid) != extendedLanesMap.end())
            {

                //          log(INFO,"Restoring Original data for id: %i in map.", rid);
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[rid];
            }
        }
    }
    return result;
}
MAKE_HOOK_OFFSETLESS(BeatmapDataObstaclesAndBombsTransform_CreateTransformedData, BeatmapData *, BeatmapData *beatmapData, int enabledObstaclesType, bool noBombs)
{
    std::map<int, int> extendedLanesMap;
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex > 3)
            {
                //      log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 3;
            }
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex < 0)
            {
                //          log(INFO,"Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 0;
            }
        }
    }
    BeatmapData *result = BeatmapDataObstaclesAndBombsTransform_CreateTransformedData(beatmapData, enabledObstaclesType, noBombs);
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            BeatmapObjectData *value = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end())
            {
                auto item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];

                //         log(INFO,"Found id: %i in map. Time %f", id, item->time);
                result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[id];
            }
        }
    }
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int rid = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            if (extendedLanesMap.find(rid) != extendedLanesMap.end())
            {

                //           log(INFO,"Restoring Original data for id: %i in map.", rid);
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[rid];
            }
        }
    }
    return result;
}

MAKE_HOOK_OFFSETLESS(NoteData_MirrorLineIndex, void, NoteData *self, int lineCount)
{
    int lineIndex = self->lineIndex;
    int flipLineIndex = self->flipLineIndex;
    NoteData_MirrorLineIndex(self, lineCount);
    if (lineIndex > 3 || lineIndex < 0)
    {
        if (lineIndex >= 1000 || lineIndex <= -1000)
        {
            int newIndex = lineIndex;
            bool leftSide = false;
            if (newIndex <= -1000)
            {
                newIndex += 2000;
            }

            if (newIndex >= 4000)
                leftSide = true;

            newIndex = 5000 - newIndex;
            if (leftSide)
                newIndex -= 2000;
            self->lineIndex = newIndex;
        }
        else if (lineIndex > 3)
        {
            int diff = ((lineIndex - 3) * 2);
            int newlaneCount = 4 + diff;
            self->lineIndex = newlaneCount - diff - 1 - lineIndex;
        }
        else if (lineIndex < 0)
        {
            int diff = ((0 - lineIndex) * 2);
            int newlaneCount = 4 + diff;
            self->lineIndex = newlaneCount - diff - 1 - lineIndex;
        }
    }

    if (flipLineIndex > 3 || flipLineIndex < 0)
    {
        if (flipLineIndex >= 1000 || flipLineIndex <= -1000)
        {
            int newIndex = flipLineIndex;
            bool leftSide = false;
            if (newIndex <= -1000)
            {
                newIndex += 2000;
            }

            if (newIndex >= 4000)
                leftSide = true;

            newIndex = 5000 - newIndex;
            if (leftSide)
                newIndex -= 2000;
            self->flipLineIndex = newIndex;
        }

        else if (flipLineIndex > 3)
        {
            int diff = ((flipLineIndex - 3) * 2);
            int newlaneCount = 4 + diff;
            self->flipLineIndex = newlaneCount - diff - 1 - flipLineIndex;
        }
        else if (flipLineIndex < 0)
        {
            int diff = ((0 - flipLineIndex) * 2);
            int newlaneCount = 4 + diff;
            self->flipLineIndex = newlaneCount - diff - 1 - flipLineIndex;
        }
    }
    return;
}
MAKE_HOOK_OFFSETLESS(ObstacleData_MirrorLineIndex, void, ObstacleData *self, int lineCount)
{
    int __state = self->lineIndex;
    ObstacleData_MirrorLineIndex(self, lineCount);

    bool precisionWidth = self->width >= 1000;
    //   Console.WriteLine("Width: " + __instance.width);
    if (__state > 3 || __state < 0 || precisionWidth)
    {
        if (__state >= 1000 || __state <= -1000 || precisionWidth) // precision lineIndex
        {
            int newIndex = __state;
            if (newIndex <= -1000) // normalize index values, we'll fix them later
            {
                newIndex += 1000;
            }
            else if (newIndex >= 1000)
            {
                newIndex += -1000;
            }
            else
            {
                newIndex = newIndex * 1000; //convert lineIndex to precision if not already
            }
            newIndex = (((newIndex - 2000) * -1) + 2000); //flip lineIndex

            int newWidth = self->width; //normalize wall width
            if (newWidth < 1000)
            {
                newWidth = newWidth * 1000;
            }
            else
            {
                newWidth -= 1000;
            }
            newIndex = newIndex - newWidth;

            if (newIndex < 0)
            { //this is where we fix them
                newIndex -= 1000;
            }
            else
            {
                newIndex += 1000;
            }
            self->lineIndex = newIndex;
        }
        else // state > -1000 || state < 1000 assumes no precision width
        {
            int mirrorLane = (((__state - 2) * -1) + 2); //flip lineIndex
            self->lineIndex = mirrorLane - self->width;  //adjust for wall width
        }
    }
}
MAKE_HOOK_OFFSETLESS(FlyingScoreSpawner_SpawnFlyingScore, void, void *self, void *noteCutInfo, int noteLineIndex, int multiplier, Vector3 pos, Color color)
{
    if (noteLineIndex > 3)
        noteLineIndex = 3;
    if (noteLineIndex < 0)
        noteLineIndex = 0;
    return FlyingScoreSpawner_SpawnFlyingScore(self, noteCutInfo, noteLineIndex, multiplier, pos, color);
}
MAKE_HOOK_OFFSETLESS(BeatmapObjectSpawnController_GetNoteOffset, Vector3, BeatmapObjectSpawnController *self, int noteLineIndex, int noteLineLayer)
{
    Vector3 offset = BeatmapObjectSpawnController_GetNoteOffset(self, noteLineIndex, noteLineLayer);
    offset.x = 1.0;
    offset.y = 0;
    offset.z = 0;

    float num = (-(self->noteLinesCount - 1) * 0.5f);
    if (noteLineIndex >= 1000 || noteLineIndex <= -1000)
    {
        if (noteLineIndex <= -1000)
            noteLineIndex += 2000;
        num = (num + (((float)noteLineIndex) * (self->noteLinesDistance / 1000)));
    }
    else
    {
        num = (num + (float)noteLineIndex) * self->noteLinesDistance;
    }

    offset.x = 1.0f * num;
    offset.y = LineYPosForLineLayer(noteLineLayer, self->topLinesYPos, self->upperLinesYPos, self->baseLinesYPos);
    offset.z = 0;
    return offset;
}
static Il2CppClass *stretchableObstacleClass;
static const MethodInfo *SetSizeMethodInfo;
static Il2CppClass *ColorSchemeClass;
static const MethodInfo *get_obstaclesColor;
static Color obstacleColor;
MAKE_HOOK_OFFSETLESS(ColorManager_SetColorScheme, void, void *self, void *colorScheme)
{
    ColorManager_SetColorScheme(self, colorScheme);
    if (ColorSchemeClass == nullptr)
        ColorSchemeClass = il2cpp_utils::GetClassFromName("", "ColorScheme");
    if (get_obstaclesColor == nullptr)
        get_obstaclesColor = il2cpp_functions::class_get_method_from_name(ColorSchemeClass, "get_obstaclesColor", 0);

    Il2CppException *exception = nullptr;
    obstacleColor = *(reinterpret_cast<Color *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(get_obstaclesColor, colorScheme, nullptr, &exception))));
}
void SetStrechableObstacleSize(void *object, float paramOne, float paramTwo, float paramThree)
{
    if (stretchableObstacleClass == nullptr)
        stretchableObstacleClass = il2cpp_utils::GetClassFromName("", "StretchableObstacle");

    if (SetSizeMethodInfo == nullptr)
        SetSizeMethodInfo = il2cpp_functions::class_get_method_from_name(stretchableObstacleClass, "SetSizeAndColor", 4);

    Il2CppException *exception = nullptr;
    float *test;
    void *params[] = {&paramOne, &paramTwo, &paramThree, &obstacleColor};
    il2cpp_functions::runtime_invoke(SetSizeMethodInfo, object, params, &exception);
}

MAKE_HOOK_OFFSETLESS(ObstacleController_Init, void, Il2CppObject *self, ObstacleData *obstacleData, Vector3 startPos, Vector3 midPos, Vector3 endPos,
          float move1Duration, float move2Duration, float startTimeOffset, float singleLineWidth, float obsHeight)
{
    static auto startPosInfo = il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("", "ObstacleController"), "_startPos");
    static auto midPosInfo = il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("", "ObstacleController"), "_midPos");
    static auto endPosInfo = il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("", "ObstacleController"), "_endPos");

    static auto obstacleDataInfo = il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("", "ObstacleController"), "_obstacleData");
    il2cpp_functions::field_set_value(self, obstacleDataInfo, obstacleData);

    ObstacleController_Init(self, obstacleData, startPos, midPos, endPos, move1Duration, move2Duration, startTimeOffset, singleLineWidth, obsHeight);
    if ((obstacleData->obstacleType == 0 || obstacleData->obstacleType == 1) && !(obstacleData->width >= 1000))
        return;
    skipWallRatings = true;
    //   ObstacleController_Init(self, obstacleData, startPos, midPos, endPos, move1Duration, move2Duration, startTimeOffset, singleLineWidth);
    int mode = (obstacleData->obstacleType >= 4001 && obstacleData->obstacleType <= 4100000) ? 1 : 0;
    int height = 0;
    int startHeight = 0;
    if (mode == 1)
    {
        int value = obstacleData->obstacleType;
        value -= 4001;
        height = value / 1000;
        startHeight = value % 1000;
    }
    else
    {
        int value = obstacleData->obstacleType;
        height = value - 1000;
    }
    float num = 0;
    if ((obstacleData->width >= 1000) || (mode == 1))
    {

        float width = (float)obstacleData->width - 1000;
        float precisionLineWidth = singleLineWidth / 1000;
        num = width * precisionLineWidth; //Change y of b for start height
        Vector3 b{b.x = (num - singleLineWidth) * 0.5f, b.y = 4 * ((float)startHeight / 1000), b.z = 0};

        Vector3 newStartPos = AddVectors(startPos, b);
        Vector3 newMidPos = AddVectors(midPos, b);
        Vector3 newEndPos = AddVectors(endPos, b);
        il2cpp_functions::field_set_value(self, startPosInfo, &newStartPos);
        il2cpp_functions::field_set_value(self, midPosInfo, &newMidPos);
        il2cpp_functions::field_set_value(self, endPosInfo, &newEndPos);
        //     self->startPos = AddVectors(startPos, b);
        //     self->midPos = AddVectors(midPos, b);
        //     self->endPos = AddVectors(endPos, b);
    }
    else
    {
        num = (float)obstacleData->width * singleLineWidth;
    }
    float num2 = VectorMagnitude( SubtractVectors(*reinterpret_cast<Vector3*>(il2cpp_functions::object_unbox(il2cpp_functions::field_get_value_object(endPosInfo, self))), *reinterpret_cast<Vector3*>(il2cpp_functions::object_unbox(il2cpp_functions::field_get_value_object(midPosInfo, self))))) / move2Duration;
    float length = num2 * obstacleData->duration;
    float multiplier = 1;
    if ((int)obstacleData->obstacleType >= 1000)
    {
        multiplier = (float)height / 1000;
    }
    static auto controllerBoundsInfo = il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("", "ObstacleController"), "_bounds");
    static auto stretchableObstacleBoundsInfo = il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("", "StretchableObstacle"), "_bounds");
    static auto stretchableObstacleInfo = il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("", "ObstacleController"), "_stretchableObstacle");
    auto stretchableObstacle = il2cpp_functions::field_get_value_object(stretchableObstacleInfo, self);
    SetStrechableObstacleSize(stretchableObstacle, (num * 0.98f), (obsHeight * multiplier), (length));
    //  dump_real(0, 50, self->stretchableObstacle);
    auto stretchableObstacleBounds = il2cpp_functions::field_get_value_object(stretchableObstacleBoundsInfo, stretchableObstacle);
  //  Vector3 center = *(reinterpret_cast<Vector3*>(il2cpp_functions::object_unbox(il2cpp_functions::field_get_value_object(il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("UnityEngine", "Bounds"), "m_Center"), stretchableObstacleBounds))));
  //      Vector3 extents = *(reinterpret_cast<Vector3*>(il2cpp_functions::object_unbox(il2cpp_functions::field_get_value_object(il2cpp_functions::class_get_field_from_name(il2cpp_utils::GetClassFromName("UnityEngine", "Bounds"), "m_Extents"), stretchableObstacleBounds))));
  //  log(INFO,  "New Bounds %f, %f, %f |  %f, %f, %f ", center.x, center.y, center.z,extents.x, extents.y, extents.z );
    il2cpp_functions::field_set_value(self, controllerBoundsInfo, stretchableObstacleBounds);
}
struct ExecutionRatingRecorder
{
    void *scoreController;
    void *beatmapobjectSpawnController;
    void *playerHeadObstacleIntersection;
    void *audioTimeSync;
    void *ratings;
    void *hitObstacles;
    void *intersecObstacles;
    void *cutScoreHandlers;
    void *unusedCutScoreHandlers;
};
MAKE_HOOK_OFFSETLESS(BeatmapObjectExecutionRatingsRecorder_HandleBeatmapObjectSpawnControllerObstacleDidPassAvoidedMark, void, ExecutionRatingRecorder *self, BeatmapObjectSpawnController *spawnController, ObstacleController *obstacleController)
{
    if (skipWallRatings)
        return;
    else
    {
        return BeatmapObjectExecutionRatingsRecorder_HandleBeatmapObjectSpawnControllerObstacleDidPassAvoidedMark(self, spawnController, obstacleController);
    }
}
MAKE_HOOK_OFFSETLESS(BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData, BeatmapData *, List<SaveDataNoteData *> *noteSaveData,
          List<SaveDataObstacleData *> *obstaclesSaveData, List<SaveDataEventData *> *eventsSaveData, float beatsPerMinute, float shuffle, float shufflePeriod)
{
    std::map<int, int> extendedLanesMap;
    int num = -1;
    for (int i = 0; i < noteSaveData->size; ++i)
    {
        num++;
        auto item = noteSaveData->items->values[i];
        if (noteSaveData->items->values[i]->lineIndex > 3)
        {
            //       log(INFO,"Putting id: %i in map.", num);
            extendedLanesMap[num] = noteSaveData->items->values[i]->lineIndex;
            noteSaveData->items->values[i]->lineIndex = 3;
        }

        if (noteSaveData->items->values[i]->lineIndex < 0)
        {
            //          log(INFO,"Putting id: %i in map.", num);
            extendedLanesMap[num] = noteSaveData->items->values[i]->lineIndex;
            noteSaveData->items->values[i]->lineIndex = 0;
        }
    }

    for (int i = 0; i < obstaclesSaveData->size; ++i)
    {
        num++;
        if (obstaclesSaveData->items->values[i]->lineIndex > 3)
        {
            //           log(INFO,"Putting id: %i in map", num);
            extendedLanesMap[num] = obstaclesSaveData->items->values[i]->lineIndex;
            obstaclesSaveData->items->values[i]->lineIndex = 3;
        }

        if (obstaclesSaveData->items->values[i]->lineIndex < 0)
        {
            //      log(INFO,"Putting id: %i in map", num);
            extendedLanesMap[num] = obstaclesSaveData->items->values[i]->lineIndex;
            obstaclesSaveData->items->values[i]->lineIndex = 0;
        }
    }

    BeatmapData *result = BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData(noteSaveData, obstaclesSaveData, eventsSaveData, beatsPerMinute, shuffle, shufflePeriod);
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            BeatmapObjectData *value = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end())
            {
                auto item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];

                //            log(INFO,"Found id: %i in map.", id);
                result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[id];
            }
        }
    }
    if (extendedLanesMap.size() > 0)
    {
        log(INFO, "Attempting to correct extended lanes not flipping");
        std::vector<NoteData *> allnotes;
        //Get all the notes
        for (int i = 0; i < result->beatmapLinesData->Length(); ++i)
        {
            for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
            {
                if (result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->beatmapObjectType == 0)
                    allnotes.push_back(reinterpret_cast<NoteData *>(result->beatmapLinesData->values[i]->beatmapObjectData->values[j]));
            }
        }
        std::vector<NoteData *> list2;
        list2.reserve(4);
        float noteTime = -1;
        float time = 0;
        NoteData *noteData = nullptr;
        for (int i = 0; i < allnotes.size(); ++i)
        {
            allnotes[i]->flipLineIndex = allnotes[i]->lineIndex;
            allnotes[i]->flipYSide = 0;
            float realTimeFromBPMTime = GetRealTimeFromBPMTime(allnotes[i]->time, beatsPerMinute, shuffle, shufflePeriod);
            time = realTimeFromBPMTime;
            int lineIndex = allnotes[i]->lineIndex;
            int lineLayer = allnotes[i]->noteLineLayer;
            int startNoteLineLayer = 0;

            if (noteData != nullptr && noteData->lineIndex == lineIndex && fabs(noteData->time - realTimeFromBPMTime) < 0.0001f)
            {
                if (noteData->startNoteLineLayer == 0)
                {
                    startNoteLineLayer = 1;
                }
                else
                {
                    startNoteLineLayer = 2;
                }
            }
            int type = allnotes[i]->noteType;
            if (list2.size() > 0 && list2[0]->time < realTimeFromBPMTime - 0.001f && (type == 0 || type == 1))
            {
                ProcessBasicNotesInTimeRow(list2, realTimeFromBPMTime);
                noteTime = list2[0]->time;
                list2.clear();
            }
            noteData = allnotes[i];
            if (noteData->noteType == 0 || noteData->noteType == 1)
                list2.push_back(noteData);
        }
        ProcessBasicNotesInTimeRow(list2, std::numeric_limits<float>::max());
    }

    return result;
}

extern "C" void load()
{

    log(INFO, "Installing Mapping Extensions Hooks!");

    il2cpp_functions::Init();

    INSTALL_HOOK_OFFSETLESS(NoteCutDirectionExtensions_Rotation, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "NoteCutDirectionExtensions"), "Rotation", 1));
    INSTALL_HOOK_OFFSETLESS(NoteData_MirrorLineIndex, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "NoteData"), "MirrorLineIndex", 1));
    INSTALL_HOOK_OFFSETLESS(NoteData_MirrorTransformCutDirection, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "NoteData"), "MirrorTransformCutDirection", 0));
    INSTALL_HOOK_OFFSETLESS(ObstacleData_MirrorLineIndex, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "ObstacleData"), "MirrorLineIndex", 1));
    INSTALL_HOOK_OFFSETLESS(FlyingScoreSpawner_SpawnFlyingScore, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "FlyingScoreSpawner"), "SpawnFlyingScore", 5));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnController_GetNoteOffset, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatmapObjectSpawnController"), "GetNoteOffset", 2));
    INSTALL_HOOK_OFFSETLESS(BeatmapDataLoader_GetBeatmapDataFromBeatmapSaveData, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatmapDataLoader"), "GetBeatmapDataFromBeatmapSaveData", 6));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnController_JumpGravityForLineLayer, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatmapObjectSpawnController"), "JumpGravityForLineLayer", 2));
    INSTALL_HOOK_OFFSETLESS(BeatDataMirrorTransform_CreateTransformedData, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatDataMirrorTransform"), "CreateTransformedData", 1));
    INSTALL_HOOK_OFFSETLESS(BeatmapDataNoArrowsTransform_CreateTransformedData, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatmapDataNoArrowsTransform"), "CreateTransformedData", 2));
    INSTALL_HOOK_OFFSETLESS(BeatmapDataObstaclesAndBombsTransform_CreateTransformedData, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatmapDataObstaclesAndBombsTransform"), "CreateTransformedData", 3));
    INSTALL_HOOK_OFFSETLESS(ObstacleController_Init, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "ObstacleController"), "Init", 9));
    INSTALL_HOOK_OFFSETLESS(ColorManager_SetColorScheme, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "ColorManager"), "SetColorScheme", 1));
    INSTALL_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "StandardLevelDetailView"), "RefreshContent", 0));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectSpawnController_Init, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatmapObjectSpawnController"), "Init", 6));
    INSTALL_HOOK_OFFSETLESS(BeatmapObjectExecutionRatingsRecorder_HandleBeatmapObjectSpawnControllerObstacleDidPassAvoidedMark, il2cpp_functions::class_get_method_from_name(il2cpp_utils::GetClassFromName("", "BeatmapObjectExecutionRatingsRecorder"), "HandleBeatmapObjectSpawnControllerObstacleDidPassAvoidedMark", 2));

    log(INFO, "Installed  Mapping Extensions Hooks!");
}
