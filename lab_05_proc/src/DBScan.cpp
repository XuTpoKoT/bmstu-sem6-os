#include "DBScan.h"

#include <algorithm>
#include <unistd.h>

void DBScan::runSerial() {
    initNeighbors();
    serialCalcNeighbors();
    run();
}

void DBScan::runParallel() {
    initNeighbors();
    parallelCalcNeighbors();
    run();
}


void DBScan::initNeighbors() {
    for (int i = 0; i < points.size(); i++) {
        neighbors[i] = vector<int>();
    }
}

void DBScan::serialCalcNeighbors() {
    for (int i = 0; i < points.size(); i++) { // 0
        neighbors[i] = getNeighbors(points[i]); // 1
    }
}

int DBScan::run() {
    curClusterID = 0; // 2
    for (size_t i = 0, n = points.size(); i < n; i++) { // 3
        if (clusterIndexes[i] == UNVISITED) { // 4
            curPointIndex = i; // 5
            if (expandCluster() != -1) {
                curClusterID++; // 6
            }
        }
    }

    return 0;
}

int DBScan::expandCluster() {
    vector<int> seeds = neighbors[curPointIndex]; // 7

    if (seeds.size() + 1 < minPointsInCluster) { // 8
        clusterIndexes[curPointIndex] = NOISE; // 9
        return -1;
    }
    clusterIndexes[curPointIndex] = curClusterID; // 10
    for (size_t i = 0, cntSeeds = seeds.size(); i < cntSeeds; ++i) { // 11
        clusterIndexes[seeds[i]] = curClusterID; // 12
    }

    while (seeds.size() > 0) { // 13
        int curSeed = seeds.back(); // 14
        seeds.pop_back(); // 15
        vector<int> seedNeighbors = neighbors[curSeed]; // 16

        size_t cntNeighbors = seedNeighbors.size(); // 17
        if (cntNeighbors + 1 >= minPointsInCluster) { // 18
            for (size_t i = 0; i < cntNeighbors; ++i) { // 19
                int curNeighbor = seedNeighbors[i]; // 20
                if (clusterIndexes[curNeighbor] == UNVISITED || clusterIndexes[curNeighbor] == NOISE) { // 21
                    if (clusterIndexes[curNeighbor] == NOISE) { // 22
                        seeds.push_back(curNeighbor); // 23
                    }
                    clusterIndexes[curNeighbor] = curClusterID; // 24
                }
            }
        }
    }

    return 0;
}

void DBScan::parallelCalcNeighbors() {
    formPointGroups();
    for (int i = 0; i < cntThreads; i++) {
        this->threads[i] = thread(&DBScan::calcGroupNeighbors, this, i);
    }
    for (int i = 0; i < cntThreads; i++) {
        this->threads[i].join();
    }
}

void DBScan::formPointGroups() {
    int pointsInGroup = points.size() / cntThreads;
    for (size_t i = 0; i < cntThreads - 1; i++) {
        vector<int> pointGroup;
        for (size_t j = i * pointsInGroup; j < pointsInGroup * (i+1); j++) {
            pointGroup.push_back(j);
        }
        pointGroups.push_back(pointGroup);
    }

    vector<int> pointGroup;
    for (size_t j = (cntThreads - 1) * pointsInGroup; j < points.size(); j++) {
        pointGroup.push_back(j);
    }
    pointGroups.push_back(pointGroup);
}

vector<int> DBScan::getNeighbors(const shared_ptr<Point> point) {
    vector<int> neighborIndexes;
    for (size_t i = 0, n = points.size(); i < n; i++) {
        double distance = point->dist(points[i]);
        if (distance <= eps && distance >= 1e-8) {
            neighborIndexes.push_back(i);
        }
    }

    return neighborIndexes;
}

void DBScan::calcGroupNeighbors(int groupNumber) {
    for (size_t i = 0, n = pointGroups[groupNumber].size(); i < n; i++) {
        vector<int> curNeighbors = getNeighbors(points[pointGroups[groupNumber][i]]);
        m.lock();
        neighbors[i] = move(curNeighbors);
        m.unlock();
    }
    sleep(100);
}

