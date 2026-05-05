#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#define SCREEN_WIDTH (1920*0.75)
#define SCREEN_HEIGHT (1200*0.75)
#define FOV 60
#define SIZE_NAME_MODEL 16
#define MSX_LEN_LINE_IN_OBJ 50
#define MAX_LEN_FILENAME 48


typedef struct Vertice_s {
    float x, y, z, w;
}Vertice;

typedef struct Polygon_s {
    int v[3];
}Polygon;


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
void parsing_obj_file(char* namefile, Vector_mdl* models, Vector_obj* objects) {
    FILE* in = fopen(namefile, "r");
    char line[50];
    int trash;
    int cnt_vertices_in_before_model = 0;
    Model m = { 0 };
    Vertice v;v.w = 1;
    Polygon p;
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
}

void save_output_file(int argc_main, char** argv_main, Vector_obj* objects, Vector_vrtc* global_render_buffer) {
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
        provide_vector_vrtc(global_render_buffer, mdl->vertices.cnt_vertices);

        for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
            apply_transform(objects->data[o].transforms, (float*)&(mdl->vertices.data[v]), (float*)&(global_render_buffer->data[v]));
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
    float coef_FOV = 1 / tanf(FOV / 2);
    float coef_screen = (float)SCREEN_HEIGHT / SCREEN_WIDTH;
    Vector_mdl models;  init_vector_mdl(&models);
    Vector_obj objects; init_vector_obj(&objects);
    Vector_vrtc global_render_buffer; init_vector_vrtc(&global_render_buffer);

    if (argc > 1) parsing_obj_file(argv[1], &models, &objects);


    while (is_running) {
        // Обработка событий
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
        }

        // Логика

        // Отрисовка
        // Установка цвета фона
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // отрисовка модели
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Белый
        for (int o = 0; o < objects.cnt_object; o++) {
            Model* mdl = objects.data[o].model;
            provide_vector_vrtc(&global_render_buffer, mdl->vertices.cnt_vertices);

            for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
                apply_transform(objects.data[o].transforms, (float*)&(mdl->vertices.data[v]), (float*)&(global_render_buffer.data[v]));
            }


            for (int p = 0; p < mdl->polygons.cnt_polygon; p++) {

                int* v = mdl->polygons.data[p].v;
                float z1 = global_render_buffer.data[v[0]].z;
                float z2 = global_render_buffer.data[v[1]].z;
                float z3 = global_render_buffer.data[v[2]].z;

                if (z1 <= 0.01 || z2 <= 0.01 || z3 <= 0.01)continue;//clipping

                int x1 = (global_render_buffer.data[v[0]].x / z1) * coef_screen * coef_FOV * (SCREEN_WIDTH / 2) + SCREEN_WIDTH / 2;
                int x2 = (global_render_buffer.data[v[1]].x / z2) * coef_screen * coef_FOV * (SCREEN_WIDTH / 2) + SCREEN_WIDTH / 2;
                int x3 = (global_render_buffer.data[v[2]].x / z3) * coef_screen * coef_FOV * (SCREEN_WIDTH / 2) + SCREEN_WIDTH / 2;

                int y1 = (global_render_buffer.data[v[0]].y / z1) * coef_FOV * (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 2;
                int y2 = (global_render_buffer.data[v[1]].y / z2) * coef_FOV * (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 2;
                int y3 = (global_render_buffer.data[v[2]].y / z3) * coef_FOV * (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 2;


                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
                SDL_RenderDrawLine(renderer, x3, y3, x1, y1);

            }

        }
        // Вывод на экран
        SDL_RenderPresent(renderer);

        // задержка
        SDL_Delay(16);
    }

    // Сохранение файла
    save_output_file(argc, argv, &objects, &global_render_buffer);

    // Очистка ресурсов
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free_vector_vrtc(&global_render_buffer);
    free_vector_mdl(&models);
    free_vector_obj(&objects);

    return 0;
}