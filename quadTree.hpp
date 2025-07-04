#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <vector>
#include <memory>
#include <iostream>
#include "utilities/mpl_writer.hpp"

class Point{
public:
    double x;
    double y;

    Point();
    Point(double xCoord, double yCoord);
};

class Rectangle{
public:
    Point bottomLeft;
    Point topRight;

    Rectangle();
    Rectangle(Point bottomLeftPoint, Point topRightPoint);

    // Method used in the constructor of the QuadTree
    static Rectangle buildingBoundary(const std::vector<Point>& collectionPoints);

    // Method used in the query part
    static bool intersect(const Rectangle& rectangle1, const Rectangle& rectangle2);
    static bool includes(const Rectangle& smallerRectangle, const Rectangle& largeRectangle);
    static bool isIn(const Point& point, const Rectangle& rectangle);
};

class QuadTree{
public:
    // The wanted constructor
    QuadTree(const std::vector<Point>& collectionPoints, const std::size_t bucketCapacity);
    
    // Other constructor and methods used in the first one
    QuadTree(const Rectangle& largeBoundary, const std::vector<Point>& collectionPoints, const std::size_t bucketCapacity);
    void subdivide(const std::vector<Point>& collectionPoints, const std::size_t bucketCapacity, const Rectangle& largeBoundary);

    // Methods used in plot
    std::vector<Rectangle> getRectangles();
    std::vector<std::vector<Point>> getVectorOfVectorOfPoints();
    
    // Methods used in query
    std::vector<Point> query(const Rectangle& searchingArea);
    std::vector<Point> pointsIn(const Rectangle& searchingArea);

private:
    std::size_t capacity;
    Rectangle boundary;
    std::vector<Point> points;

    std::unique_ptr<QuadTree> NW;
    std::unique_ptr<QuadTree> NE;
    std::unique_ptr<QuadTree> SW;
    std::unique_ptr<QuadTree> SE;
};

#endif 
