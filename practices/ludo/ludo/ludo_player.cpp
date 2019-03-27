#include "ludo_player.h"

#include <algorithm>

ludo_player::ludo_player() :
    rd(),
    gen(rd()),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    std::cout << "Constructing player" << std::endl;
    auto states = pow(2, STATE_VECTOR_SIZE);
    for (auto i = 0; i < states; i++) { // 2^parameters aka statespace
        std::vector<float> vec;
        for (auto k = 0; k < 4; k++) { // 4 actions
            vec.push_back(0);//(float)(rand()%1000)/1000.0f);
        }
        QMatrix.push_back(vec);
    }
    initStateVector();
}

int ludo_player::make_decision(){
    for (int i = 1; i < 5; i++) {
        calcPieceState(i, pos_start_of_turn);
    }
    savedAction = -1;
    wasRandomAction = 0;
    float exploration_activations = (float)(rand()%1000)/1000;
    //std::cout << exploration_activations << std::endl;
    if(exploration_activations < EXPLORE){
        wasRandomAction = 1;
        savedAction = doRandomMove();
        decisions.push_back(wasRandomAction);
        return savedAction;
    }
    int action = findQMaxAction(writeStateString(stateVector));
    if(action != -100){
        savedAction = action;
        //std::cout << "did action" << std::endl;
        decisions.push_back(wasRandomAction);
        return action;
    }
    //decide for random move
    wasRandomAction = 1;
    savedAction = doRandomMove();
    decisions.push_back(wasRandomAction);
    return savedAction;

}

int ludo_player::doRandomMove(){
    std::vector<int> valid_moves;
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                valid_moves.push_back(i);
            }
        }
    }
    for(int i = 0; i < 4; ++i){
        if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
            valid_moves.push_back(i);
        }
    }

    if(valid_moves.size() > 0){
        std::uniform_int_distribution<> piece(0, valid_moves.size()-1);
        int select = piece(gen);
        return valid_moves[select];
    }
    else{
        return -1;
    }
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

int ludo_player::findQMaxAction(std::string state){
    int action = 0;
    float max = -1000;

    int index = convertStringToIndex(state);
    for (int i = 0; i < 4; i++) {
        if(QMatrix[index][i] > max && stateVector[i * (STATE_VECTOR_SIZE/4)] == 1){
            max = QMatrix[index][i];
            action = i;
        }
    }
    if (max == -1000 || max == 0)
        return -100;

    return action;
}

int ludo_player::convertStringToIndex(std::string state){
    int i = std::stoi(state, nullptr,2);
    return i;
}

std::string ludo_player::writeStateString(std::vector<int> stateVector){
    std::string state;

    for (int i = 0; i < stateVector.size(); i++) {
        if(stateVector[i] == -1 )
            state += "0";
        else if(stateVector[i] > 0 )
            state += "1";
        else
            state += std::to_string(stateVector[i]);
    }
    return state;
}

void ludo_player::initStateVector(){
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        stateVector.push_back(0);
    }
}

void ludo_player::printStateVector(){

    /*for(std::vector<int>::size_type i = 0; i < STATE_VECTOR_SIZE; i++ )
    {
        if(!(i % (STATE_VECTOR_SIZE/4)))
          std::cout << "Placement: ";
        if(!((i+3)%(STATE_VECTOR_SIZE/4)))
          std::cout << " Enemies: ";

        std::cout << stateVector[i] << "; ";
    }
    std::cout << std::endl;*/

    std::string state;
    state = writeStateString(stateVector);
    std::cout << state << " state index: " << convertStringToIndex(state) << std::endl;

    /*std::cout << "pos vector: " << std::endl;
    for(std::vector<int>::size_type i = 0; i < pos_start_of_turn.size(); i++ )
    {
      std::cout << pos_start_of_turn[i] << ' ';
    }
    std::cout << std::endl;*/
}


