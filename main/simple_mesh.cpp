#include "simple_mesh.hpp"

SimpleMeshData concatenate( SimpleMeshData aM, SimpleMeshData const& aN )
{
	aM.positions.insert( aM.positions.end(), aN.positions.begin(), aN.positions.end() );
	aM.colors.insert( aM.colors.end(), aN.colors.begin(), aN.colors.end() );
	aM.normals.insert( aM.normals.end(), aN.normals.begin(), aN.normals.end() );
    aM.texcoords.insert( aM.texcoords.end(), aN.texcoords.begin(), aN.texcoords.end() );
	return aM;
}

GLuint create_vao( SimpleMeshData const& aMeshData )
{
    GLuint positionVBO = 0;
    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData( GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW );
    

    GLuint colorVBO = 0;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData( GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW );

    GLuint normalsVBO = 0;
    glGenBuffers(1, &normalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glBufferData( GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW );

    // Texture VBO
    GLuint textureVBO = 0;
    glGenBuffers(1, &textureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f), aMeshData.texcoords.data(), GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Position VBO
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Color VBO
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    
    // Normal VBO
    glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Texture VBO - index 3
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);


    return vao;
}

GLuint create_rectangle(const Vec3f& topLeft, const Vec3f& topRight, const Vec3f& bottomLeft, const Vec3f& bottomRight, SimpleMeshData& rectangle) {
    
    Vec3f norm = { 0.f, 0.f, 0.f };
    Vec2f texcoord = { 0.f, 0.f };
    rectangle.normals.push_back(norm);
    rectangle.texcoords.push_back(texcoord);

    rectangle.positions.push_back(topLeft);
    rectangle.positions.push_back(topRight);
    rectangle.positions.push_back(bottomLeft);
    rectangle.positions.push_back(bottomLeft);
    rectangle.positions.push_back(topRight);
    rectangle.positions.push_back(bottomRight);

    for (int i = 0; i < 6; ++i) {
      rectangle.colors.push_back({1.0f, 1.0f, 1.0f});
    }
    
    GLuint out_rectangle = create_vao(rectangle);
    
    return out_rectangle;
}