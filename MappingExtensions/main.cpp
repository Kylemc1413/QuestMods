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

#undef log
#define log(...) __android_log_print(ANDROID_LOG_INFO, "QuestHook", "[MappingExtensions v0.9.0] " __VA_ARGS__)

//#define line_y_pos_for_line_layer_offset 0x12FC1F0
#define spawn_flying_score_offset 0x1323530
#define get_note_offset_offset 0x12FD5E8
#define note_rotation_offset 0x1349508
#define jump_gravity_for_line_layer_offset 0x12FC214

#define note_rotation_mirror_offset 0x134D81C
#define note_mirror_offset 0x134D8CC
#define obstacle_mirror_offset 0x13B1008

#define get_beatmap_data_from_savedata_offset 0x12A9774
#define mirror_transformed_data_offset 0x12A4ED4
#define noarrows_transformed_data_offset 0x12A6BC4
#define obstacles_bombs_transformed_data_offset 0x12A614C

#define obstacle_controller_init_offset 0x13B03CC

//extern "C" void * /* to be casted to Array */ il2cpp_array_new(Class *klass, int32_t len);
//extern "C" Class * /* to be casted to Array */ il2cpp_class_from_type(void *Il2CppType_type);
using il2cpp_utils::GetClassFromName;
template <class T>
struct List : Object
{
    Array<T> *items;
    int size;
    int version;    // ?
    void *syncRoot; // ?
    uint padding2;  // ?
    uint padding3;  // ?

    //   auto static LIST_ADD_ITEM = reinterpret_cast<function_ptr_t<void, List<T> *, T>>(getRealOffset(0x12105A4));
};

struct UnityObject : Object
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

struct SaveDataNoteData : Object
{
    float time;
    int lineIndex;
    int lineLayer;
    int type;
    int cutDirection;
};

struct SaveDataObstacleData : Object
{
    float time;
    int lineIndex;
    int type;
    float duration;
    int width;
};

struct SaveDataEventData : Object
{
    float time;
    int eventType;
    int value;
};

struct BeatmapObjectData : Object
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

struct BeatmapEventData : Object
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

struct BeatmapLineData : Object
{
    Array<BeatmapObjectData *> *beatmapObjectData;
};

struct Bounds : Object
{
    Vector3 center;
    Vector3 extents;
};

struct StretchableObstacle : UnityObject
{
    float edgeSize;
    float coreOffset;
    void *obstacleCore;
    void *stretchableCore;
    void *obstacleFrame;
    void *obstacleFakeGlow;
    Bounds bounds;
};

struct ObstacleController : UnityObject
{
    void *activeObstaclesManager;
    StretchableObstacle *stretchableObstacle;
    float height;
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

struct BeatmapData : Object
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
/* 
MAKE_HOOK(line_y_pos_for_line_layer, line_y_pos_for_line_layer_offset, float,int lineLayer)
{
    log("Called line_y_pos_for_line_layer Hook");
    line_y_pos_for_line_layer(lineLayer);
    return (float)600;
}
*/

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
    //  log("Global offset: %f", globalYJumpOffset);
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
    log("line_y_pos hook called");
    return line_y_pos(self, lineLayer);
}
MAKE_HOOK(jump_gravity_for_line_layer, jump_gravity_for_line_layer_offset, float, BeatmapObjectSpawnController *self, int lineLayer, int startLineLayer)
{
    //  log("jump_gravity_for_line_layer hook called");
    float original = jump_gravity_for_line_layer(self, lineLayer, startLineLayer);
    //  log("original %f", original);
    float result = (float)2 * (HighestJumpPosYForLineLayer(lineLayer, self->topLinesHighestJumpPosY, self->upperLinesHighestJumpPosY, self->baseLinesHighestJumpPosY, self->globalYJumpOffset) - LineYPosForLineLayer(startLineLayer, self->topLinesYPos, self->upperLinesYPos, self->baseLinesYPos)) / powf(self->jumpDistance / self->noteJumpMovementSpeed * 0.5, 2);
    //  log("result %f jumpDis %f, njs %f", result, self->jumpDistance, self->noteJumpMovementSpeed);
    return result;
    //     return (float)2 * (HighestJumpPosYForLineLayer(lineLayer, self->globalYJumpOffset) - LineYPosForLineLayer(startLineLayer))
    //     / powf(self->jumpDistance / self->noteJumpMovementSpeed * 0.5, 2);
}
MAKE_HOOK(note_rotation, note_rotation_offset, Quaternion, int cutDirection)
{
    log("Called note_rotation Hook");

    Quaternion result1 = note_rotation(cutDirection);
    //  log("Original result %f, %f, %f, %f", result1.x, result1.y, result1.z, result1.w);
    if (cutDirection >= 1000 && cutDirection <= 1360)
    {
        int angle = 1000 - cutDirection;
        //    log("Quaternion Method: %s", il2cpp_class_get_methods(quaternionClass, i)->name);
        // il2cpp_runtime_invoke()
        Quaternion result = ToQuaternion(0, 0, angle);
        //       log("Altered result %f, %f, %f, %f", result.x, result.y, result.z, result.w);
        return result;
    }
    else
    {
        return result1;
    }
}