void ludo_player::printMatrix(){
    std::cout << "Q Matrix: " << QMatrix.size() << " x " << QMatrix[0].size() << std::endl;
    int printedElements = 0;
    for(std::vector<std::vector<int>>::size_type i = 0; i < QMatrix.size(); i++ )
    {
       for(std::vector<int>::size_type j = 0; j < QMatrix[i].size(); j++ )
       {
          if(QMatrix[i][j] > 1.0f){
            //std::cout << QMatrix[i][j] << " , "; //" i " << i << " j " << j << "; ";
            printedElements++;
          }
       }
       //std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Printed elements: " << printedElements << std::endl;
}

void ludo_player::calcPieceState(int i, std::vector<int>& state){
    int offset = (STATE_VECTOR_SIZE/4)*i;

    // has it reached goal
    //stateVector[offset-5] = reachedGoal(i-1, state);

    // Score state (how close to the goal)
    //stateVector[offset-4] = state[i-1];

    // can kill other player
    stateVector[offset-5] = killPlayer(i-1, state);

    // Danger score (is a piece in danger)
    stateVector[offset-4] = isDanger(i-1, state);

    // Piece can move to goal
    stateVector[offset-3] = moveGoal(i-1, state);

    // piece is safe
    stateVector[offset-2] = isSafe(i-1, state);

    // Is valid movable piece
    stateVector[offset-1] = isValidMove(i-1, state);
}

int ludo_player::killPlayer(int piece, std::vector<int> &state){
    int id;
    int enemies = 0;
    if (state[piece] > 50)
        return 0;
    for (int i = 4; i < state.size(); i++) {
        if(state[i] < 0 || state[i] > 50)
            continue;

        if((state[i] - 8) % 13 == 0) // non colored globes
            continue;

        if(state[i] % 13 == 0 )
            continue;

        if(state[i] == 0) // home globe
            enemies++;

        for (int k = 0; k < 4; k++) { // a pair is safe
            if(state[i] == state[k] && k != i)
                continue;
        }

        if (state[piece] != -1) {
            id = ((state[i] - state[piece]) % 52 + 52) % 52;
            if(id < 7){
                //std::cout <<"Piece: "<< piece << " is in danger from enemy: " << i << " with range: " << id <<std::endl;
                enemies++;
            }
        }
    }
    if(enemies > 0)
        return enemies;

    return 0;
}

int ludo_player::isValidMove(int piece, std::vector<int> &state){
    if(dice_roll == 6)
        if(state[piece]<0)
            return 1;

    if(state[piece]>=0 && state[piece] != 99)
        return 1;

    return 0;
}

int ludo_player::reachedGoal(int piece, std::vector<int>& state){
    if(pos_start_of_turn[piece] == 99)
        return 1;
    return 0;
}

int ludo_player::isDanger(int piece, std::vector<int>& state){
    int id;
    int enemies = 0;
    if (state[piece] < 0 || state[piece] > 50)
        return 0;
    for (int i = 4; i < state.size(); i++) {
        if(state[i] < 0 || state[i] > 50)
            continue;

        id = ((state[piece] - state[i]) % 52 + 52) % 52;
        if(id < 7){
            //std::cout <<"Piece: "<< piece << " is in danger from enemy: " << i << " with range: " << id <<std::endl;
            enemies++;
        }
    }
    if(enemies > 0)
        return enemies;
    return 0;
}

int ludo_player::moveGoal(int piece, std::vector<int>& state){
    if((state[piece] + dice_roll) == 56 || (state[piece] + dice_roll) == 50)
        return 1;
    else {
        return 0;
    }
}

int ludo_player::isSafe(int piece, std::vector<int>& state){
    if((state[piece]) < 51 && (state[piece]) > -1)
    {
        if((state[piece] - 8) % 13 == 0) // non colored globes
            return 1;
        if(state[piece] == 0) // home globe
            return 1;
        if(state[piece] != -1)
            for (int i = 0; i < 4; i++) { // a pair is safe
                if(state[piece] == state[i] && piece != i)
                    return 1;
            }
    }

    return 0;
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
    //printStateVector();
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

    int reward = 0;
    // if we won, recieve reward
    if(game_complete)
        reward = 100;

    // save previous state
    oldStateVector = stateVector;

    // calculate Q value

    // s_t - index from the old state
    int indexQState = convertStringToIndex(writeStateString(oldStateVector));
    // a_t - index from the old action we chose
    int indexAction = savedAction;

    for (int i = 1; i < 5; i++) {
        calcPieceState(i, pos_end_of_turn); // recalc the new state
    }

    // s_t+1
    int indexQStateNew = convertStringToIndex(writeStateString(stateVector));
    // a_t+1
    int action = findQMaxAction(writeStateString(stateVector));
    if(action != -100)
        savedAction = action;
    else {
        savedAction = doRandomMove();
    }
    int indexActionNew = savedAction;

    QMatrix[indexQState][indexAction] = QMatrix[indexQState][indexAction] +
            LEARN * (reward + DISCOUNT *(QMatrix[indexQStateNew][indexActionNew]- QMatrix[indexQState][indexAction]));

    //std::cout <<  "Old QMatrix index: "  << indexQState << " indexAction "<< savedAction << std::endl;
    //std::cout <<  "New QMatrix index: "  << indexQStateNew << " indexAction "<< indexActionNew << std::endl;
    //if(game_complete)
    //    printMatrix();

    emit turn_complete(game_complete);
}
