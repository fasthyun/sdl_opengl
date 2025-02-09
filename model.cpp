// ----------------------------------------------------------------------------
// Another Assimp OpenGL sample including texturing.
// Note that it is very basic and will only read and apply the model's diffuse
// textures (by their material ids)
//
// Don't worry about the "Couldn't load Image: ...dwarf2.jpg" Message.
// It's caused by a bad texture reference in the model file (I guess)
//
// If you intend to _use_ this code sample in your app, do yourself a favour
// and replace immediate mode calls with VBOs ...
//
// Thanks to NeHe on whose OpenGL tutorials this one's based on! :)
// http://nehe.gamedev.net/
// ----------------------------------------------------------------------------
#include "stable.h"
#include "element3d.h"
#include "object.h"
#include "model.h"
#include "texture.h"
#include "xmath.h"

//to map image filenames to textureIds
#include <map>
#include <list>
// assimp include files. These three are usually needed.
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>


int loadMaterials(const aiScene* scene) ;
void loadToObject(const struct aiScene *sc, const struct aiNode* nd, float scale, xObject *obj, int tab_level=0);


extern vector<xObject* > objects;
vector<xObject* > cached_models;  //
vector<xObject* > cached_objects; // cube , lamp, light ...


std::map<string, Material*> Materials;	// map Material name to texid

std::map<string, GLuint> Textures;	// map Material name to texid

// images / texture
std::map<string, GLuint*> textureIdMap;	// map image filenames to textureIds
GLuint*		textureIds;							// pointer to texture Array

// Create an instance of the Importer class
Assimp::Importer importer;

void createAILogger() {
    // Change this line to normal if you not want to analyze the import process
	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

	// Create a logger instance for Console Output
	Assimp::DefaultLogger::create("",severity, aiDefaultLogStream_STDOUT);

	// Create a logger instance for File Output (found in project folder or near .exe)
	Assimp::DefaultLogger::create("assimp_log.txt",severity, aiDefaultLogStream_FILE);

	// Now I am ready for logging my stuff
	Assimp::DefaultLogger::get()->info("this is my info-call");
}

void destroyAILogger() {
	Assimp::DefaultLogger::kill();
}

void logInfo(const std::string &logString) {
	Assimp::DefaultLogger::get()->info(logString.c_str());
}

void logDebug(const char* logString) {
	Assimp::DefaultLogger::get()->debug(logString);
}


std::string getBasePath(const std::string& path) {
	size_t pos = path.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
}

void freeTextureIds() {
    // no need to delete pointers in it manually here. (Pointers point to textureIds deleted in next step)
	textureIdMap.clear();

	if (textureIds) {
		delete[] textureIds;
		textureIds = nullptr;
	}
}

/*  All Setup For OpenGL goes here
int InitGL()
{
    GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat LightPosition[]= { 0.0f, 0.0f, 15.0f, 1.0f };

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClearDepth(1.0f);				// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // Uses default lighting parameters
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);

    return true;					// Initialization Went OK
} */


// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
void Color4f(const aiColor4D *color)
{
	glColor4f(color->r, color->g, color->b, color->a);
}

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void color4_to_float4(const aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

// TODO: rename
void reference_apply_material(const aiMaterial *mtl)
{
	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	aiColor4D diffuse;
	aiColor4D specular;
	aiColor4D ambient;
	aiColor4D emission;
	ai_real shininess, strength;
	int two_sided;
	int wireframe;
	unsigned int max;	// changed: to unsigned

	int texIndex = 0;
	aiString texPath;	//contains filename of texture

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))
	{
		//bind texture
		unsigned int texId = *textureIdMap[texPath.data];
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
	if((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}


int loadMaterials(const aiScene* scene) {
    freeTextureIds();

    if (scene->HasTextures() == true)
    {
        printf("============> this means internal texture ! \n");
        return 1;
    }

    //list<string> _paths;

    /* getTexture Filenames and num of Textures */
    for (unsigned int m=0; m < scene->mNumMaterials; m++)
    {        
        aiString path;	// filename
        aiMaterial *aimaterial=scene->mMaterials[m];
        string _name = aimaterial->GetName().C_Str();
        Material *_material = new Material(_name);

        Materials.insert({_name, _material}); // ok

        printf("material[%d]: name= %s, props= %d\n", m, _name.c_str() ,aimaterial->mNumProperties);
        int count;
        string tex_types[]={"NONE", "DIFFUSE", "SPECULA", "AMBIENT", "EMISSIVE",
                           "HEIGHT", "NORMAL", "SHINESS", "OPACITY", "DISPLACEMENT",
                           "LIGHTMAP", "RELECTION", "BASE_COLOR", "NORMAL_CAMERA", "EMISSION_COLOR",
                           "METALNESS","DIFFUSE_ROUGHNESS", "AMBIENT_OCCLUSION","UNKNOWN",
                            "SHEEN", "CLEARCOART","TRANSMISSION"};
        int found_texture=0;

        for(int type_i=0; type_i < sizeof(tex_types) ; type_i++) //
        {
            /*
             *  to get texture , must do GetTextureCount() !
             */
            string textype="";
            count = aimaterial->GetTextureCount((aiTextureType)type_i);
            if (count >0)
                printf(" + texture_type[%s]= %d\n", tex_types[type_i].c_str(), count);

            for (int j =0 ; j< count ; j++){
                int texIndex=j;
                bool exFound = aimaterial->GetTexture((aiTextureType)type_i, texIndex, &path);
                ///_paths.push_back(string(path.C_Str()));
                std::cout << " + load_texture() = " << path.C_Str() << "\n";
                Texture *_tex = new Texture(path.C_Str()); //
                if (_tex->d_tex_glname > 0)
                {
                    Textures.insert({aimaterial->GetName().C_Str(), _tex->d_tex_glname}); // c++11
                    found_texture++;
                    _material->texture=_tex; // testing texture 1개로 가정함 !!!
                }
            }
        }
        if (found_texture==0)
        {
            printf(" + ===============> texture NONE !!!!!!!!!!!!!!!!! \n");
        }

        /*
         + properties[2]=$clr.diffuse, ( 0.800000 0.746561 0.052616 )
         + properties[3]=$clr.emissive, ( 0.000000 0.000000 0.000000 )
         + properties[4]=$clr.ambient, ( 0.000000 0.000000 0.000000 )
         + properties[5]=$clr.specular, ( 0.800000 0.746561 0.052616 )
        */
        string msgs="";
        for(int i=0; i < aimaterial->mNumProperties ;i++)
        {
           aiMaterialProperty *prop = aimaterial->mProperties[i];
           string keyname=prop->mKey.C_Str();
           string msg=" + properties[" + std::to_string(i) + "]=" +keyname + ", ";
           float colorf[5];
           if (prop->mType==aiPTI_String)
           {               
               msg += ((aiString *) prop->mData)->C_Str();
           }
           else if (prop->mType==aiPTI_Float)
           {
               int count = prop->mDataLength/4;
               float *f = (float *) prop->mData;
               string str="(";
               for(int i=0 ; i< count ; i++ )
               {
                   str+=" " + std::to_string(f[i]);
                   colorf[i]=f[i];
               }
               if(keyname=="$clr.diffuse")
                   set4f(_material->diffuse,colorf);
               else if(keyname=="$clr.emissive")
                   set4f(_material->emission,colorf);
               else if(keyname=="$clr.specular")
                   set4f(_material->specular,colorf);

               str+=" )";
               msg += str;
           }
           else if (prop->mType==aiPTI_Integer)
           {
               int count = prop->mDataLength/4;
               int *j = (int *) prop->mData;
               string str="(";
               for(int i=0 ; i< count ; i++ )
                   str+=" " + std::to_string(j[i]);
               str+=" )";
               msg+=str;
           }
           else if (prop->mType==aiPTI_Buffer)
           {
               int count = prop->mDataLength;
               string str="|buffer|";
               str+=" size= " + std::to_string(count);
               msg+=str;
           }
           else
               msg+="|else|";
           msgs += msg + "\n";

           if(found_texture > 0 and keyname=="$clr.diffuse") //??
           {
               /*
                texture *_tex = new texture(5,5); //
                if (_tex->d_tex_glname > 0)
                {
                   /// MaterialTexture.insert({aimaterial->GetName().C_Str(), _tex->d_tex_glname}); // c++11
                }
                */
           }
        }
       std::cout << msgs ;
    }
    return true;
}


int loadMetadata(aiMetadata *md, xObject *obj, string name="", int level=0) {
    /* blender에서 각 object는 properties를 갖는다.
     * 이 properties가 FBX에서는 metadata임
     * xobject key를 찾으면...
     */

    if (md == nullptr)
    {
        return 0;
    }
    bool xobject_found=0;

    string tab="";
    for (int i=0; i<level; i++)
    {
        tab = tab + '\t';
    }

    if (level >=0)
        printf("%s[%s] metadata_n = %d \n",tab.c_str(), name.c_str(), md->mNumProperties);

    string msgs="";
    for(size_t i=0; i<md->mNumProperties ;i++)
    {
        const aiMetadataEntry *prop;
        const aiString *key;        
        string msg;

        if (md->Get(i,key,prop))
        {
            string keyname = key->C_Str();
            msg += tab + " + data["  + std::to_string(i) + "]=" +keyname + " : ";

            if (keyname=="xobject_type")
            {
                string value=((aiString *) prop->mData)->C_Str();
                xobject_found=true;
                if (value=="ground")
                {
                    obj->d_type=TYPE_GROUND;
                    set(obj->force,0,0,0);
                }
                else if (value=="sphere")
                {
                    /// printf("sphere!!!!!!!!----------------------\n");
                    obj->d_type=TYPE_SPHERE;
                    set(obj->force,0,-9.8,0);
                }
            }
            if (prop->mType==aiMetadataType::AI_BOOL)
            {
               bool *b= (bool*)prop->mData;
               if ( *b == true )
                   msg += "True"  ;
               else
                   msg += "False"  ;
            }
            else if (prop->mType==aiMetadataType::AI_AISTRING)
            {
               msg += ((aiString *) prop->mData)->C_Str();
            }
            else if (prop->mType==aiMetadataType::AI_FLOAT)
            {
                int count = 1;
                float *f = (float *) prop->mData;
                string str="(";

                str+=" " + std::to_string(*f);
                str+=" )";
                msg += str;
            }
            else if (prop->mType==aiMetadataType::AI_DOUBLE)
            {
                double *f = (double *) prop->mData;
                string str="(";
                str+=" " + std::to_string(*f);
                str+=" )";
                msg += str;
            }
            else if (prop->mType==aiMetadataType::AI_INT32)
            {
                int count = 1;
                int *j = (int *) prop->mData;
                string str="(";
                str+=" " + std::to_string(*j);
                str+=" )";
                msg+=str;
            }
            else if (prop->mType==aiMetadataType::AI_UINT64)
            {
                u_int64_t *j = (u_int64_t *) prop->mData;
                string str="(";
                str+=" " + std::to_string(*j);
                str+=" )";
                msg+=str;
            }
            else if (prop->mType==aiMetadataType::AI_AIVECTOR3D)
            {
                u_int64_t *j = (u_int64_t *) prop->mData;
                string str="(AIVECTOR3D)";
                msg+=str;
            }
            else
                msg +="else! unknown metadata type " + std::to_string(prop->mType);

            ///if (level >=0)
            ///    std::cout << msg << "\n" ;
        }
    }
    return xobject_found;
}
/*

[] metadata_n = 18
 + data[0]=UpAxis : ( 1 )
 + data[1]=UpAxisSign : ( 1 )
 + data[2]=FrontAxis : ( 2 )
 + data[3]=FrontAxisSign : ( 1 )
 + data[4]=CoordAxis : ( 0 )
 + data[5]=CoordAxisSign : ( 1 )
 + data[6]=OriginalUpAxis : ( -1 )
 + data[7]=OriginalUpAxisSign : ( 1 )
 + data[8]=UnitScaleFactor : ( 1.000000 )
 + data[9]=OriginalUnitScaleFactor : ( 1.000000 )
 + data[10]=AmbientColor : (AIVECTOR3D)
 + data[11]=FrameRate : ( 11 )
 + data[12]=TimeSpanStart : ( 0 )
 + data[13]=TimeSpanStop : ( 0 )
 + data[14]=CustomFrameRate : ( 24.000000 )
 + data[15]=SourceAsset_FormatVersion : 7400
 + data[16]=SourceAsset_Generator : Blender (stable FBX IO) - 2.93.18 - 4.23.1
 + data[17]=SourceAsset_Format : Autodesk FBX Importer

[RootNode].mNumMeshes=0
 + nd->mNumChildren = 3
    [Camera] metadata_n = 10
     + data[0]=UserProperties :
     + data[1]=IsNull : False
     + data[2]=AspectH : ( 1080.000000 )
     + data[3]=AspectW : ( 1920.000000 )
     + data[4]=BackgroundMode : ( 0 )
     + data[5]=DefaultAttributeIndex : ( 0 )
     + data[6]=ForegroundTransparent : True
     + data[7]=InheritType : ( 1 )
     + data[8]=ResolutionMode : ( 0 )
     + data[9]=ViewFrustum : True
    [Camera].mNumMeshes=0
     + nd->mNumChildren = 0
    load model obj.name = Camera
    [Lamp] metadata_n = 4
     + data[0]=UserProperties :
     + data[1]=IsNull : False
     + data[2]=DefaultAttributeIndex : ( 0 )
     + data[3]=InheritType : ( 1 )
    [Lamp].mNumMeshes=0
     + nd->mNumChildren = 0
    load model obj.name = Lamp
    [Sphere] metadata_n = 4
     + data[0]=UserProperties :
     + data[1]=IsNull : False
     + data[2]=DefaultAttributeIndex : ( 0 )
     + data[3]=InheritType : ( 1 )
    [Sphere].mNumMeshes=1
     + mesh->mMaterialIndex = 0
     + [0].mNumvertices=1984
     + [0].mNumFaces = 960
     +--> OK : make_glVertexArray() 19
     + nd->mNumChildren = 0
    load model obj.name = Sphere
*/


#include <glm/gtc/type_ptr.hpp> // make_mat4()
#include <glm/gtc/quaternion.hpp> //#include <glm/gtx/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
// TODO : scale remove
void loadToObject(const struct aiScene *sc, const struct aiNode* nd, float scalex, xObject *xobj, int level)
{    
    /*     
     * A node:
        +-- meta_data
        +-- meshes
           - vertexes
           - triangles
           - (already materials)
        +-- children

        30%

       1. 재귀함수 : need to change to what?
       2. object들을 xObject로 바꿔줘야한다.

      * Blender 좌표계에서 OpenGL로 바꾸는 아이디어

      * 문제는 origin이
       Model matrix ===> identity
       Loading vertex ======> transform to opengl_vertex

    * scale 과 rotate만 모델에 적용 , translate는 나중에 적용하게 ...
   */

    unsigned int i;
    unsigned int n=0, t;
    aiMatrix4x4 m = nd->mTransformation;

    string tab="";  // tab tricks. have a fun!

    for (int i=0; i<level; i++)
    {
        tab = tab + '\t';
    }

    /*
    if( nd->mName==aiString("Camera")|| nd->mName==aiString("Light"))
    {
        printf("%s[%s] skipped \n", tab.c_str(), nd->mName.C_Str());
        return;
    } */

    // process meta_data
    if(loadMetadata(nd->mMetaData, xobj, nd->mName.C_Str(), level)==true)
        xobj->xobject_found=true; // test

    // no transpose then, extract scale, translate, rotate
    // m.Transpose();  // Q: transpose for OpenGL?   A: maybe!
    //copy(xobj->model_mat, &m.a1);
    //xobj->model = glm::make_mat4(&m.a1); // without transpose!!

    /*
    if(nd->mName==aiString("Lamp"))
    {
        xobj->name="light"; // object name
    }
    else */
    xobj->name = nd->mName.C_Str(); // object name
    printf("%s[%s].mNumMeshes=%d \n", tab.c_str(), nd->mName.C_Str(), nd->mNumMeshes);

    //if (tab_level==0)
    // when non_transpose
    aiVector3f pScaling, pRotation, pPosition;
    aiQuaternion qRotation;
    //m.Decompose(pScaling,pRotation,pPosition);
    m.Decompose(pScaling,qRotation,pPosition);
    //qRotation.Normalize();
    glm::quat MyQuaternion;
    MyQuaternion = glm::quat(qRotation.w,qRotation.x,qRotation.y,qRotation.z);

    //xobj->pos[0]=pPosition.x; xobj->pos[1]=pPosition.y;  xobj->pos[2]=pPosition.z; // translate. ok
    xobj->position=glm::vec3(pPosition.x,pPosition.y,pPosition.z);
    //xobj->scale[0]=m.a1,xobj->scale[1]=m.b2,xobj->scale[2]=m.c3; //scale. ok

    glm::mat4 TranslationMatrix = glm::mat4(1) ;
    //glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1), glm::vec3( pPosition.x,pPosition.y,pPosition.z));
    glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1), glm::vec3(pScaling.x,pScaling.y,pScaling.z));
    glm::mat4 RotationMatrix = glm::mat4_cast(MyQuaternion);
    glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix; // works!!!

    if (0)
    {
        std::cout << glm::to_string(TranslationMatrix) << "\n";
        std::cout << glm::to_string(RotationMatrix) << "\n";
        std::cout << glm::to_string(ScaleMatrix) << "\n";
        std::cout << glm::to_string(ModelMatrix) << "\n";
    }
    float angleY, angleX,angleZ;

    printf("%s >>> translate(x) = %8.4f (y)=%8.4f (z)=%8.4f \n",tab.c_str(), pPosition.x,pPosition.y,pPosition.z);
    //printf("rotate (x) = %8.4f (y)=%8.4f (z)=%8.4f \n",glm::degrees(pRotation.x),glm::degrees(pRotation.y),glm::degrees(pRotation.z));
    ///printf("rotate w=%4.4f x=%4.4f y=%4.4f z=%4.4f \n",qRotation.w,qRotation.x,qRotation.y,qRotation.z);


    //m.a4=0;m.b4=0,m.c4=0;// works!!!
    /*
    m.Transpose();  // Q: transpose for OpenGL?   A: maybe!
    printf("%8.4f,%8.4f,%8.4f,%8.4f\n", m.a1, m.a2,m.a3,m.a4);
    printf("%8.4f,%8.4f,%8.4f,%8.4f\n", m.b1, m.b2,m.b3,m.b4);
    printf("%8.4f,%8.4f,%8.4f,%8.4f\n", m.c1, m.c2,m.c3,m.c4);
    printf("%8.4f,%8.4f,%8.4f,%8.4f\n", m.d1, m.d2,m.d3,m.d4);
    glm::mat4 m1= glm::make_mat4(&m.a1);
    */
    glm::mat4 m1= ModelMatrix; // glm::mat4 m1= glm::transpose(ModelMatrix); <=== up-side-down!

    //xobj->model = glm::make_mat4(&m.a1);
    xobj->model = glm::mat4(1);

    int vertex_offset=0;
    for ( n=0 ; n < nd->mNumMeshes; ++n)
    {
        /* Mesh는 material을 다르게 갖을 수 있다
         * Mesh마다 vertice 인덱스가 중복되므로 ! vertex_offset 필요!
         */
        Material *_material;

        int mesh_idx = nd->mMeshes[n];  // mesh index
        const struct aiMesh* mesh = sc->mMeshes[mesh_idx];

        // 참고 ==> apply_material(sc->mMaterials[mesh->mMaterialIndex]);
        const aiMaterial *mtl=sc->mMaterials[mesh->mMaterialIndex];
        std::cout << tab << " + *** mesh->mMaterialIndex = " << mesh->mMaterialIndex << " ===> " << mtl->GetName().C_Str() <<"\n";

        auto search = Materials.find(mtl->GetName().C_Str());
        if ( search != Materials.end())
        {
            //std::cout << "DEBUG: Found " << search->first << ' ' << search->second << '\n';
            _material=search->second;
            if (_material->texture!=nullptr)
                xobj->set_texture(_material->getTextureName());
        }
        else
        {
            _material=nullptr;
            std::cout << tab << "+ DEBUG: NO Material ===> color object????????????????\n";
        }
        /*
        if(mesh->mNormals == nullptr)
           glDisable(GL_LIGHTING);
        else
          glEnable(GL_LIGHTING);
        */

        if(mesh->mColors[0] != nullptr)
        {
            printf("%s  >>>>>>>>>>>>>>>>>>>> mesh->color found!!! !!! \n",tab.c_str());
        }
        //glDisable(GL_COLOR_MATERIAL);

        printf("%s + [%d].mNumvertices=%d \n",tab.c_str(), n,mesh->mNumVertices);
        int count_normal=0;
        int count_vertex_color=0;
        for (i = 0; i < mesh->mNumVertices ; ++i) {
            vertex vert;
            if(mesh->mNormals != nullptr)
            {
                vert.normal[0]=mesh->mNormals[i].x;
                vert.normal[1]=mesh->mNormals[i].z;
                vert.normal[2]=-mesh->mNormals[i].y;
                count_normal++;
            }

            if(mesh->mColors[0] != nullptr)
            {
                mesh->mColors[0][i];     //Color4f(&mesh->mColors[0][vertexIndex]);
                //printf("%s ================> vertice color!!! \n",tab.c_str());
                vert.r=mesh->mColors[0][i].r;
                vert.g=mesh->mColors[0][i].g;
                vert.b=mesh->mColors[0][i].b;
                vert.a=mesh->mColors[0][i].a;
                count_vertex_color++;
            }
            float tu=0,tv=0;
            //if(mesh->HasTextureCoords(0))	//HasTextureCoords(texture_coordinates_set)
            if(_material->texture != nullptr)
            {
                tu=mesh->mTextureCoords[0][i].x;
                tv= 1 - mesh->mTextureCoords[0][i].y; //mTextureCoords[channel][vertex]
                //vertex_set(vert, mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, tu,tv);
                vert.tu=tu;
                vert.tv=tv;
                vert.type=0; // texture mode
                //printf("%s %2.3f %2.3f %2.3f *tu=%.2f tv=%.2f\n",tab.c_str(),
                //       mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z,  tu,tv);
            }
            else
            {
                // temp...
                vert.r=_material->diffuse[0];
                vert.g=_material->diffuse[1];
                vert.b=_material->diffuse[2];
                vert.a=1.0;
                //printf("%s diffuse==> %2.3f %2.3f %2.3f %2.3f \n",tab.c_str(),_material->diffuse[0],_material->diffuse[1],_material->diffuse[2],_material->diffuse[3]);
                vert.type=1; // color mode
            }            

            // transform to OpenGL
                glm::vec4 v1=glm::vec4(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z,1.0);
                glm::vec4 new_v = m1 * v1;
                vert.v[0]=new_v.x;
                vert.v[1]=new_v.y;
                vert.v[2]=new_v.z;

            xobj->vertexes.push_back(vert);

        }
        printf("%s + [%d].mNumFaces = %d normal=%d  vertex_colors=%d \n",tab.c_str(), n,mesh->mNumFaces, count_normal, count_vertex_color);
        for (t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace* face = &mesh->mFaces[t];
            GLenum face_mode;

            triangle tr;
            switch(face->mNumIndices)
            {
                case 1: face_mode = GL_POINTS;
                    std::cout << "Error: not triangle " << face->mNumIndices << "\n";
                    break;
                case 2: face_mode = GL_LINES;
                    std::cout << "Error: not triangle " << face->mNumIndices << "\n";
                    break;
                case 3: face_mode = GL_TRIANGLES;
                    triangle_set(tr, face->mIndices[0] + vertex_offset, face->mIndices[1]+ vertex_offset, face->mIndices[2]+ vertex_offset);
                    //std::cout << tab << " +--> DEBUG :   " << face->mIndices[0]+ vertex_offset <<","<< face->mIndices[1]+ vertex_offset <<","<< face->mIndices[2]+ vertex_offset << "\n";
                    xobj->triangles.push_back(tr);
                    if(mesh->mNormals != nullptr)
                    {
                    }
                    else
                        std::cout << "Error: mesh.mNormals NULL " << "\n";
                    break;
                default:
                    face_mode = GL_POLYGON;
                    std::cout << "Error: not triangle " << face->mNumIndices << "\n";
                    break;
            }
        } // face
        vertex_offset=xobj->vertexes.size();

    } // Mesh

    if (xobj->triangles.size()>0 )
    {
        xobj->make_glVertexArray(); // make_glVertexArray
        std::cout << tab << " +--> OK : make_glVertexArray() " << xobj->VAO << "\n";
    }


    printf("%s + nd->mNumChildren = %d \n",tab.c_str(),nd->mNumChildren);
    //from child object
    for (n = 0; n < nd->mNumChildren; ++n)
    {
        // tempolarily needvmore time and works later!!!
        xObject *child;
        //string _name = nd->mChildren[n]->mName.C_Str();
        child=new xObject();
        child->set_parent(xobj); //
        child->set_shader(xobj->shader); // from parent's shader
        xobj->children.push_back(child);
        //if (xobj.xobject_found==true)
        loadToObject(sc, nd->mChildren[n], scalex, child, level+1);
    }
    // printf("%sobj.children=%d \n",tab.c_str(), xobj.children.size());
    printf("%sload model obj.name = %s \n",tab.c_str(), xobj->name.c_str());
}


