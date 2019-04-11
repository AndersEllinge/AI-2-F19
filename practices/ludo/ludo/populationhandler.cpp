#include "populationhandler.h"




populationHandler::populationHandler(int populationSize, int tournamentSize, int numberOfGenes, int maxGenerations) :
    rd(),
    gen(rd()),
    generations(0),
    maxGenerations(maxGenerations),
    populationSize(populationSize),
    tournamentSize(tournamentSize),
    numberOfGenes(numberOfGenes)
{
    for (int i = 0; i < populationSize; i++) {
        population.push_back(createRandomChromosome());
    }
}

chromosome populationHandler::createRandomChromosome(){
    chromosome chromo;
    chromo.wins = 0;
    chromo.games = 0;
    chromo.generation = 0;
    for (int i = 0; i < numberOfGenes; i++) {
        chromo.genes.push_back(createRandomGene());
    }
    return chromo;
}

std::bitset<32> populationHandler::createRandomGene(){
    float f;// = -1.618;

    std::uniform_real_distribution<float> piece(-50, 50);
    f = piece(gen);

    static_assert(sizeof(float) <= sizeof(long long unsigned int), "wrong sizes"); // 1.
    long long unsigned int f_as_int = 0;
    memcpy(&f_as_int, &f, sizeof(float)); // 2.
    std::bitset<8*sizeof(float)> f_as_bitset{f_as_int}; // 3.

    return f_as_bitset;
}

void populationHandler::savePopulation(){
    std::cout << "Saving population" << std::endl;
    std::ofstream f("population");
    std::vector<chromosome>::iterator chromo;
    std::vector<std::bitset<32>>::iterator gene;
    for (chromo = population.begin();  chromo != population.end(); chromo++) {
        f << chromo->wins << '\n';
        f << chromo->games << '\n';
        f << chromo->generation << '\n';
        for (gene = chromo->genes.begin(); gene != chromo->genes.end(); gene++) {
            f << *gene << '\n';
        }
        //f << '.' << '\n';
    }
    f.close();
}

void populationHandler::loadPopulation(){
    std::cout << "Loading population" << std::endl;

    std::ifstream f("population", std::ios::in);
    int value;
    std::string gene;
    population.clear();
    for (int i = 0; i < populationSize; i++) {
        chromosome chromo;
        f >> value;
        chromo.wins = value;
        f >> value;
        chromo.games = value;
        f >> value;
        chromo.generation = value;
        chromo.genes.clear();
        for (auto k = 0; k < numberOfGenes; k++) {
            f >> gene;
            //std::bitset<32> genome(gene);
            chromo.genes.push_back(std::bitset<32>(gene));
        }
        population.push_back(chromo);
    }
    f.close();
}

void populationHandler::updatePopulation(){
    // New generation
    generations++;

    // Do tournament
    calcAvgWin();

    createTournament(1);



    //chromosome offspring = crossOver(population[0], population[1]);

    //population.push_back(offspring);
}

float populationHandler::calcFloat(std::bitset<32> gene){
    auto a = gene.to_ulong();
    float tmpF;
    memcpy(&tmpF, &a, sizeof (tmpF));
    return tmpF;
}

void populationHandler::calcAvgWin(){
    for (auto i = population.begin(); i != population.end(); i++) {
        i->winRatio = float(i->wins) / float(i->games);
    }
}

chromosome populationHandler::crossOver(chromosome parent1, chromosome parent2){
    chromosome offspring;
    offspring.wins = 0; offspring.games = 0;
    offspring.generation = generations;
    float blendAlpha = 0.5;
    float differnce;
    for (std::size_t i = 0; i < parent1.genes.size(); i++) {
       // BLX-a crossover - one offspring
        float start = calcFloat(parent1.genes[i]);
        float end = calcFloat(parent2.genes[i]);
        differnce = std::abs(start - end);

        std::uniform_real_distribution<float> piece(
                    std::min(start,end) - blendAlpha * differnce,
                    std::max(start,end) - blendAlpha * differnce);
        float f = piece(gen);

        static_assert(sizeof(float) <= sizeof(long long unsigned int), "wrong sizes"); // 1.
        long long unsigned int f_as_int = 0;
        memcpy(&f_as_int, &f, sizeof(float)); // 2.
        std::bitset<8*sizeof(float)> f_as_bitset{f_as_int}; // 3.

        // nonuniform mutation - the exploration should become smaller when generations increases.
        mutateNonUniform(f_as_bitset);

        offspring.genes.push_back(f_as_bitset);
    }
    return offspring;
}

void populationHandler::mutateNonUniform(std::bitset<32> &gene){

    //non uniform mutation

    float x = calcFloat(gene), xMax = 12, xMin = -12, beta = 20;
    std::uniform_int_distribution<int> t(0,1);
    int tau = t(gen);
    if (tau == 0)
        tau = -1;
    std::uniform_real_distribution<float> r(0,1);
    float random = r(gen);
    float randomPow = powf(1 - (float(generations)/float(maxGenerations)),beta);
    float new_x = x + tau*(xMax - xMin)*(1.f - powf(random, randomPow ));
    static_assert(sizeof(float) <= sizeof(long long unsigned int), "wrong sizes"); // 1.
    long long unsigned int f_as_int = 0;
    memcpy(&f_as_int, &new_x, sizeof(float)); // 2.
    std::bitset<8*sizeof(float)> f_as_bitset{f_as_int}; // 3.
    gene = std::bitset<32>(f_as_bitset);
}

void populationHandler::createTournament(int winners){
    std::vector<chromosome> tempPop;
    std::vector<chromosome> winnerPop;
    for (auto chromo = population.begin(); chromo != population.end(); chromo = population.begin()) {

        for (int i = 0; i < tournamentSize; i++) {
            std::uniform_int_distribution<int> in(0,(population.size()-1));
            int index = in(gen);
            //std::cout << index << std::endl;
            tempPop.push_back(population[index]);
            population.erase(chromo+index);
        }

        std::vector<chromosome> temp(winners);
        for (auto var = temp.begin(); var != temp.end(); var++) {
            var->winRatio = 0;
        }

        for (auto opponent_1 = tempPop.begin(); opponent_1 != tempPop.end(); opponent_1++) {
            for (int var = 0; var < winners; var++) {
                if((opponent_1->winRatio) > (temp[var].winRatio)){
                    temp[var] = *opponent_1;
                }
            }
        }

        for (auto var = temp.begin(); var != temp.end(); var++) {
            winnerPop.push_back(*var);
        }
        temp.clear();
        tempPop.clear();
    }
    population = winnerPop;
    populationSize = population.size();
}

std::vector<float> populationHandler::getChromosomeGenes(std::size_t index){
    std::vector<float> chromosome;
    std::vector<std::bitset<32>>::iterator gene;
    for ( gene = population[index].genes.begin(); gene != population[index].genes.end(); gene++) {
        auto a = gene->to_ulong();
        float tmpF;
        memcpy(&tmpF, &a, sizeof (tmpF));
        chromosome.push_back(tmpF);
    }
    return chromosome;
}













