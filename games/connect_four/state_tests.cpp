//
// Created by nik on 11/7/2024.
//

#include <gtest/gtest.h>

#include "state.h"
#include "agent.h"

using namespace connect_four;

//TEST(connect_four, push_pop)
//{
//    auto bs = BoardState();
//
//    bs.seed({0,0,6,6,3,4,3,2,3,1,2,1,1,4,2});
//
//    auto starting_mask = bs.mask;
//    auto starting_position = bs.current_position;
//
//    for (uint32_t column = 0; column < bs.COLUMNS; ++column)
//    {
//        bs.push(column);
//
//        ASSERT_NE(starting_mask, bs.mask);
//        ASSERT_NE(starting_position, bs.current_position);
//
//        bs.pop(column);
//
//        ASSERT_EQ(starting_mask, bs.mask);
//        ASSERT_EQ(starting_position, bs.current_position);
//    }
//}
//
//TEST(connect_four, next_move_prevent_loss)
//{
//    auto bs = BoardState();
//
//    auto agent = MinimaxAgent(bs);
//
//    bs.seed({0,6,6,4,3,2,3,1,2,1,1,4,2});
//
//    auto next_move = agent.next_move();
//
////    ASSERT_TRUE(next_move == 3);
//}
//
//TEST(connect_four, next_move_prevent_one_loss)
//{
//    auto bs = BoardState();
//
//    auto agent = MinimaxAgent(bs);
//
//    bs.seed({0,0,6,6,3,4,3,2,3,1,2,1,1,4,2});
//
//    auto next_move = agent.next_move();
//
////    ASSERT_TRUE(next_move == 3 || next_move == 4);
//}

TEST(connect_four, next_move_prevent_one_lossdfsdasds)
{
    auto bs = BoardState();
    auto agent = MinimaxAgent(bs);

    bs.seed({3, 2, 3, 3, 2});

//    bs.print();

    bs.push(agent.next_move());

    bs.print();
//    bs.seed({3, 2, 3, 3, 1});
//
//    auto next_move = agent.next_move();

//    ASSERT_TRUE(next_move == 3 || next_move == 4);
}
