//
// Created by nik on 11/7/2024.
//

#ifndef AIGAMES_RENDER_H
#define AIGAMES_RENDER_H

#include "state.h"

#include "raylib.h"
#include "nml/external/date.h"

namespace connect_four
{
    typedef date::sys_time<std::chrono::nanoseconds> DateTime;
    typedef DateTime::clock Clock;

    struct ColorsScheme
    {
        Color text{255, 255, 255, 255};
        Color board{115, 95, 208, 255};
        Color background{26, 7, 89, 255};
        Color player_one{255, 204, 2, 255};
        Color player_two{215, 19, 43, 255};
        Color winner_background{0, 0, 0, 255};
    };

    struct Score
    {
        uint32_t player_one = 0;
        uint32_t player_two = 0;
    };

    struct RenderState
    {
        ColorsScheme colors{};

        int32_t fps = 30;
        uint64_t frame = 0;
        DateTime start_time = Clock::now();

        int32_t window_width = 0;
        int32_t window_height = 0;
        int32_t base_font_size = 0;

        Vector2 mouse_location{};

        bool is_hovering = false;
        bool is_left_click = false;

        Score score;
        BoardState board;
        MinimaxAgent agent;

        uint64_t win_frame = 0;
        Winner winner = Winner::NONE;

        explicit RenderState() noexcept
            : score(), board(), agent(board)
        { }

        void board_reset();
        void winner_declare(Winner winner);
        void emplace(uint8_t column) noexcept;
        [[nodiscard]] bool is_playing() const { return winner == Winner::NONE; }
    };

    void RenderState::winner_declare(Winner winner_game)
    {
        if (winner_game == Winner::NONE) return;

        win_frame = frame;
        winner = winner_game;

        score.player_one += winner_game == Winner::PLAYER_ONE;
        score.player_two += winner_game == Winner::PLAYER_TWO;
    }

    void RenderState::board_reset()
    {
        board.reset();
        winner = Winner::NONE;

        if (!board.turn_player_one)
        {
            board.push(agent.next_move());
        }
    }

    void RenderState::emplace(uint8_t column) noexcept
    {
        if (winner == Winner::NONE && !board.can_push(column)) return;

        board.push(column);

        if (board.has_winner())
        {
            winner_declare(board.turn_player_one ? Winner::PLAYER_TWO : Winner::PLAYER_ONE);
        }
        else
        {
            board.push(agent.next_move());

            if (board.has_winner())
            {
                winner_declare(board.turn_player_one ? Winner::PLAYER_TWO : Winner::PLAYER_ONE);
            }
        }
    }

    class WinnerDisplay
    {
        RenderState& _state;

    public:

        explicit WinnerDisplay(RenderState& state)
            : _state(state)
        { }

        void draw() noexcept
        {
            if (_state.winner == Winner::NONE) return;

            if (!_state.is_playing() && _state.win_frame + _state.fps < _state.frame)
            {
                if (_state.is_left_click) _state.board_reset(); return;
            }

            Color circle_color = _state.winner == Winner::PLAYER_ONE ?
                                 _state.colors.player_one : _state.colors.player_two;

            float font_size = _state.base_font_size * 3;
            float circle_radius = font_size * 0.75;

            Rectangle background =
            {
                .x = _state.window_width * 0.1f,
                .y = _state.window_height * 0.35f,
                .width = _state.window_width * 0.8f,
                .height = _state.window_height * 0.3f
            };

            float circle_padding = circle_radius + font_size;
            Vector2 left_circle_position = { background.x + circle_padding, background.y + background.height / 2};
            Vector2 right_circle_position = { background.x + background.width - circle_padding, background.y + background.height / 2};

            DrawRectangleRec(background, _state.colors.winner_background);

            DrawCircleV(left_circle_position, circle_radius, circle_color);

            const char* text = "WINNER";
            int text_width = MeasureText(text, font_size);

            float text_x = background.x + (background.width - text_width) / 2;
            float text_y = background.y + (background.height - font_size) / 2;

            DrawText(text, text_x, text_y, font_size, _state.colors.text);

            DrawCircleV(right_circle_position, circle_radius, circle_color);
        }
    };

    class Header
    {
        RenderState& _state;

    public:

        explicit Header(RenderState& state)
            : _state(state)
        { }

        void draw() noexcept
        {
            float left = _state.window_width * 0.025f, top = _state.window_height * 0.025f;

            DrawText("Score: ", left, top, _state.base_font_size, _state.colors.text);

            float circle_radius = _state.base_font_size / 2;

            Vector2 top_circle_position = { left * 1.5f, top + 2.0f * _state.base_font_size};
            Vector2 bottom_circle_position = { left * 1.5f, top + 3.25f * _state.base_font_size};

            int score_font_size = _state.base_font_size * 0.8;

            DrawCircleV(top_circle_position, circle_radius, _state.colors.player_one);
            DrawCircleV(bottom_circle_position, circle_radius, _state.colors.player_two);

            DrawText(TextFormat("%d", _state.score.player_one), top_circle_position.x + circle_radius * 1.5, top_circle_position.y - score_font_size / 2.15, score_font_size, _state.colors.text);
            DrawText(TextFormat("%d", _state.score.player_two), bottom_circle_position.x + circle_radius * 1.5, bottom_circle_position.y - score_font_size / 2.15, score_font_size, _state.colors.text);
        }
    };

    class GameBoard
    {
        RenderState& _state;

        struct BoardLocation { uint8_t row; uint8_t column; };

    public:

        explicit GameBoard(RenderState& state)
            : _state(state)
        { }

        void draw() noexcept
        {
            auto& bs = _state.board;

            Rectangle board =
            {
                _state.window_width * 0.15f,
                _state.window_height * 0.15f,
                _state.window_width * 0.7f,
                _state.window_height * 0.7f
            };

            float block_width = board.width / bs.COLUMNS, block_height = board.height / bs.ROWS;

            float circle_radius = 0.4f * std::min(block_width, block_height);

            DrawRectangleRec(board, _state.colors.board);

            for (uint32_t column_offset = 0; column_offset < bs.COLUMNS; ++column_offset)
            {
                for (uint32_t row_offset = 0; row_offset < bs.ROWS; ++row_offset)
                {
                    auto slot_state = bs.get_slot_state(bs.ROWS - row_offset - 1, column_offset);

                    Vector2 center =
                    {
                        board.x + block_width * (0.5f + column_offset),
                        board.y + block_height * (0.5f + row_offset)
                    };

                    DrawCircleV
                    (
                        center
                        , circle_radius
                        , slot_state == SlotState::EMPTY ? _state.colors.background :
                          slot_state == SlotState::PLAYER_ONE ? _state.colors.player_one : _state.colors.player_two
                    );

                    Rectangle block =
                    {
                        board.x + block_width * column_offset,
                        board.y,
                        block_width,
                        board.height
                    };

                    if
                    (
                        _state.is_playing()
                        && _state.board.column_height(column_offset) == bs.ROWS - row_offset - 1
                        && CheckCollisionPointRec(_state.mouse_location, block)
                    )
                    {
                        _state.is_hovering = true;

                        auto color = _state.board.turn_player_one ? _state.colors.player_one : _state.colors.player_two;

                        color.a = 100;

                        DrawCircleV(center, circle_radius, color);

                        if (_state.is_left_click) _state.emplace(column_offset);
                    }
                }
            }
        }
    };
}

#endif //AIGAMES_RENDER_H
