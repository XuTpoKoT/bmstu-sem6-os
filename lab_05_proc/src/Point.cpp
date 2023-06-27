#include "Point.h"
 
double Point::dist(const shared_ptr<Point> p) {
    return sqrt(pow(x - p->x, 2) + pow(y - p->y, 2));
}

void Point::print() {
    cout << x << " " << y << endl;
}

bool Point::cmpX(const shared_ptr<Point> p1, const shared_ptr<Point> p2) {
    return p1->x < p2->x;
}

bool Point::cmpY(const shared_ptr<Point> p1, const shared_ptr<Point> p2) {
    return p1->y < p2->y; 
}
