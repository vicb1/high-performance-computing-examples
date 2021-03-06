#include <iostream>
#include <fstream>
#include <future>
#include <mutex>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <omp.h>
#include "Vector.hpp"
#include "COO.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " vectorSize " << std::endl;
        return 0;
    }

    int vectorSize = std::stoi(argv[1]);
    if (vectorSize != std::round(std::sqrt(vectorSize)) * std::round(std::sqrt(vectorSize)))
        exit(-1);

    int numRuns = 100;
    if (vectorSize < 10000)
        numRuns = 100000;
    else
        numRuns = 100;

    Vector x(vectorSize);
    randomize(x);
    COOMatrix A(vectorSize, vectorSize);
    piscetize(A, std::sqrt(vectorSize), std::sqrt(vectorSize));
    Vector y(vectorSize);
    zeroize(y);

    auto seqStart = std::chrono::system_clock::now();
    for (int i = 0; i < numRuns; ++i)
        matvec(A, x, y);
    auto seqEnd = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> seqElapsed = seqEnd - seqStart;
    double seqAns = twoNorm(y);
    zeroize(y);

    auto parStart = std::chrono::system_clock::now();
    for (int i = 0; i < numRuns; ++i)
        ompMatvec(A, x, y);
    auto parEnd = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli> parElapsed = parEnd - parStart;
    double parAns = twoNorm(y);

    std::cout << std::thread::hardware_concurrency() << " " << omp_get_max_threads() << " " << vectorSize << " "
              << seqElapsed.count() / numRuns << " " << parElapsed.count() / numRuns << " "
              << (seqElapsed.count() / parElapsed.count()) << " " << (seqAns - parAns) << std::endl;

    return 0;
}
