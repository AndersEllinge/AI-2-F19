#ifndef LUDO_PLAYER_GENETIC_H
#define LUDO_PLAYER_GENETIC_H
#include <QObject>
#include <random>
#include "positions_and_dice.h"
#include <bitset>
#include <string.h>
#include <iostream>
#include <stdbool.h>
#include <algorithm>


class ludo_player_genetic : public QObject {
    Q_OBJECT
private:
    std::random_device rd;
    std::mt19937 gen;
    bool evolve;
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    std::vector<float> weights;
    std::vector<std::vector<int>> states;
    int dice_roll;

    int make_decision();
    std::vector<int> getStates(std::size_t piece, std::vector<int> positions);
    std::vector<float> sumWeightsAndStates(std::vector<std::vector<int>> states);
    int isValidMove(int piece);
    int isHome(std::size_t piece, std::vector<int> positions);       // 0 Is the piece in home
    int isGoal(std::size_t piece, std::vector<int> positions);       // 1 Is the piece in the goal zone
    int isSafe(std::size_t piece, std::vector<int> positions);       // 2 Can the piece not be killed?
    int onWinnerRoad(std::size_t piece, std::vector<int> positions); // 3 Is the piece on the winner road
    int inDanger(std::size_t piece, std::vector<int> positions);     // 4 How much danger is the piece in
    int hasPosition(std::size_t piece, std::vector<int> positions);  // 5 What position does the piece have
    int moveFromHome(std::size_t piece, std::vector<int> positions); // 6 The piece can move out from home
    int moveToGoal(std::size_t piece, std::vector<int> positions);   // 7 The piece can move to the goal
    int moveToGoalRoad(std::size_t piece, std::vector<int> positions);//8 The piece can move onto the winner road
    int moveToSafty(std::size_t piece, std::vector<int> positions);  // 9 The piece can move to a safe position
    int moveToStar(std::size_t piece, std::vector<int> positions);   // 10 The piece can move to a star
    int moveStarGoal(std::size_t piece, std::vector<int> positions); // 11 The piece can move to the star at winner road
    int canKill(std::size_t piece, std::vector<int> positions);      // 12 The piece can kill x enemies
    int moveToDanger(std::size_t piece, std::vector<int> positions); // 13 How many pieces can now kill the piece
    int moveToDeath(std::size_t piece, std::vector<int> positions);  // 14 Is the move suicide?
    int plainMove(std::size_t piece, std::vector<int> positions);    // 15 The piece can be moved

public:
    ludo_player_genetic(bool evolveEnable);
    void setWeights(std::vector<float> chromosome);
signals:
    void chromoChanged();
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void newChromosome(std::vector<float> chromo);
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_GENETIC_H
