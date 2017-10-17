//#define USEBASICX
#ifdef USEBASICX
#include "BasicX\BasicX.h"
using namespace BasicX;
#else
#include "Simplex\Simplex.h"
using namespace Simplex;
#endif // USEBASICX

#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include "SFML\OpenGL.hpp"

/// Object tracks the current and target position of the orbit to simplify lerping.
class Planet {
    uint numSides; // the dimensions of the orbital path the planet follows
    uint targetPositionIndex; // the current target position to lerp to
    std::vector<vector3> points; // a collection of points representing vertices in the orbit

public:
    Planet(uint numSides, float radius) {
        this->numSides = numSides;
        this->targetPositionIndex = 1;
        // create points along the orbit
        for (uint i = 0; i < numSides; i++) {
            float angle = 3.1415f * 2 / numSides;
            float xPoint = cosf(angle * i);
            float yPoint = sinf(angle * i);
            points.push_back(vector3(xPoint, yPoint, 0) * radius);
        }
    }

    // Access the vector point to lerp towards
    vector3 Planet::getTargetPosition() {
        return points[targetPositionIndex];
    }
    
    // Access the vector point to lerp from
    vector3 Planet::getStartingPosition() {
        if (targetPositionIndex == 0) {
            return points[points.size() - 1];
        }
        return points[targetPositionIndex - 1];
    }

    // Handles looping around the orbit without jumping
    void Planet::advanceToNextPos() {
        targetPositionIndex++;
        if (targetPositionIndex > numSides - 1) {
            targetPositionIndex = 0;
            return;
        }
    }

};