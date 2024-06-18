#ifndef vertexData_h
#define vertexData_h

#include "common.cpp"
#include "vertex.h"

const std::vector<Vertex> quadVertices = {
    // tri
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // oben rechts
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // oben links
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},   // unten rechts
    {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // unten links
};

const std::vector<uint16_t>
    quadIndices = {0, 1, 2, 2, 1, 3};

const std::vector<Vertex> triVertices = {
    // pos,col?,tex
    {{0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // oben rechts
    {{-0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // oben links
    {{0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // unten rechts
};

#endif