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

// images / texture
std::map<std::string, GLuint*> textureIdMap;	// map image filenames to textureIds
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

void apply_material(const aiMaterial *mtl)
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

std::map<string, GLuint> MaterialTexture;	// map Material name to texid


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
        aiMaterial *material=scene->mMaterials[m];

        printf("material[%d]: name= %s, props= %d\n", m, material->GetName().C_Str() ,material->mNumProperties);
        int count;
        string textypes[]={"NONE", "DIFFUSE", "SPECULA", "ABIENT", "EMISSIVE",
                           "HEIGHT", "NORMAL", "SHINESS", "OPACITY", "DISPLACEMENT",
                           "LIGHTMAP", "RELECTION", "BASE_COLOR", "NORMAL_CAMERA", "EMISSION_COLOR",
                           "METALNESS","DIFFUSE_ROUGHNESS", "AMBIENT_OCCLUSION","UNKNOWN", "SHEEN",
                           "CLEARCOART","TRANSMISSION"};
        int found_texture=0;
        for(int type_i=0; type_i<22 ; type_i++) // type_max = 22
        {
            string textype="";
            //string _path;
            count = material->GetTextureCount((aiTextureType)type_i);
            if (count >0)
                printf(" + texture_type[%s]= %d\n", textypes[type_i].c_str(), count);
            for (int j =0 ; j< count ; j++){
                int texIndex=j;
                bool exFound = material->GetTexture((aiTextureType)type_i, texIndex, &path);
                ///_paths.push_back(string(path.C_Str()));
                std::cout << " + load_texture() = " << path.C_Str() << "\n";
                texture *_tex = new texture(path.C_Str()); //
                if (_tex->d_tex_glname > 0)
                {
                    MaterialTexture.insert({material->GetName().C_Str(), _tex->d_tex_glname}); // c++11
                    found_texture++;
                }
            }
        }

        string msgs="";
        for(int i=0; i < material->mNumProperties ;i++)
        {
           aiMaterialProperty *prop = material->mProperties[i];
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
               msg+="|buffer|";
           }
           else
               msg+="|else|";
           msgs += msg + "\n";

           if(found_texture > 0 and keyname=="$clr.diffuse")
           {
                texture *_tex = new texture(5,5); //
                if (_tex->d_tex_glname > 0)
                {
                    MaterialTexture.insert({material->GetName().C_Str(), _tex->d_tex_glname}); // c++11
                }
           }
        }
       /// std::cout << msgs ;
    }
    return true;
}


// blender에서 각 object는 properties를 갖는다. 이 properties가 FBX에서는 metadata임
// xobject key를 찾으면...
int loadMetadata(aiMetadata *md, xObject *obj, string name="", int level=0) {
    if (md == NULL)
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

        //string msg;
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


            if (level >=0)
                std::cout << msg << "\n" ;
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

aiNode *findxObject(const struct aiNode* nd, int level=-1)
{
    /*
     *위에서 보듯이 root node는 메쉬가 없다. 카메라, 램프 기타 등등
      root 노드 자식노드중에 실제 메쉬가 있기때문에
      찾아서 object에 넣어줘야한다 */

    /* tempolarily need more time and works later!!! */

    aiNode * node = NULL ;
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
        delete tmp_obj;
    }
    return node;
}

void loadToObject(const struct aiScene *sc, const struct aiNode* nd, float scale, xObject *xobj, int tab_level)
{    
    /*
     *  model object들을 xObject로 ...
     *  1. process meta_data
     *  2. process meshes
     *      - vertexes
     *      - triangles
     *      - (already materials)
     *  3. process children
     *  30%
     */
    unsigned int i;
    unsigned int n=0, t;
    aiMatrix4x4 m = nd->mTransformation;
    aiMatrix4x4 m2;
    aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);
    m = m * m2;

    string tab="";  // tab tricks. have a fun!
    for (int i=0; i<tab_level; i++)
    {
        tab = tab + '\t';
    }

    if(strcmp(nd->mName.C_Str(),"Lamp")==0 ||
            strcmp(nd->mName.C_Str(),"Camera")==0 )
    {
        printf("%s[%s] skipped \n", tab.c_str(), nd->mName.C_Str());
        return;
    }

    // process meta_data
    if(loadMetadata(nd->mMetaData, xobj, nd->mName.C_Str(), tab_level)==true)
        xobj->xobject_found=true;

    m.Transpose();  // transpose for OpenGL?
    copy(xobj->model_m, &m.a1);

    //if (tab_level==0 )
    xobj->name = nd->mName.C_Str(); // object name

    printf("%s[%s].mNumMeshes=%d \n", tab.c_str(), nd->mName.C_Str(),nd->mNumMeshes);

    for ( n=0 ; n < nd->mNumMeshes; ++n)
    {
        int mesh_idx = nd->mMeshes[n];  // mesh index
        const struct aiMesh* mesh = sc->mMeshes[mesh_idx];

        //apply_material(sc->mMaterials[mesh->mMaterialIndex]);
        std::cout << tab << " + mesh->mMaterialIndex = " <<mesh->mMaterialIndex <<"\n";
        const aiMaterial *mtl=sc->mMaterials[mesh->mMaterialIndex];

        auto search = MaterialTexture.find(mtl->GetName().C_Str()) ;
        if ( search != MaterialTexture.end())
        {
            //std::cout << "Found " << search->first << ' ' << search->second << '\n';
            xobj->set_texture(search->second);
        }
        else
        {
            std::cout << tab << "+ texture Not found\n";
            //mtl->
        }
        /*
        if(mesh->mNormals == nullptr)
           glDisable(GL_LIGHTING);
        else
          glEnable(GL_LIGHTING);
        */

        if(mesh->mColors[0] != nullptr)
        {
            printf("%s COLOR !!! \n",tab.c_str());            
        }
        //glDisable(GL_COLOR_MATERIAL);

        printf("%s + [%d].mNumvertices=%d \n",tab.c_str(), n,mesh->mNumVertices);
        vertex vert;
        for (i = 0; i < mesh->mNumVertices ; ++i) {
            //mesh->mVertices[i].x;
            if(mesh->mNormals != nullptr)
                mesh->mNormals[i].x;
            if(mesh->mColors[0] != nullptr)
            {
                mesh->mColors[0][i];     //Color4f(&mesh->mColors[0][vertexIndex]);
                printf("%s vertice color!!! \n",tab.c_str());
            }
            float tu=0,tv=0;
            if(mesh->HasTextureCoords(0))	//HasTextureCoords(texture_coordinates_set)
            {
                tu=mesh->mTextureCoords[0][i].x;
                tv= 1 - mesh->mTextureCoords[0][i].y; //mTextureCoords[channel][vertex]
            }
            //printf("%s %2.3f %2.3f %2.3f *tu=%.2f tv=%.2f\n",tab.c_str(),
            //       mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z,  tu,tv);

            vertex_set(vert,mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, tu,tv);
            xobj->vertexes.push_back(vert);
        }

        printf("%s + [%d].mNumFaces = %d \n",tab.c_str(), n,mesh->mNumFaces);
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
                    triangle_set(tr, face->mIndices[0], face->mIndices[1], face->mIndices[2]);
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
        }
    }
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
        xObject *child=new xObject();
        child->set_parent(xobj); //
        child->set_shader(xobj->shader); // from parent's shader
        xobj->children.push_back(child);
        //if (xobj.xobject_found==true)
        loadToObject(sc, nd->mChildren[n], scale, child, tab_level+1);
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