/*
int DrawGLScene() //Here's where we do all the drawing
{

    GLfloat		xrot;
    GLfloat		yrot;
    GLfloat		zrot;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();				// Reset MV Matrix
    glTranslatef(0.0f, -10.0f, -40.0f);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);
    yrot += 0.2f;
    return true;					// okay
}
*/

aiNode *findxObject(const struct aiNode* nd, int level=-1)
{
    /* 사용 안함!
     * root node는 메쉬가 없다. 카메라, 램프 기타 등등
      root 노드 자식노드중에 실제 메쉬가 있기때문에
      찾아서 object에 넣어줘야한다 */

    /* tempolarily need more time and works later!!! */

    aiNode * node = nullptr ;
    for (int n = 0; n < nd->mNumChildren; ++n)
    {
        xObject *tmp_obj=new xObject();
        aiNode * tn = nd->mChildren[n] ;
        //if (xobj.xobject_found==true)
        //findxObject(nd->mChildren[n], level+1);
        if(loadMetadata(tn->mMetaData, tmp_obj, nd->mName.C_Str(), level)==true)
        {
            //tmp_obj->xobject_found=true;
            node = nd->mChildren[n];
        }
        //delete tmp_obj;
    }
    return node;
}

bool Import3DFromFile(const std::string filename, xObject *obj)
{
    /*
        * Scene에는 여러개의 Object가 있을수 있다. 카메라, 라이트 등등
        * 즉, 3d파일 1개에 여러개 Object가 있을수 있음.
        * 게다가 각 Object들이 child를 갖을수 있음

        - blender에서 각 object는 properties를 갖는다.
        - properties가 FBX에서는 metadata임
        - xobject key를 찾으면... type적용

        1. 각 OBject중 LAMP , Camera는 제외하고 등록하자... ---> 바뀜!
        2. 지금은 level==0 일때는 전부 다 개별 object로 등록하자 ---> 바뀜!
        3. tree-child로 등록해야할지 , 개별 Object로 등록할지는... 조금더 해보고 결정
        4. metadata는 child 개수에 포함 안되는듯!
    */

    const aiScene* g_scene = nullptr;

    createAILogger();   // TODO: check!
    // Check if file exists
    std::ifstream fin(filename.c_str());
    if(fin.fail()) {
        std::string message = "Couldn't open file: " + filename;
        std::cout << message << "\n";
        logInfo(importer.GetErrorString());
        return false;
    }
    fin.close();

    g_scene = importer.ReadFile(filename, aiProcess_Triangulate ); //aiProcessPreset_TargetRealtime_MaxQuality

    // If the import failed, report it
    if (g_scene == nullptr) {
        std::string message = "Couldn't open file: " + filename + "," + importer.GetErrorString();
        std::cout << message << "\n";
        logInfo( importer.GetErrorString());
        return false;
    }

    logInfo("Import of scene " + filename + " succeeded.");
    std::cout << "Import of scene " + filename + " succeeded.\n";
    // We're done. Everything will be cleaned up by the importer destructor

    loadMaterials(g_scene);  // 1. load material
    loadMetadata(g_scene->mMetaData, obj, g_scene->mName.C_Str());  // 2. scene's metadata
    loadToObject(g_scene, g_scene->mRootNode, 1.0, obj, 0); // 3. load 3D to object

    return true;
}