MAKE_HOOK(note_rotation_mirror, note_rotation_mirror_offset, void, NoteData *self)
{
    //    log("Called note_rotation_mirror Hook CutDirection %i", self->noteCutDirection);
    int state = self->noteCutDirection;
    if (state >= 1000 && state <= 1360)
    {
        int newdir = 2360 - state;
        self->noteCutDirection = newdir;
        //       log("Finished note_rotation_mirror Hook");
        return;
    }
    else
    {
        //     log("Finished note_rotation_mirror Hook");
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
    log("Called mirror_transformed_data Hook");
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int index = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
            if (index > 3 || index < 0)
            {
                log("Non-Standard Line indicies detected. Returning original data");
                return beatmapData;
            }
        }
    }
    return mirror_transformed_data(beatmapData);
    /* 
    std::map<int, int> extendedLanesMap;
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex > 3)
            {
                //               log("Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 3;
            }
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex < 0)
            {
                //           log("Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 0;
            }
        }
    }
    log("Running original mirror function on modified data");
    BeatmapData *result = mirror_transformed_data(beatmapData);
    log("Ran original mirror function on modified data");
    for (int i = 0; i < result->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < result->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            BeatmapObjectData *value = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];
            if (extendedLanesMap.find(id) != extendedLanesMap.end())
            {
                auto item = result->beatmapLinesData->values[i]->beatmapObjectData->values[j];

                //              log("Found id: %i in map. Time %f", id, item->time);
                MirrorLineIndex(result->beatmapLinesData->values[i]->beatmapObjectData->values[j],
                                result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->beatmapObjectType, extendedLanesMap[id]);
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

                //              log("Restoring Original data for id: %i in map.", rid);
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[rid];
            }
        }
    }
    return result;
    */
}

