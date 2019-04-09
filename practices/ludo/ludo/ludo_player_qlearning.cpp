#include "ludo_player_qlearning.h"

ludo_player_QLearning::ludo_player_QLearning() :
    rd(),
    gen(rd()),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    std::cout << "Constructing player" << std::endl;

    for (auto i = 0; i < IDLE_4+1; i++) {
        std::vector<float> vec;
        for (auto k = 0; k < MOVE+1; k++) {
            vec.push_back((float)(rand()%1000)/1000.0f);
        }
        QMatrix.push_back(vec);
    }
}

ludo_player_QLearning::ludo_player_QLearning(bool loadQMatrix) :
    rd(),
    gen(rd()),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    std::cout << "Constructing player with loaded QMatrix" << std::endl;

    std::ifstream f("QMatrix", std::ios::in);
    float value;

    for (auto i = 0; i < IDLE_4+1; i++) {
        std::vector<float> vec;
        for (auto k = 0; k < MOVE+1; k++) {
            f >> value;
            vec.push_back(value);
        }
        QMatrix.push_back(vec);
    }
    f.close();
}


int ludo_player_QLearning::make_decision(){
    // For every piece
    std::vector<int> allCurrentStates;
    std::vector<std::vector<int>> actionsAndStates; // action, action, action, action, states
    for (int i = 0; i < 4; i++) {
        // Get the state of the piece
        int currentState = get_piece_state(i, pos_start_of_turn);
        allCurrentStates.push_back(currentState);
        //std::cout << "Piece: " << i << " is state: " << currentState << std::endl;
        // Get the actions the piece can do
        std::vector<int> actions = get_actions(i, pos_start_of_turn);
        actionsAndStates.push_back(actions);
    }
    actionsAndStates.push_back(allCurrentStates);

    // If we cant do any actions
    for (int i = 0; i < 4; ++i){
        if(actionsAndStates[i].size() != 0){
            break;
        }
        else if (i == 3){
            //std::cout << "No actions "  <<  std::endl;
            return -1;
        }

    }

    std::tuple<int,int> bestAction;

    // Do we explore?
    float exploration_activations = (float)(rand()%1000)/1000;
    if(exploration_activations < EXPLORE_RATE){
        bestAction = do_random_movement(actionsAndStates);
    }
    else {
        bestAction = find_Q_Max(actionsAndStates);
    }

    //std::cout << "Piece: " << PIECE(bestAction) << " State: " << allCurrentStates[PIECE(bestAction)] <<" Action: " << ACTION(bestAction) << " Dice: " << dice_roll <<std::endl;

    // update the state so we can do QMatrix stuff
    std::vector<int> statePrediction = pos_start_of_turn;
    if(ACTION(bestAction) == 0){
        statePrediction[PIECE(bestAction)] = 0;
    }
    else {
        statePrediction[PIECE(bestAction)] = statePrediction[PIECE(bestAction)]+dice_roll;
    }
    std::vector<int> newCurrentStates;
    std::vector<std::vector<int>> newActionsAndStates;
    int newState;
    for (int i = 0; i < 4; i++) {
        newState = get_piece_state(i, statePrediction);
        newCurrentStates.push_back(newState);
        std::vector<int> newActions = get_actions(i, pos_start_of_turn);
        newActionsAndStates.push_back(newActions);
    }
    newActionsAndStates.push_back(newCurrentStates);

    std::tuple<int,int> newBestAction;
    newBestAction = find_Q_Max(newActionsAndStates);

    float reward = 0;
    // reward if winning
    if(statePrediction[PIECE(bestAction)] != -1){
        //reward = statePrediction[PIECE(bestAction)] + dice_roll;
    }
    if(ACTION(bestAction) == MOVE_FROM_HOME){
        reward += 50;
    }
    if(ACTION(bestAction) == MOVE_GOAL){
        reward += 20;
    }
    if(ACTION(bestAction) == MOVE_GOAL_ROAD){
        reward += 10;
    }
    if(ACTION(bestAction) == MOVE_SAFETY){
        reward += 15;
    }
    if(ACTION(bestAction) == MOVE_STAR){
        reward += 10;
    }
    if(ACTION(bestAction) == MOVE_GOAL_STAR){
        reward += 20;
    }
    if(ACTION(bestAction) == MOVE_KILL){
        reward += 20;
    }

    //reward += statePrediction[PIECE(newBestAction)]/100.0f;


    // Do QMatrix stuff
    QMatrix[allCurrentStates[PIECE(bestAction)]][ACTION(bestAction)] =
            QMatrix[allCurrentStates[PIECE(bestAction)]][ACTION(bestAction)] + LEARNING_RATE *
            (reward + DISCOUNT_FACTOR *(QMatrix[newCurrentStates[PIECE(newBestAction)]][ACTION(newBestAction)]
                                      - QMatrix[allCurrentStates[PIECE(bestAction)]][ACTION(bestAction)]));
    //std::cout << "Piece: " << PIECE(bestAction) << " Action: " << ACTION(bestAction) << " State: " <<  allCurrentStates[PIECE(bestAction)] << std::endl;
   // printQMatrix();
    return PIECE(bestAction);
}

