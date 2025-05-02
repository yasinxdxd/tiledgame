#ifndef MY_OBJ_LOADER_HPP
#define MY_OBJ_LOADER_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <exception>

struct Vertex
{
    template<typename _T = float>
    struct vec3
    {
        _T x, y, z;
        _T& operator[](unsigned int index)
        {
            switch (index)
            {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                throw std::out_of_range("vec3 element size 3");
            }
        }
    };

    template<typename _T = float>
    struct vec2
    {
        _T x, y;
        _T& operator[](unsigned int index)
        {
            switch (index)
            {
            case 0:
                return x;
            case 1:
                return y;
            default:
                throw std::out_of_range("vec2 element size 2");
            }
        }
    };

    Vertex(float _position[3], float _texcoord[2], float _normal[3])
    {
        position[0] = _position[0];
        position[1] = _position[0];
        position[2] = _position[0];
        texcoord[0] = _texcoord[0];
        texcoord[1] = _texcoord[0];
        normal[0] = _normal[0];
        normal[1] = _normal[0];
        normal[2] = _normal[0];
    }
    Vertex()
    {
        position[0] = 0.f;
        position[1] = 0.f;
        position[2] = 0.f;
        texcoord[0] = 0.f;
        texcoord[1] = 0.f;
        normal[0] = 0.f;
        normal[1] = 0.f;
        normal[2] = 0.f;
    }

    vec3<float> position;
    vec2<float> texcoord;
    vec3<float> normal;
};


bool load_obj(const char* file_path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    //std::vector<Vertex> vertices;
    vertices.clear();

    //v,vn,vt values of the .obj files
    std::vector<Vertex::vec3<float>> position_vertices;
    std::vector<Vertex::vec2<float>> textcoord_vertices;
    std::vector<Vertex::vec3<float>> normal_vertices;

    //f values of the .obj files
    //std::vector<unsigned int> indices;

    //reading
    std::stringstream ss;
    std::string line = "";
    std::string prefix = "";
    std::fstream file(file_path);

    //temps
    Vertex::vec3<float> temp_vec3;
    Vertex::vec2<float> temp_vec2;
    unsigned int temp_int;


    //error check
    if (!file.is_open())
        std::cerr << "file couldn't opened!" << std::endl;

    while (std::getline(file, line))
    {
        ss.clear();
        ss.str(line);
        if (int(line.c_str()[0]) == 0)
            continue;

        ss >> prefix;
        static int i = 0;
        //std::cout << ++i << ": " <<  << std::endl;

        if (prefix == "#" || prefix == "o" || prefix == "s" || prefix == "g" || prefix == "\n")
            continue;
        else if (prefix == "v")
        {
            ss >> temp_vec3[0] >> temp_vec3[1] >> temp_vec3[2];
            position_vertices.push_back(temp_vec3);
        }
        else if (prefix == "vt")
        {
            ss >> temp_vec2[0] >> temp_vec2[1];
            textcoord_vertices.push_back(temp_vec2);
        }
        else if (prefix == "vn")
        {
            ss >> temp_vec3[0] >> temp_vec3[1] >> temp_vec3[2];
            normal_vertices.push_back(temp_vec3);
        }
        else if (prefix == "f")
        {
            unsigned int counter = 0;
            while (ss >> temp_int)
            {
                indices.push_back(temp_int - 1); // 0th index!!!
                //TODO: add supprt for vt and vn
            }

        }
        else
            continue;
        /*
        */

    }

    file.close();

    
    //setting values of vertex and pushing the vertex in result vertices 
    
    vertices.resize(position_vertices.size(), Vertex());
    for (size_t i = 0; i < position_vertices.size(); i++)
    {
        vertices[i].position.x = position_vertices[i].x;
        vertices[i].position.y = position_vertices[i].y;
        vertices[i].position.z = position_vertices[i].z;
    }

    std::cout << "vertices::size() -> " << vertices.size() << std::endl;
    std::cout << "indices::size() -> " << indices.size() << std::endl;
    
    return true;
}
#endif//MY_OBJ_LOADER_HPP