//
// Created by nik on 11/7/2024.
//

#ifndef AIGAMES_STATE_H
#define AIGAMES_STATE_H

#include <cstring>
#include <cstdint>

#include "nml/primitives/span.h"
#include "nml/primitives/bitset.h"

using namespace nml;

namespace connect_four
{
    enum class Winner : char
    {
        NONE, PLAYER_ONE, PLAYER_TWO
    };

    enum class SlotState : char
    {
        EMPTY, PLAYER_ONE, PLAYER_TWO
    };

    struct BoardState
    {
        constexpr static uint8_t ROWS = 6, COLUMNS = 7, WIN_LENGTH = 4;
        constexpr static int32_t DIRECTIONS[4] = {ROWS + 1, 1, ROWS, ROWS + 2};

        uint64_t mask;
        uint64_t current_position;

        bool turn_player_one;
        uint16_t moves_played;
        int32_t column_remaining[COLUMNS]{ROWS};

        explicit BoardState() noexcept
            : moves_played(0), turn_player_one(true), mask(0), current_position(0)
        {
            for (int& column : column_remaining) column = ROWS;
        }

        void reset();
        void print() const;

        void pop(uint8_t column) noexcept;
        void push(uint8_t column) noexcept;
        void seed(Span<const uint8_t> moves) noexcept;

        [[nodiscard]] bool is_tie() const noexcept;
        [[nodiscard]] int32_t score() const noexcept;
        [[nodiscard]] bool has_winner() const noexcept;
        [[nodiscard]] bool can_push(uint8_t column) const noexcept;
        [[nodiscard]] uint8_t column_height(uint8_t column) const noexcept;
        [[nodiscard]] SlotState get_slot_state(uint8_t row, uint8_t column) const noexcept;
    };

    void BoardState::push(uint8_t column) noexcept
    {
        uint32_t shift = (column * DIRECTIONS[0]) + ((ROWS - column_remaining[column]) * DIRECTIONS[1]);

        mask |= UINT64_C(1) << shift;
        current_position ^= mask;

        column_remaining[column]--;
        moves_played++;

        turn_player_one = !turn_player_one;
    }

    void BoardState::pop(uint8_t column) noexcept
    {
        uint32_t shift = (column * DIRECTIONS[0]) + ((ROWS - column_remaining[column] - 1) * DIRECTIONS[1]);

        mask ^= (UINT64_C(1) << shift);
        current_position ^= (UINT64_C(1) << shift);
        current_position ^= mask;

        column_remaining[column]++;
        moves_played--;

        turn_player_one = !turn_player_one;
    }

    void BoardState::seed(Span<const uint8_t> moves) noexcept
    {
        for (uint8_t column : moves) push(column);
    }

    SlotState BoardState::get_slot_state(uint8_t row, uint8_t column) const noexcept
    {
        uint8_t offset = column * (ROWS + 1) + row;

        if ((mask & (UINT64_C(1) << offset)) == 0) return SlotState::EMPTY;

        SlotState zero_player = turn_player_one ? SlotState::PLAYER_TWO : SlotState::PLAYER_ONE;
        SlotState one_player = !turn_player_one ? SlotState::PLAYER_TWO : SlotState::PLAYER_ONE;

        if (current_position & (UINT64_C(1) << offset)) return zero_player;

        return one_player;
    }

    uint8_t BoardState::column_height(uint8_t column) const noexcept
    {
        return ROWS - column_remaining[column];
    }

    bool BoardState::is_tie() const noexcept
    {
        return ROWS * COLUMNS == moves_played - 1;
    }

    void BoardState::print() const
    {
        for (uint32_t i = 0; i < ROWS; ++i)
        {
            for (uint32_t j = 0; j < COLUMNS; ++j)
            {
                std::cout << (int)get_slot_state(ROWS - i - 1, j) << ",";
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    int32_t BoardState::score() const noexcept
    {
        int32_t score = 0;
        int32_t connected_sequence_count[2][1 + WIN_LENGTH / 2] = {0};

        for (uint8_t direction = 0, sequence_length = 0; direction < std::size(DIRECTIONS); ++direction, sequence_length = 0)
        {
            uint64_t position_player = current_position;
            uint64_t position_opponent = current_position ^ mask;

            for (uint8_t shift = 1; shift < WIN_LENGTH / 2; ++shift)
            {
                position_player &= position_player >> DIRECTIONS[direction];
                position_opponent &= position_opponent >> DIRECTIONS[direction];
            }

            connected_sequence_count[0][sequence_length] += POP_COUNT(position_player);
            connected_sequence_count[1][sequence_length] += POP_COUNT(position_opponent);

            for (; sequence_length < WIN_LENGTH / 2; ++sequence_length)
            {
                position_player &= position_player >> DIRECTIONS[direction];
                position_opponent &= position_opponent >> DIRECTIONS[direction];

                uint8_t pieceCount = POP_COUNT(position_player);
                uint8_t oppPieceCount = POP_COUNT(position_opponent);

                connected_sequence_count[0][sequence_length] -= pieceCount;
                connected_sequence_count[1][sequence_length] -= oppPieceCount;

                if (sequence_length + 1 <= WIN_LENGTH / 2)
                {
                    connected_sequence_count[0][sequence_length + 1] += pieceCount;
                    connected_sequence_count[1][sequence_length + 1] += oppPieceCount;
                }
            }
        }

        for (uint8_t player = 0; player < 2; ++player)
        {
            int32_t weight = (WIN_LENGTH + 1) << 1, multiplier = player == 0 ? 1 : -1;

            for (uint8_t pair_length = 0; pair_length < WIN_LENGTH / 2; ++pair_length)
            {
                weight >>= 1;

                score += connected_sequence_count[player][pair_length] * weight * multiplier;
            }
        }

        return -score;
    }

    bool BoardState::has_winner() const noexcept
    {
        for (uint32_t direction : DIRECTIONS)
        {
            uint64_t position = current_position;

            for (uint8_t sequence = 1; sequence < WIN_LENGTH; ++sequence)
            {
                position &= position >> direction;
            }

            if (position) return true;
        }

        return false;
    }

    bool BoardState::can_push(uint8_t column) const noexcept
    {
        return column < COLUMNS && column_remaining[column] > 0;
    }

    void BoardState::reset()
    {
        mask = 0, current_position = 0, moves_played = 0;

        for (int& column : column_remaining) column = ROWS;
    }
}

#endif //AIGAMES_STATE_H
