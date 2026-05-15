#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SIZE_NAME_MODEL 16
#define MSX_LEN_LINE_IN_OBJ 50
#define MAX_LEN_FILENAME 48
#define BASE_VALUE_Z_BUFER 1000000
#define SCREEN_WIDTH (1920*0.75)
#define SCREEN_HEIGHT (1200*0.75)

typedef struct Vertice_s {
    float x, y, z, w;
}Vertice;

typedef struct Vector_vertice_s {
    Vertice* data;
    int cnt_vertices;
    int capacity;
}Vector_vrtc;

typedef struct Polygon_s {
    int v[3];
}Polygon;

typedef struct Vector_polygon_s {
    Polygon* data;
    int cnt_polygon;
    int capacity;
}Vector_plgn;

typedef struct Model_s {
    char name[SIZE_NAME_MODEL];
    Vector_vrtc vertices;
    Vector_plgn polygons;
}Model;


typedef struct Vec_s {
    float x, y, z;
}Vec;

typedef enum Camera_mode_e {
    WIREFRAME_MODE,
    SOLID_WITHOUT_LIGHT_MODE,
    SOLID_MODE,

    CAMERA_MODE_COUNT
}Camera_mode;

typedef struct Vector_model_s {
    Model* data;
    int cnt_models;
    int capacity;
}Vector_mdl;

typedef struct Object_s {
    Model* model;
    float transforms[16];
}Object;

typedef struct Vector_object_s {
    Object* data;
    int cnt_object;
    int capacity;
}Vector_obj;



typedef struct SDL_data_s {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;

    SDL_Texture* texture;

    TTF_Font* font;

    char is_running;
}SDL_data;

typedef struct Scene_data_s {
    Vector_mdl models;
    Vector_obj objects;
    Vector_vrtc global_render_bufer;
    float* z_bufer;
    Uint32* pixels;

    float view_matrix[16];
    float shift_of_view[9];
    float projection_matrix[16];
    float mvp_matrix[16];
    float mv_matrix[16];

    float FOV;
    float z_near, z_far;

    Vec light;
    float ambient_light;

    Vec camera_displacement;
    Vec camera_direction;
    Vec start_position;
    float camera_speed;
    Camera_mode mode;
    float camera_anglex;  
    float camera_angley;  

    char reverse_normal;

}Scene_data;



void init_vector_vrtc(Vector_vrtc* v);
void init_vector_plgn(Vector_plgn* v);
void init_vector_mdl(Vector_mdl* v);
void init_vector_obj(Vector_obj* v);
void vector_vrtc_push(Vector_vrtc* v, Vertice* vrtc);
void vector_plgn_push(Vector_plgn* v, Polygon* plgn);
void vector_mdl_push(Vector_mdl* v, Model* mdl);
void vector_obj_push(Vector_obj* v, Object* obj);
void provide_vector_vrtc(Vector_vrtc* v, int n);
void free_vector_vrtc(Vector_vrtc* v);
void free_vector_plg(Vector_plgn* v);
void free_vector_mdl(Vector_mdl* v);
void free_vector_obj(Vector_obj* v);


void init_mdl(Model* m); //###
void free_mdl(Model* m);
void build_mvp_matrix(float model[16], float view[16], float proj[16], float mvp[16]);
void build_projection_matrix(float FOV, float ratio_screen, float z_near, float z_far, float proj_matrix[16]);
void start_SDL(SDL_data* data);
void start_scene(Scene_data* data);


#endif