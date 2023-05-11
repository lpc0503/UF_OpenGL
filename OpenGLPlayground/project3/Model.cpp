#include "Model.h"
#include "Log.h"

#include <tiny_obj_loader.h>
#include <map>
#include "objloader.hpp"
#include "vboindexer.hpp"

Model::Model(const std::string &path)
{
    m_Path = path;
    LoadModel(path);
}

Ref<Model> Model::LoadModel(const std::string &path)
{
    Ref<Model> model = MakeRef<Model>();
    model->m_Path = path;

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = model->GetDirectory();
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config)) {
        if (!reader.Error().empty()) {
            ERROR_TAG_STR("TinyObjReader", reader.Error());
        }
        return nullptr;
    }

    if (!reader.Warning().empty()) {
        INFO_TAG_STR("TinyObjReader", reader.Warning());
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    for (size_t s = 0; s < shapes.size(); s++)
    {
        Ref<Mesh> mesh = MakeRef<Mesh>();
//        std::map<Vertex, uint64_t> vertexToId;

        size_t index_offset = 0;
        auto shape = shapes[s];
        mesh->m_Name = shape.name;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shape.mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                Vertex vertex{};
                vertex.pos.x = vx;
                vertex.pos.y = vy;
                vertex.pos.z = vz;
                vertex.pos.w = 1.f;
//                INFO("v {} {} {} {}", idx.vertex_index, vx, vy, vz);

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0)
                {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    vertex.normal.x = nx;
                    vertex.normal.y = ny;
                    vertex.normal.z = nz;
//                    INFO("n {} {} {} {}", idx.normal_index, nx, ny, nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
//                if (idx.texcoord_index >= 0)
//                {
//                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
//                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
//                }

                // Optional: vertex colors
                 tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                 tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                 tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                 vertex.color = glm::vec4{red, green, blue, 1.f};
//                 INFO("vc {} {} {}", red, green, blue);

                // TODO: figure out why this doesn't work
//                if(!vertexToId.count(vertex))
//                {
//                    vertexToId[vertex] = mesh->m_Vertices.size();
//                    mesh->m_Vertices.push_back(vertex);
//                    INFO("new {}", vertex);
//                }
//                else
//                    INFO("else {}", vertex);
//                mesh->m_Indices.push_back(vertexToId[vertex]);

                auto it = std::find(mesh->m_Vertices.begin(), mesh->m_Vertices.end(), vertex);
                // Not found
                if(it == mesh->m_Vertices.end())
                {
                    mesh->m_Indices.push_back(mesh->m_Vertices.size());
                    mesh->m_Vertices.push_back(vertex);
                }
                else
                {
                    auto idx = std::distance(mesh->m_Vertices.begin(), it);
                    mesh->m_Indices.push_back(idx);
                }
            }
            index_offset += fv;

            // per-face material
            shape.mesh.material_ids[f];
        }

        model->m_Meshes.push_back(mesh);
    }

    return model;
}

Ref<Model> Model::LoadModel(const std::string &path, glm::vec4 color) {

    Ref<Model> model = MakeRef<Model>();
    model->m_Path = path;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ(path.c_str(), vertices, uvs, normals);
//
    std::vector<GLushort> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

    const size_t vertCount = indexed_vertices.size();
    const size_t idxCount = indices.size();

    // populate output arrays
    Ref<Mesh> mesh = MakeRef<Mesh>();
    mesh->m_Name = "test";
    for (int i = 0; i < vertCount; i++) {

        Vertex vertex{};
        vertex.pos = glm::vec4 {indexed_vertices[i].x, indexed_vertices[i].y, indexed_vertices[i].z, 1.f}; // TODO: 為什麼 1 就會過?
        vertex.normal = glm::vec3 {indexed_normals[i].x, indexed_normals[i].y, indexed_normals[i].z};
        vertex.color = color;
        vertex.uv = glm::vec2{indexed_uvs[i].x, indexed_uvs[i].y};
        mesh->m_Vertices.push_back(vertex);
    }

    for (int i = 0; i < idxCount; i++) {

        mesh->m_Indices.push_back(indices[i]);
    }

    model->m_Meshes.push_back(mesh);
    return model;
}
