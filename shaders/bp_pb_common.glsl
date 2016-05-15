
struct TextureMap {
    sampler2D texture;
    vec4 color;
    bool use_texture;
};

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
