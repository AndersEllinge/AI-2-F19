#include "ludo_player.h"
#include <random>

ludo_player::ludo_player(){   
    auto states = pow(2, STATE_VECTOR_SIZE);
    for (auto i = 0; i < states; i++) { // 2^parameters aka statespace
        std::vector<int> vec;
        for (auto k = 0; k < 4; k++) { // 4 actions
            vec.push_back(0);
        }
        QMatrix.push_back(vec);
    }
    initStateVector();
}

int ludo_player::make_decision(){
    for (int i = 1; i < 5; i++) {
            calcPieceState(i);
    }
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
    } else {
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
    }
    return -1;
}

int ludo_player::findQIndex(std::vector<int> vec){
    int index = 0;
    for (int i = 1; i < STATE_VECTOR_SIZE; i++) {
        if(vec[i-1])
            index += pow(index,i)/2;
    }
    return index;
}

void ludo_player::initQMatrix(){
    auto states = pow(2, STATE_VECTOR_SIZE);
    for (auto i = 0; i < states; i++) { // 2^parameters aka statespace
        for (auto k = 0; k < 4; k++) { // 4 actions
            QMatrix[i][k] = 0;
        }
    }
}

void ludo_player::initStateVector(){
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        stateVector[i] = 0;
    }
}

void ludo_player::printStateVector(){
    std::cout << "State vector: " << std::endl;
    for(std::vector<int>::size_type i = 0; i < STATE_VECTOR_SIZE; i++ )
    {
      std::cout << stateVector[i] << ' ';
    }
    std::cout << std::endl;
}


void ludo_player::printMatrix(){
    std::cout << "Q Matrix: " << QMatrix.size() << " x " << QMatrix[0].size() << std::endl;
    for(std::vector<std::vector<int>>::size_type i = 0; i < QMatrix.size(); i++ )
    {
       for(std::vector<int>::size_type j = 0; j < QMatrix[i].size(); j++ )
       {
          std::cout << QMatrix[i][j] << ' ';
       }
       std::cout << std::endl;
    }
}

void ludo_player::calcPieceState(int i){
    int offset = (STATE_VECTOR_SIZE/4)*i;

    // Score state (how close to the goal)
    stateVector[offset-4] = pos_start_of_turn[i-1];

    // Danger score (is a piece in danger)
    stateVector[offset-3] = isDanger(i);

    stateVector[offset-2] = 0;

    stateVector[offset-1] = 0;
}

int ludo_player::isDanger(int piece){
    for (int i = 4; i < pos_start_of_turn.size(); ++i) {
        if(pos_start_of_turn[piece] - pos_start_of_turn[i] < 7 and pos_start_of_turn[piece] - pos_start_of_turn[i] > 0)
            return 1;
        else {
            return 0;
        }
    }

}

void ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;

/*
    std::cout << "Yellow pieces are: ";
    for (int i = 4; i < 8; i++) {
        std::cout << pos_start_of_turn[i] << " ";
    }
    std::cout << std::endl;
*/
    printStateVector();
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    printStateVector();
    emit turn_complete(game_complete);
}
