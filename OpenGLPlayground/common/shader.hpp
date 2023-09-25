#ifndef SHADER_HPP
#define SHADER_HPP

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

GLuint LoadTessShaders(const char *tess_vert_file_path, const char *tess_ctrl_file_path, const char *tess_eval_file_path, const char *tess_frag_file_path);

GLuint LoadGeoShaders(const char *geo_vert_file_path, const char *geo_file_path, const char *geo_frag_file_path);

#endif
