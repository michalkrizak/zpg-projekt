#include "Model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <iostream>
#include <tuple>
#include <cmath>

Model::Model(const float* vertices, size_t size, int vertexSize) {
    count = static_cast<int>(size / (vertexSize * sizeof(float)));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    if (vertexSize >= 6) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    // Texture coordinate attribute (location = 2)
    if (vertexSize >= 8) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
}

Model::~Model() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Model::bindModel() const {
    glBindVertexArray(VAO);
}

int Model::getCount() const {
    return count;
}

// --- Minimal OBJ loader implementation ---
static bool parseFaceVertex(const std::string& token, int& vi, int& vti, int& vni) {
    // token formats: v, v/vt, v//vn, v/vt/vn (1-based)
    vi = vti = vni = 0;
    int slash1 = -1, slash2 = -1;
    for (size_t i = 0; i < token.size(); ++i) {
        if (token[i] == '/') {
            if (slash1 == -1) slash1 = static_cast<int>(i);
            else { slash2 = static_cast<int>(i); break; }
        }
    }
    try {
        if (slash1 == -1) {
            vi = std::stoi(token);
        } else if (slash2 == -1) {
            // v/vt
            vi = std::stoi(token.substr(0, slash1));
            std::string vtStr = token.substr(slash1 + 1);
            if (!vtStr.empty()) vti = std::stoi(vtStr);
        } else {
            // v/vt/vn or v//vn
            vi = std::stoi(token.substr(0, slash1));
            if (slash2 > slash1 + 1) {
                std::string vtStr = token.substr(slash1 + 1, slash2 - slash1 - 1);
                if (!vtStr.empty()) vti = std::stoi(vtStr);
            }
            std::string vnStr = token.substr(slash2 + 1);
            if (!vnStr.empty()) vni = std::stoi(vnStr);
        }
    } catch (...) {
        return false;
    }
    return true;
}

static void computeFaceNormal(const float a[3], const float b[3], const float c[3], float n[3]) {
    float u[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2] };
    float v[3] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };
    // cross(u, v)
    n[0] = u[1] * v[2] - u[2] * v[1];
    n[1] = u[2] * v[0] - u[0] * v[2];
    n[2] = u[0] * v[1] - u[1] * v[0];
    float len = std::sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    if (len > 1e-6f) { n[0] /= len; n[1] /= len; n[2] /= len; }
}