MAKE_HOOK(noarrows_transformed_data, noarrows_transformed_data_offset, BeatmapData *, BeatmapData *beatmapData, bool randomColors)
{
    log("Called noarrows_transformed_data Hook");
    std::map<int, int> extendedLanesMap;
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex > 3)
            {
                //               log("Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 3;
            }
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex < 0)
            {
                //             log("Putting id: %i in map.", id);
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

                //          log("Found id: %i in map. Time %f", id, item->time);
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

                //          log("Restoring Original data for id: %i in map.", rid);
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[rid];
            }
        }
    }
    return result;
}
MAKE_HOOK(obstacles_bombs_transformed_data, obstacles_bombs_transformed_data_offset, BeatmapData *, BeatmapData *beatmapData, int enabledObstaclesType, bool noBombs)
{
    log("Called obstacles_bombs_transformed_data Hook");
    std::map<int, int> extendedLanesMap;
    for (int i = 0; i < beatmapData->beatmapLinesData->Length(); ++i)
    {
        for (int j = 0; j < beatmapData->beatmapLinesData->values[i]->beatmapObjectData->Length(); ++j)
        {
            int id = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->id;
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex > 3)
            {
                //      log("Putting id: %i in map.", id);
                extendedLanesMap[id] = beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex;
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = 3;
            }
            if (beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex < 0)
            {
                //          log("Putting id: %i in map.", id);
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

                //         log("Found id: %i in map. Time %f", id, item->time);
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

                //           log("Restoring Original data for id: %i in map.", rid);
                beatmapData->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[rid];
            }
        }
    }
    return result;
}

