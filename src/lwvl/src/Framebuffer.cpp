#include "lwvl/lwvl.hpp"

GLuint lwvl::Framebuffer::ID::reserve() {
    GLuint temp;
    glCreateFramebuffers(1, &temp);
    return temp;
}

lwvl::Framebuffer::ID::~ID() {}


void lwvl::Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id());
}

void lwvl::Framebuffer::attach(Attachment point, const Texture &texture, GLint level) {
    glNamedFramebufferTexture(id(), static_cast<GLenum>(point), texture.id(), level);
}

void lwvl::Framebuffer::attachLayer(Attachment point, const Texture &texture, GLint level, GLint layer) {
    glNamedFramebufferTextureLayer(id(), static_cast<GLenum>(point), texture.id(), level, layer);
}

void lwvl::Framebuffer::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint lwvl::Framebuffer::id() const {
    return m_offsite_id->id;
}
