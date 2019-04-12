#include "populationhandler.h"




populationHandler::populationHandler(int _populationSize, int _tournamentSize, int _numberOfGenes, int _maxGenerations) :
    rd(),
    gen(rd()),
    generations(0),
    maxGenerations(_maxGenerations),
    populationSize(_populationSize),
    tournamentSize(_tournamentSize),
    numberOfGenes(_numberOfGenes)
{
    for (int i = 0; i < populationSize; i++) {
        population.push_back(createRandomChromosome());
    }
}

chromosome populationHandler::createRandomChromosome(){
    chromosome chromo;
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

    // Do tournament selection
    calcAvgWin();
    createTournament();

    // Mate everyone with everyone and create children
    std::vector<chromosome> childrenPool;
    for (auto i = population.begin(); i != population.end(); i++) {
        for (auto k = population.begin(); k != population.end(); k++) {
            if(i != k){
                chromosome child = crossOver(*i,*k);
                childrenPool.push_back(child);
            }
        }
    }

    // randomly select children to put into the population
    for (std::size_t var = population.size(); var < std::size_t(populationSize); var++) {
        std::uniform_int_distribution<int> in(0,(int(childrenPool.size())-1));
        int randomIndex = in(gen);
        std::vector<chromosome>::iterator index = childrenPool.begin();
        population.push_back(childrenPool[std::size_t(randomIndex)]);
        childrenPool.erase(index + randomIndex);
    }
    childrenPool.clear();

}

float populationHandler::calcFloat(std::bitset<32> gene){
    auto a = gene.to_ulong();
    float tmpF;
    memcpy(&tmpF, &a, sizeof (tmpF));
    return tmpF;
}

void populationHandler::calcAvgWin(){
    for (auto i = population.begin(); i != population.end(); i++) {
        if(i->games != 0)
            i->winRatio = float(i->wins) / float(i->games);
    }
}

chromosome populationHandler::crossOver(chromosome parent1, chromosome parent2){
    chromosome offspring;
    //offspring.wins = 0; offspring.games = 0; offspring.winRatio = 0;
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

void populationHandler::createTournament(){
    std::vector<chromosome> tempPop;
    std::vector<chromosome> winnerPop;
    for (auto chromo = population.begin(); chromo != population.end(); chromo = population.begin()) {

        for (int i = 0; i < tournamentSize; i++) {
            std::uniform_int_distribution<int> in(0,(population.size()-1));
            int index = in(gen);
            tempPop.push_back(population[index]);
            population.erase(chromo+index);
        }

        // sort the selected tournament
        std::vector<chromosome> ranking = sortByWinRation(tempPop);

        // select with probability the winner
        std::vector<chromosome> temp = probabilisticSelection(ranking, 0.8, tournamentSize-1);

        // save the winner
        for (auto var = temp.begin(); var != temp.end(); var++) {
            winnerPop.push_back(*var);
        }
        temp.clear();
        tempPop.clear();
    }
    population = winnerPop;
}

std::vector<chromosome> populationHandler::sortByWinRation(std::vector<chromosome> chromosomes){
    std::vector<chromosome> chromoCopy = chromosomes;
    std::sort(chromoCopy.begin(), chromoCopy.end());
    return chromoCopy;
}

std::vector<chromosome> populationHandler::probabilisticSelection(std::vector<chromosome> chromosomes, float p, int participants){
    std::vector<chromosome> bins;
    std::vector<chromosome> result;
    int temp = 0;
    float proba = 100*(p * powf((1-p),temp));
    for (std::size_t var = 0; var < proba; var++) {

        bins.push_back(chromosomes[temp]);

        if(temp < participants & var == ceil(proba-1)){
            temp++;
            proba = 100*(p * powf((1-p),temp));
            var = 0;
        }
        else if(temp == participants & var == ceil(proba-1)){
            proba = 99 - bins.size();
            var = 0;
        }
    }

    // select winner
    std::uniform_int_distribution<int> t(0,bins.size()-1);
    int tau = t(gen);
    //std::cout << "tau is " << tau << std::endl;
    result.push_back(bins[tau]);


    return result;
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

void populationHandler::addWin(std::size_t index){
    population[index].wins += 1;
}

void populationHandler::setWins(std::size_t index, int wins){

}

void populationHandler::addGame(std::size_t index){
    population[index].games += 1;
}

void populationHandler::setGames(std::size_t index, int games){
    population[index].games = games;
}