std::unique_ptr<Model> Model::loadFromOBJ(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return nullptr;
    }

    std::vector<float> positions; // flat array 3 floats per vertex
    std::vector<float> normals;   // flat array 3 floats per normal
    std::vector<float> texcoords; // flat array 2 floats per texcoord
    std::vector<float> out;       // interleaved pos(3)+normal(3)+texcoord(2)

    std::string line;
    positions.reserve(1024);
    normals.reserve(1024);
    texcoords.reserve(1024);
    out.reserve(4096);

    auto pushVertex = [&](int vi, int vti, int vni, const float fallbackN[3]) {
        // OBJ indices are 1-based; allow negative indices (relative to end)
        auto fixIndex = [](int idx, int count) {
            if (idx > 0) return idx - 1;          // 1..N -> 0..N-1
            if (idx < 0) return count + idx;      // -1 -> last, etc.
            return 0; // shouldn't happen
        };

        int pCount = static_cast<int>(positions.size() / 3);
        int nCount = static_cast<int>(normals.size() / 3);
        int tCount = static_cast<int>(texcoords.size() / 2);
        int pIdx = fixIndex(vi, pCount);
        int nIdx = (vni != 0) ? fixIndex(vni, nCount) : -1;
        int tIdx = (vti != 0) ? fixIndex(vti, tCount) : -1;

        const float* p = &positions[pIdx * 3];
        const float* n = fallbackN;
        float tempN[3];
        if (nIdx >= 0) {
            n = &normals[nIdx * 3];
        } else if (!fallbackN) {
            // no normal and no fallback, provide (0,1,0)
            tempN[0] = 0.0f; tempN[1] = 1.0f; tempN[2] = 0.0f;
            n = tempN;
        }

        // Get texture coordinates or use default (0,0)
        float u = 0.0f, v = 0.0f;
        if (tIdx >= 0) {
            u = texcoords[tIdx * 2];
            v = texcoords[tIdx * 2 + 1];
        }

        out.insert(out.end(), { p[0], p[1], p[2], n[0], n[1], n[2], u, v });
    };

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        // Trim leading spaces
        size_t start = 0; while (start < line.size() && std::isspace(static_cast<unsigned char>(line[start]))) ++start;
        if (start >= line.size()) continue;
        if (line[start] == '#') continue;

        std::istringstream iss(line.substr(start));
        std::string tag;
        iss >> tag;
        if (tag == "v") {
            float x, y, z; iss >> x >> y >> z;
            positions.push_back(x); positions.push_back(y); positions.push_back(z);
        } else if (tag == "vn") {
            float x, y, z; iss >> x >> y >> z;
            // Normalize normals just in case
            float len = std::sqrt(x*x + y*y + z*z);
            if (len > 1e-6f) { x/=len; y/=len; z/=len; }
            normals.push_back(x); normals.push_back(y); normals.push_back(z);
        } else if (tag == "vt") {
            float u, v; iss >> u >> v;
            texcoords.push_back(u); texcoords.push_back(v);
        } else if (tag == "f") {
            // Read face vertices (3+ tokens)
            std::vector<std::tuple<int,int,int>> face; face.reserve(8);
            std::string vert;
            while (iss >> vert) {
                int vi=0, vti=0, vni=0;
                if (!parseFaceVertex(vert, vi, vti, vni)) {
                    std::cerr << "Failed to parse face token: " << vert << std::endl;
                    face.clear(); break;
                }
                face.emplace_back(vi, vti, vni);
            }
            if (face.size() < 3) continue;

            // Triangulate as fan: (0, i, i+1)
            for (size_t i = 1; i + 1 < face.size(); ++i) {
                int vi0 = std::get<0>(face[0]);
                int vt0 = std::get<1>(face[0]);
                int vn0 = std::get<2>(face[0]);
                int vi1 = std::get<0>(face[i]);
                int vt1 = std::get<1>(face[i]);
                int vn1 = std::get<2>(face[i]);
                int vi2 = std::get<0>(face[i+1]);
                int vt2 = std::get<1>(face[i+1]);
                int vn2 = std::get<2>(face[i+1]);

                // If any normal missing, compute per-face normal
                float fallbackN[3];
                float* fallbackPtr = nullptr;
                if (vn0 == 0 || vn1 == 0 || vn2 == 0) {
                    int pCount = static_cast<int>(positions.size() / 3);
                    auto fixIndex = [](int idx, int count) {
                        if (idx > 0) return idx - 1; if (idx < 0) return count + idx; return 0; };
                    int i0 = fixIndex(vi0, pCount);
                    int i1 = fixIndex(vi1, pCount);
                    int i2 = fixIndex(vi2, pCount);
                    const float* a = &positions[i0*3];
                    const float* b = &positions[i1*3];
                    const float* c = &positions[i2*3];
                    computeFaceNormal(a, b, c, fallbackN);
                    fallbackPtr = fallbackN;
                }

                pushVertex(vi0, vt0, vn0, fallbackPtr);
                pushVertex(vi1, vt1, vn1, fallbackPtr);
                pushVertex(vi2, vt2, vn2, fallbackPtr);
            }
        }
        // ignore other tags (mtllib, usemtl, etc.) for now
    }

    if (out.empty()) {
        std::cerr << "OBJ contained no triangles: " << path << std::endl;
        return nullptr;
    }

    // Build Model from the interleaved vertex data with UV coordinates
    // Format: pos(3) + normal(3) + texcoord(2) = 8 floats per vertex
    return std::make_unique<Model>(out.data(), out.size() * sizeof(float), 8);
}
