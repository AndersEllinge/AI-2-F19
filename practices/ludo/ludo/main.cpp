#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"

Q_DECLARE_METATYPE( positions_and_dice )

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();

    //instanciate the players here
    ludo_player p4; //green (p1), yellow (p2)
    ludo_player_random p1, p2, p3; //blue (p3), red (p4)
    //XXXXXX your player e.g., p4 xxxxxxx//

    game g;
    g.setGameDelay(0); //if you want to see the game, set a delay

    //* Add a GUI <-- remove the '/' to uncomment block
    Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner(int)));
    QObject::connect(&g,SIGNAL(close()),                          &a,SLOT(quit()));
    w.show();

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

    for (int var = 0; var < 500; var++) {
        g.start();
        a.exec();
        g.reset();
        std::cout << "Game: " << var << std::endl;
       /* std::cout << "Win ratio: Green " <<
             (float)std::count(g.winList.begin(), g.winList.end(),0)/g.winList.size() <<
             " Yellow " << (float)std::count(g.winList.begin(), g.winList.end(),1)/g.winList.size() <<
             " Blue " << (float)std::count(g.winList.begin(), g.winList.end(),2)/g.winList.size() <<
             " Red " << (float)std::count(g.winList.begin(), g.winList.end(),3)/g.winList.size() <<
             std::endl;*/
        p4.printMatrix();
    }
    p4.printMatrix();
    std::cout << "% of random actions" << (float)std::count(p4.decisions.begin(), p4.decisions.end(),1)/p4.decisions.size() << std::endl;
    return a.exec();
}
