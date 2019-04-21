#include "ludo_player_genetic.h"

ludo_player_genetic::ludo_player_genetic(bool evolveEnable):
    rd(),
    gen(rd()),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0),
    evolve(evolveEnable),
    weights(15)
{

}


int ludo_player_genetic::make_decision(){

    states.clear();
    for (std::size_t var = 0; var < 4; var++) {
        states.push_back(getStates(var,pos_start_of_turn));
    }


    std::vector<float> results = sumWeightsAndStates(states);
    /*std::cout << " -------- " << std::endl;
    for (auto i = results.begin(); i != results.end(); i++) {
        std::cout << "Result: " << *i << std::endl;
    }*/


    std::vector<float>::iterator max = std::max_element(results.begin(),results.end());
    int piece = int(std::distance(results.begin(), max));
    if(isValidMove(piece)){
  //      std::cout << "Moving piece: " << piece << std::endl;
        return piece;
    }
    *max = -999999999;
    max = std::max_element(results.begin(),results.end());
    piece = int(std::distance(results.begin(), max));
    if(isValidMove(piece)){
  //      std::cout << "Moving piece: " << piece << std::endl;
        return piece;
    }
    *max = -999999999;
    max = std::max_element(results.begin(),results.end());
    piece = int(std::distance(results.begin(), max));
    if(isValidMove(piece)){
  //     std::cout << "Moving piece: " << piece << std::endl;
        return piece;
    }
    *max = -999999999;
    max = std::max_element(results.begin(),results.end());
    piece = int(std::distance(results.begin(), max));
    if(isValidMove(piece)){
 //       std::cout << "Moving piece: " << piece << std::endl;
        return piece;
    }
    *max = -999999999;

    return -1;
}

std::vector<int> ludo_player_genetic::getStates(std::size_t piece, std::vector<int> positions){
    std::vector<int> state(16);
    state[0] = isHome(piece, positions);
    state[1] = isGoal(piece, positions);
    state[2] = isSafe(piece, positions);
    state[3] = onWinnerRoad(piece, positions);
    state[4] = inDanger(piece, positions);
    state[5] = hasPosition(piece, positions);
    state[6] = moveFromHome(piece, positions);
    state[7] = moveToGoal(piece, positions);
    state[8] = moveToGoalRoad(piece, positions);
    state[9] = moveToSafty(piece, positions);
    state[10] = moveToStar(piece, positions);
    state[11] = moveStarGoal(piece, positions);
    state[12] = canKill(piece, positions);
    state[13] = moveToDanger(piece, positions);
    state[14] = moveToDeath(piece, positions);
    state[15] = plainMove(piece, positions);
    return state;
}

std::vector<float> ludo_player_genetic::sumWeightsAndStates(std::vector<std::vector<int>> states){

    std::vector<float> sums(4,0);
    std::vector<float>::iterator sumIter;
    std::vector<float>::iterator weightIter;
    sumIter = sums.begin();
    for (auto piece = states.begin(); piece != states.end(); piece++) { // pieces
        weightIter = weights.begin();
        for (auto st = piece->begin(); st != piece->end(); st++) { // state inputs
            *sumIter += *st * *weightIter;
            weightIter++;
        }
        sumIter++;
    }
    return sums;
}

int ludo_player_genetic::isValidMove(int piece){

    if(states[piece][0] == 1 && dice_roll == 6)
        return 1;

    if(states[piece][15] == 1)
        return 1;

    return 0;
}

int ludo_player_genetic::isHome(std::size_t piece, std::vector<int> positions){
    if (positions[piece] == -1)
        return 1;
    return 0;
}

int ludo_player_genetic::isGoal(std::size_t piece, std::vector<int> positions){
    if (positions[piece] == 99 || positions[piece] == 56)
        return 1;
    return 0;
}

int ludo_player_genetic::isSafe(std::size_t piece, std::vector<int> positions){
    if((positions[piece]) < 51 && (positions[piece]) > -1)
    {
        if((positions[piece] - 8) % 13 == 0) // non colored globes
            return 1;
        if(positions[piece] == 0) // home globe
            return 1;
        for (int i = 0; i < 4; i++)  // a pair is safe
            if(positions[piece] == positions[i] && piece != i)
                return 1;
    }
    return 0;
}

