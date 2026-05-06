#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#define SCREEN_WIDTH (1920*0.75)
#define SCREEN_HEIGHT (1200*0.75)
#define SIZE_NAME_MODEL 16
#define MSX_LEN_LINE_IN_OBJ 50
#define MAX_LEN_FILENAME 48
#define BASE_VALUE_Z_BUFER 1000000
#define RGBA(R,G,B,A) ((R<<24)|(G<<16)|(B<<8)|(A))


typedef struct Vertice_s {
    float x, y, z, w;
}Vertice;

typedef struct Polygon_s {
    int v[3];
}Polygon;

typedef struct Vec_s {
    float x, y, z;
}Vec;

typedef struct Vector_vertice_s {
    Vertice* data;
    int cnt_vertices;
    int capacity;
}Vector_vrtc;

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

typedef struct Object_s {
    Model* model;
    float transforms[16];
}Object;



typedef struct Vector_model_s {
    Model* data;
    int cnt_models;
    int capacity;
}Vector_mdl;

typedef struct Vector_object_s {
    Object* data;
    int cnt_object;
    int capacity;
}Vector_obj;

typedef enum Camera_mode_e {
    WIREFRAME_MODE,
    BACK_FACES_MODE,
    SOLID_MODE,
    CAMERA_MODE_COUNT
}Camera_mode;

void init_vector_vrtc(Vector_vrtc* v) {
    v->capacity = 1;v->cnt_vertices = 0;
    v->data = malloc(1 * sizeof(Vertice));
}

void init_vector_plgn(Vector_plgn* v) {
    v->capacity = 1;v->cnt_polygon = 0;
    v->data = malloc(1 * sizeof(Polygon));
}

void init_mdl(Model* m) { //###
    init_vector_plgn(&(m->polygons));
    init_vector_vrtc(&(m->vertices));
}

void init_vector_mdl(Vector_mdl* v) {
    v->capacity = 1;v->cnt_models = 0;
    v->data = malloc(1 * sizeof(Model));

    init_vector_plgn(&(v->data->polygons));
    init_vector_vrtc(&(v->data->vertices));
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
    while (v->capacity < n) {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * sizeof(Vertice));
    }
}

void free_vector_vrtc(Vector_vrtc* v) {
    free((v->data));
}

void free_vector_plg(Vector_plgn* v) {
    free(v->data);
}

void free_mdl(Model* m) {
    free_vector_vrtc(&(m->vertices));
    free_vector_plg(&(m->polygons));
}

void free_vector_mdl(Vector_mdl* v) {
    for (int m = 0; m < v->cnt_models; m++) {
        free_vector_vrtc(&(v->data[m].vertices));
        free_vector_plg(&(v->data[m].polygons));
    }
    free(v->data);
}

void free_vector_obj(Vector_obj* v) {
    free(v->data);
}

void apply_transform(float transform[16], float v[4], float res[4]) {
    for (int i = 0; i < 4; i++) {
        res[i] = 0;
        for (int j = 0; j < 4; j++) {
            res[i] += v[j] * transform[i * 4 + j];
        }
    }
}

// parsing 
int parsing_obj_file(char* namefile, Vector_mdl* models, Vector_obj* objects) {
    FILE* in = fopen(namefile, "r");
    if (in == NULL)return 0;
    char line[50];
    int trash;
    int cnt_vertices_in_before_model = 0;
    Model m = { 0 };
    Vertice v;v.w = 1;
    Polygon p;
    fprintf(stderr, "START PARSING\n");
    while (fgets(line, MSX_LEN_LINE_IN_OBJ, in)) {
        switch (line[0]) {
        case 'o':
            if (m.vertices.cnt_vertices > 0)vector_mdl_push(models, &m);
            cnt_vertices_in_before_model += m.vertices.cnt_vertices;
            init_mdl(&m);
            memcpy(m.name, line + 2, SIZE_NAME_MODEL);m.name[SIZE_NAME_MODEL - 1] = '\0';
            char* end_name = strchr(m.name, '\n');
            if (end_name)*end_name = '\0';
            break;
        case 'v':
            switch (line[1]) {
            case ' ':
                sscanf(line, "v %f %f %f", &(v.x), &(v.y), &(v.z));
                v.z *= -1;
                vector_vrtc_push(&(m.vertices), &v);
                break;

            default:
                break;
            }
            break;
        case 'f':
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &(p.v[0]), &trash, &trash, &(p.v[1]), &trash, &trash, &(p.v[2]), &trash, &trash);
            p.v[0] -= cnt_vertices_in_before_model + 1;
            p.v[1] -= cnt_vertices_in_before_model + 1;
            p.v[2] -= cnt_vertices_in_before_model + 1;
            vector_plgn_push(&(m.polygons), &p);
            break;
        default:
            break;
        }
    }
    if (m.vertices.cnt_vertices > 0)vector_mdl_push(models, &m);

    Object o = { 0 };
    for (int i = 0; i < 4; i++) {
        o.transforms[i * 4 + i] = 1;
    }

    for (int m = 0; m < models->cnt_models; m++) {
        o.model = &(models->data[m]);
        vector_obj_push(objects, &o);
    }
    fclose(in);
    return 1;
}

