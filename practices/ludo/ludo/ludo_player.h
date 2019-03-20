#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include <math.h>

#define STATE_VECTOR_SIZE 16


class ludo_player : public QObject {
    Q_OBJECT
private:
    int make_decision();
    int findQIndex(std::vector<int> vec);
    void initQMatrix();
    void initStateVector();
    void printStateVector();
    void printMatrix();
    void calcPieceState(int i);
    int isDanger(int i);

    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    std::vector<int> stateVector{STATE_VECTOR_SIZE}; //state size
    std::vector<std::vector<int>> QMatrix;
    int dice_roll;

public:
    ludo_player();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
