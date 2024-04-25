#include <map>
#include <vector>
#include <array>
#include <functional>
#include "gl_classes.hpp"

class ReverseDelete {
public:
    struct EdgeInfo {
        size_t firstNode;
        size_t secondNode;
    };

    ReverseDelete(const size_t numberOfNodes, const std::multimap<GLfloat, EdgeInfo>& edges) : numberOfNodes(numberOfNodes), adjacencyMatrix(numberOfNodes*(numberOfNodes + 1)/2, false), program("../shaders/general.vert", "../shaders/general.frag") {
        double sqrt_numberOfNodes = std::sqrt(numberOfNodes);
        gridInfo.sizeX = std::ceil(sqrt_numberOfNodes);
        gridInfo.sizeY = std::ceil(numberOfNodes/gridInfo.sizeX);

        for (const auto& it : edges) {
            if (it.second.firstNode != it.second.secondNode) {
                const std::array<GLfloat, 4> GLdata = {
                        (static_cast<GLfloat>(it.second.firstNode % gridInfo.sizeX) + 0.5f)/GLfloat(gridInfo.sizeX),
                        (static_cast<GLfloat>(it.second.firstNode / gridInfo.sizeX) + 0.5f)/GLfloat(gridInfo.sizeX),
                        (static_cast<GLfloat>(it.second.secondNode % gridInfo.sizeX) + 0.5f)/GLfloat(gridInfo.sizeX),
                        (static_cast<GLfloat>(it.second.secondNode / gridInfo.sizeX) + 0.5f)/GLfloat(gridInfo.sizeX)
                };
                (this->edges).emplace(it.first, EdgeInfoGL{it.second.firstNode, it.second.secondNode, GLdata});

                adjacencyMatrix[index(it.second.firstNode, it.second.secondNode)] = true;
            }
        }

        // Init nodeVBO
        std::vector<GLfloat> nodeData(2*numberOfNodes);
        for (size_t i = 0; i < numberOfNodes; i++) {
            nodeData[2*i] = ((i % gridInfo.sizeX) + 0.5f)/gridInfo.sizeX;
            nodeData[2*i + 1] = ((i / gridInfo.sizeX) + 0.5f)/gridInfo.sizeX;
        }
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, nodeVBO))
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*numberOfNodes, nodeData.data(), GL_STATIC_DRAW))
        GL_CALL(glBindVertexArray(pointVAO))
        GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr))
        GL_CALL(glEnableVertexAttribArray(0))
        GL_CALL(glBindVertexArray(0))
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0u))

        // TODO: Remove IT!
        for (const auto& it : this->edges) {
            std::cout << it.first << " " << it.second.firstNode << " " << it.second.secondNode << " " << it.second.data[0] << " " << it.second.data[1] << " " << it.second.data[2] << " " << it.second.data[3] << std::endl;
        }
    }

    void draw() {
        // Draw Edges
        GL_CALL(glUseProgram(program))
        GL_CALL(glBindVertexArray(0))
        GL_CALL(glEnableVertexAttribArray(0))
        for (const auto& it: edges) {
            GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, it.second.data.data()))
            GL_CALL(glUniform1f(0, it.first))
            GL_CALL(glDrawArrays(GL_LINES, 0, 2))
        }

        // Draw Nodes
        GL_CALL(glBindVertexArray(pointVAO))
        GL_CALL(glUniform1f(0, 1.0f))
        GL_CALL(glDrawArrays(GL_POINTS, 0, numberOfNodes))
    }

    void solve() {

        auto&& it = std::prev(edges.end());
        while(it != edges.begin()) {
            // TODO: Problem here
            const auto checkGraphConnectivity = [&](size_t row, size_t column){
                if (column > row) {
                    std::swap(row, column);
                }

                bool connectivityX = false;
                for (size_t j = 0; j <= row; j++) {
                    if (j == column) {continue;}
                    if (adjacencyMatrix[index(row, j)]) {
                        connectivityX = true;
                        break;
                    }
                }
                for (size_t i = row + 1; i < numberOfNodes; i++) {
                    if (adjacencyMatrix[index(i, row)]) {
                        connectivityX = true;
                        break;
                    }
                }

                bool connectivityY = false;
                for (size_t i = column; i < numberOfNodes; i++) {
                    if (i == row) {continue;}
                    if (adjacencyMatrix[index(i, column)]) {
                        connectivityY = true;
                        break;
                    }
                }
                for (size_t j = 0; j < column; j++) {
                    if (adjacencyMatrix[index(column, j)]) {
                        connectivityY = true;
                        break;
                    }
                }

                return connectivityX && connectivityY;
            };

            size_t row = it->second.firstNode, column = it->second.secondNode;
            if (!checkGraphConnectivity(row, column)) {
                it--;
                continue;
            }

            adjacencyMatrix[index(row, column)] = false;

            // TODO: Remove IT!
            for (size_t i = 0; i < numberOfNodes; i++) {
                for (size_t j = 0; j <= i; j++) {
                    std::cout << adjacencyMatrix[index(i, j)] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "-------------------------" << std::endl;

            edges.erase(it--);
        }
    }

private:
    struct EdgeInfoGL : public EdgeInfo {
        std::array<GLfloat, 4> data = {};
    };

    struct GridInfo {
        size_t sizeX = 0;
        size_t sizeY = 0;
    };

    // Info fields
    const size_t numberOfNodes;
    std::multimap<GLfloat, EdgeInfoGL> edges;
    GridInfo gridInfo;
    std::vector<bool> adjacencyMatrix;
    std::function<size_t(size_t, size_t)> index = [](size_t row, size_t column) -> size_t {
        if (column > row) {
            std::swap(row, column);
        }
        return (row*(row + 1)/2) + column;
    };
    // GL fields
    OpenGL::VAO pointVAO;
    OpenGL::GraphicsProgram program;
    OpenGL::VBO nodeVBO;
};

void windowThings() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Count2", sf::Style::Close, sf::ContextSettings(24, 8, 1, 4, 3));
    window.setFramerateLimit(144);
    glewExperimental = true;
    if (GLEW_OK != glewInit()) {
        std::cerr << "glew was not initialized";
        exit(0);
    }

    // Init
    bool windowIsOpen = true;
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f))
    GL_CALL(glPointSize(15.0f))
    GL_CALL(glLineWidth(3.0f))

    size_t nodeNum, edgeNum;
    std::cin >> nodeNum >> edgeNum;
    std::multimap<GLfloat, ReverseDelete::EdgeInfo> data;
    for (size_t i = 0; i < edgeNum; i++) {
        GLfloat weight;
        size_t firstNode, secondNode;
        std::cin >> weight >> firstNode >> secondNode;
        data.emplace(weight, ReverseDelete::EdgeInfo{firstNode, secondNode});
    }
    ReverseDelete reverseDelete(nodeNum, data);

    while(windowIsOpen) {
        sf::Event event;

        while(window.pollEvent(event)) {
            switch(event.type) {
                case sf::Event::Closed: {
                    windowIsOpen = false;
                    break;
                }
                case sf::Event::KeyPressed: {
                    reverseDelete.solve();
                    break;
                }
            }
        }

        GL_CALL(glClear(GL_COLOR_BUFFER_BIT))
        reverseDelete.draw();
        window.display();
    }
}

int main() {

    windowThings();

    return 0;
}
