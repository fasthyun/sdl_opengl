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
#include <iostream>
#include <GL/glew.h>  // why?

#include "element3d.h"
#include "object.h"
#include "zmath.h"

#include <fstream>

//to map image filenames to textureIds
#include <string>
#include <map>

// assimp include files. These three are usually needed.
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

void loadToObject(const struct aiScene *sc, const struct aiNode* nd, float scale,xObject &obj,int level=0);

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


bool Import3DFromFile(const std::string &filename,xObject &obj)
{
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
	
	g_scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality);

	// If the import failed, report it
	if (g_scene == nullptr) {
		logInfo( importer.GetErrorString());
        return false;
    }

	// Now we can access the file's contents.
    logInfo("Import of scene " + filename + " succeeded.");
    std::cout << "Import of scene " + filename + " succeeded.\n";
    // We're done. Everything will be cleaned up by the importer destructor

    loadToObject(g_scene, g_scene->mRootNode, 1.0, obj);
    return true;
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

int LoadGLTextures(const aiScene* scene) {
	freeTextureIds();

    if (scene->HasTextures() == true)   // ?????????
		return 1;

	/* getTexture Filenames and Numb of Textures */
	for (unsigned int m=0; m<scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;
		aiString path;	// filename

		while (texFound == AI_SUCCESS)
		{
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			textureIdMap[path.data] = nullptr; //fill map with textures, pointers still NULL yet
			texIndex++;
		}
	}

	const size_t numTextures = textureIdMap.size();

	/* create and fill array with GL texture ids */
	textureIds = new GLuint[numTextures];
	glGenTextures(static_cast<GLsizei>(numTextures), textureIds); /* Texture name generation */

	/* get iterator */
	std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();

    //std::string basepath = getBasePath(modelpath);
	for (size_t i=0; i<numTextures; i++)
	{
		std::string filename = (*itr).first;  // get filename
		(*itr).second =  &textureIds[i];	  // save texture id for filename in map
		itr++;								  // next texture

        //std::string fileloc = basepath + filename;	/* Loading of image */

        /*
         * int x, y, n;
        unsigned char *data ;// = stbi_load(ileloc.c_str(), &x, &y, &n, STBI_rgb_alpha);

		if (nullptr != data )
		{
            // Binding of texture name
            glBindTexture(GL_TEXTURE_2D, textureIds[i]);
			// redefine standard texture values
            // We will use linear interpolation for magnification filter
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            // We will use linear interpolation for minifying filter
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            // Texture specification
            glTexImage2D(GL_TEXTURE_2D, 0, n, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);// Texture specification.

            // we also want to be able to deal with odd texture dimensions
            glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
            glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
            glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
            glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
           // stbi_image_free(data);
        } else {
            // Error occurred
            const std::string message = "Couldn't load Image: " + filename;
            std::wstring targetMessage;
            wchar_t *tmp = new wchar_t[message.size() + 1];
            memset(tmp, L'\0', sizeof(wchar_t) *(message.size() + 1));
            //utf8::utf8to16(message.c_str(), message.c_str() + message.size(), tmp);

		}
        */
	}

    return true;
}

// All Setup For OpenGL goes here
int InitGL()
{
    //if (!LoadGLTextures(g_scene))

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
}


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

void loadToObject(const struct aiScene *sc, const struct aiNode* nd, float scale, xObject &obj, int level)
{    
    unsigned int i;
    unsigned int n=0, t;
    aiMatrix4x4 m = nd->mTransformation;
    aiMatrix4x4 m2;
    aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);
    m = m * m2;

    string tab="";

    for (int i=0;i<level;i++)
    {
        tab= tab + '\t';
    }

    m.Transpose();  // update transform for OpenGL
    copy(obj.model_m,&m.a1);
    //glPushMatrix();
    //glMultMatrixf((float*)&m);
    //print(obj.model_m);
    // draw all meshes assigned to this node    
    printf("%s[%s].mNumMeshes=%d \n",tab.c_str(),nd->mName.C_Str(),nd->mNumMeshes);
    for (n=0 ; n < nd->mNumMeshes; ++n)
    {
        int mesh_idx=nd->mMeshes[n]; // mesh index
        const struct aiMesh* mesh = sc->mMeshes[mesh_idx];
        //apply_material(sc->mMaterials[mesh->mMaterialIndex]);
        /*
        if(mesh->mNormals == nullptr)
           glDisable(GL_LIGHTING);
        else
          glEnable(GL_LIGHTING);
        */

        if(mesh->mColors[0] != nullptr)
        {  glEnable(GL_COLOR_MATERIAL);
        }
        else
        {  glDisable(GL_COLOR_MATERIAL);
        }
        printf("%s[%d].mNumvertices=%d \n",tab.c_str(), n,mesh->mNumVertices);
        vertex vert;
        for (i = 0; i < mesh->mNumVertices ; ++i) {
            //mesh->mVertices[i].x;
            if(mesh->mNormals != nullptr)
                mesh->mNormals[i].x;
            if(mesh->mColors[0] != nullptr)
                mesh->mColors[0][i];     //Color4f(&mesh->mColors[0][vertexIndex]);
            float tu=0,tv=0;
            if(mesh->HasTextureCoords(0))	//HasTextureCoords(texture_coordinates_set)
            {
                tu=mesh->mTextureCoords[0][i].x;
                tv= 1 - mesh->mTextureCoords[0][i].y; //mTextureCoords[channel][vertex]
            }
            //printf("%s %2.3f %2.3f %2.3f *tu=%.2f tv=%.2f\n",tab.c_str(),
            //       mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z,  tu,tv);

            vertex_set(vert,mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, tu,tv);
            obj.vertexes.push_back(vert);
        }

        printf("%s[%d].mNumFaces=%d \n",tab.c_str(), n,mesh->mNumFaces);
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
                    obj.triangles.push_back(tr);
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
    if (obj.triangles.size()>0 )
    {
        obj.make_glVertexArray(); // make_glVertexArray
        std::cout << tab << " +--> OK : make_glVertexArray() " << obj.VAO << "\n";
    }
    if (nd->mNumChildren > 0)
    {
        printf("%snd->mNumChildren=%d \n",tab.c_str(),nd->mNumChildren);

        //from child object
        for (n = 0; n < nd->mNumChildren; ++n)
        {
            xObject *child=new xObject();
            child->set_parent(&obj);
            child->set_shader(obj.shader); // get from parent's shader
            child->set_texture(obj.texname);
            obj.children.push_back(child);
            loadToObject(sc, nd->mChildren[n], scale, *child, level+1);
        }
        printf("%sobj.children=%d \n",tab.c_str(), obj.children.size());
    }
}

int DrawGLScene()				//Here's where we do all the drawing
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

    logInfo("drawing objects");
    yrot += 0.2f;
    return true;					// okay
}

/*
int WINAPI WinMain()

{
	MSG msg = {};
	BOOL done=FALSE;

	createAILogger();
	logInfo("App fired!");

	if (!Import3DFromFile(modelpath))
	{
		cleanup();
		return 0;
	}

	logInfo("=============== Post Import ====================");
}
*/