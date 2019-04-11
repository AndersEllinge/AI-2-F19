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
    int wins, games, generation, avgTurnsToWin;
    float winRatio;
    std::vector<std::bitset<32>> genes;
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
    void createTournament(int winners);

public:
    populationHandler(int populationSize,
                      int tournamentSize,
                      int numberOfGenes,
                      int maxGenerations);
    void savePopulation();
    void saveChromosome();
    void loadPopulation();
    void updatePopulation();
    std::vector<float> getChromosomeGenes(std::size_t index);

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
