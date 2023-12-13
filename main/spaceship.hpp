#ifndef SPACESHIP_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB
#define SPACESHIP_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB

#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"

class Spaceship
{
public:
    Spaceship(std::size_t aSubdivs = 16,
	Mat44f aPreTransform = kIdentity44f);



    void update(float ts);
    void render(Mat44f projCameraWorld);
    int numVertices;
    
    float speed;
    float angle;


    Vec3f offset;
    Vec3f location;
    // void Spawn(const ParticleInit& particleInit);
private:
	GLuint spaceshipVAO;
};


#endif // SPACESHIP_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB
