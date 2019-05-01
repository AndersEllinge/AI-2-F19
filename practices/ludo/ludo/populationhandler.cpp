#include "populationhandler.h"




populationHandler::populationHandler(int _populationSize, int _tournamentSize, int _numTournaments , int _numberOfGenes, int _maxGenerations, int _trainingGames) :
    rd(),
    gen(rd()),
    generations(0),
    maxGenerations(_maxGenerations),
    populationSize(_populationSize),
    tournamentSize(_tournamentSize),
    numTournaments(_numTournaments),
    trainingGames(_trainingGames),
    numberOfGenes(_numberOfGenes)
{
    for (int i = 0; i < populationSize; i++) {
        population.push_back(createRandomChromosome());
    }
}

int populationHandler::nonTrainedChromosome(){
    for(int chromo = 0;  chromo < populationSize; chromo++){
        if(!population[chromo].trained)
            return chromo;
    }
    return -1;
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
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Saving population with " << generations << " generations" << std::endl;
    std::ofstream f("population");
    std::vector<chromosome>::iterator chromo;
    std::vector<std::bitset<32>>::iterator gene;
    f << generations << '\n';
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

void populationHandler::saveData(){
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Saving new data with " << generations << " generations" << std::endl;
    std::ofstream f("data", std::ios::out | std::ios::app);
    /*f.open("data");
    if(f.fail()){
        std::cout << "ERROR - Data NOT saved!" << std::endl;
        return;
    }*/
    //f.exceptions(f.exceptions() | std::ios::failbit | std::ifstream::badbit);
    for (auto chromo = population.begin();  chromo != population.end(); chromo++) {
        f << chromo->wins << ", ";
        f << chromo->games << ", ";
        f << chromo->generation << ", ";
        for (auto gene = chromo->genes.begin(); gene != chromo->genes.end(); gene++) {
            f << calcFloat(*gene) << ", ";
        }
        f << '\n';
    }
    f.close();
}

void populationHandler::loadPopulation(){
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Loading population" << std::endl;

    std::ifstream f("population", std::ios::in);
    if(f.fail()){
        std::cout << "ERROR - Pop NOT loaded!" << std::endl;
        std::cout << "---------1---------2---------3---------4---------5---------6---------7---------8---------9---------0" << std::endl;
        gameNumber = 1;
        traineeIndex = nonTrainedChromosome();
        emit newChromosome(getChromosomeGenes(traineeIndex));
        return;
    }
    int value;
    std::string gene;
    population.clear();
    f >> value;
    generations = value;
    std::cout << "Population loaded with " << generations << " generations." << std::endl;
    std::cout << "---------1---------2---------3---------4---------5---------6---------7---------8---------9---------0" << std::endl;
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

    gameNumber = 1;
    traineeIndex = nonTrainedChromosome();
    emit newChromosome(getChromosomeGenes(trainingGames));
}

void populationHandler::updatePopulation(){
    // New generation
    saveData();
    generations++;
    std::cout << "Calculating averages" << std::endl;
    calcAvgWin();

    // Do tournament selection

    std::cout << "Doing tournaments" << std::endl;
    createTournament(numTournaments);

    // Mate everyone with everyone and create children
    std::cout << "Making children" << std::endl;
    std::vector<chromosome> childrenPool;
    for (auto i = population.begin(); i != population.end(); i++) {
        for (auto k = population.begin(); k != population.end(); k++) {
            //if(i != k){ // It can now mate with it self
                chromosome child = crossOver(*i,*k);
                childrenPool.push_back(child);
            //}
        }
    }

    // randomly select children to put into the population
    std::cout << "Selecting children" << std::endl;
    for (std::size_t var = population.size(); var < std::size_t(populationSize); var++) {
        std::uniform_int_distribution<int> in(0,(int(childrenPool.size())-1));
        int randomIndex = in(gen);
        std::vector<chromosome>::iterator index = childrenPool.begin();
        population.push_back(childrenPool[std::size_t(randomIndex)]);
        childrenPool.erase(index + randomIndex);
    }
    childrenPool.clear();
    std::cout << "Update complete!" << std::endl;
    savePopulation();
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
                    std::max(start,end) + blendAlpha * differnce); // is this - or + ?
        float f = piece(gen);

        static_assert(sizeof(float) <= sizeof(long long unsigned int), "wrong sizes"); // 1.
        long long unsigned int f_as_int = 0;
        memcpy(&f_as_int, &f, sizeof(float)); // 2.
        std::bitset<8*sizeof(float)> f_as_bitset{f_as_int}; // 3.

        // nonuniform mutation - the exploration should become smaller when generations increases.
        // 1/16 chance, this should on average give one mutation per chromosome
        std::uniform_int_distribution<int> random(1,16);
        int r = random(gen);
        if(r == 1)
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

void populationHandler::createTournament(int numTournaments){
    std::vector<chromosome> tempPop;
    std::vector<int> tempIndex;
    std::vector<chromosome> winnerPop;
    std::cout << "We will have " << numTournaments << " tournaments with " << tournamentSize << " chromosomes" << std::endl;
    for (auto numberOfTournaments = 0; numberOfTournaments < numTournaments; numberOfTournaments++) {

        std::cout << "Tournament number: " << numberOfTournaments << std::endl;
        std::cout << "Select random players for tournament" << std::endl;
        // selects random chromes to play
        for (int i = 0; i < tournamentSize; i++) {
            std::uniform_int_distribution<int> in(0,(population.size()-1));
            int index = in(gen);

            // If we allready picked one, we dont need it again.
            if(checkIfPicked(index,tempIndex)){
                i--;
                continue;
            }
            tempIndex.push_back(index);
            tempPop.push_back(population[index]);
        }
        tempIndex.clear();

        std::cout << "Sort the selected in the tournament" << std::endl;
        // sort the selected tournament
        std::vector<chromosome> ranking = sortByWinRation(tempPop);

        std::cout << "Secelt with probabilty the winner" << std::endl;
        // select with probability the winner
        std::vector<chromosome> temp = probabilisticSelection(ranking, 0.9, tournamentSize-1);

        std::cout << "Save the winner of the tournament" << std::endl;
        // save the winner
        //for (auto var = temp.begin(); var != temp.end(); var++)
        if(checkIfWinner(temp[0],winnerPop)){
            numberOfTournaments--;
            temp.clear();
            tempPop.clear();
            continue;
        }
        winnerPop.push_back(*temp.begin());
        //}
        temp.clear();
        tempPop.clear();
    }
    std::cout << "Updating population" << std::endl;
    population = winnerPop;

/*  std::vector<chromosome> tempPop;
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
        std::vector<chromosome> temp = probabilisticSelection(ranking, 1.f, tournamentSize-1);

        // save the winner
        for (auto var = temp.begin(); var != temp.end(); var++) {
            winnerPop.push_back(*var);
        }
        temp.clear();
        tempPop.clear();
    }
    population = winnerPop;*/
}

/*
 * This function checks if the chromosome has allrdy been picked.
 */
bool populationHandler::checkIfPicked(int index, std::vector<int> tempIndex){
    for(auto indicies = tempIndex.begin(); indicies != tempIndex.end(); indicies++){
        if(index == *indicies){
            return true;
        }
    }
    return false;
}

bool populationHandler::checkIfWinner(chromosome index, std::vector<chromosome> chromo){
    for(auto indicies = chromo.begin(); indicies != chromo.end(); indicies++){
        if(index.genes == indicies->genes)
            return true;
    }
    return false;

}

std::vector<chromosome> populationHandler::sortByWinRation(std::vector<chromosome> chromosomes){
    std::vector<chromosome> chromoCopy = chromosomes;
    std::sort(chromoCopy.begin(), chromoCopy.end());
    return chromoCopy;
}

/*
* This function will given a vector of chromosomes, a probability and a number of participants select randomly between the chromosomes.
* The probability is the chance of selecting the best, then depending on the number of participants their chance will
* split in a exponentional way the rest of the space.
* Hence a weak participant has a chance of survival.
*/
std::vector<chromosome> populationHandler::probabilisticSelection(std::vector<chromosome> chromosomes, float p, int participants){
    std::vector<chromosome> bins;
    std::vector<chromosome> result;
    int temp = 0;
    float proba = 100*(p * powf((1-p),temp));

    if(p == 1.f) {
        result.push_back(chromosomes[0]);
        return result;
    }

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

void populationHandler::gameFinish(int color){
    std::cout << "#" << std::flush;
    addGame(traineeIndex);
    if(color == 0)
        addWin(traineeIndex);
    if(gameNumber == trainingGames){
        population[traineeIndex].trained = true;
        gameNumber = 1;
        std::cout << std::endl;
        std::cout << "Completed playing for a specimen." << std::endl;
        std::cout << "Gen: " << population[traineeIndex].generation << " index: " << traineeIndex << std::endl;
        std::cout << "This specimen won: " << float(population[traineeIndex].wins) /  float (population[traineeIndex].games) << std::endl;
        std::cout << std::endl;
        if(nonTrainedChromosome() == -1){ // No more to train
            std::cout << "Updating generation" << std::endl;
            updatePopulation();
            traineeIndex = nonTrainedChromosome();
            std::cout << "Training individual nr: " << traineeIndex << std::endl;
            std::cout << "---------1---------2---------3---------4---------5---------6---------7---------8---------9---------0" << std::endl;
            emit newChromosome(getChromosomeGenes(traineeIndex));
        }
        else{ // Train new
            traineeIndex = nonTrainedChromosome();
            std::cout << "Training individual nr: " << traineeIndex << std::endl;
            std::cout << "---------1---------2---------3---------4---------5---------6---------7---------8---------9---------0" << std::endl;
            emit newChromosome(getChromosomeGenes(traineeIndex));
        }

    }
    else{
        gameNumber++;
        emit newGame(true);
    }
}

void populationHandler::chromoChanged(){
    emit newGame(true);
}