bool Import3DFromFile(const std::string filename,xObject *obj)
{
    /*
        * Scene에는 여러개의 Object가 있다
        * 즉, 3d파일 1개에 여러개 Object가 있을수 있음.
        * 게다가 object들이 child를 갖을수 있음

        -blender에서 각 object는 properties를 갖는다.
        -이 properties가 FBX에서는 metadata임
        -xobject key를 찾으면...

        1. 각 OBject중 LAMP , Camera는 제외하고 등록하자...
        2. 지금은 level==0 일때는 전부 다 개별 object로 등록하자
        3. tree-child로 등록해야할지 , 개별 Object로 등록할지는... 조금더
        해보고 결정

        참고: metadata는 child 개수에 포함 안되는듯!
    */

    const aiScene* g_scene = nullptr;

    createAILogger();
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

    // 1. load material
    // 2. scene's metadata
    loadMaterials(g_scene);
    //loadMetadata(g_scene->mMetaData, obj, g_scene->mName.C_Str());
    loadToObject(g_scene, g_scene->mRootNode, 1.0, obj, 0); // 3. load object ?

    /*
    aiNode *node= findxObject(g_scene->mRootNode);
    if (node!=NULL)
    {
        loadToObject(g_scene, node, 1.0, obj);
    }
    else
        std::cout << "Error !!!!!!!!!!!!!!!!!!!!!  can't find xObject in metadata! \n";
    */

    return true;
}



std::vector<xObject> models; // cached 3D models

model_object::model_object(string _path): xObject()
{
    //texname=texture_manager::get_glname("check.bmp");
    printf("model_object=%s \n", _path.c_str());
    shader=new Shader();
    shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");

    for ( size_t i=0 ; i < models.size(); i++)
    {
        if(models[i].path==_path)
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
    shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");

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

extern vector<xObject* > objects;


void loadObjectFrom3Dfile(string _path)
{
    //xObject *obj;
    xObject *dummy;
    dummy = new xObject();
    Shader *_shader;
    _shader=new Shader();
    _shader->Load("./shader/texture_vertex.glsl","./shader/texture_fragment.glsl");
    Import3DFromFile(_path, dummy);

    for ( size_t i=0 ; i < dummy->children.size(); i++)
     {
         xObject *obj = dummy->children[i];
         obj->parent=NULL;
         obj->shader=_shader;
         // printf("obj.children()=%d  %d\n",i,obj->VAO);
         objects.push_back(obj);
     }
    delete dummy;
}


void init_models()
{
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

    //xObject *model_obj=new model_object("./model/stage.blend");
    //set(model_obj->pos,0,-4,0);
    //objects.push_back(model_obj);

    //obj=new model_object("./model/stage.fbx");
    //obj->name="ground";
    //set(obj->pos,0,0,0);
    //objects.push_back(obj);

    loadObjectFrom3Dfile("./model/stage.fbx");

    /*
    for ( int i=0 ; i < 1 ; i++)
    {
        obj=new model_object("./model/ball.fbx");
        obj->name="ball";
        float x,y,z;
        x=(rand()%60) - 30;
        y=rand()%30;
        z=rand()%60 - 30 ;
        set(obj->pos,x,y,z);
        objects.push_back(obj);
        printf("x=%f  y=%f  z=%f \n",x,y,z);
    }
    */

    //xObject *model_obj=new model_object("./model/box.fbx");
    //set(model_obj->pos,0,10,0);
    //objects.push_back(model_obj);

    //obj=new texture_manager();
    //objects.push_back(obj);

    printf("init_models()\n");
}
