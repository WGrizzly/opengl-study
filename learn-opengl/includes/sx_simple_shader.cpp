#include "sx_simple_shader.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>

namespace GLSLShaderInfo {
	std::map<std::string, GLSLShader::GLSLShaderType> extensions = 
    {
		{          ".vs",   GLSLShader::VERTEX          },
		{        ".vert",   GLSLShader::VERTEX          },
		{   "_vert.glsl",   GLSLShader::VERTEX          },
		{   ".vert.glsl",   GLSLShader::VERTEX          },
		{          ".gs",   GLSLShader::GEOMETRY        },
		{        ".geom",   GLSLShader::GEOMETRY        },
		{   ".geom.glsl",   GLSLShader::GEOMETRY        },
		{         ".tcs",   GLSLShader::TESS_CONTROL    },
		{    ".tcs.glsl",   GLSLShader::TESS_CONTROL    },
		{         ".tes",   GLSLShader::TESS_EVALUATION },
		{    ".tes.glsl",   GLSLShader::TESS_EVALUATION },
		{          ".fs",   GLSLShader::FRAGMENT        },
		{        ".frag",   GLSLShader::FRAGMENT        },
		{   "_frag.glsl",   GLSLShader::FRAGMENT        },
		{   ".frag.glsl",   GLSLShader::FRAGMENT        },
		{          ".cs",   GLSLShader::COMPUTE         },
		{     ".cs.glsl",   GLSLShader::COMPUTE         }
	};
}

sx_simple_shader::sx_simple_shader()
{
    m_ID = 0;
    m_is_linked = false;
}

sx_simple_shader::~sx_simple_shader()
{
    
}

std::string get_extension(const std::string name)
{
    size_t dot_loc = name.find_last_of('.');
    if (dot_loc != std::string::npos) 
    {
        std::string ext = name.substr(dot_loc);
        if( ext == ".glsl" ) 
        {

            size_t loc = name.find_last_of('.', dot_loc - 1);
            if( loc == std::string::npos ) 
            {
                loc = name.find_last_of('_', dot_loc - 1);
            }
            if( loc != std::string::npos ) 
            {
                return name.substr(loc);
            }
        } 
        else 
        {
            return ext;
        }
    }
    return "";
}

void sx_simple_shader::compile_shader(const std::string glsl_path)
{
    if( 0 >= m_ID)
    {
        m_ID = glCreateProgram();
    }

    std::string ext = get_extension(glsl_path);
    GLSLShader::GLSLShaderType type = GLSLShader::VERTEX;
	auto it = GLSLShaderInfo::extensions.find(ext);
	if (it != GLSLShaderInfo::extensions.end())
    {
		type = it->second;
	}
    else
    {
		std::string msg = "Unrecognized extension: " + ext;
		throw std::runtime_error(msg);
	}


    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        file.open(glsl_path);

        std::stringstream stream;
        stream << file.rdbuf();

        file.close();
        code = stream.str();
    }
    catch(const std::exception& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    unsigned int program = glCreateShader(type);
    const char* cc_code = code.c_str();
    glShaderSource(program, 1, &cc_code, NULL);
    glCompileShader(program);
    _check_compile_errors(program, type);

    glAttachShader(m_ID, program);
}

void sx_simple_shader::link()
{
    if (m_is_linked) return;
    if (m_ID <= 0) throw std::runtime_error("Program has not been compiled.");

    glLinkProgram(m_ID);
    _check_compile_errors(m_ID, GLSLShader::PROGRAM);

	m_is_linked = true;
}

// void sx_simple_shader::checkCompileErrors(GLuint shader, std::string type)
void sx_simple_shader::_check_compile_errors(GLuint shader, GLSLShader::GLSLShaderType type_)
{
    std::string type;
    switch (type_)
    {
    case GLSLShader::VERTEX:            type = "VERTEX";        break;
    case GLSLShader::GEOMETRY:          type = "GEOMETRY";      break;
    case GLSLShader::TESS_CONTROL:      type = "TESSCONTROL";   break;
    case GLSLShader::TESS_EVALUATION:   type = "TESSEVAL";      break;
    case GLSLShader::FRAGMENT:          type = "FRAGMENT";      break;
    case GLSLShader::COMPUTE:           type = "COMPUTE";       break;
    case GLSLShader::PROGRAM:           type = "PROGRAM";       break;
    default:        break;
    }

    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
