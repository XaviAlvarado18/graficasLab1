#include <iostream>
#include "Color.h"
#include "Vertex2.h"
#include <vector>
#include <fstream>
#include <cstdint>
#include <limits>
#include <algorithm>



// Declaración de la variable global framebuffer
std::vector<Color> framebuffer;
Color clearColor(0, 0, 0); // Definir clearColor como NAVY_BLUE

// Declaración de la variable global currentColor
Color currentColor(255, 255, 255); // Puedes cambiar los valores según tu elección
Color fillColor{255, 255, 0};  // Color rojo


std::vector<Vertex2> points; // Declaración y definición de 'points' como un vector de Vertex2

std::vector<Vertex2> polygonVertices;


int width = 1000;
int height = 1800;

using namespace std;

struct Vertex {
    float x;
    float y;
};

void punto(Vertex vertex, Color color) {
    int x = static_cast<int>(vertex.x);
    int y = static_cast<int>(vertex.y);
    if (x >= 0 && x < width && y >= 0 && y < height) {
        framebuffer[y * width + x] = color;
    }
}

bool isPointInPolygon(const Vertex& point, const std::vector<Vertex>& vertices, const Color& fillcolor) {
    int i, j;
    bool isInside = false;
    int n = vertices.size();

    for (i = 0, j = n - 1; i < n; j = i++) {
        if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
            (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
            isInside = !isInside;
        }
    }

    if (isInside) {
        punto(point, fillcolor); // Dibujar el punto con el color de relleno
    }

    return isInside;
}


struct Edge {
    float x;       // Coordenada x del borde (intersección)
    float slopeReciprocal; // Recíproco de la pendiente del borde
};

void clear() {
    std::fill(framebuffer.begin(), framebuffer.end(), clearColor);
}





void renderBuffer() {
    std::ofstream file("out.bmp", std::ios::binary);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo de salida." << std::endl;
        return;
    }

    // Encabezado del archivo BMP
    uint32_t fileSize = 54 + (width * height * 3);
    uint32_t reserved = 0;
    uint32_t dataOffset = 54;
    uint32_t dibHeaderSize = 40;
    uint32_t imageWidth = width;
    uint32_t imageHeight = height;
    uint16_t planes = 1;
    uint16_t bitsPerPixel = 24;
    uint32_t compression = 0;
    uint32_t imageSize = width * height * 3;
    uint32_t horizontalResolution = 2835;  // 72 dpi
    uint32_t verticalResolution = 2835;    // 72 dpi

    file.write("BM", 2);
    file.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
    file.write(reinterpret_cast<char*>(&reserved), sizeof(reserved));
    file.write(reinterpret_cast<char*>(&dataOffset), sizeof(dataOffset));
    file.write(reinterpret_cast<char*>(&dibHeaderSize), sizeof(dibHeaderSize));
    file.write(reinterpret_cast<char*>(&imageWidth), sizeof(imageWidth));
    file.write(reinterpret_cast<char*>(&imageHeight), sizeof(imageHeight));
    file.write(reinterpret_cast<char*>(&planes), sizeof(planes));
    file.write(reinterpret_cast<char*>(&bitsPerPixel), sizeof(bitsPerPixel));
    file.write(reinterpret_cast<char*>(&compression), sizeof(compression));
    file.write(reinterpret_cast<char*>(&imageSize), sizeof(imageSize));
    file.write(reinterpret_cast<char*>(&horizontalResolution), sizeof(horizontalResolution));
    file.write(reinterpret_cast<char*>(&verticalResolution), sizeof(verticalResolution));
    file.write(reinterpret_cast<char*>(&reserved), sizeof(reserved));
    file.write(reinterpret_cast<char*>(&reserved), sizeof(reserved));

    // Datos del framebuffer
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            Color pixel = framebuffer[(height - 1 - y) * width + x];
            file.write(reinterpret_cast<char*>(&pixel.b), sizeof(pixel.b));
            file.write(reinterpret_cast<char*>(&pixel.g), sizeof(pixel.g));
            file.write(reinterpret_cast<char*>(&pixel.r), sizeof(pixel.r));
        }
    }

    file.close();
    std::cout << "Archivo BMP creado exitosamente." << std::endl;
}