std::tuple<int,int> ludo_player_QLearning::find_Q_Max(std::vector<std::vector<int> > actionsAndStates){
    int max = -1000;
    int newMax;
    std::vector<std::tuple<int, int>> valid_moves;
    //std::vector<int> pieceStates = actionsAndStates[4];
    for (int i = 0; i < actionsAndStates[4].size(); i++) {
        if(actionsAndStates[i].size() == 0)
            continue;
        for (int k = 0; k < actionsAndStates[i].size(); k++) {
            if(QMatrix[actionsAndStates[4][k]][actionsAndStates[i][k]] >= max){
                max = QMatrix[actionsAndStates[4][k]][actionsAndStates[i][k]];

                if(newMax != max)
                    valid_moves.empty();
                valid_moves.push_back(std::tuple<int,int>(i,actionsAndStates[i][k]));
                newMax = max;
            }
        }
    }
    std::uniform_int_distribution<> piece(0, valid_moves.size()-1);
    int select = piece(gen);
    return valid_moves[select];
}

void ludo_player_QLearning::printQMatrix(){

    std::cout << "Q Matrix -----------Action---------------------" << std::endl;
    std::cout << "   ";
    for(unsigned int i = 0; i < MOVE+1 ;i++){
        std::cout<< " " << i << "\t";
    }
    std::cout << std::endl;

    for(unsigned int i = 0; i < (MOVE+1)*7+11 ;i++){
        std::cout<< "-";
    }
    std::cout << std::endl;
    for(unsigned int state = 0; state < QMatrix.size(); state++){
        std::cout << state << " |";
        for(unsigned int action = 0; action < QMatrix[0].size(); action++){
            std::cout << " " << QMatrix[state][action] << "\t";
    }
    std::cout << std::endl;
    }
    std::cout << "Q Matrix end ----------------------------" << std::endl;
}

void ludo_player_QLearning::saveMatrix(){

    std::ofstream f("QMatrix");
    std::vector< std::vector<float>>::iterator row;
    std::vector<float>::iterator col;
    for (row = QMatrix.begin();  row != QMatrix.end(); row++) {
        for (col = row->begin(); col != row->end(); col++) {
            f << *col << '\n';
        }
    }
    f.close();

}

std::tuple<int,int> ludo_player_QLearning::do_random_movement(std::vector<std::vector<int>> actionsAndStates){
    std::vector<std::tuple<int, int>> valid_moves;
    std::vector<int> pieceStates = actionsAndStates[4];
    for (int i = 0; i < 4; i++) {
        if(actionsAndStates[i].size() == 0)
            continue;
        for (int k = 0; k < actionsAndStates[i].size(); k++) {
            valid_moves.push_back(std::tuple<int,int>(i,actionsAndStates[i][k]));
        }
    }

    std::uniform_int_distribution<> piece(0, valid_moves.size()-1);
    int select = piece(gen);
    return valid_moves[select];
}

