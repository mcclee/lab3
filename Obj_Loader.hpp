
#ifndef Loader_hpp
#define Loader_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm.hpp>
#include <algorithm>


class Obj_Loader{
    
public:
    void  static load_obj(const char* filename, std::vector<glm::vec3> &vertices,std::vector<unsigned int> &vertexIndices) ;

};



void  Obj_Loader :: load_obj(const char* filename, std::vector<glm::vec3> &vertices,std::vector<unsigned int> &vertexIndices)
{
    std::vector< glm::vec3 > cur_vertices;
    std::vector< glm::vec2 > uvs;
    std::vector< glm::vec3 > normalize;
    
    std::ifstream in;
    std::string newline,word;
    in.open(filename);
    if(!in){
        printf("Impossible to open the file !\n");
        return ;
    }
    while(getline(in,newline)){
        std::istringstream iss(newline);
        iss>>word;
        if(word =="v"){
            glm::vec3 vertex;
            iss>>vertex.x>>vertex.y>>vertex.z;
            vertices.push_back(vertex);
        }
        else if(word == "f"){
            std::string fwordline;
            unsigned int a[12];
            iss>>fwordline;
            std::size_t found = fwordline.find('/');
            if (found == std::string::npos){
                std::istringstream f_is(newline);
                char ch;
                f_is>>ch;
                f_is>>a[0]>>a[1]>>a[2];
                vertexIndices.push_back(a[0]-1);
                vertexIndices.push_back(a[1]-1);
                vertexIndices.push_back(a[2]-1);
                if(!f_is.eof()){
                    f_is>>a[3];
                    vertexIndices.push_back(a[0]-1);
                    vertexIndices.push_back(a[2]-1);
                    vertexIndices.push_back(a[3]-1);
                }
            }
            else{
                replace(newline.begin(),newline.end(),'/',' ');
                std::istringstream f_is(newline);
                std::string temp;
                unsigned int vertexindice = 0;
                int count = 0;
                f_is>>temp;
                while(f_is>>vertexindice){
                    a[count] = vertexindice;
                    count++;
                }
                if (count == 6){
                    vertexIndices.push_back(a[0]);
                    vertexIndices.push_back(a[2]);
                    vertexIndices.push_back(a[4]);
                } else if(count == 8){
                    vertexIndices.push_back(a[0]);
                    vertexIndices.push_back(a[2]);
                    vertexIndices.push_back(a[4]);
                    vertexIndices.push_back(a[0]);
                    vertexIndices.push_back(a[4]);
                    vertexIndices.push_back(a[6]);
                }
                else if(count == 9) {
                    vertexIndices.push_back(a[0]);
                    vertexIndices.push_back(a[3]);
                    vertexIndices.push_back(a[6]);
                }
                else if(count>9){
                    vertexIndices.push_back(a[0]);
                    vertexIndices.push_back(a[3]);
                    vertexIndices.push_back(a[6]);
                    vertexIndices.push_back(a[0]);
                    vertexIndices.push_back(a[6]);
                    vertexIndices.push_back(a[9]);
                }
            }
        }
        
    }
    in.close();
    
    
}


#endif /* Loader_hpp */