model_object::model_object(string _path): xObject()
{
    //texname=texture_manager::get_glname("check.bmp");
    printf("model_object=%s \n", _path.c_str());
    shader=new Shader();
    shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");

    for ( size_t i=0 ; i < cached_models.size(); i++)
    {
        //if(cached_models[i].path==_path)
        {
            //obj=models[i];
            //return true;
        }
    }
    Import3DFromFile(_path, this);
    make_radius();
    //obj.path=filename; //tmp
    //models.push_back( obj);
}

void model_object::update(float dt)
{
    xObject::update(dt);
}

void model_object::draw()
{
   xObject::draw();
}

shader_object::shader_object(string _type)
{
    printf("shader_object=%s \n", path.c_str());
    shader=new Shader();
    shader->Load("./shader/texture_vertex.glsl", "./shader/texture_fragment.glsl");

    /*
    for ( size_t i=0 ; i < models.size(); i++)
    {
        if(models[i].path==path)
        {
            //obj=models[i];
            //return true;
        }
    } */
}

void shader_object::update(float dt)
{
    xObject::update(dt);
}

void shader_object::draw()
{
   xObject::draw();
}


xObject *findCachedObject(string _name)
{
    xObject *node=nullptr;
    /* tempolarily need more time and works later!!! */
    for ( size_t i=0 ; i < cached_objects.size(); i++)
     {
        xObject *obj = cached_objects[i];
        if(obj->name==_name)
            node=obj;
    }
    return node;
}

