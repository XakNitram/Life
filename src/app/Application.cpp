#include "pch.hpp"
#include "Window/Window.hpp"
#include "Window/Event.hpp"

using namespace lwvl::debug;
using namespace std::chrono;

/* Plans:
 *   pause with spacebar
 *   click to add cell
 *   press spacebar again to resume simulation
 *   drag to add multiple cells?
 *   simulation starts paused
 *   zoom with mousewheel?
 */

/* Rules:
 * - Any live cell with fewer than two live neighbours dies, as if by underpopulation.
 * - Any live cell with two or three live neighbours lives on to the next generation.
 * - Any live cell with more than three live neighbours dies, as if by overpopulation.
 * - Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 */

class Board {
    static constexpr size_t s_padding = 5;
    std::unique_ptr<bool[]> m_primaryData;
    std::unique_ptr<bool[]> m_secondaryData;
    size_t m_width, m_height;
public:
    [[nodiscard]] size_t width() const {
        return m_width - (2 * s_padding);
    }

    [[nodiscard]] size_t height() const {
        return m_height - (2 * s_padding);
    }

    [[nodiscard]] bool get(size_t i, size_t j) const {
        return m_primaryData[m_width * (j + s_padding) + (i + s_padding)];
    }

    // Make the board 1 longer than visible on each side
    Board(size_t width, size_t height) :
            m_primaryData(std::make_unique<bool[]>((width + (2 * s_padding)) * (height + (2 * s_padding)))),
            m_secondaryData(std::make_unique<bool[]>((width + (2 * s_padding)) * (height + (2 * s_padding)))),
            m_width(width + (2 * s_padding)), m_height(height + (2 * s_padding))
    {
        for (size_t j = 0; j < m_height; j++) {
            for (size_t i = 0; i < m_width; i++) {
                bool status = false;
                const size_t index = m_width * j + i;
                m_primaryData[index] = status;
                m_secondaryData[index] = status;
            }
        }

        size_t yOff = 20;
        size_t xOff = 6;
        m_primaryData[m_width * (m_height - (0 + yOff)) + (0 + xOff)] = true;
        m_primaryData[m_width * (m_height - (0 + yOff)) + (1 + xOff)] = true;
        m_primaryData[m_width * (m_height - (0 + yOff)) + (7 + xOff)] = true;
        m_primaryData[m_width * (m_height - (0 + yOff)) + (8 + xOff)] = true;

        m_primaryData[m_width * (m_height - (1 + yOff)) + (0 + xOff)] = true;
        m_primaryData[m_width * (m_height - (1 + yOff)) + (1 + xOff)] = true;
        m_primaryData[m_width * (m_height - (1 + yOff)) + (7 + xOff)] = true;
        m_primaryData[m_width * (m_height - (1 + yOff)) + (8 + xOff)] = true;

        m_primaryData[m_width * (m_height - (3 + yOff)) + (4 + xOff)] = true;
        m_primaryData[m_width * (m_height - (3 + yOff)) + (5 + xOff)] = true;

        m_primaryData[m_width * (m_height - (4 + yOff)) + (4 + xOff)] = true;
        m_primaryData[m_width * (m_height - (4 + yOff)) + (5 + xOff)] = true;

        m_primaryData[m_width * (m_height - (9 + yOff)) + (22 + xOff)] = true;
        m_primaryData[m_width * (m_height - (9 + yOff)) + (23 + xOff)] = true;
        m_primaryData[m_width * (m_height - (9 + yOff)) + (25 + xOff)] = true;
        m_primaryData[m_width * (m_height - (9 + yOff)) + (26 + xOff)] = true;

        m_primaryData[m_width * (m_height - (10 + yOff)) + (21 + xOff)] = true;
        m_primaryData[m_width * (m_height - (10 + yOff)) + (27 + xOff)] = true;

        m_primaryData[m_width * (m_height - (11 + yOff)) + (21 + xOff)] = true;
        m_primaryData[m_width * (m_height - (11 + yOff)) + (28 + xOff)] = true;
        m_primaryData[m_width * (m_height - (11 + yOff)) + (31 + xOff)] = true;
        m_primaryData[m_width * (m_height - (11 + yOff)) + (32 + xOff)] = true;

        m_primaryData[m_width * (m_height - (12 + yOff)) + (21 + xOff)] = true;
        m_primaryData[m_width * (m_height - (12 + yOff)) + (22 + xOff)] = true;
        m_primaryData[m_width * (m_height - (12 + yOff)) + (23 + xOff)] = true;
        m_primaryData[m_width * (m_height - (12 + yOff)) + (27 + xOff)] = true;
        m_primaryData[m_width * (m_height - (12 + yOff)) + (31 + xOff)] = true;
        m_primaryData[m_width * (m_height - (12 + yOff)) + (32 + xOff)] = true;

        m_primaryData[m_width * (m_height - (13 + yOff)) + (26 + xOff)] = true;

        //m_primaryData[m_width * (m_height - (17 + yOff)) + (20 + xOff)] = true;
        //m_primaryData[m_width * (m_height - (17 + yOff)) + (21 + xOff)] = true;
        //
        //m_primaryData[m_width * (m_height - (18 + yOff)) + (20 + xOff)] = true;
        //
        //m_primaryData[m_width * (m_height - (19 + yOff)) + (21 + xOff)] = true;
        //m_primaryData[m_width * (m_height - (19 + yOff)) + (22 + xOff)] = true;
        //m_primaryData[m_width * (m_height - (19 + yOff)) + (23 + xOff)] = true;
        //
        //m_primaryData[m_width * (m_height - (20 + yOff)) + (23 + xOff)] = true;
    }

