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
#include "../beatsaber-hook/shared/utils/logging.h"
#include "../beatsaber-hook/shared/inline-hook/inlineHook.h"
#include "../beatsaber-hook/shared/utils/utils.h"
#include "../beatsaber-hook/shared/utils/typedefs.h"
//#define line_y_pos_for_line_layer_offset 0x4F5AC0
#define spawn_flying_score_offset 0xA4D46C
#define get_note_offset_offset 0x9D261C
#define note_rotation_offset 0xA80980
#define jump_gravity_for_line_layer_offset 0x9D15AC

#define note_rotation_mirror_offset 0xA84300
#define note_mirror_offset 0xA84398
#define obstacle_mirror_offset 0xCDAA28

#define get_beatmap_data_from_savedata_offset 0x9A1D0C
#define mirror_transformed_data_offset 0x99DD8C
#define noarrows_transformed_data_offset 0x99F678
#define obstacles_bombs_transformed_data_offset 0x99ED58

#define obstacle_controller_init_offset 0xCD9F24
#define color_manager_set_color_scheme_offset 0xA345A8
#define MOD_ID "MappingExtenions"
#define VERSION "0.11.0"

using il2cpp_utils::GetClassFromName;
using TYPEDEFS_H::Quaternion;

static void dump_real(int before, int after, void* ptr) {
    log(DEBUG, "Dumping Immediate Pointer: %p: %lx", ptr, *reinterpret_cast<long*>(ptr));
    auto begin = static_cast<long*>(ptr) - before;
    auto end = static_cast<long*>(ptr) + after;
    for (auto cur = begin; cur != end; ++cur) {
        log(DEBUG, "0x%lx: %lx", (long)cur - (long)ptr, *cur);
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
    void *obstacleCore;
    void *stretchableCore;
    void *obstacleFrame;
    void *obstacleFakeGlow;
    void* addColorSetters;
    void* tintcolorSetters;
    Bounds bounds;
};

struct ObstacleController : UnityObject
{
    void *activeObstaclesManager;
    StretchableObstacle *stretchableObstacle;
    void* color;
    float endDistanceOffset;
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
    float passedAvoidedMarkTime;
    float finishMovementTime;
    bool initialized;
    Bounds bounds;
    bool dissolving;
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

MAKE_HOOK(line_y_pos, 0x12FC1F0, float, BeatmapObjectSpawnController *self, int lineLayer)
{
    log(INFO, "line_y_pos hook called");
    return line_y_pos(self, lineLayer);
}
MAKE_HOOK(jump_gravity_for_line_layer, jump_gravity_for_line_layer_offset, float, BeatmapObjectSpawnController *self, int lineLayer, int startLineLayer)
{

    float original = jump_gravity_for_line_layer(self, lineLayer, startLineLayer);

    float result = (float)2 * (HighestJumpPosYForLineLayer(lineLayer, self->topLinesHighestJumpPosY, self->upperLinesHighestJumpPosY, self->baseLinesHighestJumpPosY, self->globalYJumpOffset) - LineYPosForLineLayer(startLineLayer, self->topLinesYPos, self->upperLinesYPos, self->baseLinesYPos)) / powf(self->jumpDistance / self->noteJumpMovementSpeed * 0.5, 2);

    return result;
}
MAKE_HOOK(note_rotation, note_rotation_offset, Quaternion, int cutDirection)
{
    log(INFO, "Called note_rotation Hook");

    Quaternion result1 = note_rotation(cutDirection);
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

MAKE_HOOK(note_rotation_mirror, note_rotation_mirror_offset, void, NoteData *self)
{
    //    log(INFO,"Called note_rotation_mirror Hook CutDirection %i", self->noteCutDirection);
    int state = self->noteCutDirection;
    if (state >= 1000 && state <= 1360)
    {
        int newdir = 2360 - state;
        self->noteCutDirection = newdir;
        //       log(INFO,"Finished note_rotation_mirror Hook");
        return;
    }
    else
    {
        //     log(INFO,"Finished note_rotation_mirror Hook");
        return note_rotation_mirror(self);
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
MAKE_HOOK(mirror_transformed_data, mirror_transformed_data_offset, BeatmapData *, BeatmapData *beatmapData)
{
    log(INFO, "Called mirror_transformed_data Hook");
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
    return mirror_transformed_data(beatmapData);
}

MAKE_HOOK(noarrows_transformed_data, noarrows_transformed_data_offset, BeatmapData *, BeatmapData *beatmapData, bool randomColors)
{
    log(INFO, "Called noarrows_transformed_data Hook");
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
    BeatmapData *result = noarrows_transformed_data(beatmapData, randomColors);
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
MAKE_HOOK(obstacles_bombs_transformed_data, obstacles_bombs_transformed_data_offset, BeatmapData *, BeatmapData *beatmapData, int enabledObstaclesType, bool noBombs)
{
    log(INFO, "Called obstacles_bombs_transformed_data Hook");
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
    BeatmapData *result = obstacles_bombs_transformed_data(beatmapData, enabledObstaclesType, noBombs);
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

MAKE_HOOK(note_mirror, note_mirror_offset, void, NoteData *self, int lineCount)
{
    //   log(INFO,"Called note_mirror Hook LineIndex %i, flipIndex %i", self->lineIndex, self->flipLineIndex);
    int lineIndex = self->lineIndex;
    int flipLineIndex = self->flipLineIndex;
    note_mirror(self, lineCount);
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
    //      log(INFO,"Finished note_mirror Hook");
    return;
}
MAKE_HOOK(obstacle_mirror, obstacle_mirror_offset, void, ObstacleData *self, int lineCount)
{
    //log(INFO,"Called obstacle_mirror Hook");
    int __state = self->lineIndex;
    obstacle_mirror(self, lineCount);

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
MAKE_HOOK(spawn_flying_score, spawn_flying_score_offset, void, void *self, void *noteCutInfo, int noteLineIndex, int multiplier, Vector3 pos, Color color)
{
    //    log(INFO, "Called spawn_flying_score Hook");
    if (noteLineIndex > 3)
        noteLineIndex = 3;
    if (noteLineIndex < 0)
        noteLineIndex = 0;
    return spawn_flying_score(self, noteCutInfo, noteLineIndex, multiplier, pos, color);
}
MAKE_HOOK(get_note_offset, get_note_offset_offset, Vector3, BeatmapObjectSpawnController *self, int noteLineIndex, int noteLineLayer)
{
    //   log(INFO, "Called get_note_offset Hook");
    Vector3 offset = get_note_offset(self, noteLineIndex, noteLineLayer);
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
MAKE_HOOK(color_manager_set_color_scheme, color_manager_set_color_scheme_offset, void, void* self, void* colorScheme)
{
    log(INFO, "Callec color_manager_set_color_scheme hook");
    color_manager_set_color_scheme(self, colorScheme);
        if (ColorSchemeClass == nullptr)
        ColorSchemeClass = GetClassFromName("", "ColorScheme");
        if(get_obstaclesColor == nullptr)
            get_obstaclesColor = il2cpp_functions::class_get_method_from_name(ColorSchemeClass, "get_obstaclesColor", 0);

    Il2CppException *exception = nullptr;
    obstacleColor = *(reinterpret_cast<Color *>(il2cpp_functions::object_unbox(il2cpp_functions::runtime_invoke(get_obstaclesColor, colorScheme, nullptr, &exception))));

}
void SetStrechableObstacleSize(void *object, float paramOne, float paramTwo, float paramThree)
{
    if (stretchableObstacleClass == nullptr)
        stretchableObstacleClass = GetClassFromName("", "StretchableObstacle");

        if(SetSizeMethodInfo == nullptr)
    SetSizeMethodInfo = il2cpp_functions::class_get_method_from_name(stretchableObstacleClass, "SetSizeAndColor", 4);

    Il2CppException *exception = nullptr;
    float *test;
    void *params[] = {&paramOne, &paramTwo, &paramThree, &obstacleColor};
    il2cpp_functions::runtime_invoke(SetSizeMethodInfo, object, params, &exception);
}
MAKE_HOOK(obstacle_controller_init, obstacle_controller_init_offset, void, ObstacleController *self, ObstacleData *obstacleData, Vector3 startPos, Vector3 midPos, Vector3 endPos,
          float move1Duration, float move2Duration, float startTimeOffset, float singleLineWidth, float obsHeight)
{
      log(INFO, "Called obstacle_controller_init Hook");
    obstacle_controller_init(self, obstacleData, startPos, midPos, endPos, move1Duration, move2Duration, startTimeOffset, singleLineWidth, obsHeight);
    if((obstacleData->obstacleType == 0 || obstacleData->obstacleType == 1) && !(obstacleData->width >= 1000))
        return;
 //   obstacle_controller_init(self, obstacleData, startPos, midPos, endPos, move1Duration, move2Duration, startTimeOffset, singleLineWidth);
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
        self->startPos = AddVectors(startPos, b);
        self->midPos = AddVectors(midPos, b);
        self->endPos = AddVectors(endPos, b);
    }
    else
    {
        num = (float)obstacleData->width * singleLineWidth;
    }
    float num2 = VectorMagnitude(SubtractVectors(self->endPos, self->midPos)) / move2Duration;
    float length = num2 * obstacleData->duration;
    float multiplier = 1;
    if ((int)obstacleData->obstacleType >= 1000)
    {
        multiplier = (float)height / 1000;
    }
    SetStrechableObstacleSize(self->stretchableObstacle, fabs(num * 0.98f), fabs(obsHeight * multiplier), fabs(length));
   dump_real(0, 50, self);
   // self->bounds = self->stretchableObstacle->bounds;
}
MAKE_HOOK(get_beatmap_data_from_savedata, get_beatmap_data_from_savedata_offset, BeatmapData *, List<SaveDataNoteData *> *noteSaveData,
          List<SaveDataObstacleData *> *obstaclesSaveData, List<SaveDataEventData *> *eventsSaveData, float beatsPerMinute, float shuffle, float shufflePeriod)
{
    log(INFO, "Called get_beatmap_data_from_savedata Hook");
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

    BeatmapData *result = get_beatmap_data_from_savedata(noteSaveData, obstaclesSaveData, eventsSaveData, beatsPerMinute, shuffle, shufflePeriod);
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

    log(INFO, "get_beatmap_data_from_savedata Hook finished");
    return result;
}

__attribute__((constructor)) void lib_main()
{

    log(INFO, "Installing Mapping Extensions Hooks!");
    INSTALL_HOOK(note_rotation);
    INSTALL_HOOK(note_rotation_mirror);
    INSTALL_HOOK(note_mirror);
    INSTALL_HOOK(obstacle_mirror);
    INSTALL_HOOK(spawn_flying_score);
    INSTALL_HOOK(get_note_offset);
    INSTALL_HOOK(get_beatmap_data_from_savedata);
    INSTALL_HOOK(jump_gravity_for_line_layer);
    INSTALL_HOOK(line_y_pos);
    INSTALL_HOOK(mirror_transformed_data);
    INSTALL_HOOK(noarrows_transformed_data);
    INSTALL_HOOK(obstacles_bombs_transformed_data);
    INSTALL_HOOK(obstacle_controller_init);
    INSTALL_HOOK(color_manager_set_color_scheme);
    log(INFO, "Installed  Mapping Extensions Hooks!");
    log(INFO, "Initializing Il2Cpp Functions for Mapping Extensions");
    il2cpp_functions::Init();
    log(INFO, "Initialized Il2Cpp Functions for Mapping Extensions");
}