xObject *findCachedModel(string _name)
{
    xObject *node=nullptr;
    /* tempolarily need more time and works later!!! */
    for ( size_t i=0 ; i < cached_models.size(); i++)
     {
        xObject *obj = cached_models[i];
        if(obj->name==_name)
            node=obj;
    }
    return node;
}


xObject* loadObjectFrom3Dfile(string _path, string _name) // importObjectFrom3Dfile
{
    xObject *obj;
    obj=findCachedModel(_name);
    if (obj!=nullptr)
    {
        xObject *joe = new xObject();
        joe->copy(obj);
        return joe;
    }

    xObject *dummy = new xObject();
    Shader *_shader;
    _shader=new Shader();
    _shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");
    Import3DFromFile(_path, dummy);

    for ( size_t i=0 ; i < dummy->children.size(); i++)
    {
         xObject *obj = dummy->children[i];
         obj->parent=nullptr;
         obj->shader=_shader;
         // printf("obj.children()=%d  %d\n",i,obj->VAO);
         if(obj->name==_name)
         {
             cached_models.push_back(obj); //save
             xObject *joe = new xObject();
             joe->copy(obj);//*joe=*obj;
             return joe;
         }
    }
    return nullptr;
}


vector<xObject*> loadObjectsFrom3Dfile(string _path) // importObjectsFrom3Dfile
{
    /*
     * Temp :  fbx파일에서 레벨 0인 object만 따로 등록하기

       triangles.size() ==0 일때 skip!
    */
    xObject *dummy = new xObject();
    Shader *_shader;
    _shader=new Shader();
    _shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");
    Import3DFromFile(_path, dummy);

    vector<xObject* > array_objects;
    for ( size_t i=0 ; i < dummy->children.size(); i++)
     {
         xObject *obj = dummy->children[i];
         obj->parent=nullptr;
         obj->shader=_shader;
         // printf("obj.children()=%d  %d\n",i,obj->VAO);
         //if (obj->triangles.size()==0)
         if(obj->name=="Camera")
         //if(obj->name=="Camera" or obj->name=="Light")
         {
           //printf("%s[%s] skipped \n", tab.c_str(), nd->mName.C_Str());
             printf("[%s] skipped \n", obj->name.c_str());
             continue;
         }
         array_objects.push_back(obj);
     }
    delete dummy;
    return array_objects;
}


