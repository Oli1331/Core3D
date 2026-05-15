#include "easy_math.h"

void apply_transform(float transform[16], float v[4], float res[4]) {
    for (int i = 0; i < 4; i++) {
        res[i] = 0;
        for (int j = 0; j < 4; j++) {
            res[i] += v[j] * transform[i * 4 + j];
        }
    }
}

void multiply_matrix_4x4(float A[16], float B[16], float out[16]) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            out[r * 4 + c] = 0;
            for (int k = 0; k < 4; k++) {
                out[r * 4 + c] += A[r * 4 + k] * B[k * 4 + c];
            }
        }
    }
}

float edge_function(float x1, float y1, float x2, float y2, float x3, float y3) {
    return (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
}

void make_vec(Vertice* a, Vertice* b, Vec* v) {
    v->x = b->x - a->x;
    v->y = b->y - a->y;
    v->z = b->z - a->z;
}

void normalize_vec(Vec* v) {
    float len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len != 0) {
        v->x /= len; v->y /= len; v->z /= len;
    }
}

void cross_product(const Vec* a, const Vec* b, Vec* n) {
    n->x = a->y * b->z - a->z * b->y;
    n->y = a->z * b->x - a->x * b->z;
    n->z = a->x * b->y - a->y * b->x;
}

float dot_product(Vec* a, Vec* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void build_mvp_matrix(float model[16], float view[16], float proj[16], float mvp[16]) {
    float temp_view_model[16];
    multiply_matrix_4x4(view, model, temp_view_model); //  View * Model
    multiply_matrix_4x4(proj, temp_view_model, mvp);   //  Proj * (View * Model)
}

void build_view_matrix(Scene_data* scene) {
    const Vec Vec_Up = { 0, 1, 0 };
   
    scene->camera_direction.x = cosf(scene->camera_angley) * sinf(scene->camera_anglex);
    scene->camera_direction.y = sinf(scene->camera_angley);
    scene->camera_direction.z = cosf(scene->camera_angley) * cosf(scene->camera_anglex);
    Vec R; cross_product(&Vec_Up, &scene->camera_direction, &R);
    Vec U; cross_product(&scene->camera_direction, &R, &U);

    scene->view_matrix[0 * 4 + 0] = R.x + scene->shift_of_view[0 * 3 + 0];
    scene->view_matrix[0 * 4 + 1] = R.y + scene->shift_of_view[0 * 3 + 1];
    scene->view_matrix[0 * 4 + 2] = R.z + scene->shift_of_view[0 * 3 + 2];

    scene->view_matrix[1 * 4 + 0] = U.x + scene->shift_of_view[1 * 3 + 0];
    scene->view_matrix[1 * 4 + 1] = U.y + scene->shift_of_view[1 * 3 + 1];
    scene->view_matrix[1 * 4 + 2] = U.z + scene->shift_of_view[1 * 3 + 2];

    scene->view_matrix[2 * 4 + 0] = scene->camera_direction.x + scene->shift_of_view[2 * 3 + 0];
    scene->view_matrix[2 * 4 + 1] = scene->camera_direction.y + scene->shift_of_view[2 * 3 + 1];
    scene->view_matrix[2 * 4 + 2] = scene->camera_direction.z + scene->shift_of_view[2 * 3 + 2];

    scene->view_matrix[0 * 4 + 3] = dot_product(&(scene->camera_displacement), &R) + scene->start_position.x;
    scene->view_matrix[1 * 4 + 3] = dot_product(&(scene->camera_displacement), &U) + scene->start_position.y;
    scene->view_matrix[2 * 4 + 3] = dot_product(&(scene->camera_displacement), &scene->camera_direction) + scene->start_position.z;

    // scene->view_matrix[15] = 1.0f;
    // scene->view_matrix[3] = 0.f;
    // scene->view_matrix[7] = 0.f;
    // scene->view_matrix[11] = 0.f;
}

void rotate_object_x(Object* obj, float angle) {
    float rotate_matrix[16] =
    { 1,0,0,0,
    0, cosf(angle),-sinf(angle),0,
    0, sinf(angle), cosf(angle),0,
    0,0,0,1 };
    float buf[16];
    multiply_matrix_4x4(rotate_matrix, (obj->transforms), buf);
    memcpy(obj->transforms, buf, 16 * (sizeof(float)));
}

void rotate_object_y(Object* obj, float angle) {
    float rotate_matrix[16] =
    { cosf(angle),0,sinf(angle),0,
    0,1,0,0,
    -sinf(angle),0, cosf(angle),0,
    0,0,0,1 };
    float buf[16];
    multiply_matrix_4x4(rotate_matrix, (obj->transforms), buf);

    memcpy(obj->transforms, buf, 16 * (sizeof(float)));
}

void rotate_object_z(Object* obj, float angle) {
    float rotate_matrix[16] =
    { cosf(angle),-sinf(angle),0,0,
      sinf(angle), cosf(angle),0,0,
      0,0,1,0,
      0,0,0,1 };
    float buf[16];
    multiply_matrix_4x4(rotate_matrix, (obj->transforms), buf);
    memcpy(obj->transforms, buf, 16 * (sizeof(float)));
}

void rotate_object_YXZ(Object* obj, float anglex, float angley, float anglez) {
    float rotate_x_matrix[16] =
    { 1,0,0,0,
    0, cosf(anglex),-sinf(anglex),0,
    0, sinf(anglex), cosf(anglex),0,
    0,0,0,1 };
    float rotate_y_matrix[16] =
    { cosf(angley),0,sinf(angley),0,
    0,1,0,0,
    -sinf(angley),0, cosf(angley),0,
    0,0,0,1 };
    float rotate_z_matrix[16] =
    { cosf(anglez),-sinf(anglez),0,0,
      sinf(anglez), cosf(anglez),0,0,
      0,0,1,0,
      0,0,0,1 };
    float buf1[16];
    float buf2[16];
    multiply_matrix_4x4(rotate_z_matrix, rotate_y_matrix, buf1);
    multiply_matrix_4x4(buf1, rotate_x_matrix, buf2);
    multiply_matrix_4x4(buf2, (obj->transforms), buf1);
    memcpy(obj->transforms, buf1, 16 * (sizeof(float)));
}