MAKE_HOOK(note_mirror, note_mirror_offset, void, NoteData *self, int lineCount)
{
    //   log("Called note_mirror Hook LineIndex %i, flipIndex %i", self->lineIndex, self->flipLineIndex);
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
    //      log("Finished note_mirror Hook");
    return;
}
MAKE_HOOK(obstacle_mirror, obstacle_mirror_offset, void, ObstacleData *self, int lineCount)
{
    //log("Called obstacle_mirror Hook");
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
MAKE_HOOK(spawn_flying_score, spawn_flying_score_offset, void, void *self, void *noteCutInfo, int noteLineIndex, int multiplier, Vector3 pos, Color color, void *saberAfterCutSwingRatingCounter)
{
    log("Called spawn_flying_score Hook");
    if (noteLineIndex > 3)
        noteLineIndex = 3;
    if (noteLineIndex < 0)
        noteLineIndex = 0;
    return spawn_flying_score(self, noteCutInfo, noteLineIndex, multiplier, pos, color, saberAfterCutSwingRatingCounter);
}
MAKE_HOOK(get_note_offset, get_note_offset_offset, Vector3, BeatmapObjectSpawnController *self, int noteLineIndex, int noteLineLayer)
{
    log("Called get_note_offset Hook");
    //    log("NoteLineCount %f", self->noteLinesCount);
    //        log("NoteLineDistance %f", self->noteLinesDistance);
    //   log("NoteLineLayer %i", noteLineLayer);
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

    //    log("Index: %i Layer: %i Offset: %f, %f, %f", noteLineIndex, noteLineLayer, offset.x, offset.y, offset.z);
    return offset;
}
static Il2CppClass *stretchableObstacleClass;
static const MethodInfo *SetSizeMethodInfo;
static const MethodInfo *(*get_methods)(Il2CppClass *, void **) = nullptr;
static Il2CppObject *(*runtime_invoke)(const MethodInfo *, void *, void **, Il2CppException **) = nullptr;
void SetStrechableObstacleSize(void *object, float paramOne, float paramTwo, float paramThree)
{
    if (stretchableObstacleClass == nullptr)
        stretchableObstacleClass = GetClassFromName("", "StretchableObstacle");

    if(get_methods == nullptr || runtime_invoke == nullptr)
    {
    void *imagehandle = dlopen("/data/app/com.beatgames.beatsaber-1/lib/arm/libil2cpp.so", 1);
    if(get_methods == nullptr)
        *(void **)(&get_methods) = dlsym(imagehandle, "il2cpp_class_get_methods");
    if(runtime_invoke == nullptr)
        *(void **)(&runtime_invoke) = dlsym(imagehandle, "il2cpp_runtime_invoke");
    dlclose(imagehandle);
    }


    void *iter = nullptr;
    MethodInfo const *m;
    if (SetSizeMethodInfo == nullptr || SetSizeMethodInfo->parameters_count != 3)
        while ((m = get_methods(stretchableObstacleClass, &iter)) != nullptr)
        {
            if (std::strncmp(m->name, "SetSize", 7) == 0)
            {
                //          log("Stretchable Obstacle Method: %s", m->name);
                SetSizeMethodInfo = m;
                //       log("SetSizeMethod: %i", SetSizeMethodInfo->parameters_count);
                break;
            }
        }

    Il2CppException *exception = nullptr;
    float *test;
    void *params[] = {&paramOne, &paramTwo, &paramThree};
 //   log("Calling runtime_invoke");
    runtime_invoke(SetSizeMethodInfo, object, params, &exception);
  //  log("Called runtime_invoke");
}
MAKE_HOOK(obstacle_controller_init, obstacle_controller_init_offset, void, ObstacleController *self, ObstacleData *obstacleData, Vector3 startPos, Vector3 midPos, Vector3 endPos,
          float move1Duration, float move2Duration, float startTimeOffset, float singleLineWidth)
{
    log("Called obstacle_controller_init Hook");
        obstacle_controller_init(self, obstacleData, startPos, midPos, endPos, move1Duration, move2Duration, startTimeOffset, singleLineWidth);
    
    obstacle_controller_init(self, obstacleData, startPos, midPos, endPos, move1Duration, move2Duration, startTimeOffset, singleLineWidth);
    int mode = (obstacleData->obstacleType >= 4001 && obstacleData->obstacleType <= 4100000) ? 1 : 0;
    int height = 0;
    int startHeight = 0;
    if (mode == 1)
    {
        int value = obstacleData->obstacleType;
        value -= 4001;
        height = value / 1000;
        startHeight = value % 1000;
        //     log("startHeight %i", startHeight);
        //     Console.WriteLine(height + "<---Height       StartHeight---> " + startHeight);
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
        //      log("Width %f   PrecisionWidth %f", width, precisionLineWidth);
        num = width * precisionLineWidth; //Change y of b for start height
                                          //     log("num %f", num);
        //    Vector3 b = new Vector3((num - singleLineWidth) * 0.5f, 4 * ((float)startHeight / 1000), 0f);
        Vector3 b{b.x = (num - singleLineWidth) * 0.5f, b.y = 4 * ((float)startHeight / 1000), b.z = 0};
        //     log("b.y %f",b.y);
              self->startPos = AddVectors(startPos, b);
              self->midPos = AddVectors(midPos, b);
              self->endPos = AddVectors(endPos, b);

   //        log("startPos %f   %f   %f", self->startPos.x, self->startPos.y, self->startPos.z);
   //        log("midPos %f   %f   %f", self->midPos.x, self->midPos.y, self->midPos.z);
   //        log("endPos %f   %f   %f", self->endPos.x, self->endPos.y, self->endPos.z);
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
//    log("Calling SetStrechableObstacleSize %f | %f | %f | %f", fabs(num * 0.98f), fabs(self->height * multiplier), fabs(length), self->height);
    SetStrechableObstacleSize(self->stretchableObstacle, fabs(num * 0.98f), fabs(self->height * multiplier), fabs(length));
 //   log("extents 1 %f   %f   %f", self->bounds.extents.x, self->bounds.extents.y, self->bounds.extents.z);
 //       log("extents 2 %f   %f   %f", self->stretchableObstacle->bounds.extents.x, self->stretchableObstacle->bounds.extents.y, self->stretchableObstacle->bounds.extents.z);
            self->bounds.center.x = self->stretchableObstacle->bounds.center.x;
            self->bounds.center.y = self->stretchableObstacle->bounds.center.y;
            self->bounds.center.z = self->stretchableObstacle->bounds.center.z;
    //  self->bounds = self->stretchableObstacle->bounds;
    //     ____stretchableObstacle.SetSize(fabs(num * 0.98f),fabs(self->height * multiplier), fabs(length));
    //  ____bounds = ____stretchableObstacle.bounds;
    
}
MAKE_HOOK(get_beatmap_data_from_savedata, get_beatmap_data_from_savedata_offset, BeatmapData *, List<SaveDataNoteData *> *noteSaveData,
          List<SaveDataObstacleData *> *obstaclesSaveData, List<SaveDataEventData *> *eventsSaveData, float beatsPerMinute, float shuffle, float shufflePeriod)
{
    log("Called get_beatmap_data_from_savedata Hook");
/* 
  if (stretchableObstacleClass == nullptr)
        stretchableObstacleClass = GetClassFromName("", "StretchableObstacle");

    void *imagehandle = dlopen("/data/app/com.beatgames.beatsaber-1/lib/arm/libil2cpp.so", 1);

    const MethodInfo *(*get_methods)(Il2CppClass *, void **);
    *(void **)(&get_methods) = dlsym(imagehandle, "il2cpp_class_get_methods");
    Il2CppObject *(*runtime_invoke)(const MethodInfo *, void *, void **, Il2CppException **);
    *(void **)(&runtime_invoke) = dlsym(imagehandle, "il2cpp_runtime_invoke");
    // log("Prepared Il2cpp functions");

    void *iter = nullptr;
    MethodInfo const *m;
    if (SetSizeMethodInfo == nullptr || SetSizeMethodInfo->parameters_count != 3)
        while ((m = get_methods(stretchableObstacleClass, &iter)) != nullptr)
        {
            if (std::strncmp(m->name, "SetSize", 7) == 0)
            {
                          log("Stretchable Obstacle Method: %s", m->name);
                SetSizeMethodInfo = m;
                //       log("SetSizeMethod: %i", SetSizeMethodInfo->parameters_count);
                break;
            }
        }

    dlclose(imagehandle);
*/
    std::map<int, int> extendedLanesMap;
    //    std::map<int, int> extendedLayersMap;
    int num = -1;
    for (int i = 0; i < noteSaveData->size; ++i)
    {
        num++;
        auto item = noteSaveData->items->values[i];
        if (noteSaveData->items->values[i]->lineIndex > 3)
        {
     //       log("Putting id: %i in map.", num);
            extendedLanesMap[num] = noteSaveData->items->values[i]->lineIndex;
            noteSaveData->items->values[i]->lineIndex = 3;
        }

        if (noteSaveData->items->values[i]->lineIndex < 0)
        {
  //          log("Putting id: %i in map.", num);
            extendedLanesMap[num] = noteSaveData->items->values[i]->lineIndex;
            noteSaveData->items->values[i]->lineIndex = 0;
        }
        /* 
        if (noteSaveData->items->values[i]->lineLayer < 0 || noteSaveData->items->values[i]->lineLayer > 2)
        {
            log("Storing layer %i for id %i", noteSaveData->items->values[i]->lineLayer, num);
            extendedLayersMap[num] = noteSaveData->items->values[i]->lineLayer;
        }
        */
    }

    for (int i = 0; i < obstaclesSaveData->size; ++i)
    {
        num++;
        if (obstaclesSaveData->items->values[i]->lineIndex > 3)
        {
 //           log("Putting id: %i in map", num);
            extendedLanesMap[num] = obstaclesSaveData->items->values[i]->lineIndex;
            obstaclesSaveData->items->values[i]->lineIndex = 3;
        }

        if (obstaclesSaveData->items->values[i]->lineIndex < 0)
        {
      //      log("Putting id: %i in map", num);
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

    //            log("Found id: %i in map.", id);
                result->beatmapLinesData->values[i]->beatmapObjectData->values[j]->lineIndex = extendedLanesMap[id];
            }
        }
    }
    if (extendedLanesMap.size() > 0)
    {
        log("Attempting to correct extended lanes not flipping");
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

    log("get_beatmap_data_from_savedata Hook finished");
    return result;
}

__attribute__((constructor)) void lib_main()
{

    log("Installing Mapping Extensions Hooks!");
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
    log("Installed  Mapping Extensions Hooks!");
}