    void update() {
        for (ptrdiff_t j = 0; j < m_height; ++j) {
            for (ptrdiff_t i = 0; i < m_width; ++i) {
                //if (i) {std::cout << ", "; }
                const size_t index = m_width * j + i;
                bool status = m_primaryData[index];
                ptrdiff_t count = 0;
                for (ptrdiff_t ny = -1; ny < 2; ++ny) {
                    for (ptrdiff_t nx = -1; nx < 2; ++nx) {
                        if (nx == 0 && ny == 0) {
                            continue;
                        }

                        const ptrdiff_t newJ = j + ny;
                        const ptrdiff_t newI = i + nx;
                        if (newJ < 0 || newJ >= m_height || newI < 0 || newI >= m_width) {
                            continue;
                        }

                        if (m_primaryData[m_width * newJ + newI]) {
                            count++;
                        }
                    }
                }

                if (status && (count < 2 || count > 3)) {
                    status = false;
                } else if (!status && count == 3) {
                    status = true;
                }

                m_secondaryData[index] = status;
                //std::cout << status;
            }
            //std::cout << std::endl;
        }
        //std::cout << std::endl;

        std::unique_ptr<bool[]> temp = std::move(m_primaryData);
        m_primaryData = std::move(m_secondaryData);
        m_secondaryData = std::move(temp);
    }
};

class Renderer {
    struct BoardVertex {
        float x, y;
    };

    lwvl::Program m_outlineControl;
    lwvl::VertexArray m_outlineLayout;
    lwvl::Buffer m_outlineIndices;

    lwvl::Program m_boardControl;
    lwvl::VertexArray m_boardLayout;
    lwvl::Buffer m_boardIndices;

    lwvl::Buffer m_cellBuffer;
    lwvl::Texture m_cellTexture{lwvl::Texture::Target::TextureBuffer};

    lwvl::Buffer m_vertices;

