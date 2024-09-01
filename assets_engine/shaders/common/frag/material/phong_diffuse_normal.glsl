struct Material {
    sampler2D ambient_diffuse;
    vec3 specular;
    float shininess;
    sampler2D normal;
};

uniform Material u_material;
