#include "gltf-loader.h"

#include <iostream>
#include <memory>  // c++11
#include <boost/filesystem.hpp>
//#include


#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "./support/tiny_gltf.h"

using namespace std;
 //utils
static string GetFilePathExtension(const string &FileName) {
  if (FileName.find_last_of(".") != string::npos)
    return FileName.substr(FileName.find_last_of(".") + 1);
  return "";
}


bool get_position(){
    return true;
}
///
/// Loads glTF 2.0 mesh
///
bool LoadGLTF(const string &filename, float scale,
              vector<Mesh<float> > *meshes,
              vector<Material> *materials,
              vector<Texture> *textures) {
  // TODO(syoyo): Texture
  // TODO(syoyo): Material

  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  string err;
  string warn;

  boost::filesystem::path mypath(filename);

  const string ext = GetFilePathExtension(filename);
  //const string ext=mypath.extension();
  bool ret = false;
  if (ext.compare("glb") == 0) {
    // assume binary glTF.
    ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename.c_str());
  } else {
    // assume ascii glTF.
    ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename.c_str());
  }


  if (!warn.empty()) {
    cout << "glTF parse warning: " << warn << endl;
  }

  if (!err.empty()) {
    cerr << "glTF parse error: " << err << endl;
  }
  if (!ret) {
    cerr << "Failed to load glTF: " << filename << endl;
    return false;
  }

  cout << "loaded glTF file has:\n"
            << model.accessors.size() << " accessors\n"
            << model.animations.size() << " animations\n"
            << model.buffers.size() << " buffers\n"
            << model.bufferViews.size() << " bufferViews\n"
            << model.materials.size() << " materials\n"
            << model.meshes.size() << " meshes\n"
            << model.nodes.size() << " nodes\n"
            << model.textures.size() << " textures\n"
            << model.images.size() << " images\n"
            << model.skins.size() << " skins\n"
            << model.samplers.size() << " samplers\n"
            << model.cameras.size() << " cameras\n"
            << model.scenes.size() << " scenes\n"
            << model.lights.size() << " lights\n";

  // Iterate through all the meshes in the glTF file
  for (const auto &gltfMesh : model.meshes) {
    cout << "Current mesh has " << gltfMesh.primitives.size()
              << " primitives:\n";

    // Create a mesh object
    Mesh<float> loadedMesh(sizeof(float) * 3); //왜 3개 --> stride란거 3개라고 알려줌

    // To store the min and max of the buffer (as 3D vector of floats)
    v3f pMin = {}, pMax = {};

    // Store the name of the glTF mesh (if defined)
    loadedMesh.name = gltfMesh.name;

    cout << " mesh name: " << gltfMesh.name << " \n";
    cout << " primitives len : " << gltfMesh.primitives.size() << " \n";

    // For each primitive
    for (const auto &mesh : gltfMesh.primitives) {
      // Boolean used to check if we have converted the vertex buffer format
      bool convertedToTriangleList = false;
      // This permit to get a type agnostic way of reading the index buffer
      unique_ptr<intArrayBase> indicesArrayPtr = nullptr;
      {
        const auto &indicesAccessor = model.accessors[mesh.indices];
        const auto &bufferView = model.bufferViews[indicesAccessor.bufferView];
        const auto &buffer = model.buffers[bufferView.buffer];
        const auto dataAddress = buffer.data.data() + bufferView.byteOffset +
                indicesAccessor.byteOffset;
        const auto byteStride = indicesAccessor.ByteStride(bufferView);
        const auto count = indicesAccessor.count;

        // Allocate the index array in the pointer-to-base declared in the
        // parent scope
        switch (indicesAccessor.componentType) {
          case TINYGLTF_COMPONENT_TYPE_BYTE:
            indicesArrayPtr =
                unique_ptr<intArray<char> >(new intArray<char>(
                    arrayAdapter<char>(dataAddress, count, byteStride)));
            break;

          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            indicesArrayPtr = unique_ptr<intArray<unsigned char> >(
                new intArray<unsigned char>(arrayAdapter<unsigned char>(
                    dataAddress, count, byteStride)));
            break;

          case TINYGLTF_COMPONENT_TYPE_SHORT:
            indicesArrayPtr =
                unique_ptr<intArray<short> >(new intArray<short>(
                    arrayAdapter<short>(dataAddress, count, byteStride)));
            break;

          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            indicesArrayPtr = unique_ptr<intArray<unsigned short> >(
                new intArray<unsigned short>(arrayAdapter<unsigned short>(
                    dataAddress, count, byteStride)));
            break;

          case TINYGLTF_COMPONENT_TYPE_INT:
            indicesArrayPtr = unique_ptr<intArray<int> >(new intArray<int>(
                arrayAdapter<int>(dataAddress, count, byteStride)));
            break;

          case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            indicesArrayPtr = unique_ptr<intArray<unsigned int> >(
                new intArray<unsigned int>(arrayAdapter<unsigned int>(
                    dataAddress, count, byteStride)));
            break;
          default:
            break;
        }
      }
      const auto &indices = *indicesArrayPtr;

      if (indicesArrayPtr) {
          cout << "indices " << indicesArrayPtr->size() << ":";
          for (size_t i(0); i < indicesArrayPtr->size(); ++i) {
              cout << indices[i] << " ";
              loadedMesh.faces.push_back(indices[i]);
          }
          cout << '\n';
      }

      switch (mesh.mode) {
      // We re-arrange the indices so that it describe a simple list of
      // triangles
      case TINYGLTF_MODE_TRIANGLE_FAN:
          if (!convertedToTriangleList) {
              cout << "TRIANGLE_FAN\n";
              // This only has to be done once per primitive
              convertedToTriangleList = true;

              // We steal the guts of the vector
              auto triangleFan = move(loadedMesh.faces);
              loadedMesh.faces.clear();

              // Push back the indices that describe just one triangle one by one
              for (size_t i{2}; i < triangleFan.size(); ++i) {
                  loadedMesh.faces.push_back(triangleFan[0]);
                  loadedMesh.faces.push_back(triangleFan[i - 1]);
                  loadedMesh.faces.push_back(triangleFan[i]);
              }
          }
          break;
      case TINYGLTF_MODE_TRIANGLE_STRIP:
          if (!convertedToTriangleList) {
              cout << "TRIANGLE_STRIP\n";
              // This only has to be done once per primitive
              convertedToTriangleList = true;

              auto triangleStrip = move(loadedMesh.faces);
              loadedMesh.faces.clear();

              for (size_t i{2}; i < triangleStrip.size(); ++i) {
                  loadedMesh.faces.push_back(triangleStrip[i - 2]);
                  loadedMesh.faces.push_back(triangleStrip[i - 1]);
                  loadedMesh.faces.push_back(triangleStrip[i]);
              }
          }
          break;
      case TINYGLTF_MODE_TRIANGLES:  // this is the simpliest case to handle
          cout << "TRIANGLES" << " attr size =" << mesh.attributes.size() <<  "\n";
          for (const auto &attr : mesh.attributes) {
              const auto attribAccessor = model.accessors[attr.second];
              const auto &bufferView =
                      model.bufferViews[attribAccessor.bufferView];
              const auto &buffer = model.buffers[bufferView.buffer];
              const auto dataPtr = buffer.data.data() + bufferView.byteOffset +
                      attribAccessor.byteOffset;
              const auto byte_stride = attribAccessor.ByteStride(bufferView);
              const auto count = attribAccessor.count;

              cout << " current attribute string is : " << attr.first << '\n';
              cout << " + has count " << count
                        << " and stride " << byte_stride << " bytes\n";

              /*
              v3f *data;
              if (attribAccessor.type == TINYGLTF_TYPE_VEC3)
              {
                  if (attribAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                  {
                      data = (v3f*) dataPtr;
                       //v3fArray positions(arrayAdapter<v3f>(dataPtr, count, byte_stride));
                  }
                  if (attribAccessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE)
                  {
                      fprintf(stderr,"ERROR: not implemeted DOUBLE");
                       //v3d *data = (v3d*) dataPtr; copy to
                  }
              }*/

              if (attr.first == "POSITION") {
                  // get the position min/max for computing the boundingbox
                  pMin.x = attribAccessor.minValues[0];
                  pMin.y = attribAccessor.minValues[1];
                  pMin.z = attribAccessor.minValues[2];
                  pMax.x = attribAccessor.maxValues[0];
                  pMax.y = attribAccessor.maxValues[1];
                  pMax.z = attribAccessor.maxValues[2];
                  // 3D vector of float
                  v3f *data = (v3f*) dataPtr;
                  //v3fArray positions(arrayAdapter<v3f>(dataPtr, count, byte_stride));
                    //cout << "positions's size : " << positions.size() << '\n';
                  for (size_t i{0}; i < count ; ++i) {
                      const auto v = data[i];
                      //cout << "   positions[" << i << "]: (" << v.x << ", "
                      //          << v.y << ", " << v.z << ")\n";
                      loadedMesh.vertices.push_back(v.x * scale);
                      loadedMesh.vertices.push_back(v.y * scale);
                      loadedMesh.vertices.push_back(v.z * scale);
                  }
              } // if  "POSITION"

              if (attr.first == "NORMAL") {
                  //v3fArray normals( arrayAdapter<v3f>(dataPtr, count, byte_stride));
                  // IMPORTANT: We need to reorder normals (and texture
                  // coordinates into "facevarying" order) for each face
                  v3f*  normal = (v3f*) dataPtr;
                  // For each triangle :
                  for (size_t i{0}; i < indices.size() / 3; ++i) {
                      // get the i'th triange's indexes
                      auto f0 = indices[3 * i + 0];
                      auto f1 = indices[3 * i + 1];
                      auto f2 = indices[3 * i + 2];

                      // get the 3 normal vectors for that face
                      v3f n0, n1, n2;
                      n0 = normal[f0];
                      n1 = normal[f1];
                      n2 = normal[f2];

                      // Put them in the array in the correct order
                      loadedMesh.facevarying_normals.push_back(n0.x);
                      loadedMesh.facevarying_normals.push_back(n0.y);
                      loadedMesh.facevarying_normals.push_back(n0.z);

                      loadedMesh.facevarying_normals.push_back(n1.x);
                      loadedMesh.facevarying_normals.push_back(n1.y);
                      loadedMesh.facevarying_normals.push_back(n1.z);

                      loadedMesh.facevarying_normals.push_back(n2.x);
                      loadedMesh.facevarying_normals.push_back(n2.y);
                      loadedMesh.facevarying_normals.push_back(n2.z);
                  }
              }
              // Face varying comment on the normals is also true for the UVs
              if (attr.first == "TEXCOORD_0") {
                  v2f *uvs=(v2f*) dataPtr;
                  //if (attribAccessor.type == TINYGLTF_TYPE_VEC2)
                //v2fArray uvs(arrayAdapter<v2f>(dataPtr, count, byte_stride));

                  for (size_t i{0}; i < indices.size() / 3; ++i) {
                      // get the i'th triange's indexes
                      auto f0 = indices[3 * i + 0];
                      auto f1 = indices[3 * i + 1];
                      auto f2 = indices[3 * i + 2];

                      // get the texture coordinates for each triangle's
                      // vertices
                      v2f uv0, uv1, uv2;
                      uv0 = uvs[f0];
                      uv1 = uvs[f1];
                      uv2 = uvs[f2];

                      // push them in order into the mesh data
                      loadedMesh.facevarying_uvs.push_back(uv0.x);
                      loadedMesh.facevarying_uvs.push_back(uv0.y);

                      loadedMesh.facevarying_uvs.push_back(uv1.x);
                      loadedMesh.facevarying_uvs.push_back(uv1.y);

                      loadedMesh.facevarying_uvs.push_back(uv2.x);
                      loadedMesh.facevarying_uvs.push_back(uv2.y);
                  }

              }

           //cerr << "unrecognized vector type for UV";
           //cerr << "unreconized componant type for UV";
          }break;    // case TINYGLTF_MODE_TRIANGLES

      case TINYGLTF_MODE_POINTS:// These aren't triangles:
      case TINYGLTF_MODE_LINE:
      case TINYGLTF_MODE_LINE_LOOP:
          cerr << "primitive is not triangle based! NEED develop! ";
          break;
      default:
          cerr << "primitive(mesh.mode) not implemented " << mesh.mode;
          break;
      } //switch (meshPrimitive.mode)

      // bbox :
      v3f bCenter;
      bCenter.x = 0.5f * (pMax.x - pMin.x) + pMin.x;
      bCenter.y = 0.5f * (pMax.y - pMin.y) + pMin.y;
      bCenter.z = 0.5f * (pMax.z - pMin.z) + pMin.z;

      for (size_t v = 0; v < loadedMesh.vertices.size() / 3; v++) {
        loadedMesh.vertices[3 * v + 0] -= bCenter.x;
        loadedMesh.vertices[3 * v + 1] -= bCenter.y;
        loadedMesh.vertices[3 * v + 2] -= bCenter.z;
      }

      loadedMesh.pivot_xform[0][0] = 1.0f;
      loadedMesh.pivot_xform[0][1] = 0.0f;
      loadedMesh.pivot_xform[0][2] = 0.0f;
      loadedMesh.pivot_xform[0][3] = 0.0f;

      loadedMesh.pivot_xform[1][0] = 0.0f;
      loadedMesh.pivot_xform[1][1] = 1.0f;
      loadedMesh.pivot_xform[1][2] = 0.0f;
      loadedMesh.pivot_xform[1][3] = 0.0f;

      loadedMesh.pivot_xform[2][0] = 0.0f;
      loadedMesh.pivot_xform[2][1] = 0.0f;
      loadedMesh.pivot_xform[2][2] = 1.0f;
      loadedMesh.pivot_xform[2][3] = 0.0f;

      loadedMesh.pivot_xform[3][0] = bCenter.x;
      loadedMesh.pivot_xform[3][1] = bCenter.y;
      loadedMesh.pivot_xform[3][2] = bCenter.z;
      loadedMesh.pivot_xform[3][3] = 1.0f;

      // TODO handle materials
      //for (size_t i{0}; i < loadedMesh.faces.size(); ++i)
      //loadedMesh.material_ids.push_back(model.materials->at(0).id); //segfalut

      meshes->push_back(loadedMesh);
      ret = true;
    }
  } // model.meshes

  // Iterate through all texture declaration in glTF file
  for (const auto &gltfTexture : model.textures) {
    cout << "Found texture!";
    Texture loadedTexture;
    const auto &image = model.images[gltfTexture.source];
    loadedTexture.components = image.component;
    loadedTexture.width = image.width;
    loadedTexture.height = image.height;

    const auto size =
        image.component * image.width * image.height * sizeof(unsigned char);
    loadedTexture.image = new unsigned char[size];
    memcpy(loadedTexture.image, image.image.data(), size);
    textures->push_back(loadedTexture);
  }
  return ret;
}

