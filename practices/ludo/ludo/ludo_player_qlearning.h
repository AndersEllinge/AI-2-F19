#ifndef LUDO_PLAYER_QLEARNING_H
#define LUDO_PLAYER_QLEARNING_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include <random>
#include <tuple>

/*
    Every piece goes through the SAME QMatrix. The piece with the high Q value gets to do the action.
    If any tiebreakers, then a random choice is taken.

                  MOVE_FROM_HOME | MOVE_GOAL | MOVE_GOAL_ROAD | MOVE_SAFETY | MOVE_STAR | MOVE_KILL | MOVE
                | -----------------------------------------------------------------------------------------
    HOME        |
    GOAL        |
    SAFE        |
    WINNER_ROAD |
    DANGER      |
    KILL        |
    IDLE        |

*/

// States
enum states
{
    HOME,       // 0
    GOAL,       // 1
    SAFE,       // 2
    WINNER_ROAD,// 3
    DANGER,     // 4
    IDLE // always last
};

enum actions
{
    MOVE_FROM_HOME, // 0
    MOVE_GOAL,      // 1
    MOVE_GOAL_ROAD, // 2
    MOVE_SAFETY,    // 3
    MOVE_STAR,      // 4
    MOVE_KILL,      // 5
    MOVE // always last
};

#define PIECE(var) std::get<0>(var)
#define ACTION(var) std::get<1>(var)

#define EXPLORE_RATE 0.1f
#define DISCOUNT_FACTOR 0.1f
#define LEARNING_RATE 0.7f

class ludo_player_QLearning : public QObject {
    Q_OBJECT
private:

    int get_piece_state(int piece, std::vector<int> positions);
    std::vector<int> get_actions(int piece, std::vector<int> positions);
    std::tuple<int,int> do_random_movement(std::vector<std::vector<int>> actionsAndStates);
    std::tuple<int,int> find_Q_Max(std::vector<std::vector<int>> actionsAndStates);


    std::vector<std::vector<float>> QMatrix;
    std::random_device rd;
    std::mt19937 gen;
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
public:
    ludo_player_QLearning();
    void printQMatrix();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};


#endif // LUDO_PLAYER_QLEARNING_H




