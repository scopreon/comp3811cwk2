#ifndef SPACESHIP_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB
#define SPACESHIP_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB

#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"


SimpleMeshData make_spaceship(
	std::size_t aSubdivs = 16,
	Mat44f aPreTransform = kIdentity44f
);

#endif // SPACESHIP_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB
