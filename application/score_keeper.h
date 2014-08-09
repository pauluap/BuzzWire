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

#ifndef __BUZZWIRE_SCORE_KEEPER_H__
#define __BUZZWIRE_SCORE_KEEPER_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t runningTime;
    uint32_t penalties;
    uint32_t totalTime;
    bool valid;
} score_t;

/**
 * @brief Sets up the scoring system
 */
void ScoreKeeper_Initialize(void);

/**
 * @brief Starts a game
 */
void ScoreKeeper_Start(void);

/**
 * @brief Count a penalty
 */
void ScoreKeeper_Penalty(void);

/**
 * @brief Ends a game
 */
void ScoreKeeper_End(void);

/**
 * @brief Gets the score for the current game
 *
 * If there is no game running, this obtains the last completed score
 * @return score
 */
const score_t ScoreKeeper_GetScore(void);

/**
 * @brief Gets the score for the last completed game
 *
 * @return score
 */
const score_t ScoreKeeper_GetLastScore(void);

/**
 * @brief Gets the score for the game with the best running time
 *
 * @return score
 */
const score_t ScoreKeeper_GetBestRunningTime(void);

/**
 * @brief Gets the score for the game with the lowest penalties
 *
 * @return score
 */
const score_t ScoreKeeper_GetLowestPenalties(void);

/**
 * @brief Gets the score for the best total time
 *
 * @return score
 */
const score_t ScoreKeeper_GetBestTime(void);

/**
 * @brief Gets the running time ranking of the last game performed
 *
 * @return ranking if ranked or score is valid, else -1
 */
const int8_t ScoreKeeper_GetRunningTimeRank(void);

/**
 * @brief Gets the total time ranking of the last game performed
 *
 * @return ranking if ranked or score is valid, else -1
 */
const int8_t ScoreKeeper_GetTotalTimeRank(void);

/**
 * @brief Gets the penalty ranking of the last game performed
 *
 * @return ranking if ranked or score is valid, else -1
 */
const int8_t ScoreKeeper_GetPenaltyRank(void);

#endif /* __BUZZWIRE_SCORE_KEEPER_H__ */

