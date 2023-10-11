#include "Model.h"
#include "Log.h"

#include <tiny_obj_loader.h>
#include <map>
#include "objloader.hpp"
#include "vboindexer.hpp"
#include <glm/glm.hpp>
#include <glm/vector_relational.hpp>
#include <sstream>

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

    std::map<glm::vec4, glm::vec3> m;

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
    std::vector<glm::vec3> indexed_normalsforavg;
    indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_normalsforavg);

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

    struct normalcnt {

        glm::vec3 normal = glm::vec3(0.f, 0.f, 0.f);
        double cnt = 0.f;
    };

    std::map<uint32_t, normalcnt> m;
    for(int i = 0 ; i < indices.size() ; i++) {

        m[indices[i]].normal += indexed_normalsforavg[i];
        m[indices[i]].cnt += 1;
    }

    for(int i = 0 ; i < vertCount ; i++) {

        auto xx = m.find(i);
        mesh->m_Vertices[i].normal = xx->second.normal/float(xx->second.cnt);
    }

    for (int i = 0; i < idxCount; i++) {

        mesh->m_Indices.push_back(indices[i]);
    }

    model->m_Meshes.push_back(mesh);
    return model;
}

Ref<Model> Model::LoadModel(const std::string &path, glm::vec3 color) {

    Ref<Model> model = MakeRef<Model>();

    std::stringstream ss;
    std::vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;

    std::string str = "assets/model/";
    std::string err;
    if ( !tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &err, path.c_str(), str.c_str()) )
    {
        throw std::runtime_error("Failed to load model : " + str);
        return nullptr;
    }
//    INFO("{}", attrib.vertices.size());

    if (materials.size() == 0)
    {
        throw std::runtime_error("No material found in model : " + path);
        return nullptr;
    }

    //< There is a bug here: err may contain multiple '\n' terminated string
    //< Research how to output multiple line log by spdlog
    if (!err.empty())
        printf("%s", err.c_str());

    model->m_Meshes.resize(materials.size());
    for(auto &e : model->m_Meshes) {

        e = MakeRef<Mesh>();
    }

    //< is this macro: FLT_MAX OS dependent ?
    //< should always prefer os independent ones
    glm::vec3 pmin(FLT_MAX), pmax(-FLT_MAX);


    // Additional data structure for storing the adjacency information
    std::map<int, std::vector<glm::vec3>> vertex_to_normals;

//    std::vector<SimpleVertex> silh_vertices;
    std::vector<Vertex> vec;
    int maxindice = 0;

    // Loop over shapes
    for (const auto &shape : shapes)
    {
        size_t index_offset = 0, face = 0;

        //< Loop over all faces(polygon) in a mesh. vertex number of face maybe variable
        //< for triangle num_face_vertices = 3
        for (const auto &num_face_vertex : shape.mesh.num_face_vertices)
        {
            int mat = shape.mesh.material_ids[face];
            // Loop over triangles in the face.
            for (size_t v = 0; v < num_face_vertex; ++v)
            {
                tinyobj::index_t index = shape.mesh.indices[index_offset + v];

//                model->m_Meshes.at(mat)->m_Vertices.emplace_back(); //< use "material id" to index mesh : mesh <--> material id is one-one map
                vec.emplace_back();
//                auto &vert = model->m_Meshes.at(mat)->m_Vertices.back();
                auto &vert = vec.back();
                model->m_Meshes.at(mat)->m_Indices.push_back(index.vertex_index);
                maxindice = std::max(index.vertex_index, maxindice);

                vert.pos =
                        {
                                attrib.vertices[3 * index.vertex_index + 0],
                                attrib.vertices[3 * index.vertex_index + 1],
                                attrib.vertices[3 * index.vertex_index + 2],
                                1.0f
                        };
                vert.color = glm::vec4(color, 1.f);
                pmin = glm::min(glm::vec3(vert.pos), pmin);
                pmax = glm::max(glm::vec3(vert.pos), pmax);
                if (~index.normal_index) //< -1 == 0xFFFFFFFF, it is equal to if (index.normal_index != -1)
                {

                    glm::vec3 normal =
                            {
                                    attrib.normals[3 * index.normal_index + 0],
                                    attrib.normals[3 * index.normal_index + 1],
                                    attrib.normals[3 * index.normal_index + 2]
                            };


                    vertex_to_normals[index.vertex_index].push_back(normal);
                }
                else
                {
                    throw std::runtime_error("No normal channel found in vertex");
                    return nullptr;
                }
            }
            index_offset += num_face_vertex;
            face++;
        }
    }

    // Average the normals
    for (auto& mesh : model->m_Meshes) {

        for (const auto& [vertexIndex, vertexNormals] : vertex_to_normals) {
            glm::vec3 sum(0.0f);
            for (const glm::vec3& normal : vertexNormals) {
                sum += glm::normalize(normal);
            }
            glm::vec3 averagedNormal = glm::normalize(sum / static_cast<float>(vertexNormals.size()));

            // Update all vertices with this index

            auto indice = model->m_Meshes.back()->m_Indices;
            for(int i = 0 ; i < indice.size() ; i++) {

                if(indice[i] == vertexIndex) {

                    vec[i].normal = averagedNormal;
                }
            }

            mesh->m_Vertices.resize(maxindice+1);
            for(int i = 0 ; i < vec.size() ; i++) {

                mesh->m_Vertices[indice[i]] = vec[i];
            }
        }
    }

