#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#include "shader.hpp"

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex2 Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex2 Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex2 Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint LoadTessShaders(const char *tess_vert_file_path, const char *tess_ctrl_file_path, const char *tess_eval_file_path, const char *tess_frag_file_path)
{
#if defined(__APPLE__)
    std::string glsl_version = "#version 330 core\n";
#elif defined(_WIN32)
    std::string glsl_version = "#version 460 core\n";
#endif

    GLuint tessVertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint tessCtrlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
    GLuint tessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
    GLuint tessFragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string tessVertexShaderCode = glsl_version;
    std::ifstream tessVertexShaderStream(tess_vert_file_path, std::ios::in);
    if(tessVertexShaderStream.is_open())
    {
        std::string line;
        while(std::getline(tessVertexShaderStream, line))
        {
            tessVertexShaderCode += "\n" + line;
        }
        tessVertexShaderStream.close();
    }
    else
    {
        printf("Couldn't open %s", tess_vert_file_path);
        exit(1);
    }

    std::string tessCtrlShaderCode = glsl_version;
    std::ifstream tessCtrlShaderStream(tess_ctrl_file_path, std::ios::in);
    if(tessCtrlShaderStream.is_open())
    {
        std::string line;
        while(std::getline(tessCtrlShaderStream, line))
        {
            tessCtrlShaderCode += "\n" + line;
        }
        tessCtrlShaderStream.close();
    }
    else
    {
        printf("Couldn't open %s", tess_ctrl_file_path);
//        Err(std::string("Couldn't open ") + tess_ctrl_file_path);
        exit(1);
    }

    std::string tessEvalShaderCode = glsl_version;
    std::ifstream tessEvalShaderStream(tess_eval_file_path, std::ios::in);
    if(tessEvalShaderStream.is_open())
    {
        std::string line;
        while(std::getline(tessEvalShaderStream, line))
        {
            tessEvalShaderCode += "\n" + line;
        }
        tessEvalShaderStream.close();
    }
    else
    {
        printf("Couldn't open %s", tess_eval_file_path);
//        Err(std::string("Couldn't open ") + tess_eval_file_path);
        exit(1);
    }

    std::string tessFragShaderCode = glsl_version;
    std::ifstream tessFragShaderStream(tess_frag_file_path, std::ios::in);
    if(tessFragShaderStream.is_open())
    {
        std::string line;
        while(std::getline(tessFragShaderStream, line))
        {
            tessFragShaderCode += "\n" + line;
        }
        tessFragShaderStream.close();
    }
    else
    {
        printf("Couldn't open %s", tess_frag_file_path);
//        Err(std::string("Couldn't open ") + tess_frag_file_path);
        exit(1);
    }

    GLint result = false;
    int infoLogLength;

    printf("Compiling shader: %s\n", tess_vert_file_path);
    char const* tessVertSourcePointer = tessVertexShaderCode.c_str();
    glShaderSource(tessVertShaderID, 1, &tessVertSourcePointer, NULL);
    glCompileShader(tessVertShaderID);
    glGetShaderiv(tessVertShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(tessVertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> tessVertShaderErrMsg(infoLogLength + 1);
        glGetShaderInfoLog(tessVertShaderID, infoLogLength, NULL, &tessVertShaderErrMsg[0]);
        printf("%s\n", &tessVertShaderErrMsg[0]);
    }

    printf("Compiling shader: %s\n", tess_ctrl_file_path);
    char const* tessCtrlSourcePointer = tessCtrlShaderCode.c_str();
    glShaderSource(tessCtrlShaderID, 1, &tessCtrlSourcePointer, NULL);
    glCompileShader(tessCtrlShaderID);
    glGetShaderiv(tessCtrlShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(tessCtrlShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> tessCtrlShaderErrMsg(infoLogLength + 1);
        glGetShaderInfoLog(tessCtrlShaderID, infoLogLength, NULL, &tessCtrlShaderErrMsg[0]);
        printf("%s\n", &tessCtrlShaderErrMsg[0]);
    }

    printf("Compiling shader: %s\n", tess_eval_file_path);
    char const* tessEvalSourcePointer = tessEvalShaderCode.c_str();
    glShaderSource(tessEvalShaderID, 1, &tessEvalSourcePointer, NULL);
    glCompileShader(tessEvalShaderID);
    glGetShaderiv(tessEvalShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(tessEvalShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        std::vector<char> tessEvalShaderErrMsg(infoLogLength + 1);
        glGetShaderInfoLog(tessEvalShaderID, infoLogLength, NULL, &tessEvalShaderErrMsg[0]);
        printf("%s\n", &tessEvalShaderErrMsg[0]);
    }

    printf("Compiling shader: %s\n", tess_frag_file_path);
    char const* tessFragSourcePointer = tessFragShaderCode.c_str();
    glShaderSource(tessFragShaderID, 1, &tessFragSourcePointer, NULL);
    glCompileShader(tessFragShaderID);
    glGetShaderiv(tessFragShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(tessFragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> tessFragShaderErrMsg(infoLogLength + 1);
        glGetShaderInfoLog(tessFragShaderID, infoLogLength, NULL, &tessFragShaderErrMsg[0]);
        printf("%s\n", &tessFragShaderErrMsg[0]);
    }

    printf("Linking Shader\n");
    GLuint tessProgramID = glCreateProgram();
    glAttachShader(tessProgramID, tessVertShaderID);
    glAttachShader(tessProgramID, tessCtrlShaderID);
    glAttachShader(tessProgramID, tessEvalShaderID);
    glAttachShader(tessProgramID, tessFragShaderID);
    glLinkProgram(tessProgramID);

    glGetProgramiv(tessProgramID, GL_LINK_STATUS, &result);
    glGetProgramiv(tessProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> tessProgramErrMsg(infoLogLength + 1);
        glGetProgramInfoLog(tessProgramID, infoLogLength, NULL, &tessProgramErrMsg[0]);
        printf("%s\n", &tessProgramErrMsg[0]);
    }

    glDetachShader(tessProgramID, tessVertShaderID);
    glDetachShader(tessProgramID, tessCtrlShaderID);
    glDetachShader(tessProgramID, tessEvalShaderID);
    glDetachShader(tessProgramID, tessFragShaderID);

    glDeleteShader(tessVertShaderID);
    glDeleteShader(tessCtrlShaderID);
    glDeleteShader(tessEvalShaderID);
    glDeleteShader(tessFragShaderID);

    return tessProgramID;
}

GLuint LoadGeoShaders(const char *geo_vert_file_path, const char *geo_file_path, const char *geo_frag_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex2 Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(geo_vert_file_path, std::ios::in);
    if(VertexShaderStream.is_open()){
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", geo_vert_file_path);
        getchar();
        return 0;
    }

    std::string geoShaderCode;
    std::ifstream geoShaderStream(geo_file_path, std::ios::in);
    if(geoShaderStream.is_open())
    {
        std::string line;
        while(std::getline(geoShaderStream, line))
        {
            geoShaderCode += "\n" + line;
        }
        geoShaderStream.close();
    }
    else
    {
        printf("Couldn't open %s", geo_file_path);
//        Err(std::string("Couldn't open ") + tess_ctrl_file_path);
        exit(1);
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(geo_frag_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex2 Shader
    printf("Compiling shader : %s\n", geo_vert_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex2 Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Geometry shader
    printf("Compiling shader : %s\n", geo_file_path);
    char const * GeoSourcePointer = geoShaderCode.c_str();
    glShaderSource(GeometryShaderID, 1, &GeoSourcePointer , NULL);
    glCompileShader(GeometryShaderID);

    // Check Geometry Shader
    glGetShaderiv(GeometryShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(GeometryShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> GeometryShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(GeometryShaderID, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
        printf("%s\n", &GeometryShaderErrorMessage[0]);
    }


    // Compile Fragment Shader
    printf("Compiling shader : %s\n", geo_frag_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);


    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }


    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, GeometryShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, GeometryShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(GeometryShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}


