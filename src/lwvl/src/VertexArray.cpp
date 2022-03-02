#include "lwvl/lwvl.hpp"
#include <iostream>

GLuint lwvl::VertexArray::ID::reserve() {
    GLuint temp;
    glCreateVertexArrays(1, &temp);
    return temp;
}

lwvl::VertexArray::ID::~ID() {
    glDeleteVertexArrays(1, &id);
}

void lwvl::VertexArray::_format(GLuint vao, GLuint index, uint8_t dimensions, lwvl::ByteFormat type, GLuint offset) {
    switch(type) {
        case ByteFormat::Int:
        case ByteFormat::Int_2_10_10_10:
        case ByteFormat::UnsignedInt:
        case ByteFormat::UnsignedInt_2_10_10_10:
        case ByteFormat::UnsignedInt_10F_11F_11F: {
            std::cout << "int" << std::endl;
            glVertexArrayAttribIFormat(vao, index, dimensions, static_cast<GLenum>(type), offset);
            break;
        }
        case ByteFormat::Float:
        case ByteFormat::HalfFloat:
        default: {
            std::cout << "float" << std::endl;
            glVertexArrayAttribFormat(vao, index, dimensions, static_cast<GLenum>(type), GL_FALSE, offset);
            break;
        }
    }
}

GLuint lwvl::VertexArray::id() const {
    return m_offsite_id->id;
}

void lwvl::VertexArray::bind() {
    glBindVertexArray(id());
}

void lwvl::VertexArray::clear() {
    glBindVertexArray(0);
}

void lwvl::VertexArray::array(lwvl::Buffer const& buffer, GLuint bindIndex, GLintptr offset, GLsizei stride) {
    glVertexArrayVertexBuffer(id(), bindIndex, buffer.id(), offset, stride);
}

void lwvl::VertexArray::element(lwvl::Buffer const& buffer) {
    glVertexArrayElementBuffer(id(), buffer.id());
}

void lwvl::VertexArray::attribute(GLuint index, uint8_t dimensions, lwvl::ByteFormat type, GLuint offset) {
    const GLuint vao = id();
    glEnableVertexArrayAttrib(vao, index);  // One would assume setting a format enables it.
    _format(vao, index, dimensions, type, offset);
}

void lwvl::VertexArray::attribute(GLuint binding, GLuint attribute, uint8_t dimensions, lwvl::ByteFormat type, GLuint offset) {
    const GLuint vao = id();
    glEnableVertexArrayAttrib(vao, attribute);
    _format(vao, attribute, dimensions, type, offset);
    glVertexArrayAttribBinding(vao, attribute, binding);
}

void lwvl::VertexArray::attach(GLuint bindIndex, GLuint attributeIndex) {
    glVertexArrayAttribBinding(id(), attributeIndex, bindIndex);
}

void lwvl::VertexArray::enable(GLuint attributeIndex) {
    glEnableVertexArrayAttrib(id(), attributeIndex);
}

void lwvl::VertexArray::disable(GLuint attributeIndex) {
    glDisableVertexArrayAttrib(id(), attributeIndex);
}

void lwvl::VertexArray::drawArrays(lwvl::PrimitiveMode mode, int count) const {
    glBindVertexArray(id());
    if (instances > 1) {
        glDrawArraysInstanced(static_cast<GLenum>(mode), 0, count, instances);
    } else {
        glDrawArrays(static_cast<GLenum>(mode), 0, count);
    }
    glBindVertexArray(0);
}

void lwvl::VertexArray::drawElements(lwvl::PrimitiveMode mode, int count, lwvl::ByteFormat type) const {
    GLint currentVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
    glBindVertexArray(id());
    if (instances > 1) {
        glDrawElementsInstanced(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), nullptr, instances);
    } else {
        glDrawElements(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), nullptr);
    }
    // Rebind the previous VAO
    glBindVertexArray(currentVAO);
}
