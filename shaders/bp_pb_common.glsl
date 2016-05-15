
// Texture maps

struct TextureMap {
    sampler2D texture;
    vec4 color;
    bool use_texture;
};

// Materials

struct BlinnPhongMaterial {
    TextureMap ambient;
    TextureMap diffuse;
    TextureMap normal;
    TextureMap specular;
    TextureMap reflection;
    float shininess;
};

struct PhysicallyBasedMaterial {
    TextureMap diffuse;
    TextureMap normal;
    TextureMap roughness;
    TextureMap metalness;
};

// Lights

#define POINT_LIGHT 0
#define SPOT_LIGHT  1

struct Light {
    vec3 position;
    vec3 color;
    float luminance;
    float luminous_intensity;
    float inverse_radius;
};

struct PointLight {
    Light parent;
};

struct SpotLight {
    Light parent;
    vec3 direction;
    float inner_cone_angle;
    float outer_cone_angle;
};
