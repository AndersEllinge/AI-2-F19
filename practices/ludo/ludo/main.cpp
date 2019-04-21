#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_qlearning.h"
#include "ludo_player_random.h"
#include "populationhandler.h"
#include "ludo_player_genetic.h"
#include "positions_and_dice.h"

Q_DECLARE_METATYPE( positions_and_dice )

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();

    //instanciate the players here
    //ludo_player p4; //green (p1), yellow (p2)
    //ludo_player_QLearning p1(true);
    ludo_player_genetic p1(true);
    ludo_player_random p2, p3, p4; //blue (p3), red (p4)
    //XXXXXX your player e.g., p4 xxxxxxx//
    // REMEMBER POPSIZE AND TOURNAMENTSIZE NEEDS TO FIT

    game g;
    g.setGameDelay(0); //if you want to see the game, set a delay

    //* Add a GUI <-- remove the '/' to uncomment block
    Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner(int)));

    w.show();//*/
    QObject::connect(&g,SIGNAL(close()),                          &a,SLOT(quit()));

    //set up for each player
    QObject::connect(&g, SIGNAL(player1_start(positions_and_dice)),&p1,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p1,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player1_end(std::vector<int>)),    &p1,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p1,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player2_start(positions_and_dice)),&p2,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p2,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player2_end(std::vector<int>)),    &p2,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p2,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player3_start(positions_and_dice)),&p3,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p3,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player3_end(std::vector<int>)),    &p3,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p3,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player4_start(positions_and_dice)),&p4,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p4,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player4_end(std::vector<int>)),    &p4,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p4,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    //ph.loadPopulation();
    //ph.updatePopulation();
    //p1.setWeights(ph.getChromosomeGenes(std::size_t(0)));
    //ph.savePopulation();
    /*for (std::size_t i = 0; i < ph.populationSize; i++) {

        std::vector<float> test = ph.getChromosomeGenes(i);
        std::cout << "chomo: " << i << std::endl;
        for (std::size_t k = 0; k < test.size(); k++) {
            std::cout << k << " : " << test[k] << std::endl;
        }
        std::cout << std::endl;
    }*/
    int populationSize = 20;
    int tournamentSize = 4;
    int numberOfGenes = 16;
    int maxGeneration = 100;
    populationHandler ph(populationSize,tournamentSize,numberOfGenes,maxGeneration); // pop,tournSize,nrGene,maxGen

    for (std::size_t i = 0; i < maxGeneration-1; i++) { // gen
        for (std::size_t k = 0; k < populationSize-1; k++) { // chromo
            p1.setWeights(ph.getChromosomeGenes(k));
            for (std::size_t game = 0; game < 99; game++) { // 100 games pr. training session
                //std::cout << "Gen: " << i << " chomo: " << k << " Game: " << game << std::endl;
                g.start();
                a.exec();
                if(std::count(g.winList.begin(), g.winList.end(),0))
                    ph.addWin(k);
                ph.addGame(k);
                g.reset();
            }
            std::cout << "Gen: " << i << " chomo: " << k << std::endl;
            std::cout << "This chromo won: " << float(ph.population[k].wins) /  float (ph.population[k].games) << std::endl;
        }
        ph.updatePopulation();
        ph.savePopulation();
    }



    /*std::cout << "Win ratio:"
              " Green " <<(float)std::count(g.winList.begin(), g.winList.end(),0)/g.winList.size() <<
              " Yellow " << (float)std::count(g.winList.begin(), g.winList.end(),1)/g.winList.size() <<
              " Blue " << (float)std::count(g.winList.begin(), g.winList.end(),2)/g.winList.size() <<
              " Red " << (float)std::count(g.winList.begin(), g.winList.end(),3)/g.winList.size() <<
              std::endl;*/
    return a.exec();
}
