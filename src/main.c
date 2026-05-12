//#include <stdbool.h>
#include "elements.h"
#include "easy_math.h"
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#define RGBA(R,G,B,A) ((R<<24)|(G<<16)|(B<<8)|(A))

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
    fprintf(stderr, "FINISH PARSING.\nCOUNT of models : % d\n", models->cnt_models);

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

void draw_triangle(float* z_bufer, Uint32* pixels, float light_intesity, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
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
                float color = 200 * light_intesity;
                int bufer_index = y * SCREEN_WIDTH + x;

                if (pixel_z < z_bufer[bufer_index]) {
                    z_bufer[bufer_index] = pixel_z;
                    pixels[bufer_index] = RGBA((int)color, (int)color, (int)color, 0);
                }
            }
        }
    }
}

void draw_const_triangle(float* z_bufer, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
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

// Обработка событий
void event_handling(SDL_data* sdl, Scene_data* scene) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&(sdl->event))) {

        if (sdl->event.type == SDL_MOUSEMOTION && state[SDL_SCANCODE_SPACE]) {
            scene->camera_anglex += sdl->event.motion.xrel * 0.01;
            scene->camera_angley -= sdl->event.motion.yrel * 0.01;
        }

        switch (sdl->event.type) {
        case SDL_QUIT:
            sdl->is_running = 0;
            break;
        case SDL_KEYDOWN:
            switch (sdl->event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                sdl->is_running = 0;
                break;
            case SDL_SCANCODE_Z:
                scene->mode = (scene->mode + 1) % CAMERA_MODE_COUNT;
                fprintf(stderr, "CHANGE MODE to - %d\n", scene->mode);
                break;
                break;
            case SDL_SCANCODE_I:
                scene->reverse_normal = (scene->reverse_normal + 1) % 2;
                fprintf(stderr, "REVERSE NORMAL\n");
                break;
            case SDL_SCANCODE_1:
                if (state[SDL_SCANCODE_LCTRL])scene->FOV -= 0.1;
                else scene->FOV += 0.1;
                break;
            case SDL_SCANCODE_2:
                if (state[SDL_SCANCODE_LCTRL])scene->ambient_light -= 0.1;
                else scene->ambient_light += 0.1;
                break;

            default: break;
            }
            break;
        }
    }

    for (int i = 0; i < 9; i++) {
        if (state[89 + i]) {
            int ind = (2 - i / 3) * 3 + (i % 3);
            if (state[SDL_SCANCODE_LCTRL])scene->shift_of_view[ind] -= 0.01;
            else scene->shift_of_view[ind] += 0.01;
        }
    }

    if (state[SDL_SCANCODE_UP])     scene->camera_displacement.z -= scene->camera_speed;
    if (state[SDL_SCANCODE_DOWN])   scene->camera_displacement.z += scene->camera_speed;
    if (state[SDL_SCANCODE_LEFT])   scene->camera_displacement.x += scene->camera_speed;
    if (state[SDL_SCANCODE_RIGHT])  scene->camera_displacement.x -= scene->camera_speed;
    if (state[SDL_SCANCODE_W])      scene->camera_displacement.y -= scene->camera_speed;
    if (state[SDL_SCANCODE_S])      scene->camera_displacement.y += scene->camera_speed;

}

void wireframe(SDL_data* sdl, Scene_data* scene) {

    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);// цвета фона
    SDL_RenderClear(sdl->renderer);
    SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255); // цвет линий
    for (int o = 0; o < scene->objects.cnt_object; o++) {
        Model* mdl = scene->objects.data[o].model;

        multiply_matrix_4x4(scene->view_matrix, scene->objects.data[o].transforms, scene->mv_matrix); //  View * Model
        multiply_matrix_4x4(scene->projection_matrix, scene->mv_matrix, scene->mvp_matrix);   //  Proj * (View * Model)

        provide_vector_vrtc(&(scene->global_render_bufer), mdl->vertices.cnt_vertices);
        for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
            apply_transform(scene->mvp_matrix, (float*)&(mdl->vertices.data[v]), (float*)&(scene->global_render_bufer.data[v]));
        }

        for (int p = 0; p < mdl->polygons.cnt_polygon; p++) {

            int* v = mdl->polygons.data[p].v;
            float w1 = scene->global_render_bufer.data[v[0]].w;
            float w2 = scene->global_render_bufer.data[v[1]].w;
            float w3 = scene->global_render_bufer.data[v[2]].w;

            if (w1 < scene->z_near || w2 < scene->z_near || w3 < scene->z_near || w1 > scene->z_far || w2 > scene->z_far || w3 > scene->z_far)continue;//clipping

            float x1 = (scene->global_render_bufer.data[v[0]].x / w1 + 1) * 0.5 * SCREEN_WIDTH;
            float x2 = (scene->global_render_bufer.data[v[1]].x / w2 + 1) * 0.5 * SCREEN_WIDTH;
            float x3 = (scene->global_render_bufer.data[v[2]].x / w3 + 1) * 0.5 * SCREEN_WIDTH;

            float y1 = (1 - scene->global_render_bufer.data[v[0]].y / w1) * 0.5 * SCREEN_HEIGHT;
            float y2 = (1 - scene->global_render_bufer.data[v[1]].y / w2) * 0.5 * SCREEN_HEIGHT;
            float y3 = (1 - scene->global_render_bufer.data[v[2]].y / w3) * 0.5 * SCREEN_HEIGHT;

            float normal = edge_function(x1, y1, x2, y2, x3, y3); //векторное произведение ребер грани
            if (scene->reverse_normal)normal *= -1;
            if (normal > 0)continue;

            SDL_RenderDrawLine(sdl->renderer, (int)x1, (int)y1, (int)x2, (int)y2);
            SDL_RenderDrawLine(sdl->renderer, (int)x2, (int)y2, (int)x3, (int)y3);
            SDL_RenderDrawLine(sdl->renderer, (int)x3, (int)y3, (int)x1, (int)y1);

        }

    }
    SDL_RenderPresent(sdl->renderer);
    // задержка
    //SDL_Delay(12);
}

