//
// Created by nik on 10/23/2024.
//

#ifndef AIGAMES__GAME_H
#define AIGAMES__GAME_H

#include <chrono>
#include "raylib.h"

#include "nml/primitives/span.h"

#include "connect_four/agent.h"
#include "connect_four/render.h"

using namespace nml;

namespace connect_four
{
    class ConnectFour
    {
        Header _header;
        GameBoard _board;
        RenderState _state;
        WinnerDisplay _winner;

    public:
        explicit ConnectFour() noexcept;

        void draw_frame() noexcept;
        void seed(Span<const uint8_t> moves) noexcept;

        RenderState& get_state() noexcept { return _state; };
    };

    inline static void play() noexcept
    {
        auto game = ConnectFour();

        RenderState& state = game.get_state();

        SetTraceLogLevel(LOG_WARNING);

        SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

        InitWindow(800, 600, "Connect Four");

        SetTargetFPS(state.fps);

        while (!WindowShouldClose())
        {
            BeginDrawing();

            game.draw_frame();

            EndDrawing();
        }

        CloseWindow();
    }

    ConnectFour::ConnectFour() noexcept
        : _state()
        , _board(_state), _header(_state), _winner(_state)
    { }

    void ConnectFour::seed(Span<const uint8_t> moves) noexcept
    {
        _state.board.seed(moves);
    }

    void ConnectFour::draw_frame() noexcept
    {
        _state.is_hovering = false;
        _state.is_left_click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        _state.window_width = GetScreenWidth();
        _state.window_height = GetScreenHeight();
        _state.base_font_size = _state.window_width / 50;

        _state.mouse_location = GetMousePosition();

        ClearBackground(_state.colors.background);

        _board.draw();
        _header.draw();
        _winner.draw();

        _state.frame++;

        if (!_state.is_hovering) SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        else SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
}


#endif //AIGAMES__GAME_H
