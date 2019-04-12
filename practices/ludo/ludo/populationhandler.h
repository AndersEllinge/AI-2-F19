#ifndef POPULATIONHANDLER_H
#define POPULATIONHANDLER_H
#include <QObject>
#include <random>
#include "positions_and_dice.h"
#include <bitset>
#include <string.h>
#include <iostream>
#include <stdbool.h>
#include <iostream>
#include <fstream>

struct chromosome
{
    int wins = 0, games = 0, generation = 0, avgTurnsToWin = 0;
    float winRatio = 0;
    std::vector<std::bitset<32>> genes;

    bool operator < (const chromosome &other) const{
        return winRatio > other.winRatio;
    }
};

class populationHandler: public QObject {
    Q_OBJECT
private:
    std::random_device rd;
    std::mt19937 gen;
    bool evolve;


    chromosome createRandomChromosome();
    std::bitset<32> createRandomGene();
    float calcFloat(std::bitset<32> gene);
    void calcAvgWin();
    chromosome crossOver(chromosome parent1, chromosome parent2);
    void mutateNonUniform(std::bitset<32> &offspring);
    void createTournament();
    std::vector<chromosome> sortByWinRation(std::vector<chromosome> chromosomes);
    std::vector<chromosome> probabilisticSelection(std::vector<chromosome> chromosomes, float p, int participants);
    void createChildren();

public:
    populationHandler(int _populationSize,
                      int _tournamentSize,
                      int _numberOfGenes,
                      int _maxGenerations);
    void savePopulation();
    void saveChromosome();
    void loadPopulation();
    void updatePopulation();
    std::vector<float> getChromosomeGenes(std::size_t index);
    void addWin(std::size_t index);
    void setWins(std::size_t index, int wins);
    void addGame(std::size_t index);
    void setGames(std::size_t index, int games);

    std::vector<chromosome> population;
    int generations;
    int maxGenerations;
    int populationSize;
    int tournamentSize;
    int numberOfGenes;

signals:

public slots:

};

#endif // POPULATIONHANDLER_H
