/*
BuzzWire Project
Copyright (C) 2012  Paul Thompson (paul.r.thompson@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "score_keeper.h"

#include "common/timers.h"

#define PENALTY_TIME 500
#define NUMBER_OF_RECORDS_TO_RETAIN 10

typedef struct
{
    uint32_t record;
    bool valid;
} record_t;

static record_t top_running[NUMBER_OF_RECORDS_TO_RETAIN];
static record_t top_penalty[NUMBER_OF_RECORDS_TO_RETAIN];
static record_t top_total[NUMBER_OF_RECORDS_TO_RETAIN];

static stopwatch_t sw;
static score_t score;

/**
 * @brief updates the record list
 *
 * If the given record is lower than any of the existing records, it is
 * inserted in the list.  If it is equal, then it will be replicated,
 * the record list holds no duplicates.
 *
 * If the record list has not been completely populated, and the record
 * does not beat any of the valid records, it is put at the end of the
 * list
 *
 * @param recordList Pointer to array of records to update
 * @param record The record to attempt to insert
 */
static void UpdateRecord(record_t *recordList, uint32_t record)
{
    uint8_t i;
    uint8_t j;

    for (i = 0; i < NUMBER_OF_RECORDS_TO_RETAIN; i++)
    {
        /* if the record isn't valid, then just set it */
        if (!recordList[i].valid)
        {
            recordList[i].valid = true;
            recordList[i].record = record;
        }
        else if (recordList[i].record >= record)
        {
            /* if it's a tie, don't duplicate it */
            uint8_t firstRecordToShift = (recordList[i].record == record ? i + 1 : i);

            /* Shift the other records down */
            for (j = NUMBER_OF_RECORDS_TO_RETAIN - 2; j >= firstRecordToShift; j--)
            {
                recordList[j + 1] = recordList[j];
            }

            recordList[i].record = record;
            break;
        }
    }
}

/**
 * @brief Get the ranking of the given record within the record list
 *
 * An exact match is required
 *
 * @param recordList Pointer to array of records to update
 * @param record The record to find the ranking for
 *
 * @return ranking if is in records, -1 otherwise
 */
static int GetRank(record_t *recordList, uint32_t record)
{
    int rank = -1;
    uint8_t i;

    for (i = 0; i < NUMBER_OF_RECORDS_TO_RETAIN; i++)
    {
        if (recordList[i].valid && (recordList[i].record == record))
        {
            /* Use one-indexed, this is for public consumption */
            rank = i + 1;
        }
    }

    return rank;
}

void ScoreKeeper_Initialize(void)
{
    uint8_t i;

    score.valid = false;

    for (i = 0; i < NUMBER_OF_RECORDS_TO_RETAIN; i++)
    {
        top_running[i].valid = false;
        top_penalty[i].valid = false;
        top_total[i].valid = false;
    }
}

void ScoreKeeper_Start(void)
{
    Stopwatch_Initialize(&sw);
    score.penalties = 0;
    score.runningTime = 0;
    score.totalTime = 0;
    score.valid = true;
}

void ScoreKeeper_Penalty(void)
{
    score.penalties++;
}

void ScoreKeeper_End(void)
{
    Stopwatch_Stop(&sw);

    /* Get a local copy so that the total time is accurate */
    const score_t local = ScoreKeeper_GetScore();

    /* Update the records */
    UpdateRecord(top_penalty, local.penalties);

    /* The times are truncated to the nearest tenth of a second */
    UpdateRecord(top_running, (local.runningTime - (local.runningTime % 100)));
    UpdateRecord(top_total, (local.totalTime - (local.totalTime % 100)));
}

const score_t ScoreKeeper_GetScore(void)
{
    score.runningTime = Stopwatch_GetElapsed(&sw);
    score.totalTime = score.runningTime + score.penalties * PENALTY_TIME;

    return score;
}

const score_t ScoreKeeper_GetLastScore(void)
{
    return score;
}

const score_t ScoreKeeper_GetBestRunningTime(void)
{
    return score;
}

const score_t ScoreKeeper_GetLowestPenalties(void)
{
    return score;
}

const score_t ScoreKeeper_GetBestTime(void)
{
    return score;
}

const int8_t ScoreKeeper_GetRunningTimeRank(void)
{
    int8_t ranking = -1;

    if (score.valid)
    {
        /* Truncate to tenths of a second */
        ranking = GetRank(top_running, (score.runningTime - (score.runningTime % 100)));
    }

    return ranking;
}

const int8_t ScoreKeeper_GetTotalTimeRank(void)
{
    int8_t ranking = -1;

    if (score.valid)
    {
        /* Truncate to tenths of a second */
        ranking = GetRank(top_total, (score.totalTime - (score.totalTime)));
    }

    return ranking;
}

const int8_t ScoreKeeper_GetPenaltyRank(void)
{
    int8_t ranking = -1;

    if (score.valid)
    {
        ranking = GetRank(top_penalty, score.penalties);
    }

    return ranking;
}

