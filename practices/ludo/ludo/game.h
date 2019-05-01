#ifndef GAME_H
#define GAME_H

#include <vector>
#include <random>
#include <iostream>
#include <QtCore>
#include <QThread>


#include "positions_and_dice.h"

static int global_color = 5;

class game : public QThread
{
    Q_OBJECT
private:
    int maxGeneration;
    int populationSize;
    int trainingGames;
    bool game_complete;
    bool turn_complete;
    int turn_counter;
    unsigned int game_delay;
    positions_and_dice relative;
    int dice_result;
    std::vector<int> relativePosition();
    int isStar(int index);
    bool isGlobe(int index);
    int isOccupied(int index); //see if it is occupied and return the piece number
    int rel_to_fixed(int relative_piece_index);
    void send_them_home(int index);
    void move_start(int fixed_piece);
    int next_turn(unsigned int delay);
    static void msleep(unsigned long msecs){
        QThread::msleep(msecs);
    }
public:
    bool newGame;
    int color;
    std::vector<int> winList;
    std::vector<int> player_positions;
    void rollDice(){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 6);
        dice_result = dis(gen);
    }
    int getDiceRoll() {return dice_result; }
    game();
    game(int maxGen, int popSize, int trainGames);
    void setGameDelay(unsigned int mili_seconds){ game_delay = mili_seconds; }
    void reset();
signals:
    void player1_start(positions_and_dice);
    void player2_start(positions_and_dice);
    void player3_start(positions_and_dice);
    void player4_start(positions_and_dice);

    void player1_end(std::vector<int>);
    void player2_end(std::vector<int>);
    void player3_end(std::vector<int>);
    void player4_end(std::vector<int>);

    void update_graphics(std::vector<int>);
    void set_color(int);
    void set_dice_result(int);
    void declare_winner(int);
    void close();

public slots:
    void turnComplete(bool win);
    void startNewGame(bool start);
    void movePiece(int relative_piece); //check game rules
protected:
    void run();
};

#endif // GAME_H
