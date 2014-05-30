#ifndef __SYPHON_OUTPUT__
#define __SYPHON_OUTPUT__

bool init_syphon_output();
void publish_texture(GLuint texture, GLenum target, GLuint width, GLuint height);
void cleanup_syphon_output();

#endif