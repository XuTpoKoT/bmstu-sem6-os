#include <stdio.h>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "DBScan.h"
#include "CSVWriter.h"

#define MIN_POINTS_IN_CLUSTER 3
#define EPSILON 3.5

long long getThreadCpuTimeNs() {
    struct timespec t;
    if (clock_gettime(CLOCK_REALTIME, &t)) {
        perror("clock_gettime");
        return 0;
    }
    return t.tv_sec * 1000000000LL + t.tv_nsec;
}

void printResults(const vector<shared_ptr<Point>>& points, const vector<int> &indexes) {
    size_t cntPoints = points.size(); 
    printf("Number of points: %zu\n"
        " x     y     z     cluster_id\n"
        "-----------------------------\n"
        , cntPoints);
    for (size_t i = 0; i < cntPoints; i++) {
        printf("%5.2lf %5.2lf: %d\n",
                points[i]->x,
                points[i]->y, 
                indexes[i]);
    }
}

int readPoints(const char *filename, vector<shared_ptr<Point>> &points) {
    FILE *f = fopen(filename,"r");
    if (f == NULL) {
        return -1;
    }

    size_t cntPoints;
    if (fscanf(f, "%zu\n", &cntPoints) != 1) {
        fclose(f);
        return -1;
    }

    for (size_t i = 0; i < cntPoints; i++) {
        auto p = make_shared<Point>();
        if (fscanf(f, "%lf,%lf\n", &(p->x), &(p->y)) != 2) {
            fclose(f);
            return -1;
        }
        points.push_back(p);
    }

    fclose(f);
    return 0;
}

void timeMes(vector<shared_ptr<Point>> points) {
	CSVWriter w1{"../report/ts.csv"}, w2{"../report/tp.csv"};
	w1 << "size" << "time" << endrow;
	w2 << "size" << "time" << endrow;

    vector<int> cntTreads{1, 2, 4, 8, 16};
	for (auto i : cntTreads) {
        DBScan ds(points, MIN_POINTS_IN_CLUSTER, EPSILON, i);
        auto t1 = getThreadCpuTimeNs();
        ds.runSerial();
        auto t2 = getThreadCpuTimeNs();
        w1 << i << (t2 - t1) / 1000 << endrow;

        t1 = getThreadCpuTimeNs();
        ds.runParallel();
        t2 = getThreadCpuTimeNs();
        w2 << i << (t2 - t1) / 1000 << endrow;        
	}
    w1.flush();
    w2.flush();
}

int main() {
    FILE *f = fopen("pid.txt","w");
    fprintf(f, "%d\n", getpid());
    fclose(f);
    f = fopen("clusters.txt","w");

    vector<shared_ptr<Point>> points;
    vector<int> clusterIndexes;
    int threadCnt = 4;

    if (readPoints("data1.txt", points)) {
        printf("read data failed\n");
        return -1;
    }

    DBScan ds(points, MIN_POINTS_IN_CLUSTER, EPSILON, threadCnt);
    auto t1 = getThreadCpuTimeNs();
    ds.runParallel();
    auto t2 = getThreadCpuTimeNs();
    printf("%lld \n", (t2 - t1) / 1000);
    int x;
    
    // t1 = getThreadCpuTimeNs();
    // ds.runParallel();
    // t2 = getThreadCpuTimeNs();
    // printf("%lld \n", (t2 - t1) / 1000);

    for (size_t i = 0; i < ds.clusterIndexes.size(); i++) {
        fprintf(f, "%.2lf %.2lf %d\n", points[i]->x, points[i]->y, ds.clusterIndexes[i]);
    }
    scanf("%d", &x);
    fclose(f);

    printf("\nEeend!\n");
    return 0;
}
