#ifndef EASY_MATH_H
#define EASY_MATH_H
#include "elements.h"
#include <math.h>


void apply_transform(float transform[16], float v[4], float res[4]);
void multiply_matrix_4x4(float A[16], float B[16], float out[16]);
float edge_function(float x1, float y1, float x2, float y2, float x3, float y3);
void make_vec(Vertice* a, Vertice* b, Vec* v);
void cross_product(Vec* a, Vec* b, Vec* n);
void normalize_vec(Vec* v);
float dot_product(Vec* a, Vec* b);
void build_mvp_matrix(float model[16], float view[16], float proj[16], float mvp[16]);


#endif