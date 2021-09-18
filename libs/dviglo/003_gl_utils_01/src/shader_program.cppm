export module dviglo.shader_program;

export import <GL/glew.h>;
export import <glm/mat2x2.hpp>;

// Модули движка
import dviglo.file;
import dviglo.log;
import dviglo.std_string_utils;

// Стандартная библиотека
import <string>;

using namespace std;

// Возвращает идентификатор скомпилированного шейдера или ноль в случае ошибки
static GLuint compile_shader(const string& file_path, GLenum type)
{
    // Читаем файл
    string vs_src = dv_file::read_all_text(file_path);
    if (vs_src.empty())
        return 0; // Если не удалось прочесть файл, сообщение об ошибке уже выведено в лог

    // Компилируем шейдер
    GLuint gpu_object_name = glCreateShader(type);
    const char* vs_src_c_str = vs_src.c_str();
    glShaderSource(gpu_object_name, 1, &vs_src_c_str, nullptr);
    glCompileShader(gpu_object_name);

    // Успешно ли прошла компиляция
    GLint success;
    glGetShaderiv(gpu_object_name, GL_COMPILE_STATUS, &success);
    if (success)
        return gpu_object_name;

    // Печатаем ошибку в лог
    GLint buffer_size; // длина строки + нуль-терминатор
    glGetShaderiv(gpu_object_name, GL_INFO_LOG_LENGTH, &buffer_size);
    string msg(buffer_size - 1, '\0'); // buffer_size - 1 так как std::string сам выделяет место для нуль-терминатора
    glGetShaderInfoLog(gpu_object_name, buffer_size, nullptr, msg.data());
    trim_end(msg, "\n");
    LOG().write_error(file_path + " | " + msg);

    glDeleteShader(gpu_object_name);
    return 0;
}

export class dvShaderProgram
{
private:
    // Идентификатор объекта OpenGL
    GLuint gpu_object_name_ = 0;

public:
    // Идентификатор объекта OpenGL
    inline GLuint gpu_object_name() const
    {
        return gpu_object_name_;
    }

    dvShaderProgram() = default;

    // Геометрический шейдер может отсутствовать
    dvShaderProgram(const string& vertex_shader_path, const string& fragment_shader_path, const string& geometry_shader_path = string())
    {
        GLuint vertex_shader = compile_shader(vertex_shader_path, GL_VERTEX_SHADER);
        if (!vertex_shader)
            return;

        GLuint fragment_shader = compile_shader(fragment_shader_path, GL_FRAGMENT_SHADER);
        if (!fragment_shader)
        {
            glDeleteShader(vertex_shader);
            return;
        }

        GLuint geometry_shader = 0;
        if (!geometry_shader_path.empty()) // Геометрический шейдер может отсутствовать
        {
            geometry_shader = compile_shader(geometry_shader_path, GL_GEOMETRY_SHADER);
            if (!geometry_shader)
            {
                glDeleteShader(vertex_shader);
                glDeleteShader(fragment_shader);
                return;
            }
        }

        // Линкуем все шейдеры в шейдерную программу
        gpu_object_name_ = glCreateProgram();
        glAttachShader(gpu_object_name_, vertex_shader);
        glAttachShader(gpu_object_name_, fragment_shader);
        if (!geometry_shader_path.empty())
            glAttachShader(gpu_object_name_, geometry_shader);
        glLinkProgram(gpu_object_name_);

        // Были ли ошибки при линковке
        GLint success;
        glGetProgramiv(gpu_object_name_, GL_LINK_STATUS, &success);
        if (!success)
        {
            // Печатаем ошибку в лог
            GLint buffer_size; // длина строки + нуль-терминатор
            glGetProgramiv(gpu_object_name_, GL_INFO_LOG_LENGTH, &buffer_size);
            string msg(buffer_size - 1, '\0'); // buffer_size - 1 так как std::string сам выделяет место для нуль-терминатора
            glGetProgramInfoLog(gpu_object_name_, buffer_size, nullptr, msg.data());
            trim_end(msg, "\n");
            LOG().write_error(vertex_shader_path + " + " + fragment_shader_path + " + " + geometry_shader_path + " | " + msg);

            glDeleteProgram(gpu_object_name_);
            gpu_object_name_ = 0;
        }

        // Шейдеры после линковки не нужны
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteShader(geometry_shader); // Проверка на 0 не нужна
    }

    ~dvShaderProgram()
    {
        glDeleteProgram(gpu_object_name_); // Проверка на 0 не нужна
    }

    inline void use() const
    {
        glUseProgram(gpu_object_name_);
    }

    inline void set(const string& name, GLint value) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform1i(location, value);
    }

    inline void set(const string& name, GLboolean value) const
    {
        set(name, (GLint)value);
    }

    inline void set(const string& name, GLfloat value) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform1f(location, value);
    }

    inline void set(const string& name, const glm::vec2& value) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform2fv(location, 1, &value[0]);
    }

    inline void set(const string& name, GLfloat x, GLfloat y) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform2f(location, x, y);
    }

    inline void set(const string& name, const glm::vec3& value) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform3fv(location, 1, &value[0]);
    }

    inline void set(const string& name, GLfloat x, GLfloat y, GLfloat z) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform3f(location, x, y, z);
    }

    inline void set(const string& name, const glm::vec4& value) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform4fv(location, 1, &value[0]);
    }

    inline void set(const string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniform4f(location, x, y, z, w);
    }

    inline void set(const string& name, const glm::mat2& mat) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    inline void set(const string& name, const glm::mat3& mat) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    inline void set(const string& name, const glm::mat4& mat) const
    {
        GLint location = glGetUniformLocation(gpu_object_name_, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
    }
};
