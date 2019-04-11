#include "ludo_player_genetic.h"

ludo_player_genetic::ludo_player_genetic(bool evolveEnable):
    rd(),
    gen(rd()),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0),
    evolve(evolveEnable),
    weights(10)
{

}


int ludo_player_genetic::make_decision(){


    std::vector<std::bitset<32>> chromosome = createRandomChromosome();

    for (int var = 0; var < chromosome.size(); var++) {
        auto a = chromosome[var].to_ulong();
        float testFloat;
        memcpy(&testFloat, &a, sizeof (testFloat));
        //std::cout << "testfloat " << var << " is: " << testFloat << std::endl;
    }

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

std::vector<std::bitset<32>> ludo_player_genetic::createRandomChromosome(){
    //std::bitset<32> emptyGene;
    std::vector<std::bitset<32>> chromo;
    for (int i = 0; i < 10; i++) {

        chromo.push_back(createRandomGene());
    }
    return chromo;
}

std::bitset<32> ludo_player_genetic::createRandomGene(){
    float f;// = -1.618;

    std::uniform_real_distribution<float> piece(-2, 2);
    f = piece(gen);

    static_assert(sizeof(float) <= sizeof(long long unsigned int), "wrong sizes"); // 1.
    long long unsigned int f_as_int = 0;
    memcpy(&f_as_int, &f, sizeof(float)); // 2.
    std::bitset<8*sizeof(float)> f_as_bitset{f_as_int}; // 3.

    return f_as_bitset;
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