void save_output_file(int argc_main, char** argv_main, Vector_obj* objects, Vector_vrtc* global_render_bufer) {
    char filename_output[MAX_LEN_FILENAME] = "a.obj";
    if (argc_main > 2 && strlen(argv_main[2]) < MAX_LEN_FILENAME) {
        strncpy(filename_output, argv_main[2], MAX_LEN_FILENAME);filename_output[MAX_LEN_FILENAME - 1] = '\0';
    }
    FILE* out = fopen(filename_output, "w");
    fprintf(out, "# Core3D 1.0.0\n");
    int cnt_vertices_in_before_model = 0;

    for (int o = 0; o < objects->cnt_object; o++) {
        Model* mdl = objects->data[o].model;
        int trash = -1;
        provide_vector_vrtc(global_render_bufer, mdl->vertices.cnt_vertices);

        for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
            apply_transform(objects->data[o].transforms, (float*)&(mdl->vertices.data[v]), (float*)&(global_render_bufer->data[v]));
        }
        fprintf(out, "\no %s\n", mdl->name);
        for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
            fprintf(out, "v %f %f %f\n", mdl->vertices.data[v].x, mdl->vertices.data[v].y, mdl->vertices.data[v].z);
        }
        for (int p = 0; p < mdl->polygons.cnt_polygon; p++) {
            fprintf(out, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", mdl->polygons.data[p].v[0] + 1 + cnt_vertices_in_before_model, trash, trash,
                mdl->polygons.data[p].v[1] + 1 + cnt_vertices_in_before_model, trash, trash,
                mdl->polygons.data[p].v[2] + 1 + cnt_vertices_in_before_model, trash, trash);
        }
        cnt_vertices_in_before_model += mdl->vertices.cnt_vertices;
    }
    fclose(out);
}

void multiply_matrix_4x4(float A[16], float B[16], float out[16]) {
    for (int r = 0; r < 4; r++) {           // Строки (row)
        for (int c = 0; c < 4; c++) {       // Столбцы (col)
            out[r * 4 + c] = 0;
            for (int k = 0; k < 4; k++) {   // Элементы
                out[r * 4 + c] += A[r * 4 + k] * B[k * 4 + c];
            }
        }
    }
}

void build_mvp_matrix(float model[16], float view[16], float proj[16], float mvp[16]) {
    float temp_view_model[16];
    multiply_matrix_4x4(view, model, temp_view_model); //  View * Model
    multiply_matrix_4x4(proj, temp_view_model, mvp);   //  Proj * (View * Model)
}