vector<xObject*> loadMapObjectsFrom3Dfile(string _path) // importObjectsFrom3Dfile
{
    vector<xObject* > array_objects;
    array_objects=loadObjectsFrom3Dfile(_path);
    //objects.push_back(array_objects);
    //objects.insert(std::end(objects), std::begin(array_objects), std::end(array_objects)); // tooo long...

    for ( size_t i=0 ; i < array_objects.size(); i++)
     {
         xObject *obj = array_objects[i];
         // printf("obj.children()=%d  %d\n",i,obj->VAO);
         if(obj->name == "Lamp")
         {
            xObject *light = new objLight();
            light->copy(obj);
            //light->name="light";// temp
            obj=light;
            printf("=============> found Lamp!!! %s \n", obj->name.c_str());
         }

         if(obj->triangles.size()==0)
         {
            xObject *model=findCachedModel(obj->name);
            if(model!=nullptr)
            {
                obj->copyModel(model);
                printf("=============> copyModel!!!!!! %s \n", obj->name.c_str());
            }
         }

         if (obj->name.find("Cube") != string::npos)
         {
             // std::transform(_name.begin(), _name.end(), _name.begin(), ::tolower); // 소문자로 바꿔주기
             xObject *_cube=new cube();
             _cube->copy(obj);
             obj=_cube;
             //printf("=============> found cube %s", _name.c_str());
         }
         objects.push_back(obj);
     }
    return array_objects;
}

