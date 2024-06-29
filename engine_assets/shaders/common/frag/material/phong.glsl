struct Material {
    vec3 ambient_diffuse;
    vec3 specular;
    float shininess;
    // TODO here could also have been normal or emission maps
};

uniform Material u_material;