void solid_without_light(SDL_data* sdl, Scene_data* scene) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) scene->z_bufer[i] = BASE_VALUE_Z_BUFER;

    for (int o = 0; o < scene->objects.cnt_object; o++) {
        Model* mdl = scene->objects.data[o].model;

        multiply_matrix_4x4(scene->view_matrix, scene->objects.data[o].transforms, scene->mv_matrix); //  View * Model
        multiply_matrix_4x4(scene->projection_matrix, scene->mv_matrix, scene->mvp_matrix);   //  Proj * (View * Model)


        provide_vector_vrtc(&(scene->global_render_bufer), mdl->vertices.cnt_vertices);
        for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
            apply_transform(scene->mvp_matrix, (float*)&(mdl->vertices.data[v]), (float*)&(scene->global_render_bufer.data[v]));
        }

        for (int p = 0; p < mdl->polygons.cnt_polygon; p++) {

            int* v = mdl->polygons.data[p].v;
            float w1 = scene->global_render_bufer.data[v[0]].w;
            float w2 = scene->global_render_bufer.data[v[1]].w;
            float w3 = scene->global_render_bufer.data[v[2]].w;

            if (w1 < scene->z_near || w2 < scene->z_near || w3 < scene->z_near || w1 > scene->z_far || w2 > scene->z_far || w3 > scene->z_far)continue;//clipping

            float x1 = (scene->global_render_bufer.data[v[0]].x / w1 + 1) * 0.5 * SCREEN_WIDTH;
            float x2 = (scene->global_render_bufer.data[v[1]].x / w2 + 1) * 0.5 * SCREEN_WIDTH;
            float x3 = (scene->global_render_bufer.data[v[2]].x / w3 + 1) * 0.5 * SCREEN_WIDTH;

            float y1 = (1 - scene->global_render_bufer.data[v[0]].y / w1) * 0.5 * SCREEN_HEIGHT;
            float y2 = (1 - scene->global_render_bufer.data[v[1]].y / w2) * 0.5 * SCREEN_HEIGHT;
            float y3 = (1 - scene->global_render_bufer.data[v[2]].y / w3) * 0.5 * SCREEN_HEIGHT;

            float normal = edge_function(x1, y1, x2, y2, x3, y3); //векторное произведение ребер грани
            if (scene->reverse_normal)normal *= -1;
            if (normal > 0)continue;

            float z1 = scene->global_render_bufer.data[v[0]].z / w1;
            float z2 = scene->global_render_bufer.data[v[1]].z / w2;
            float z3 = scene->global_render_bufer.data[v[2]].z / w3;
            draw_const_triangle(scene->z_bufer, x1, y1, z1, x2, y2, z2, x3, y3, z3);

        }

    }
    for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
        if (scene->z_bufer[i] < BASE_VALUE_Z_BUFER)
            scene->pixels[i] = RGBA(100, 100, 250, 255);
        else
            scene->pixels[i] = RGBA(0, 0, 100, 255);
    }
    SDL_UpdateTexture(sdl->texture, NULL, scene->pixels, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL);
    SDL_RenderPresent(sdl->renderer);
    // задержка
    //SDL_Delay(6);
}

