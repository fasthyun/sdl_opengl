#include <vector>
#include <string>
#include <iostream>
using namespace std;

struct vertex {
    float v[3]; // padding 4byte
    float tu,tv; //
    float r,g,b,a;
    float normal[3];
    int32_t type;  // 오히려 triangle에 type이 있어야하지 않나? --> particle도 있음
};


class xObject {

public:
    xObject(){};
    virtual ~xObject();  // virtual why?
    virtual void update(float dt=0);
    virtual void draw();
    vector<vertex>  vertexes; // verts

    float forward[3]; //
    float up[3];
    float force[3];
    string name;
    string uuid;
    int d_type;
    bool xobject_found=false;
    string path; // tmp
    float radius; // tmp
    bool flag_axis_on=false;
    bool flag_shader_global=false;
    bool flag_gravity=false;
};

vector<xObject* > objects;


int main(int argc, char *argv[])
{
    cout << "sizeof(float) = "  << sizeof(float) << "bytes" << endl;



    for ( int i=0 ; i < objects.size() ; i++)
    {
        xObject *obj= objects[i];
        std::cout << i << "," << obj->name << " : " << obj << "\n";
    }


    return 0;
}
