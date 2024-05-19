#pragma once

#include <string>
#include <filesystem>

#include <GL/glew.h>
#include <fstream>
#include <vector>
#include "glm/ext.hpp"

class ShaderProgram {
public:
    GLuint ID{0};
	// you can add more constructors for pipeline with GS, TS etc.
	ShaderProgram(void) = default; //does nothing
	ShaderProgram(const std::filesystem::path & VS_file, const std::filesystem::path & FS_file){
        GLuint VS_h = compile_shader(VS_file, GL_VERTEX_SHADER);
        GLuint FS_h = compile_shader(FS_file, GL_FRAGMENT_SHADER);
        this->ID = link_shader({VS_h, FS_h});
    }

	void activate(void){
        glUseProgram(ID);
    }
	void deactivate(void){
        glUseProgram(0);
    }
	void clear(void){
        glDeleteProgram(ID);
    }
    
    // set uniform according to name 
    // https://docs.gl/gl4/glUniform
    void setUniform(const std::string & name, const float val) {
		auto loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) {
			std::cerr << "no uniform with name:" << name << '\n';
			return;
		}
		glUniform1f(loc, val);
	} 
    
    void setUniform(const std::string & name, const int val){
        auto loc = glGetUniformLocation(ID, name.c_str());
        if (loc == -1) {
            std::cerr << "no uniform with name:" << name << '\n';
            return;
        }
        glUniform1i(loc, (int) val);

    }
    void setUniform(const std::string & name, const glm::vec3 val){
        auto loc = glGetUniformLocation(ID, name.c_str());
        if (loc == -1) {
            std::cerr << "no uniform with name:" << name << '\n';
            return;
        }
        glUniform3fv(loc, 1, glm::value_ptr(val));
    }

    void setUniform(const std::string & name, const glm::vec4 val) {
		auto loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) {
			std::cerr << "no uniform with name:" << name << '\n';
			return;
		}
		glUniform4fv(loc, 1, glm::value_ptr(val));
	}

    void setUniform(const std::string & name, const glm::mat3 val){
        auto loc = glGetUniformLocation(ID, name.c_str());
        if (loc == -1) {
            std::cout << "no uniform with name:" << name << '\n';
            return;
        }
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
    }
    void setUniform(const std::string & name, const glm::mat4 val) {
		auto loc = glGetUniformLocation(ID, name.c_str());
		if (loc == -1) {
			std::cout << "no uniform with name:" << name << '\n';
			return;
		}
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
	}

private:// default = 0, empty shader
	std::string getShaderInfoLog(const GLuint obj){
        int infologLength = 0;
        std::string s;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
        if (infologLength > 0) {
            std::vector<char> v(infologLength);
            glGetShaderInfoLog(obj, infologLength, NULL, v.data());
            s.assign(begin(v), end(v));
        }
        return s;
    }
	std::string getProgramInfoLog(const GLuint obj) {
        int infologLength = 0;
        std::string s;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
        if (infologLength > 0) {
            std::vector<char> v(infologLength);
            glGetProgramInfoLog(obj, infologLength, NULL, v.data());
            s.assign(begin(v), end(v));
        }
        return s;
    }

	GLuint compile_shader(const std::filesystem::path & source_file, const GLenum type){
        std::string shader_string = textFileRead(source_file);
        GLuint shader;
        shader = glCreateShader(type);
        glShaderSource(shader, 1, reinterpret_cast<const GLchar *const *>(&shader_string), NULL);
        glCompileShader(shader);
        return shader;
    }
	GLuint link_shader(const std::vector<GLuint> shader_ids){
        GLuint prog_h = glCreateProgram();
        glAttachShader(prog_h, shader_ids[0]);
        glAttachShader(prog_h, shader_ids[1]);
        glLinkProgram(prog_h);
        glDetachShader(prog_h, shader_ids[0]);
        glDetachShader(prog_h, shader_ids[1]);
        glDeleteShader(shader_ids[0]);
        glDeleteShader(shader_ids[1]);
        return prog_h;
    }
    std::string textFileRead(const std::filesystem::path & filename){
        FILE *f = fopen(filename.string().c_str(), "rt");
        assert(f);
        fseek(f, 0, SEEK_END);
        long length = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buffer = (char *) malloc(length + 1);
        buffer[length] = '\0';
        fread(buffer, 1, length, f);
        fclose(f);
        return buffer;
    }
};