int ludo_player_genetic::onWinnerRoad(std::size_t piece, std::vector<int> positions){
    if((positions[piece]) > 50 && (positions[piece]) < 56)
        return 1;
    return 0;
}

int ludo_player_genetic::inDanger(std::size_t piece, std::vector<int> positions){
    int id = NULL;
    int enemies = 0;
    if(!isHome(piece,positions) &&
       !isGoal(piece,positions) &&
       !onWinnerRoad(piece,positions) &&
       !isSafe(piece,positions))
        for (std::size_t i = 4; i < positions.size(); i++) {
            if(positions[i] < 0 || positions[i] > 50)
                continue;
            // you can be in danger from far away on a star
            if((positions[piece]) == 5  ||
               (positions[piece]) == 18 ||
               (positions[piece]) == 31 ||
               (positions[piece]) == 44){
                id = ((positions[piece] - 7 - positions[i]) % 52 + 52) % 52;
                if(id < 7)
                    enemies++;;
            } else if((positions[piece]) == 11 ||
                      (positions[piece]) == 24 ||
                      (positions[piece]) == 37 ||
                      (positions[piece]) == 50){
                id = ((positions[piece] - 6 - positions[i]) % 52 + 52) % 52;
                if(id < 7)
                    enemies++;;
            }
            id = ((positions[piece] - positions[i]) % 52 + 52) % 52;
            if(id < 7){
                enemies++;
            }
        }
    if(enemies > 0)
        return enemies;
    return 0;
}

int ludo_player_genetic::hasPosition(std::size_t piece, std::vector<int> positions){
    return positions[piece];
}

int ludo_player_genetic::moveFromHome(std::size_t piece, std::vector<int> positions){
    if(positions[piece] == -1 && dice_roll == 6)
        return 1;
    return 0;
}

int ludo_player_genetic::moveToGoal(std::size_t piece, std::vector<int> positions){
    if((positions[piece] + dice_roll) == 56)
        return 1;
    return 0;
}

int ludo_player_genetic::moveToGoalRoad(std::size_t piece, std::vector<int> positions){
    if((positions[piece] + dice_roll) > 50 && positions[piece] < 50)
        return 1;
    return 0;
}

int ludo_player_genetic::moveToSafty(std::size_t piece, std::vector<int> positions){
    if((positions[piece]) < 51 && (positions[piece]) > -1 && !moveToDeath(piece,positions))
    {
        if(((positions[piece] + dice_roll) - 8) % 13 == 0) // non colored globes
            return 1;
        for (int i = 0; i < 4; i++)  // a pair is safe
            if((positions[piece] + dice_roll) == positions[i] && piece != i && !moveToGoal(piece,positions))
                return 1;

    }
    return 0;
}

int ludo_player_genetic::moveToStar(std::size_t piece, std::vector<int> positions){
    if(positions[piece] != -1)
        if((positions[piece] + dice_roll) == 5  ||
           (positions[piece] + dice_roll) == 18 ||
           (positions[piece] + dice_roll) == 31 ||
           (positions[piece] + dice_roll) == 44){
            return 1;
        } else if((positions[piece] + dice_roll) == 11 ||
                  (positions[piece] + dice_roll) == 24 ||
                  (positions[piece] + dice_roll) == 37 ||
                  (positions[piece] + dice_roll) == 50){
            return 1;
        }
    return 0;
}

int ludo_player_genetic::moveStarGoal(std::size_t piece, std::vector<int> positions){
    if(positions[piece] != -1)
        if((positions[piece] + dice_roll) == 50)
            return 1;
    return 0;
}

