#include "quadTree.hpp"
#include "utilities/point_reader.hpp"
#include "utilities/random_points.hpp"
#include "utilities/timer.hpp"
#include <typeinfo>

int main(){
    using namespace sf;

    // QUADTREE
    // Opening data
    auto points = sf::readCsvPoints<Point>("./test_data/swe.csv");

    // Creating the QuadTree (catching error if the input vector is empty)
    size_t bucketCapacity = 64;
    try{
        QuadTree tree(points, bucketCapacity);


        // Writing the Python plotting file of this tree
        sf::MplWriter<Point, Rectangle> writer1("plotQuadTree.py");

        std::vector<Rectangle> rectangles = tree.getRectangles();
        for (const Rectangle& rect : rectangles){
            writer1 << rect;
        }

        std::vector<std::vector<Point>> vectorOfVectorOfPoints = tree.getVectorOfVectorOfPoints();
        for (const std::vector<Point>& points : vectorOfVectorOfPoints){
            writer1 << points;
        }
    } catch (const std::invalid_argument& e){
        std::cerr << "Error in QuadTree creation : " << e.what() << std::endl;
    }


    // QUERY METHOD 
    // Generating random data
    sf::RandomPointGenerator<Point> generator1{2565};
    generator1.addUniformPoints(1000, Point(1.0, 1.0));
    generator1.addNormalPoints(1000, Point(1.0, 1.0));
    auto randomPoints1 = generator1.takePoints();

    // Creating the QuadTree (catching error if the input vector is empty)
    try{
        // Using query method
        QuadTree treeRandom1(randomPoints1, 10);
        Rectangle searchingArea(Point(-0.6, -1.6), Point(0.7, 0.4));
        std::vector<Point> foundPoints = treeRandom1.query(searchingArea);

        // Plotting the QuadTree
        MplWriter<Point, Rectangle> writer2("plotRandomPoints.py");

        std::vector<Rectangle> rectangles = treeRandom1.getRectangles();
        for (const Rectangle& rect : rectangles){
            writer2 << rect;
        }

        std::vector<std::vector<Point>> vectorOfVectorOfPoints = treeRandom1.getVectorOfVectorOfPoints();
        for (const std::vector<Point>& points : vectorOfVectorOfPoints){
            writer2 << points;
        }

        // Adding the searching area (don't forget to change its color in the python file)
        writer2 << searchingArea;
        // Adding the found points (don't forget to change their color in the python file)
        writer2 << foundPoints;

    } catch (const std::invalid_argument& e){
        std::cerr << "Error in QuadTree creation : " << e.what() << std::endl;
    }


    // PERFORMANCE
    // 1
    std::cout << "Test performance normal distribution 1 \n";

    // Generating random data
    sf::RandomPointGenerator<Point> generator2{2565};
    //generator2.addUniformPoints(100000000, Point(0, 0));
    generator2.addNormalPoints(1000000);
    auto randomPoints2 = generator2.takePoints();
    sf::Timer timer;

    try{
        std::cout << " * Generating QuadTree : ";
        timer.start();
        QuadTree treeRandom2(randomPoints2, 128);
        timer.stop();

        Rectangle searchingArea(Point(-0.6, -0.6), Point(-0.4, -0.4));

        std::cout << " * Searching using query : ";
        timer.start();
        std::vector<Point> foundPoints = treeRandom2.query(searchingArea);
        timer.stop();

        std::cout << " * Searching using brute force : ";
        timer.start();
        std::vector<Point> foundPointsBrute;
        for (const Point& p : randomPoints2){
            if (Rectangle::isIn(p, searchingArea)) foundPointsBrute.emplace_back(p.x, p.y);
        }
        timer.stop();

        std::cout << " * Percentage of foundPoints in respect to the global size : " << foundPoints.size()*1.0/randomPoints2.size()*100 << " %\n";

    } catch (const std::invalid_argument& e){
        std::cerr << "Error in QuadTree creation : " << e.what() << std::endl;
    }

    return 0;
}