int ludo_player_QLearning::get_piece_state(int piece, std::vector<int> positions){

    // HOME
    if (positions[piece] == -1)
        return HOME;

    // GOAL
    if (positions[piece] == 99 || positions[piece] == 56)
        return GOAL;

    // SAFE_1
    if((positions[piece]) < 51 && (positions[piece]) > -1)
    {
        if((positions[piece] - 8) % 13 == 0) // non colored globes
            return 3 * calc_quardrant(piece, positions);
        if(positions[piece] == 0) // home globe
            return 3 * calc_quardrant(piece, positions);
        for (int i = 0; i < 4; i++)  // a pair is safe
            if(positions[piece] == positions[i] && piece != i)
                return 3 * calc_quardrant(piece, positions);
    }

    // WINNER_ROAD
    if((positions[piece]) > 50 && (positions[piece]) < 56)
        return WINNER_ROAD;

    // DANGER
    int id = NULL;
    int enemies = 0;
    for (int i = 4; i < positions.size(); i++) {
        if(positions[i] < 0 || positions[i] > 50)
            continue;
        id = ((positions[piece] - positions[i]) % 52 + 52) % 52;
        if(id < 7){
            enemies++;
        }
    }
    if(enemies > 0)
        return 3 * calc_quardrant(piece, positions) + 1;

    return 3 * calc_quardrant(piece, positions) + 2;
}

int ludo_player_QLearning::calc_quardrant(int piece, std::vector<int> positions){
    if(positions[piece] <= 12)
        return 1;
    if(positions[piece] <= 25)
        return 2;
    if(positions[piece] <= 38)
        return 3;
    if(positions[piece] <= 51)
        return 4;
}

std::vector<int> ludo_player_QLearning::get_actions(int piece, std::vector<int> positions){
    std::vector<int> actions;

    // Move out from home
    if(positions[piece] == -1 && dice_roll == 6)
        actions.push_back(MOVE_FROM_HOME);

    // Move to the goal
    if((positions[piece] + dice_roll) == 56)
        actions.push_back(MOVE_GOAL);

    // Move to goal road
    if((positions[piece] + dice_roll) > 50 && positions[piece] < 50)
        actions.push_back(MOVE_GOAL_ROAD);

    // Move to safety
    if((positions[piece]) < 51 && (positions[piece]) > -1)
    {
        if(((positions[piece] + dice_roll) - 8) % 13 == 0) // non colored globes
            actions.push_back(MOVE_SAFETY);
        for (int i = 0; i < 4; i++)  // a pair is safe
            if((positions[piece] + dice_roll) == positions[i] && piece != i)
                actions.push_back(MOVE_SAFETY);
    }

    // Move to star
    if(positions[piece] != -1)
        if((positions[piece] + dice_roll) == 5  ||
           (positions[piece] + dice_roll) == 18 ||
           (positions[piece] + dice_roll) == 31 ||
           (positions[piece] + dice_roll) == 44){
            actions.push_back(MOVE_STAR);
        } else if((positions[piece] + dice_roll) == 11 ||
                  (positions[piece] + dice_roll) == 24 ||
                  (positions[piece] + dice_roll) == 37 ||
                  (positions[piece] + dice_roll) == 50){
            actions.push_back(MOVE_STAR);
        }

    // Move to star then goal
    if(positions[piece] != -1)
        if((positions[piece] + dice_roll) == 50)
            actions.push_back(MOVE_GOAL_STAR);

    // Move to kill
    int id = NULL;
    int enemies = 0;
    if((positions[piece]) < 51 && (positions[piece]) > -1) {
        for (int i = 4; i < positions.size(); i++) {
            if(positions[i] < 0 || positions[i] > 50)
                continue;
            if((positions[i] - 8) % 13 == 0) // non colored globes
                continue;
            if(positions[i] % 13 == 0 )
                continue;
            for (int k = 4; k < positions.size(); k++) { // a pair is safe
                if(positions[i] == positions[k] && k != i)
                    continue;
                if((positions[piece] + dice_roll) == positions[i])
                    enemies++;
            }
        }
    }
    if(enemies > 0)
        actions.push_back(MOVE_KILL);

    // Just move if it can move
    if(positions[piece] != -1 && positions[piece] < 56)
        actions.push_back(MOVE);

    return actions;
}

void ludo_player_QLearning::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player_QLearning::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}
