#include "quadTree.hpp"

// --- Point ---
Point::Point(): x(0.0), y(0.0) {}; //default constructor
Point::Point(double xCoord, double yCoord): x(xCoord), y(yCoord) {};

// --- Rectangle ---
Rectangle::Rectangle(): bottomLeft(Point()), topRight(Point()) {};
Rectangle::Rectangle(Point bottomLeftPoint, Point topRightPoint): bottomLeft(bottomLeftPoint), topRight(topRightPoint) {};

// Building the limits boundary of a collection of points (used in the initialization of the constructor)
Rectangle Rectangle::buildingBoundary(const std::vector<Point>& collectionPoints){
    double minX = collectionPoints[0].x;
    double maxX = collectionPoints[0].x;
    double minY = collectionPoints[0].y;
    double maxY = collectionPoints[0].y;

    // Going throw all points to find the limits
    for (const Point& p : collectionPoints){
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }
    return Rectangle(Point(minX, minY), Point(maxX, maxY));
}

// Checking if at least one corner of the "this" rectangle is in the argument rectangle (the searching area)
bool Rectangle::intersect(const Rectangle& rectangle1, const Rectangle& rectangle2){
    return !(rectangle1.bottomLeft.x > rectangle2.topRight.x || rectangle1.bottomLeft.y > rectangle2.topRight.y 
            || rectangle1.topRight.x < rectangle2.bottomLeft.x || rectangle1.topRight.y < rectangle2.bottomLeft.y);
}

// Checking if a smaller rectangle is totally inside the other
bool Rectangle::includes(const Rectangle& smallerRectangle, const Rectangle& largeRectangle){
    return (smallerRectangle.bottomLeft.x > largeRectangle.bottomLeft.x && smallerRectangle.bottomLeft.y > largeRectangle.bottomLeft.y
            && smallerRectangle.topRight.x < largeRectangle.topRight.x && smallerRectangle.topRight.y < largeRectangle.topRight.y);
}

// Checking if a point is insight a rectangle
bool Rectangle::isIn(const Point& point, const Rectangle& rectangle){
    return (point.x >= rectangle.bottomLeft.x && point.x <= rectangle.topRight.x) && (point.y >= rectangle.bottomLeft.y && point.y <= rectangle.topRight.y);
}



// --- QuadTree ---
QuadTree::QuadTree(const std::vector<Point>& collectionPoints, const std::size_t bucketCapacity){
    capacity = bucketCapacity;
    
    if (collectionPoints.empty()){
        throw std::invalid_argument("Empty Initial Vector"); 
    } else {
        // First initialization of the quadtree this way
        Rectangle largeBoundary = Rectangle::buildingBoundary(collectionPoints);
        boundary = largeBoundary;
        points = collectionPoints;

        if (collectionPoints.size() > bucketCapacity){
            subdivide(collectionPoints, bucketCapacity, boundary);
        }
    }
}

// The main constructor used after 
QuadTree::QuadTree(const Rectangle& largeBoundary, const std::vector<Point>& collectionPoints, const std::size_t bucketCapacity){
    capacity = bucketCapacity;
    boundary = largeBoundary;
    points = collectionPoints;

    if (collectionPoints.size() > bucketCapacity){
        subdivide(collectionPoints, bucketCapacity, boundary);
    }
}

void QuadTree::subdivide(const std::vector<Point>& collectionPoints, const std::size_t bucketCapacity, const Rectangle& largeBoundary){
    Point middle((largeBoundary.bottomLeft.x + largeBoundary.topRight.x)/2, (largeBoundary.bottomLeft.y + largeBoundary.topRight.y)/2);
    
    // First going throw the collections to know the size of each new collection, 
    // avoid dynamically memory allocation which is inefficient with lot of data
    int nmbPointsNW = 0, nmbPointsNE = 0, nmbPointsSW = 0, nmbPointsSE = 0;

    for (const Point& p : collectionPoints){
        if (p.x >= middle.x){
            if (p.y >= middle.y) nmbPointsNE++;
            else nmbPointsSE++;
        } else {
            if (p.y >= middle.y) nmbPointsNW++;
            else nmbPointsSW++;
        }
    }

    // Allocating memory to the vectors of points, and adding the points in the right place
    std::vector<Point> pointsNW(nmbPointsNW), pointsNE(nmbPointsNE), pointsSW(nmbPointsSW), pointsSE(nmbPointsSE);

    int counterNW = 0, counterNE = 0, counterSW = 0, counterSE = 0;

    for (const Point& p : collectionPoints){
        if (p.x >= middle.x){
            if (p.y >= middle.y) pointsNE[counterNE++] = p;
            else pointsSE[counterSE++] = p;
        } else {
            if (p.y >= middle.y) pointsNW[counterNW++] = p;
            else pointsSW[counterSW++] = p;
        }
    }

    // Creating the new leaves (unique pointers to sons)
    Rectangle NWrectangle = Rectangle(Point(largeBoundary.bottomLeft.x, middle.y), Point(middle.x, largeBoundary.topRight.y));
    Rectangle NErectangle = Rectangle(middle, largeBoundary.topRight);
    Rectangle SWrectangle = Rectangle(largeBoundary.bottomLeft, middle);
    Rectangle SErectangle = Rectangle(Point(middle.x, largeBoundary.bottomLeft.y), Point(largeBoundary.topRight.x, middle.y));

    NW = std::make_unique<QuadTree>(NWrectangle, pointsNW, bucketCapacity);
    NE = std::make_unique<QuadTree>(NErectangle, pointsNE, bucketCapacity);
    SW = std::make_unique<QuadTree>(SWrectangle, pointsSW, bucketCapacity);
    SE = std::make_unique<QuadTree>(SErectangle, pointsSE, bucketCapacity);
}