    Board const* m_board;
public:
    Renderer(Board const* board, float aspect): m_board(board) {
        const size_t xQuads = m_board->width();
        const size_t yQuads = m_board->height();
        m_boardControl.link(
            lwvl::VertexShader::readFile("Data/Shaders/board.vert"),
            lwvl::FragmentShader::readFile("Data/Shaders/board.frag")
        );

        m_outlineControl.link(
            lwvl::VertexShader::readFile("Data/Shaders/outline.vert"),
            lwvl::FragmentShader::readFile("Data/Shaders/outline.frag")
        );

        // 1 x and 1 y should be the same length,
        // regardless of the width and height of the board.
        {
            const float boardAspect = static_cast<float>(xQuads) / static_cast<float>(yQuads);
            // if aspect >= 1:
            // - bars will be on the horizontal sides if y > x
            // - bars will be on the vertical sides if x > y
            // if aspect < 1:
            // - bars will be on the horizontal sides if x > y
            // - bars will be on the vertical sides if y > x

            std::cout
                << "Aspect: " << aspect
                << " board aspect: " << boardAspect
                << " aspect diff: " << aspect - boardAspect
                << std::endl;

            float w, h;
            float xOffset, yOffset;
            /*//if (aspect >= 1.0f) {
            //    w = aspect;
            //    h = 1.0f;
            //    if (boardAspect >= 1.0f) {
            //        xOffset = -1.0f / aspect;
            //        yOffset = -1.0f;
            //    } else {
            //
            //    }
            //} else {
            //    w = 1.0f;
            //    h = 1.0f / aspect;
            //    if (boardAspect >= 1.0f) {
            //        xOffset = -1.0f;
            //        yOffset = -aspect;
            //    } else {
            //
            //    }
            //}*/
            //w = aspect - boardAspect * 0.25f;

            //w = aspect / boardAspect;
            //h = 1.0f;
            //xOffset = -1.0f / w;
            //yOffset = -1.0f;
            //
            //float ortho[16] {
            //    2.0f / w, 0.0f,     0.0f, 0.0f,
            //    0.0f,     2.0f / h, 0.0f, 0.0f,
            //    0.0f,     0.0f,     1.0f, 0.0f,
            //    xOffset,  yOffset,  0.0f, 1.0f
            //};
            //m_control.uniform("projection").matrix4F(ortho);
        }

        m_boardControl.bind();
        m_boardControl.uniform("data").setI(0);
        m_boardControl.uniform("projection").orthoAspectSimple(aspect);

        m_outlineControl.bind();
        m_outlineControl.uniform("projection").orthoAspectSimple(aspect);

        float xSlice = 1.0f / static_cast<float>(xQuads);
        float ySlice = 1.0f / static_cast<float>(yQuads);

        // Create vertex array
        const size_t vertexDataSize = xQuads * yQuads * 4;
        auto* vertexData = new BoardVertex[vertexDataSize];
        const size_t vertexOffsets[8] { 0, 0, 1, 0, 1, 1, 0, 1 };
        for(size_t row = 0; row < yQuads; ++row) {
            for(size_t col = 0; col < xQuads; ++col) {
                for (size_t i = 0; i < 4; ++i) {
                    const size_t index = ((xQuads * row + col) * 4) + (i);
                    const size_t offset = i * 2;
                    auto& currentVertex = vertexData[index];
                    currentVertex.x = static_cast<float>(col + vertexOffsets[offset + 0]) * xSlice;
                    currentVertex.y = static_cast<float>(row + vertexOffsets[offset + 1]) * ySlice;
                }
            }
        }

        // Upload vertex data and link into vertex array.
        m_vertices.store(vertexData, vertexDataSize * sizeof(BoardVertex));
        delete[] vertexData;
        m_boardLayout.array(m_vertices, 0, 0, sizeof(BoardVertex));
        m_boardLayout.attribute(
            0, 0, 2,
            lwvl::ByteFormat::Float,
            offsetof(BoardVertex, x)
        );

        m_outlineLayout.array(m_vertices, 0, 0, sizeof(BoardVertex));
        m_outlineLayout.attribute(
            0, 0, 2,
            lwvl::ByteFormat::Float,
            offsetof(BoardVertex, x)
        );

        // Create board index array
        const size_t boardIndexCount = xQuads * yQuads * 6;
        auto* boardIndexData = new uint32_t[boardIndexCount];

        size_t currentVertex = 0;
        const size_t boardIndexOffsets[6] {0, 1, 2, 2, 3, 0 };
        for(size_t row = 0; row < yQuads; ++row) {
            for(size_t col = 0; col < xQuads; ++col) {
                const size_t index = (row * xQuads + col) * 6;
                for (size_t i = 0; i < 6; ++i) {
                    boardIndexData[index + i] = currentVertex + boardIndexOffsets[i];
                }

                currentVertex += 4;
            }
        }

        // Upload index data and link into vertex array.
        m_boardIndices.store(boardIndexData, boardIndexCount * sizeof(uint32_t));
        delete[] boardIndexData;
        m_boardLayout.element(m_boardIndices);

        // Create outline index array
        const size_t outlineIndexCount = xQuads * yQuads * 8;
        auto* outlineIndexData = new uint32_t[outlineIndexCount];

        currentVertex = 0;
        const size_t outlineIndexOffsets[8] { 0, 1, 1, 2, 2, 3, 3, 0 };
        for(size_t row = 0; row < yQuads; ++row) {
            for(size_t col = 0; col < xQuads; ++col) {
                const size_t index = (xQuads * row + col) * 8;
                for(size_t i = 0; i < 8; ++i) {
                    outlineIndexData[index + i] = currentVertex + outlineIndexOffsets[i];
                }

                currentVertex += 4;
            }
        }

        m_outlineIndices.store(outlineIndexData, outlineIndexCount * sizeof(uint32_t));;
        delete[] outlineIndexData;
        m_outlineLayout.element(m_outlineIndices);

        auto cellData = new uint32_t[xQuads * yQuads];
        for(size_t j = 0; j < yQuads; ++j) {
            for(size_t i = 0; i < xQuads; ++i) {
                cellData[xQuads * j + i] = m_board->get(i, j);
            }
        }

        m_cellBuffer.store(cellData, xQuads * yQuads * sizeof(uint32_t), lwvl::bits::Dynamic);
        m_cellTexture.format(m_cellBuffer, lwvl::ChannelLayout::R32UI);

        //auto blockData = new uint32_t[((xQuads * yQuads) / 32) + 1];
        //for(size_t j = 0; j < yQuads / 32; ++j) {
        //    for(size_t i = 0; i < xQuads / 32; ++i) {
        //        for (size_t b = 0; b < 32; b++) {
        //            blockData[xQuads * j + i] = m_board->get(i * 32 + b, j);
        //        }
        //    }
        //}
    }

