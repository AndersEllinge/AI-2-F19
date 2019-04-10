#ifndef LUDO_PLAYER_GENETIC_H
#define LUDO_PLAYER_GENETIC_H
#include <QObject>
#include <random>
#include "positions_and_dice.h"
#include <bitset>
#include <string.h>
#include <iostream>

class ludo_player_genetic : public QObject {
    Q_OBJECT
private:
    std::random_device rd;
    std::mt19937 gen;
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    std::vector<std::vector<std::bitset<32>>> population;
    int dice_roll;
    int make_decision();
    std::vector<std::bitset<32>> createRandomChromosome();
    std::bitset<32> createRandomGene();
public:
    ludo_player_genetic();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_GENETIC_H
