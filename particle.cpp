
#include "particle.h"

#include <glm/gtc/random.hpp>

particle2::particle2():xObject()
{
    name="particle2";
    d_size=200;

    Texture *_tex = new Texture("particle.png"); //
    texname=_tex->getTextureName();

    shader=new Shader();
    shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");

    motes = (struct_motes*)malloc(sizeof(struct_motes)*d_size);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    for ( int i=0 ; i < d_size ;i++ )
    {
        vertex vert;
        float x,y,z;
        x=0; //rand()%1000 - 500;
        y=0; //rand()%1000 - 500;
        z=0; //rand()%1000 - 500;

        //glm::vec4 v1=glm::vec4(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z,1.0);
        //glm::vec4 new_v = m1 * v1;
        vert.v[0]=x;
        vert.v[1]=y;
        vert.v[2]=z;
        vertexes.push_back(vert);
        motes[i].life=glm::linearRand(500.0,9000.0);
        //motes[i].force=glm::sphericalRand(30);
        motes[i].force=glm::ballRand(30.0);
        //printf("x=%f  y=%f  z=%f \n",x,y,z );
        //printf("x=%f  y=%f  z=%f \n",motes[i].force.x ,motes[i].force.y ,motes[i].force.z);
    }

    flag_axis_on=true;
    make_axis();//
    update_VBO();
}

void particle2::update_VBO()
{

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * d_size /* bytes */, vertexes, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertexes.size()*sizeof(vertex), vertexes.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0); // input to vertex-shader index 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex)/* bytes */, (void*)0);
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glVertexPointer(3, GL_FLOAT,sizeof(float) * 3 /* bytes */, (void*)0);
    glVertexAttribI1i(4, 4); //glVertexAttrib1f(4, 2); // 고정값: type = 4

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    glBindVertexArray(0); // unbind

}

void particle2::update(float dt)
{
    for ( int i=0 ; i < d_size ;i++ )
    {
        //glm::vec3 v=glm::make_vec3(&vertexData[i*3]);
        vertexes[i].v[0] += motes[i].force.x *dt;
        vertexes[i].v[1] += motes[i].force.y *dt;
        vertexes[i].v[2] += motes[i].force.z *dt;
        ///motes[i].force;
       // printf("x=%f  y=%f  z=%f \n",x,y,z);
        motes[i].life -= dt;
    }
    update_VBO();
}

void particle2::draw()
{
    update_model_matrix();
   //model = glm::mat4(1.0); // identity
    float _m[16];
    if (parent)
    {
        ///loadIdentity(_m);
        //set(_m,parent->model_m);
        //mat4x4_mult(_m, parent->model_mat, model_mat);
    }
    //else mat4x4_set(_m, model_mat); // copy


    if (VAO>0)
    {
        if (texname > 0)
        {
            //glEnable(GL_BLEND);
            //glDisable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glBindTexture(GL_TEXTURE_2D, texname);
            glEnable(GL_POINT_SPRITE);
            //glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        }

        GLint location;
        glDisable(GL_DEPTH_TEST);

        //shader->setMat4("model",_m1);
        shader->setMat4("model",model); // works

        /* location = glGetUniformLocation(shader->mProgram, "model");
        if (location >= 0)
            glUniformMatrix4fv(location, 1, GL_FALSE, _m); // ( location, count,  transpose, float *value )
        */

        location = glGetUniformLocation(shader->mProgram, "ourTexture");
        if(location >=0) glUniform1i(location, 0); // 0: GL_TEXTURE0 works!!! 0값이 전달될때 sampler2D로 변환되는것 같다.

        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(5);
        glBindVertexArray(VAO);
        //glVertexAttribI1i(4, 3); //glVertexAttrib1f(4, 2);
        glDrawArrays(GL_POINTS, 0, d_size); // 0~2000

        glBindVertexArray(0); // break
        //glDisableVertexAttribArray(0);
        glDisable(GL_POINT_SPRITE);
    }
    draw_axis();
}