    void update() {
        const size_t xQuads = m_board->width();
        const size_t yQuads = m_board->height();

        auto cellData = new uint32_t[xQuads * yQuads];
        for(size_t j = 0; j < yQuads; ++j) {
            for(size_t i = 0; i < xQuads; ++i) {
                cellData[xQuads * j + i] = m_board->get(i, j);
            }
        }

        m_cellBuffer.update(cellData, sizeof(uint32_t) * xQuads * yQuads);
    }

    void draw() {
        const size_t xQuads = m_board->width();
        const size_t yQuads = m_board->height();

        //m_boardLayout.bind();
        m_boardControl.bind();
        //m_cellBuffer.bind(lwvl::Buffer::IndexedTarget::ShaderStorage, 0);
        m_cellTexture.bind(0);
        m_boardLayout.drawElements(
            lwvl::PrimitiveMode::Triangles,
            static_cast<int>(xQuads * yQuads * 6),
            lwvl::ByteFormat::UnsignedInt
        );

        //glEnable(GL_LINE_SMOOTH);
        //m_outlineLayout.bind();
        //m_outlineControl.bind();
        //m_outlineLayout.drawElements(
        //    lwvl::PrimitiveMode::Lines,
        //    static_cast<int>(xQuads * yQuads * 8),
        //    lwvl::ByteFormat::UnsignedInt
        //);
    }
};


