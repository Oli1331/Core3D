#include "elements.h"
#include <SDL2/SDL_ttf.h>
void init_mdl(Model* m) { //###
    init_vector_plgn(&(m->polygons));
    init_vector_vrtc(&(m->vertices));
}

void free_mdl(Model* m) {
    free_vector_vrtc(&(m->vertices));
    free_vector_plg(&(m->polygons));
}



void build_projection_matrix(float FOV, float ratio_screen, float z_near, float z_far, float proj_matrix[16]) {
    for (int i = 0; i < 16; i++) proj_matrix[i] = 0.0f;

    float s = 1.0f / tanf(FOV / 2.0f);
    proj_matrix[0] = s / ratio_screen;
    proj_matrix[5] = s;
    proj_matrix[10] = z_far / (z_far - z_near);
    proj_matrix[11] = -(z_near * z_far) / (z_far - z_near);
    proj_matrix[14] = 1.0f;
}



void start_SDL(SDL_data* data) {
    // Инициализация видео-подсистемы
    SDL_Init(SDL_INIT_VIDEO);
    // Создание окна
    data->window = SDL_CreateWindow(
        "Core3D",          // Заголовок
        SDL_WINDOWPOS_CENTERED,      // X (по центру)
        SDL_WINDOWPOS_CENTERED,      // Y (по центру)
        SCREEN_WIDTH, SCREEN_HEIGHT, // Размеры
        SDL_WINDOW_SHOWN             // Флаги
    );
    // Создание рендерера (ускоренного видеокартой)
    data->renderer = SDL_CreateRenderer(data->window, -1, SDL_RENDERER_ACCELERATED);
    data->texture = SDL_CreateTexture(
        data->renderer,                   // Твой SDL_Renderer*
        SDL_PIXELFORMAT_RGBA8888,   // Формат пикселей (должен совпадать с твоим массивом)
        SDL_TEXTUREACCESS_STREAMING,// Важно! STREAMING оптимизирован для частого обновления
        SCREEN_WIDTH,                        // Ширина (width)
        SCREEN_HEIGHT                            // Высота (height)
    );
    data->is_running = 1;

    TTF_Init();
    data->font = TTF_OpenFont("font.ttf", 24);
    if (data->font == NULL)fprintf(stderr, "Font not found.\n");
}

void start_scene(Scene_data* data) {
    init_vector_mdl(&(data->models));
    init_vector_obj(&(data->objects));
    init_vector_vrtc(&(data->global_render_bufer));

    data->z_bufer = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(float));
    data->pixels = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
    for (int i = 0; i < 4; i++) { // fill view_matrix
        for (int j = 0; j < 4; j++) {
            if (i == j)data->view_matrix[i * 4 + j] = 1;
            else data->view_matrix[i * 4 + j] = 0;
        }
    }
    memset(data->projection_matrix, 0, 16 * sizeof(float));
    memset(data->shift_of_view, 0, 9 * sizeof(float));

    data->start_position.x = 0;
    data->start_position.y = -2;
    data->start_position.z = 10;

    data->FOV = 70;
    data->z_near = 0.01, data->z_far = 10000;
    data->mode = WIREFRAME_MODE;

    data->camera_displacement.x = 0;
    data->camera_displacement.y = 0;
    data->camera_displacement.z = 0;
    data->camera_direction.x = 0;
    data->camera_direction.y = 0;
    data->camera_direction.z = 1;
    data->camera_speed = 0.05;

    data->light.x = 0;
    data->light.y = 0;
    data->light.z = 1;
    data->ambient_light = 0.1;

    data->reverse_normal = 0;

    data->camera_anglex = 0;
    data->camera_angley = 0;
}

void init_vector_vrtc(Vector_vrtc* v) {
    v->capacity = 1;v->cnt_vertices = 0;
    v->data = malloc(1 * sizeof(Vertice));
}

void init_vector_plgn(Vector_plgn* v) {
    v->capacity = 1;v->cnt_polygon = 0;
    v->data = malloc(1 * sizeof(Polygon));
}

void init_vector_mdl(Vector_mdl* v) {
    v->capacity = 1;v->cnt_models = 0;
    v->data = malloc(1 * sizeof(Model));

    // init_vector_plgn(&(v->data->polygons));
    // init_vector_vrtc(&(v->data->vertices));
}

void init_vector_obj(Vector_obj* v) {
    v->capacity = 1;v->cnt_object = 0;
    v->data = malloc(1 * sizeof(Object));
}


void vector_vrtc_push(Vector_vrtc* v, Vertice* vrtc) {
    if (v->cnt_vertices == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * sizeof(Vertice));
    }
    v->data[v->cnt_vertices++] = *vrtc;
}

void vector_plgn_push(Vector_plgn* v, Polygon* plgn) {
    if (v->cnt_polygon == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * sizeof(Polygon));
    }
    v->data[v->cnt_polygon++] = *plgn;
}

void vector_mdl_push(Vector_mdl* v, Model* mdl) {
    if (v->cnt_models == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * sizeof(Model));
    }
    v->data[v->cnt_models++] = *mdl;
}

void vector_obj_push(Vector_obj* v, Object* obj) {
    if (v->cnt_object == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * sizeof(Object));
    }
    v->data[v->cnt_object++] = *obj;
}

void provide_vector_vrtc(Vector_vrtc* v, int n) {
    if (v->capacity < n) {
        v->capacity = n;
        v->data = realloc(v->data, v->capacity * sizeof(Vertice));
    }
}

void free_vector_vrtc(Vector_vrtc* v) {
    if (v->data)free((v->data));
}

void free_vector_plg(Vector_plgn* v) {
    if (v->data)free(v->data);
}

void free_vector_mdl(Vector_mdl* v) {
    for (int m = 0; m < v->cnt_models; m++) {
        free_vector_vrtc(&(v->data[m].vertices));
        free_vector_plg(&(v->data[m].polygons));
    }
    if (v->data)free(v->data);
}

void free_vector_obj(Vector_obj* v) {
    if (v->data)free(v->data);
}
