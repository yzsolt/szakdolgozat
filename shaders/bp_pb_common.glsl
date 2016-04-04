
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

vec4 get_texture_map_value(TextureMap texture_map) {
    return vec4(0);
}

#define VISUALIZE_NOTHING       0
#define VISUALIZE_POSITIONS     1
#define VISUALIZE_NORMALS       2
#define VISUALIZE_TANGENTS      3
#define VISUALIZE_AMBIENT       4
#define VISUALIZE_DIFFUSE       5
#define VISUALIZE_NORMAL_MAP    6
#define VISUALIZE_SPECULARITY   7
#define VISUALIZE_REFLECTIVITY  8
