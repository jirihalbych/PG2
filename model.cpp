#include "mesh.cpp"
#include "tangentspace.cpp"

class Model{
    std::vector<Mesh> meshes;
public:
    Model(const char* file_name) {

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        bool res = loadOBJ(file_name, vertices, uvs, normals);

        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        computeTangentBasis(
                vertices, uvs, normals, // input
                tangents, bitangents    // output
        );
        std::vector<unsigned short> indices;
        std::vector<glm::vec3> indexed_vertices;
        std::vector<glm::vec2> indexed_uvs;
        std::vector<glm::vec3> indexed_normals;
        std::vector<glm::vec3> indexed_tangents;
        std::vector<glm::vec3> indexed_bitangents;
       indexVBO_TBN(
               vertices, uvs, normals, tangents, bitangents,
               indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents
        );
        //indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
        //texture + mtl
        meshes.emplace_back(indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents);
    }

    Model() {}

    void draw(){
        for (auto &mesh: meshes) {
            mesh.draw();
        }
    }

    void clear(){
        for (auto &mesh: meshes) {
            mesh.clear();
        }
    }
};