void linea(Vertex start, Vertex end) {
    // Convertir los valores de punto flotante a enteros
    int x0 = static_cast<int>(start.x);
    int y0 = static_cast<int>(start.y);
    int x1 = static_cast<int>(end.x);
    int y1 = static_cast<int>(end.y);

    // Calcular las diferencias entre los valores iniciales y finales
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    // Determinar las direcciones en las que se incrementarán los valores
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    // Calcular el error inicial
    int error = dx - dy;

    while (true) {
        punto(Vertex{static_cast<float>(x0), static_cast<float>(y0)}, currentColor);

        // Verificar si se alcanzó el punto final
        if (x0 == x1 && y0 == y1) {
            break;
        }

        int e2 = 2 * error;
        if (e2 > -dy) {
            error -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            error += dx;
            y0 += sy;
        }
    }
}

void drawPolygon(const std::vector<Vertex>& vertices) {
    // Verificar si hay suficientes vértices para dibujar un polígono
    if (vertices.size() < 2) {
        std::cout << "El polígono debe tener al menos 2 vértices." << std::endl;
        return;
    }

    // Dibujar las líneas del polígono
    for (size_t i = 0; i < vertices.size() - 1; ++i) {
        Vertex start = vertices[i];
        Vertex end = vertices[i + 1];
        linea(start, end);
    }

    // Conectar el último vértice con el primer vértice para cerrar el polígono
    Vertex lastVertex = vertices.back();
    Vertex firstVertex = vertices.front();
    linea(lastVertex, firstVertex);
}

void fillPolygon(const std::vector<Vertex>& vertices, const Color& fillcolor) {
    // Verificar si hay suficientes vértices para dibujar un polígono
    if (vertices.size() < 3) {
        std::cout << "El polígono debe tener al menos 3 vértices." << std::endl;
        return;
    }

    // Obtener los límites del polígono
    float minX = vertices[0].x;
    float minY = vertices[0].y;
    float maxX = vertices[0].x;
    float maxY = vertices[0].y;

    for (const auto& vertex : vertices) {
        minX = std::min(minX, vertex.x);
        minY = std::min(minY, vertex.y);
        maxX = std::max(maxX, vertex.x);
        maxY = std::max(maxY, vertex.y);
    }

    // Iterar sobre cada píxel dentro de los límites del polígono
    for (float y = minY; y <= maxY; ++y) {
        for (float x = minX; x <= maxX; ++x) {
            // Construir un punto en las coordenadas (x, y)
            Vertex point{x, y};

            // Verificar si el punto está dentro del polígono
            isPointInPolygon(point, vertices, fillcolor);
        }
    }
}



void render() {
    clear();

    std::vector<Vertex> polygonVertices;
    

    polygonVertices.push_back({165.0f, 380.0f});
    polygonVertices.push_back({185.0f, 360.0f});
    polygonVertices.push_back({180.0f, 330.0f});
    polygonVertices.push_back({207.0f, 345.0f});
    polygonVertices.push_back({233.0f, 330.0f});
    polygonVertices.push_back({230.0f, 360.0f});
    polygonVertices.push_back({250.0f, 380.0f});
    polygonVertices.push_back({220.0f, 385.0f});
    polygonVertices.push_back({205.0f, 410.0f});
    polygonVertices.push_back({193.0f, 383.0f});

    
    fillPolygon(polygonVertices, fillColor);

    renderBuffer();
}

int main() {
    // Asignar memoria para el framebuffer
    framebuffer.resize(width * height);

    // Llamar a la función de renderizado
    render();

    return 0;
}