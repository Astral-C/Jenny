#include "UPathRenderer.hpp"
#include <filesystem>
#include "stb_image.h"
#include <glad/glad.h>


const char* default_path_vtx_shader_source = "#version 330\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec4 color;\n\
layout (location = 2) in int size;\n\
out vec4 line_color;\n\
uniform mat4 gpu_ModelViewProjectionMatrix;\n\
void main()\n\
{\n\
    gl_Position = gpu_ModelViewProjectionMatrix * vec4(position, 1.0);\n\
    gl_PointSize = min(size, size / gl_Position.w);\n\
    line_color = color;\n\
}\
";

const char* default_path_frg_shader_source = "#version 330\n\
uniform sampler2D spriteTexture;\n\
in vec4 line_color;\n\
uniform bool pointMode;\n\
void main()\n\
{\n\
    if(pointMode){\n\
        vec2 p = gl_PointCoord * 2.0 - vec2(1.0);\n\
        float r = sqrt(dot(p,p));\n\
        if(dot(p,p) > r || dot(p,p) < r*0.75){\n\
            discard;\n\
        } else {\n\
            gl_FragColor = vec4(1.0,1.0,1.0,1.0);\n\
        }\n\
    } else {\n\
        gl_FragColor = line_color;\n\
    }\n\
}\
";

void CPathRenderer::Init() {
	//Compile Shaders
	{
	    char glErrorLogBuffer[4096];
	    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	
	    glShaderSource(vs, 1, &default_path_vtx_shader_source, NULL);
	    glShaderSource(fs, 1, &default_path_frg_shader_source, NULL);
	
	    glCompileShader(vs);
	
	    GLint status;
	    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	    if(status == GL_FALSE){
	        GLint infoLogLength;
	        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);
	
	        glGetShaderInfoLog(vs, infoLogLength, NULL, glErrorLogBuffer);
	
	        printf("Compile failure in vertex shader:\n%s\n", glErrorLogBuffer);
	    }
	
	    glCompileShader(fs);
	
	    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	    if(status == GL_FALSE){
	        GLint infoLogLength;
	        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLogLength);
	
	        glGetShaderInfoLog(fs, infoLogLength, NULL, glErrorLogBuffer);
	
	        printf("Compile failure in fragment shader:\n%s\n", glErrorLogBuffer);
	    }
	
	    mShaderID = glCreateProgram();
	
	    glAttachShader(mShaderID, vs);
	    glAttachShader(mShaderID, fs);
	
	    glLinkProgram(mShaderID);
	
	    glGetProgramiv(mShaderID, GL_LINK_STATUS, &status); 
	    if(GL_FALSE == status) {
	        GLint logLen; 
	        glGetProgramiv(mShaderID, GL_INFO_LOG_LENGTH, &logLen); 
	        glGetProgramInfoLog(mShaderID, logLen, NULL, glErrorLogBuffer); 
	        printf("Path Shader Program Linking Error:\n%s\n", glErrorLogBuffer);
	    } 
	
	    glDetachShader(mShaderID, vs);
	    glDetachShader(mShaderID, fs);
	
	    glDeleteShader(vs);
	    glDeleteShader(fs);

	}

    mMVPUniform = glGetUniformLocation(mShaderID, "gpu_ModelViewProjectionMatrix");
    mPointModeUniform = glGetUniformLocation(mShaderID, "pointMode");

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CPathPoint), (void*)offsetof(CPathPoint, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(CPathPoint), (void*)offsetof(CPathPoint, Color));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(CPathPoint), (void*)offsetof(CPathPoint, SpriteSize));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

CPathRenderer::CPathRenderer() {}

CPathRenderer::~CPathRenderer() {
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);
}

void CPathRenderer::UpdateData(){
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    uint32_t vtxCount = 0;
    std::vector<CPathPoint> buffer;
    for(auto& line : mPaths){
        for(auto& point : line){
            buffer.push_back(point);
        }
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(CPathPoint) * buffer.size(), &buffer[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CPathRenderer::Draw(USceneCamera *Camera) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_POINT_SPRITE);

    glLineWidth(2.0f);

	glm::mat4 mvp;
	mvp = Camera->GetProjectionMatrix() * Camera->GetViewMatrix() * glm::identity<glm::mat4>();

    glUseProgram(mShaderID);

    glBindVertexArray(mVao);

    glUniformMatrix4fv(mMVPUniform, 1, 0, (float*)&mvp[0]);

 
    uint32_t start = 0;
    glUniform1i(mPointModeUniform, GL_TRUE);
    for(auto& line : mPaths){
        glDrawArrays(GL_POINTS, start, line.size());
        start += line.size();
    }

    start = 0;
    glUniform1i(mPointModeUniform, GL_FALSE);
    for(auto& line : mPaths){
        glDrawArrays(GL_LINE_STRIP, start, line.size());
        start += line.size();
    }

    glBindVertexArray(0);
    glFlush();
}