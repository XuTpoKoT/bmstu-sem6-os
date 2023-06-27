#ifndef DBScan_H
#define DBScan_H

#include <stdio.h>
#include <vector>
#include <cmath>
#include <memory>
#include <thread>
#include <mutex>
#include <map>

#include "Point.h"
#include <thread>

using namespace std;

#define UNVISITED -2
#define NOISE -1

class DBScan {
public:
    const vector<shared_ptr<Point>> points;
    vector<int> clusterIndexes;
    
    DBScan(vector<shared_ptr<Point>> points, size_t minPointsInCluster, double eps, int cntThreads) :
        points(points), minPointsInCluster(minPointsInCluster), eps(eps), threads(cntThreads) {
            this->cntThreads = cntThreads;
            
            for (size_t i = 0; i < points.size(); i++) {
                clusterIndexes.push_back(UNVISITED);
            }
        } 

    void runSerial();
    void runParallel();

private:
    const size_t minPointsInCluster;
    const double eps;
    int cntThreads;
    int curPointIndex;
    int curClusterID;
    vector<thread> threads;
    vector<vector<int>> pointGroups;
    map<int, vector<int>> neighbors;
    mutex m;

    void initNeighbors();
    void serialCalcNeighbors();
    void parallelCalcNeighbors();
    void formPointGroups();

    int run();
    int expandCluster();
    vector<int> getNeighbors(const shared_ptr<Point> point);
    void calcGroupNeighbors(int groupNumber);
};

#endif // DBScan_H
