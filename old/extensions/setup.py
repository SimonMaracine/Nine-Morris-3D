from distutils.core import setup, Extension

extension = Extension("load_model",
                      include_dirs=["include"],
                      libraries=["assimp"],
                      runtime_library_dirs=["lib/assimp"],
                      sources=["extensions/load_model.cpp"],
                      language="c++")

if __name__ == "__main__":
    setup(
        name="load_model",
        version="1.0",
        description="Load 3D models",
        ext_modules=[extension]
    )
