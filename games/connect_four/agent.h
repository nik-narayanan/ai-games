//
// Created by nik on 11/8/2024.
//

#ifndef AIGAMES_AGENT_H
#define AIGAMES_AGENT_H

#include "state.h"

#include "nml/primitives/span.h"
#include "nml/primitives/list.h"

namespace connect_four
{
    class MinimaxAgent
    {
        BoardState& _state;
        uint8_t _column = 0;

        constexpr static uint8_t MAX_DEPTH = 10;

    public:

        explicit MinimaxAgent(BoardState& state)
            : _state(state)
        { }

        uint8_t next_move() noexcept;

    private:

        int32_t principal_variation(int32_t alpha, int32_t beta, uint8_t depth) noexcept;
    };

    uint8_t MinimaxAgent::next_move() noexcept
    {
        uint32_t score = principal_variation(-1e9, 1e9, MAX_DEPTH);

        return _column;
    }

    int32_t MinimaxAgent::principal_variation(int32_t alpha, int32_t beta, uint8_t depth) noexcept
    {
        if (_state.has_winner()) return -(_state.ROWS * _state.COLUMNS + depth - MAX_DEPTH);

        if (_state.is_tie()) return 0;

        if (depth == 0) return _state.score();

        int score = 0; bool solved = false;

        for (uint8_t distance = 0; distance <= _state.COLUMNS / 2; ++distance)
        {
            for (uint8_t column = _state.COLUMNS / 2 - distance; column <= _state.COLUMNS / 2 + distance; column += (distance == 0) ? 1 : 2 * distance)
            {
                if (!_state.can_push(column)) continue;

                _state.push(column);

                if (solved)
                {
                    score = -principal_variation(-alpha - 1, -alpha, depth - 1);

                    if (alpha < score && beta > score)
                    {
                        score = -principal_variation(-beta, -alpha, depth - 1);
                    }
                }
                else
                {
                    score = -principal_variation(-beta, -alpha, depth - 1);
                }

                _state.pop(column);

                if (beta <= score) return beta;

                if (alpha < score)
                {
                    alpha = score, solved = true;

                    if (depth == MAX_DEPTH) _column = column;
                }
            }
        }

        return alpha;
    }
}

#endif //AIGAMES_AGENT_H