// To get a vector of all the rectangles of the QuadTree
std::vector<Rectangle> QuadTree::getRectangles(){
    std::vector<Rectangle> rectangles;

    // Recursively 
    if (NW){
        std::vector<Rectangle> NWfoundRectangles = NW->getRectangles();
        rectangles.insert(rectangles.end(), NWfoundRectangles.begin(), NWfoundRectangles.end());
    }
    if (NE){
        std::vector<Rectangle> NEfoundRectangles = NE->getRectangles();
        rectangles.insert(rectangles.end(), NEfoundRectangles.begin(), NEfoundRectangles.end());
    }
    if (SW){
        std::vector<Rectangle> SWfoundRectangles = SW->getRectangles();
        rectangles.insert(rectangles.end(), SWfoundRectangles.begin(), SWfoundRectangles.end());
    }
    if (SE){
        std::vector<Rectangle> SEfoundRectangles = SE->getRectangles();
        rectangles.insert(rectangles.end(), SEfoundRectangles.begin(), SEfoundRectangles.end());
    }

    // We're at the leaf
    if (!NW && !NE && !SW && !SE){
        rectangles.emplace_back(boundary.bottomLeft, boundary.topRight);
    } 

    return rectangles;
}

// To get a vector of all the points of the QuadTree
std::vector<std::vector<Point>> QuadTree::getVectorOfVectorOfPoints(){
    std::vector<std::vector<Point>> vectorOfVectorOfPoints;

    // Recursively 
    if (NW){
        std::vector<std::vector<Point>> NWfoundVectorOfVectorOfPoints = NW->getVectorOfVectorOfPoints();
        vectorOfVectorOfPoints.insert(vectorOfVectorOfPoints.end(), NWfoundVectorOfVectorOfPoints.begin(), NWfoundVectorOfVectorOfPoints.end());
    }
    if (NE){
        std::vector<std::vector<Point>> NEfoundVectorOfVectorOfPoints = NE->getVectorOfVectorOfPoints();
        vectorOfVectorOfPoints.insert(vectorOfVectorOfPoints.end(), NEfoundVectorOfVectorOfPoints.begin(), NEfoundVectorOfVectorOfPoints.end());
    }
    if (SW){
        std::vector<std::vector<Point>> SWfoundVectorOfVectorOfPoints = SW->getVectorOfVectorOfPoints();
        vectorOfVectorOfPoints.insert(vectorOfVectorOfPoints.end(), SWfoundVectorOfVectorOfPoints.begin(), SWfoundVectorOfVectorOfPoints.end());
    }
    if (SE){
        std::vector<std::vector<Point>> SEfoundVectorOfVectorOfPoints = SE->getVectorOfVectorOfPoints();
        vectorOfVectorOfPoints.insert(vectorOfVectorOfPoints.end(), SEfoundVectorOfVectorOfPoints.begin(), SEfoundVectorOfVectorOfPoints.end());
    }

    // We're at the leaf
    if (!NW && !NE && !SW && !SE){
        vectorOfVectorOfPoints.push_back(points);
    }

    return vectorOfVectorOfPoints;
}

// The method query using the structure of the QuadTree to perform well
std::vector<Point> QuadTree::query(const Rectangle& searchingArea){
    std::vector<Point> foundPoints;

    // If no overlap return empty vector
    if (!Rectangle::intersect(boundary, searchingArea)){
        return foundPoints;
    }

    // If the searching area includes the entire boundary
    if (Rectangle::includes(boundary, searchingArea)){
        return points;
    }

    // Check if at least one child is not empty
    if (NW){
        foundPoints.reserve(foundPoints.size() + points.size());

        std::vector<Point> NWpoints = NW->query(searchingArea);
        foundPoints.insert(foundPoints.end(), std::make_move_iterator(NWpoints.begin()), std::make_move_iterator(NWpoints.end()));

        std::vector<Point> NEpoints = NE->query(searchingArea);
        foundPoints.insert(foundPoints.end(), std::make_move_iterator(NEpoints.begin()), std::make_move_iterator(NEpoints.end()));

        std::vector<Point> SWpoints = SW->query(searchingArea);
        foundPoints.insert(foundPoints.end(), std::make_move_iterator(SWpoints.begin()), std::make_move_iterator(SWpoints.end()));

        std::vector<Point> SEpoints = SE->query(searchingArea);
        foundPoints.insert(foundPoints.end(), std::make_move_iterator(SEpoints.begin()), std::make_move_iterator(SEpoints.end()));
    } else {
        // If we're at the leaf, adding the points
        return pointsIn(searchingArea);
    }

    return foundPoints;
}

// A method that returns the points in a given searching area
std::vector<Point> QuadTree::pointsIn(const Rectangle& searchingArea){
    std::vector<Point> foundPoints;
    for (const Point& point : points){
        if (Rectangle::isIn(point, searchingArea)) foundPoints.emplace_back(point.x, point.y);
    }
    return foundPoints;
}