static inline double delta(time_point<steady_clock> start) {
    return 0.000001 * static_cast<double>(duration_cast<microseconds>(
        high_resolution_clock::now() - start
    ).count());
}


int run(int width, int height) {
    Window window{width, height, "Game of Life"};
    const float aspect = static_cast<float>(width) / static_cast<float>(height);

#ifndef NDEBUG
    GLEventListener listener(
        [](
            Source source, Type type,
            Severity severity, unsigned int id, int length,
            const char *message, const void *userState
        ) {
            if (type != Type::OTHER) {
                std::cout << "[OpenGL] " << message << std::endl;
            }
        }
    );
#endif

    Board board{64, 64};
    Renderer renderer(&board, aspect);
    bool paused = false;

    double updateInterval = 1.0 / 16.0;
    auto updateStart = high_resolution_clock::now();

    double dragEndX = 0.0;
    double dragEndY = 0.0;
    bool startedDrag = false;
    bool finishedDrag = false;
    int motionEvents = 0;

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(intptr_t frameCount = 0; !window.shouldClose(); ++frameCount) {
        Window::update();

        while (std::optional<Event> possible = window.pollEvent()) {
            if (!possible.has_value()) {
                continue;
            }

            Event const& concrete = possible.value();
            if (concrete.type == Event::Type::KeyRelease) {
                KeyboardEvent key_event{std::get<KeyboardEvent>(concrete.event)};
                if (key_event.key == GLFW_KEY_ESCAPE) {
                    window.shouldClose(true);
                } else if (key_event.key == GLFW_KEY_SPACE) {
                    paused ^= true;
                }
            } else if (concrete.type == Event::Type::MouseDown) {
                MouseButtonEvent mouse_event{std::get<MouseButtonEvent>(concrete.event)};
                if (mouse_event.button == GLFW_MOUSE_BUTTON_1) {
                    startedDrag = true;
                }
            } else if (concrete.type == Event::Type::MouseUp) {
                MouseButtonEvent mouse_event{std::get<MouseButtonEvent>(concrete.event)};
                if (mouse_event.button == GLFW_MOUSE_BUTTON_1) {
                    dragEndX = mouse_event.xpos;
                    dragEndY = mouse_event.ypos;
                    finishedDrag = true;
                }
            }

            // Add motion events to queue to calculate path.
            // Max 1024 path points?
            // Path debug renderer for fun?
            // - Line strip between points and a diamond at each path point.
            // - Nice yellow color to go with the pictoral carmine.
            else if (concrete.type == Event::Type::MouseMotion) {
                MouseMotionEvent mouse_event{std::get<MouseMotionEvent>(concrete.event)};
                if (startedDrag && !finishedDrag) {
                    ++motionEvents;
                }
            }
        }

        if (startedDrag && finishedDrag) {
            //if (motionEvents < 1) {
            //    std::cout << "Click at (" << dragEndX << ", " << dragEndY << ")." << std::endl;
            //} else {
            //    std::cout << "Drag ending at (" << dragEndX << ", " << dragEndY << ") with " << motionEvents << " path points." << std::endl;
            //}
            startedDrag = finishedDrag = false;
            motionEvents = 0;
        }

        if(delta(updateStart) >= updateInterval && !paused) {
            board.update();
            renderer.update();
            updateStart = high_resolution_clock::now();
        }

        lwvl::clear();

        renderer.draw();

        window.swapBuffers();

        if (!(frameCount & 63)) {
            frameCount = 0;
        }
    }

    return 0;
}

int main() {
    try {
        return run(800, 600);
    } catch(std::exception& err) {
        std::cerr << err.what() << std::endl;
    }
}
