TODO write how to compile glfw in this readme and add the file that tells AI agent to go kill themselves

## Loading OBJ models

`ModelLoader` uses Assimp to load and triangulate an OBJ file, generate normals
when they are missing, and create a `Model` ready for the existing vertex-normal
shaders. Call it after initializing the OpenGL context and GLAD:

```cpp
#include "code/ModelLoader.hpp"

auto model = ModelLoader::LoadObj("models/model.obj");
auto drawable = std::make_shared<Drawable>(shaderProgram, model);
```

From the `project` directory, compile with the vendored Assimp include path and
library:

```sh
g++ main.cpp ../glad/src/glad.c -o main \
  -I../glad/include -I../assimp/include \
  -L../glfw/build/src -L../assimp/bin \
  -Wl,-rpath,../assimp/bin \
  -lglfw -lassimp -lGL -ldl -lpthread
```

The Assimp include path is important: it ensures Assimp's own nested headers
come from the same vendored version as `Importer.hpp`. Loading failures throw
`std::runtime_error` with the file path and Assimp's diagnostic message.

To load an OBJ with its MTL material textures and lighting, use the house
shaders and add every returned drawable to the scene:

```cpp
auto materialShader = std::make_shared<ShaderProgram>(
    "shaders/house_vertex_shader.glsl",
    "shaders/house_fragment_shader.glsl"
);
materialShader->setVec3("lightPos", glm::vec3(2.0f, 3.0f, 3.0f));
materialShader->setVec3("lightColor", glm::vec3(1.0f));
for (const auto &drawable : ModelLoader::LoadObjDrawables("models/model.obj", materialShader)) {
    scene->AddDrawable(drawable);
}
```

Keep the OBJ, MTL, and image files together in their referenced relative paths.
The loader creates one drawable per OBJ mesh/material and loads diffuse,
normal, and shininess/roughness maps into a `Material`. `Material::Apply`
binds those maps and material values for the lighting shader.
