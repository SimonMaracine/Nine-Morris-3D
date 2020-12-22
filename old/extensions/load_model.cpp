#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <iostream>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

// The function itself and its helpers
////////////////////////////////////////////////////////////////
struct Model
{
    std::vector<float>* positions;
    std::vector<float>* texture_coordinates;
    std::vector<float>* normals;
    std::vector<unsigned int>* indices;
};

struct vec2
{
    float x, y;
};

struct vec3
{
    float x, y, z;
};

const Model load_model(const char* file_path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(file_path, 0);
    if (!scene) {
        std::cout << "Failed loading model " << file_path << std::endl;
        std::cout << importer.GetErrorString() << std::endl;
        exit(1);
    }

    aiMesh* mesh = scene->mMeshes[0];

    std::vector<vec3> positions;
    std::vector<vec2> tex_coordinates;
    std::vector<vec3> normals;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        positions.push_back(position);

        if (mesh->mTextureCoords[0]) {
            vec2 tex_coordinate;
            tex_coordinate.x = mesh->mTextureCoords[0][i].x;
            tex_coordinate.y = mesh->mTextureCoords[0][i].y;
            tex_coordinates.push_back(tex_coordinate);
        } else {
            std::cout << "Model doesn't have texture coordinates" << std::endl;
        }

        vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        normals.push_back(normal);
    }

    std::vector<float>* fpositions = new std::vector<float>();
    for (unsigned int i = 0; i < positions.size(); i++) {
        fpositions->push_back(positions[i].x);
        fpositions->push_back(positions[i].y);
        fpositions->push_back(positions[i].z);
    }

    std::vector<float>* ftex_coordinates = new std::vector<float>();
    for (unsigned int i = 0; i < tex_coordinates.size(); i++) {
        ftex_coordinates->push_back(tex_coordinates[i].x);
        ftex_coordinates->push_back(tex_coordinates[i].y);
    }

    std::vector<float>* fnormals = new std::vector<float>();
    for (unsigned int i = 0; i < normals.size(); i++) {
        fnormals->push_back(normals[i].x);
        fnormals->push_back(normals[i].y);
        fnormals->push_back(normals[i].z);
    }

    std::vector<unsigned int>* indices = new std::vector<unsigned int>();
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices->push_back(face.mIndices[j]);
        }
    }

    Model model;
    model.positions = fpositions;
    model.texture_coordinates = ftex_coordinates;
    model.normals = fnormals;
    model.indices = indices;

    return model;
}
////////////////////////////////////////////////////////////////

// The wrapper
///////////////////////////////////////////////
static PyObject* Wrapper_load_model(PyObject* self, PyObject* args)
{
    char* file_path = NULL;

    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    Model model = load_model(file_path);

    PyObject* pos_list = PyList_New(model.positions->size());
    PyObject* tex_coords_list = PyList_New(model.texture_coordinates->size());
    PyObject* norm_list = PyList_New(model.normals->size());
    PyObject* ind_list = PyList_New(model.indices->size());

    for (unsigned int i = 0; i < model.positions->size(); i++) {
        PyObject* float_val = Py_BuildValue("f", model.positions->at(i));
        PyList_SetItem(pos_list, i, float_val);
    }

    for (unsigned int i = 0; i < model.texture_coordinates->size(); i++) {
        PyObject* float_val = Py_BuildValue("f", model.texture_coordinates->at(i));
        PyList_SetItem(tex_coords_list, i, float_val);
    }

    for (unsigned int i = 0; i < model.normals->size(); i++) {
        PyObject* float_val = Py_BuildValue("f", model.normals->at(i));
        PyList_SetItem(norm_list, i, float_val);
    }

    for (unsigned int i = 0; i < model.indices->size(); i++) {
        PyObject* int_val = Py_BuildValue("i", model.indices->at(i));
        PyList_SetItem(ind_list, i, int_val);
    }

    PyObject* tuple = PyTuple_New(4);
    PyTuple_SetItem(tuple, 0, pos_list);
    PyTuple_SetItem(tuple, 1, tex_coords_list);
    PyTuple_SetItem(tuple, 2, norm_list);
    PyTuple_SetItem(tuple, 3, ind_list);

    delete model.positions;
    delete model.texture_coordinates;
    delete model.normals;
    delete model.indices;

    return tuple;
}
///////////////////////////////////////////////

static PyMethodDef methods[] = {
    {"load_model", Wrapper_load_model, METH_VARARGS, "The load function"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef load_model_module = {
    PyModuleDef_HEAD_INIT,
    "load_model",
    "Loads 3D models",
    -1,
    methods
};

PyMODINIT_FUNC PyInit_load_model()
{
    return PyModule_Create(&load_model_module);
}