void solid(SDL_data* sdl, Scene_data* scene) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        scene->pixels[i] = RGBA(33, 33, 33, 0);
        scene->z_bufer[i] = BASE_VALUE_Z_BUFER;
    }

    normalize_vec(&(scene->light));
    for (int o = 0; o < scene->objects.cnt_object; o++) {
        Model* mdl = scene->objects.data[o].model;

        multiply_matrix_4x4(scene->view_matrix, scene->objects.data[o].transforms, scene->mv_matrix); //  View * Model

        provide_vector_vrtc(&(scene->global_render_bufer), mdl->vertices.cnt_vertices);
        Vertice* vertices_mdl = scene->global_render_bufer.data;
        for (int v = 0; v < mdl->vertices.cnt_vertices; v++) {
            apply_transform(scene->mv_matrix, (float*)&(mdl->vertices.data[v]), (float*)&(vertices_mdl[v]));
        }

        for (int p = 0; p < mdl->polygons.cnt_polygon; p++) {
            int* plgs_vertices = mdl->polygons.data[p].v;

            Vec a, b, n;
            make_vec(&(vertices_mdl[plgs_vertices[0]]), &(vertices_mdl[plgs_vertices[1]]), &a);
            make_vec(&(vertices_mdl[plgs_vertices[0]]), &(vertices_mdl[plgs_vertices[2]]), &b);
            cross_product(&a, &b, &n);

            normalize_vec(&n);
            if (scene->reverse_normal) {
                n.x *= -1;
                n.y *= -1;
                n.z *= -1;
            }

            float light_intensity = dot_product(&n, &(scene->light));
            if (light_intensity < 0)light_intensity = 0;
            light_intensity += scene->ambient_light;
            if (light_intensity > 1)light_intensity = 1;

            Vertice transform_vertices[3];
            for (int i = 0; i < 3; i++) {
                apply_transform(scene->projection_matrix, (float*)&(vertices_mdl[mdl->polygons.data[p].v[i]]), (float*)&(transform_vertices[i]));
            }

            float w1 = transform_vertices[0].w;
            float w2 = transform_vertices[1].w;
            float w3 = transform_vertices[2].w;

            if (w1 < scene->z_near || w2 < scene->z_near || w3 < scene->z_near || w1 > scene->z_far || w2 > scene->z_far || w3 > scene->z_far)continue;//clipping

            float x1 = (transform_vertices[0].x / w1 + 1) * 0.5 * SCREEN_WIDTH;
            float x2 = (transform_vertices[1].x / w2 + 1) * 0.5 * SCREEN_WIDTH;
            float x3 = (transform_vertices[2].x / w3 + 1) * 0.5 * SCREEN_WIDTH;

            float y1 = (1 - transform_vertices[0].y / w1) * 0.5 * SCREEN_HEIGHT;
            float y2 = (1 - transform_vertices[1].y / w2) * 0.5 * SCREEN_HEIGHT;
            float y3 = (1 - transform_vertices[2].y / w3) * 0.5 * SCREEN_HEIGHT;

            //float normal = edge_function(x1, y1, x2, y2, x3, y3); //векторное произведение ребер грани
            //if (scene->reverse_normal)normal *= -1;
            //if (normal > 0)continue;

            float z1 = transform_vertices[0].z / w1;
            float z2 = transform_vertices[1].z / w2;
            float z3 = transform_vertices[2].z / w3;
            if (scene->reverse_normal)draw_triangle(scene->z_bufer, scene->pixels, light_intensity, x3, y3, z3, x2, y2, z2, x1, y1, z1);
            else draw_triangle(scene->z_bufer, scene->pixels, light_intensity, x1, y1, z1, x2, y2, z2, x3, y3, z3);

        }

    }

    SDL_UpdateTexture(sdl->texture, NULL, scene->pixels, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL);
    SDL_RenderPresent(sdl->renderer);
    // задержка
    //SDL_Delay(6);
}

int main(int argc, char* argv[]) {
    SDL_data data_sdl;
    start_SDL(&data_sdl);
    Scene_data data_scene;
    start_scene(&data_scene);
    float angle = 0; // crutch


    if (argc > 1) {
        if (parsing_obj_file(argv[1], &(data_scene.models), &(data_scene.objects)) == 0) {
            fprintf(stderr, "FILE not found\n");
        }
    }


    fprintf(stderr, "START MAIN LOOP\n");
    while (data_sdl.is_running) {
        event_handling(&data_sdl, &data_scene);


        build_projection_matrix(data_scene.FOV, (float)SCREEN_WIDTH / SCREEN_HEIGHT, data_scene.z_near, data_scene.z_far, data_scene.projection_matrix);
        build_view_matrix(&data_scene);

        data_scene.objects.data[0].transforms[0 * 4 + 0] = cosf(angle);
        data_scene.objects.data[0].transforms[0 * 4 + 1] = -sinf(angle);
        data_scene.objects.data[0].transforms[1 * 4 + 0] = sinf(angle);
        data_scene.objects.data[0].transforms[1 * 4 + 1] = cosf(angle);
        angle += 0.01;

        switch (data_scene.mode) {
        case WIREFRAME_MODE:
            wireframe(&data_sdl, &data_scene);
            break;
        case SOLID_WITHOUT_LIGHT_MODE:
            solid_without_light(&data_sdl, &data_scene);
            break;
        case SOLID_MODE:
            solid(&data_sdl, &data_scene);
            break;
        default:
            break;
        }
    }

    // Сохранение файла
    fprintf(stderr, "SAVE FILE\n");
    save_output_file(argc, argv, &data_scene.objects, &data_scene.global_render_bufer);

    // Очистка ресурсов
    fprintf(stderr, "FREE MEMORY\n");
    SDL_DestroyTexture(data_sdl.texture);
    SDL_DestroyRenderer(data_sdl.renderer);
    SDL_DestroyWindow(data_sdl.window);
    SDL_Quit();
    free_vector_vrtc(&data_scene.global_render_bufer);
    free_vector_mdl(&data_scene.models);
    free_vector_obj(&data_scene.objects);

    return 0;
}