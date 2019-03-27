#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include <math.h>
#include <random>
//#include <bits/stdc++.h>

#define STATE_VECTOR_SIZE 20 //n*2^2 where n is number of state attributes --> 4*2^2 = 16
//#define LEARN 0.7
//#define DISCOUNT 0.9

class ludo_player : public QObject {
    Q_OBJECT
private:
    int make_decision();//action selection
    int doRandomMove();
    int findQIndex(std::vector<int> vec);
    void initQMatrix();
    int findQMaxAction(std::string state);
    int convertStringToIndex(std::string state);
    std::string writeStateString(std::vector<int> stateVector);
    void initStateVector();
    void printStateVector();

    void calcPieceState(int i, std::vector<int>& stateVector);
    int killPlayer(int piece, std::vector<int>& stateVector);
    int isValidMove(int piece, std::vector<int>& stateVector);
    int reachedGoal(int piece, std::vector<int>& stateVector);
    int isDanger(int piece, std::vector<int>& stateVector); // potentially dies here
    int moveGoal(int piece, std::vector<int>& stateVector);
    int isSafe(int piece, std::vector<int>& stateVector); // cant die here

    int wasRandomAction;
    int savedAction;

    std::random_device rd;
    std::mt19937 gen;
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    std::vector<int> stateVector; //state size
    std::vector<int> oldStateVector; // previous state
    std::vector<std::vector<float>> QMatrix;
    int dice_roll;
    float LEARN = 0.7;
    float DISCOUNT = 0.9;
    float EXPLORE = 0.1;

public:
    std::vector<int> decisions;
    ludo_player();
    void printMatrix();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
