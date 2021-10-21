#ifndef SHADER_HPP
#define SHADER_HPP
#pragma once

// System headers
#include <glad/glad.h>

// Standard headers
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

namespace Gloom
{
    class Shader
    {
    public:
        Shader()            { mProgram = glCreateProgram(); }

        // Public member functions
        void   activate()   { glUseProgram(mProgram); }
        void   deactivate() { glUseProgram(0); }
        GLuint get()        { return mProgram; }
        void   destroy()    { glDeleteProgram(mProgram); }

        /* Attach a shader to the current shader program */
        void attach(const std::filesystem::path& filename)
        {
            // Load GLSL Shader from source
            std::ifstream fd(filename.c_str());
            if (fd.fail())
            {
                fprintf(stderr,
                    "Something went wrong when attaching the Shader file at \"%ls\".\n"
                    "The file may not exist or is currently inaccessible.\n",
                    filename.c_str());
                return;
            }
            auto src = std::string(std::istreambuf_iterator<char>(fd),
                                  (std::istreambuf_iterator<char>()));

            // Create shader object
            const char * source = src.c_str();
            auto shader = create(filename);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            // Display errors
            glGetShaderiv(shader, GL_COMPILE_STATUS, &mStatus);
            if (!mStatus)
            {
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &mLength);
                std::unique_ptr<char[]> buffer(new char[mLength]);
                glGetShaderInfoLog(shader, mLength, nullptr, buffer.get());
                fprintf(stderr, "%ls\n%s", filename.c_str(), buffer.get());
            }

            assert(mStatus);

            // Attach shader and free allocated memory
            glAttachShader(mProgram, shader);
            glDeleteShader(shader);
        }


        /* Links all attached shaders together into a shader program */
        void link()
        {
            // Link all attached shaders
            glLinkProgram(mProgram);

            // Display errors
            glGetProgramiv(mProgram, GL_LINK_STATUS, &mStatus);
            if (!mStatus)
            {
                glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &mLength);
                std::unique_ptr<char[]> buffer(new char[mLength]);
                glGetProgramInfoLog(mProgram, mLength, nullptr, buffer.get());
                fprintf(stderr, "%s\n", buffer.get());
            }

            assert(mStatus);
        }


        /* Convenience function that attaches and links a vertex and a
           fragment shader in a shader program */
        void makeBasicShader(const std::filesystem::path& vertexFilename,
                             const std::filesystem::path& fragmentFilename)
        {
            attach(vertexFilename);
            attach(fragmentFilename);
            link();
        }


        /* Used for debugging shader programs (expensive to run) */
        bool isValid()
        {
            // Validate linked shader program
            glValidateProgram(mProgram);

            // Display errors
            glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &mStatus);
            if (!mStatus)
            {
                glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &mLength);
                std::unique_ptr<char[]> buffer(new char[mLength]);
                glGetProgramInfoLog(mProgram, mLength, nullptr, buffer.get());
                fprintf(stderr, "%s\n", buffer.get());
                return false;
            }
            return true;
        }


        /* Helper function for creating shaders */
        GLuint create(const std::filesystem::path& filename)
        {
	        // Extract file extension and create the correct shader type
	        auto ext = filename.extension();
	        if (ext == ".comp") return glCreateShader(GL_COMPUTE_SHADER);
	        if (ext == ".frag") return glCreateShader(GL_FRAGMENT_SHADER);
	        if (ext == ".geom") return glCreateShader(GL_GEOMETRY_SHADER);
	        if (ext == ".tcs") return glCreateShader(GL_TESS_CONTROL_SHADER);
	        if (ext == ".tes") return glCreateShader(GL_TESS_EVALUATION_SHADER);
	        if (ext == ".vert") return glCreateShader(GL_VERTEX_SHADER);
	        return false;
        }

    private:
        // Disable copying and assignment
        Shader(Shader const &) = delete;
        Shader & operator =(Shader const &) = delete;

        // Private member variables
        GLuint mProgram;
        GLint  mStatus;
        GLint  mLength;
    };
}

#endif
