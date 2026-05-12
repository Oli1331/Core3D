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

void cross_product(Vec* a, Vec* b, Vec* n) {
    n->x = a->y * b->z - a->z * b->y;
    n->y = a->z * b->x - a->x * b->z;
    n->z = a->x * b->y - a->y * b->x;
}

void normalize_vec(Vec* v) {
    float len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= len; v->y /= len; v->z /= len;
}

float dot_product(Vec* a, Vec* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void build_mvp_matrix(float model[16], float view[16], float proj[16], float mvp[16]) {
    float temp_view_model[16];
    multiply_matrix_4x4(view, model, temp_view_model); //  View * Model
    multiply_matrix_4x4(proj, temp_view_model, mvp);   //  Proj * (View * Model)
}