void build_view_matrix(float view_matrix[16], float displacement_x, float displacement_y, float displacement_z) {
    for (int i = 0; i < 16; i++) view_matrix[i] = 0.0f;
    view_matrix[0] = 1.0f;
    view_matrix[5] = 1.0f;
    view_matrix[10] = 1.0f;
    view_matrix[15] = 1.0f;
    view_matrix[0 * 4 + 3] = displacement_x;
    view_matrix[1 * 4 + 3] = displacement_y;
    view_matrix[2 * 4 + 3] = displacement_z;
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

float edge_function(float x1, float y1, float x2, float y2, float x3, float y3) {
    return (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
}

void draw_triangle(float* z_bufer, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
    int min_x = fmaxf(0, fminf(x1, fminf(x2, x3)));
    int min_y = fmaxf(0, fminf(y1, fminf(y2, y3)));
    int max_x = fminf(SCREEN_WIDTH - 1, fmaxf(x1, fmaxf(x2, x3)));
    int max_y = fminf(SCREEN_HEIGHT - 1, fmaxf(y1, fmaxf(y2, y3)));

    float area = edge_function(x1, y1, x2, y2, x3, y3);
    if (area == 0.0f) return;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {

            float w1 = edge_function(x2, y2, x3, y3, x, y);
            float w2 = edge_function(x3, y3, x1, y1, x, y);
            float w3 = edge_function(x1, y1, x2, y2, x, y);

            if (w1 < 0 && w2 < 0 && w3 < 0) {

                w1 /= area; w2 /= area; w3 /= area;

                float pixel_z = (z1 * w1) + (z2 * w2) + (z3 * w3);

                int bufer_index = y * SCREEN_WIDTH + x;

                if (pixel_z < z_bufer[bufer_index]) {
                    z_bufer[bufer_index] = pixel_z;
                }
            }
        }
    }
}


int main(int argc, char* argv[]) {
    // Инициализация видео-подсистемы
    SDL_Init(SDL_INIT_VIDEO);
    // Создание окна
    SDL_Window* window = SDL_CreateWindow(
        "Core3D",          // Заголовок
        SDL_WINDOWPOS_CENTERED,      // X (по центру)
        SDL_WINDOWPOS_CENTERED,      // Y (по центру)
        SCREEN_WIDTH, SCREEN_HEIGHT, // Размеры
        SDL_WINDOW_SHOWN             // Флаги
    );
    // Создание рендерера (ускоренного видеокартой)
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool is_running = true;
    SDL_Event event;
    Vector_mdl models;  init_vector_mdl(&models);
    Vector_obj objects; init_vector_obj(&objects);
    Vector_vrtc global_render_bufer; init_vector_vrtc(&global_render_bufer);

    SDL_Texture* texture = texture = SDL_CreateTexture(
        renderer,                   // Твой SDL_Renderer*
        SDL_PIXELFORMAT_RGBA8888,   // Формат пикселей (должен совпадать с твоим массивом)
        SDL_TEXTUREACCESS_STREAMING,// Важно! STREAMING оптимизирован для частого обновления
        SCREEN_WIDTH,                        // Ширина (width)
        SCREEN_HEIGHT                            // Высота (height)
    );
    float* z_bufer = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(float));
    Uint32* pixels = malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
    float view_matrix[16] = { 0 }; for (int i = 0; i < 4; i++) view_matrix[i * 4 + i] = 1;
    float projection_matrix[16] = { 0 };
    float mvp_matrix[16] = { 0 };
    float FOV = 70;
    float z_near = 0.01, z_far = 10000;
    float angle = 0;
    Camera_mode mode = WIREFRAME_MODE;

    Vec camera = { 0 };
    //Vec light = { 0 };
    float camera_speed = 0.01;

    if (argc > 1) {
        if (parsing_obj_file(argv[1], &models, &objects) == 0) {
            fprintf(stderr, "FILE not found\n");
        }
    }
    fprintf(stderr, "START MAIN LOOP\n");
    while (is_running) {
        // Обработка событий
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_Z:
                    mode = (mode + 1) % CAMERA_MODE_COUNT;
                    fprintf(stderr, "CHANGE MODE to - %d\n", mode);

                    break;
                default: break;
                }
                break;
            }
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP])     camera.z -= camera_speed;
        if (state[SDL_SCANCODE_DOWN])   camera.z += camera_speed;
        if (state[SDL_SCANCODE_LEFT])   camera.x += camera_speed;
        if (state[SDL_SCANCODE_RIGHT])  camera.x -= camera_speed;
        if (state[SDL_SCANCODE_W])      camera.y -= camera_speed;
        if (state[SDL_SCANCODE_S])      camera.y += camera_speed;

        // Логика

        // Отрисовка
        // Установка цвета фона
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // отрисовка модели
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Белый

        build_projection_matrix(FOV, (float)SCREEN_WIDTH / SCREEN_HEIGHT, z_near, z_far, projection_matrix);
        build_view_matrix(view_matrix, camera.x, camera.y, camera.z);
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            z_bufer[i] = BASE_VALUE_Z_BUFER;
        }
        objects.data[0].transforms[0 * 4 + 0] = cosf(angle);
        objects.data[0].transforms[0 * 4 + 1] = -sinf(angle);
        objects.data[0].transforms[1 * 4 + 0] = sinf(angle);
        objects.data[0].transforms[1 * 4 + 1] = cosf(angle);
        angle += 0.01;
        for (int o = 0; o < objects.cnt_object; o++) {
            Model* mdl = objects.data[o].model;
            build_mvp_matrix(objects.data[o].transforms, view_matrix, projection_matrix, mvp_matrix);

            provide_vector_vrtc(&global_render_bufer, mdl->vertices.cnt_vertices);
            for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
                apply_transform(mvp_matrix, (float*)&(mdl->vertices.data[v]), (float*)&(global_render_bufer.data[v]));
            }

            for (int p = 0; p < mdl->polygons.cnt_polygon; p++) {

                int* v = mdl->polygons.data[p].v;
                float w1 = global_render_bufer.data[v[0]].w;
                float w2 = global_render_bufer.data[v[1]].w;
                float w3 = global_render_bufer.data[v[2]].w;

                if (w1 < z_near || w2 < z_near || w3 < z_near || w1 > z_far || w2 > z_far || w3 > z_far)continue;//clipping

                float x1 = (global_render_bufer.data[v[0]].x / w1 + 1) * 0.5 * SCREEN_WIDTH;
                float x2 = (global_render_bufer.data[v[1]].x / w2 + 1) * 0.5 * SCREEN_WIDTH;
                float x3 = (global_render_bufer.data[v[2]].x / w3 + 1) * 0.5 * SCREEN_WIDTH;

                float y1 = (1 - global_render_bufer.data[v[0]].y / w1) * 0.5 * SCREEN_HEIGHT;
                float y2 = (1 - global_render_bufer.data[v[1]].y / w2) * 0.5 * SCREEN_HEIGHT;
                float y3 = (1 - global_render_bufer.data[v[2]].y / w3) * 0.5 * SCREEN_HEIGHT;

                float normal = ((x2 - x1) * (y3 - y1)) - ((y2 - y1) * (x3 - x1)); //векторное произведение ребер грани
                if (mode == BACK_FACES_MODE)normal *= -1;
                if (normal > 0)continue;

                if (mode == SOLID_MODE) {
                    float z1 = global_render_bufer.data[v[0]].z / w1;
                    float z2 = global_render_bufer.data[v[1]].z / w2;
                    float z3 = global_render_bufer.data[v[2]].z / w3;
                    draw_triangle(z_bufer, x1, y1, z1, x2, y2, z2, x3, y3, z3);
                }
                if (mode == WIREFRAME_MODE || mode == BACK_FACES_MODE) {
                    SDL_RenderDrawLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
                    SDL_RenderDrawLine(renderer, (int)x2, (int)y2, (int)x3, (int)y3);
                    SDL_RenderDrawLine(renderer, (int)x3, (int)y3, (int)x1, (int)y1);
                }
            }

        }
        if (mode == SOLID_MODE) {
            for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
                if (z_bufer[i] < BASE_VALUE_Z_BUFER)
                    pixels[i] = RGBA(100, 100, 250, 255);
                else pixels[i] = RGBA(0, 0, 100, 255);
            }
            SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
            SDL_RenderCopy(renderer, texture, NULL, NULL);
        }
        // Вывод на экран
        SDL_RenderPresent(renderer);

        // задержка
        SDL_Delay(10);
    }

    // Сохранение файла
    fprintf(stderr, "SAVE FILE\n");
    save_output_file(argc, argv, &objects, &global_render_bufer);

    // Очистка ресурсов
    fprintf(stderr, "FREE MEMORY\n");
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free_vector_vrtc(&global_render_bufer);
    free_vector_mdl(&models);
    free_vector_obj(&objects);

    return 0;
}