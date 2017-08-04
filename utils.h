#pragma once

#include <random>
#include <array>
#include <functional>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(0.0, 1.0);

namespace utils {

    double random() {
        return dist(mt);
    }

    double randomBetween(double min, double max) {
        return random() * (max - min) + min;
    }

    double nonLinearRandomBetween(double min, double max, std::function<double(double)> f) {
        return f(random()) * (max - min) + min;
    }

    int weightedRandom(std::vector<double> weights) {
        double roll = random();
        double sum = 0;
        for(double w : weights) {
            sum += w;
        }
        double scaledRoll = roll * sum;
        sum = 0;
        for(int i = 0; i < weights.size(); i++) {
            sum += weights[i];
            if(sum >= scaledRoll) return i;
        }
    }


}