int ludo_player_genetic::canKill(std::size_t piece, std::vector<int> positions){
    int id = NULL;
    int enemies = 0;
    if(!onWinnerRoad(piece,positions) &&
       !isGoal(piece,positions)) {
        for (std::size_t  i = 4; i < positions.size(); i++) {
            if(positions[i] < 0 || positions[i] > 50)
                continue;
            if((positions[i] - 8) % 13 == 0) // non colored globes
                continue;
            if(positions[i] % 13 == 0 && positions[i] != 0)
                continue;
            for (std::size_t  k = 4; k < positions.size(); k++) { // a pair is safe
                if(positions[i] == positions[k] && k != i){
                    if(positions[k] != 0) // we can kill on our home globe
                        continue;
                }
                if(moveToStar(piece,positions)){
                    if((positions[piece] + dice_roll) == 5  ||
                       (positions[piece] + dice_roll) == 18 ||
                       (positions[piece] + dice_roll) == 31 ||
                       (positions[piece] + dice_roll) == 44){
                        if((positions[piece] + dice_roll + 6) == positions[i] && positions[piece] != -1 && k == 12)
                            enemies++;;
                    } else if((positions[piece] + dice_roll) == 11 ||
                              (positions[piece] + dice_roll) == 24 ||
                              (positions[piece] + dice_roll) == 37 ||
                              (positions[piece] + dice_roll) == 50){
                        if((positions[piece] + dice_roll + 7) == positions[i] && positions[piece] != -1 && k == 12)
                            enemies++;
                    }
                }
                else {
                    if((positions[piece] + dice_roll) == positions[i] && positions[piece] != -1 && k == 12)
                        enemies++;
                    if(positions[piece] == -1 && dice_roll == 6 && positions[i] == 0 && k == 12)
                        enemies++;
                }

            }
        }
    }
    if(enemies > 0)
        return enemies;
    return 0;
}

int ludo_player_genetic::moveToDanger(std::size_t piece, std::vector<int> positions){
    int id = NULL;
    int enemies = 0;
    std::vector<int> newPos = positions;
    if(!isHome(piece,newPos) &&
       !isGoal(piece,newPos) &&
       !onWinnerRoad(piece,newPos)) {
        newPos[piece] = newPos[piece] + dice_roll;
        if(newPos[piece] == 5  ||
           newPos[piece] == 18 ||
           newPos[piece] == 31 ||
           newPos[piece] == 44){
            newPos[piece] += 6;
        }
        else if(newPos[piece] == 11 ||
                  newPos[piece] == 24 ||
                  newPos[piece] == 37 ||
                  newPos[piece] == 50){
            newPos[piece] += 7;
        }
        for (std::size_t i = 4; i < positions.size(); i++) {
            if(newPos[i] < 0 || newPos[i] > 50)
                continue;
            id = ((newPos[piece] - newPos[i]) % 52 + 52) % 52;
            if(id < 7 && id != 0){
                enemies++;
            }
        }
    }
    if(enemies > 0)
        return enemies;
    return 0;
}

int ludo_player_genetic::moveToDeath(std::size_t piece, std::vector<int> positions){
    if(!onWinnerRoad(piece,positions) &&
       !isGoal(piece,positions) &&
       !isHome(piece,positions)) {
        for (std::size_t  i = 4; i < positions.size(); i++) {
            if((positions[i] - 8) % 13 == 0 && // if globe and we move there
               (positions[piece] + dice_roll) == positions[i])
                return 1;
            if(positions[i] % 13 == 0 && positions[i] != 0 &&
              (positions[piece] + dice_roll) == positions[i])
                return 1;
            for (std::size_t  k = 4; k < positions.size(); k++) { // a pair is death
                if(positions[i] == positions[k] && k != i &&
                  (positions[piece] + dice_roll) == positions[i]){
                        return 1;
                }
            }
        }
    }

    return 0;
}

int ludo_player_genetic::plainMove(std::size_t piece, std::vector<int> positions){
    if(positions[piece] != -1 && positions[piece] < 56)
        return 1;
    return 0;
}

void ludo_player_genetic::setWeights(std::vector<float> chromosome){
    weights = chromosome;
}

void ludo_player_genetic::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player_genetic::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    if(evolve)

    emit turn_complete(game_complete);
}