void init_models()
{
      xObject *obj;
      vector<xObject* > array_objects;

      obj=new xObject(); // fail? why?
      obj->name="center";
      obj->flag_axis_on=true;
      obj->position=glm::vec3(0,1500,0);
      objects.push_back(obj);

      array_objects=loadObjectsFrom3Dfile("./model/light.fbx"); // Lamp, Light
      cached_models.insert(std::end(cached_models), std::begin(array_objects), std::end(array_objects)); // tooo long...

      /*
      obj=loadObjectFrom3Dfile("./model/light.fbx","Light");
      obj->position.x=0;
      obj->position.y=900;
      obj->position.z=0;
      objects.push_back(obj);
      */

      //obj=new particle();
      //set(obj->pos,0,0,0);
      //obj->position=glm::vec3(0,1000,0);
      //objects.push_back(obj);

    //xObject *texobj = new texture_object("check.bmp");
    //set(texobj->pos,-2,0,0);
    //objects.push_back(texobj);

    //texobj=new texture_object("font-map-mtl.png");
    //set(texobj->pos,2,0,0);
    //objects.push_back(texobj);

    //texobj=new texture_object("font-map.png");
    //set(texobj->pos,4,0,0);
    //objects.push_back(texobj);

    /*
        xObject *model_obj=new model_object("./model/stage.blend"); // Fail to load textures!!!
        set(model_obj->pos,0,-4,0);
        objects.push_back(model_obj);
    */
    /*
    xObject *obj;
    obj=new model_object("./model/Bob.fbx");
    obj->name="ground";
    set(obj->pos,0,0,0);
    objects.push_back(obj);
    */
    xObject *model_obj;
    //xObject *model_obj=new model_object("./model/teapot.fbx");
    //set(model_obj->pos,0,0,0);
    //objects.push_back(model_obj);
    //loadObjectsFrom3Dfile("./model/axis.fbx");
    //loadObjectsFrom3Dfile("./model/box.fbx");

    array_objects=loadMapObjectsFrom3Dfile("./model/teapot.fbx");
    //loadObjectsFrom3Dfile("./model/stage.fbx");

    // tmp
    for ( int i=0 ; i < 0 ; i++)
    {
        xObject *obj;
        obj=loadObjectFrom3Dfile("./model/ball.fbx","ball");
        if (obj!=nullptr)
        {
            // 임시 방편 ... 너무...장황하지만...
            obj->flag_gravity=true;
            //obj->name="ball";
            float x,y,z;
            x=(rand()%6000) - 3000;
            y=rand()%3000;
            z=(rand()%6000) - 3000 ;
            obj->position.x=x;
            obj->position.y=y;
            obj->position.z=z;
            objects.push_back(obj);
            ///printf("x=%f  y=%f  z=%f \n",x,y,z);
        }
        // obj=new model_object("./model/ball.fbx"); // hmmm...
    }

    //model_obj=new model_object("./model/Bob.fbx");
    //set(model_obj->pos,0,0,-5);
    //objects.push_back(model_obj);

    //obj=new texture_manager();
    //objects.push_back(obj);
    printf("init_models()\n");
}