//    Mesh silhou;
//    silhou.vertices = silh_vertices;
//    silh.push_back(silhou);
//
//    aabb.Extend(pmin);
//    aabb.Extend(pmax);

    ss << "After binning," << model->m_Meshes.size() << " meshes constructed(each mesh contains only one material)" << "\n"
       << "Mesh file : " << path << "\n"
            ;

    printf("%s \n", ss.str().c_str());

#if 0
    //< normalize all the vertex to [-1, 1]
    glm::vec3 extent3 = pmax - pmin;
    float extent = glm::max(extent3.x, glm::max(extent3.y, extent3.z)) * 0.5f;
    float inv_extent = 1.0f / extent;

    glm::vec3 center = (pmax + pmin) * 0.5f;

    for (auto &m : meshes)
        for (auto &v : m.vertices)
            v.position = (v.position - center) * inv_extent;

    Log("[SCENE]: load {} meshes from file {}\t vertex position normalized to [-1, 1]", meshes.size(), filename);

#endif

//    return aabb;


    return model;
}

Ref<Model> Model::LoadQuadModel(const std::string &path, glm::vec4 color) {

    Ref<Model> model = MakeRef<Model>();
    Ref<Mesh> mesh = MakeRef<Mesh>();

    std::vector<unsigned int> vertexIndices, normalIndices;
    std::vector<Vertex> tmp_vertices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;

    std::map<int, std::vector<glm::vec3>> vertex_to_normals;
    unsigned int maxindice = 0;

    FILE * file = fopen(path.c_str(), "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        getchar();
        return nullptr;
    }

    while(1){

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
//        INFO("{}", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3, vertex4;
            unsigned int vertexIndex[4], normalIndex[4];
            char test='a', test2;
            int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], &vertexIndex[3], &normalIndex[3] );
//            INFO("{}//{} {}//{} {}//{} {}//{}", vertexIndex[0], normalIndex[0], vertexIndex[1], normalIndex[1], vertexIndex[2], normalIndex[2], vertexIndex[3], normalIndex[3]);
            if (matches != 8){
                printf("ERROR: NO NORMALS PRESENT IN FILE! YOU NEED NORMALS FOR LIGHTING CALCULATIONS!\n");
                printf("File can't be read by our simple parser :-( Try exporting with other options. See the definition of the loadOBJ fuction.\n");
                return nullptr;
            }
            // Store normals for each vertex
            for (int i = 0; i < 4; ++i) {
                vertex_to_normals[vertexIndex[i] - 1].push_back(temp_normals[normalIndex[i] - 1]);
                vertexIndices.push_back(vertexIndex[i]);
                mesh->m_Indices.push_back(vertexIndex[i]-1);
                maxindice = std::max(vertexIndex[i], maxindice);
            }

        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

//    INFO("{} {}", temp_vertices.size(), mesh->m_Indices.size());
//    for(auto i : mesh->m_Indices) {
//
//        INFO("{}", i);
//    }


    mesh->m_Vertices.resize(maxindice);
    for (const auto& [vertexIndex, vertexNormals] : vertex_to_normals) {

        Vertex vert;
//        INFO("{}", vertexIndex);
//        INFO("{} {} {}", temp_vertices[vertexIndex-1].x, temp_vertices[vertexIndex-1].y, temp_vertices[vertexIndex-1].z);
        vert.pos = glm::vec4(temp_vertices[vertexIndex].x, temp_vertices[vertexIndex].y, temp_vertices[vertexIndex].z, 1);
        vert.color = color;

        mesh->m_Vertices[vertexIndex].pos = vert.pos;
        mesh->m_Vertices[vertexIndex].color = vert.color;

        glm::vec3 sum(0.0f);
        for (const glm::vec3& normal : vertexNormals) {
            sum += glm::normalize(normal);
        }
        glm::vec3 averagedNormal = glm::normalize(sum / static_cast<float>(vertexNormals.size()));

        // Update all vertices with this index
        for (auto& vertexIndex_ : vertexIndices) {
            if (vertexIndex_ == vertexIndex) {
                mesh->m_Vertices[vertexIndex].normal = averagedNormal;
            }
        }
    }
    model->m_Meshes.push_back(mesh);

    return model;
}
