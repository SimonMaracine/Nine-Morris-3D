struct Material {
    sampler2D ambient_diffuse;
    vec3 specular;
    float shininess;
};

uniform Material u_material;
