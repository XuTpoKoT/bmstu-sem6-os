#pragma once

#include <memory>
#include <cmath>
#include <iostream>

using namespace std;

struct Point
{
    double x, y;
    int clusterID;

    double dist(const shared_ptr<Point> p);
    void print();
    static bool cmpX(const shared_ptr<Point> p1, const shared_ptr<Point> p2);
    static bool cmpY(const shared_ptr<Point> p1, const shared_ptr<Point> p2);
};