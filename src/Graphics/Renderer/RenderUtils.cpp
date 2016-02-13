#include "RenderUtils.hpp"
#include <glad/glad.h>
#include <GL/gl.h>
#include "../Util/GLUtils.hpp"

void RenderQuad()
{
    GLfloat quadVert[] =
    {
       -1.0f,  1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    GLuint quadVao;
    glGenVertexArrays(1, &quadVao);
    glBindVertexArray(quadVao);
    {
        GLuint quadVbo;
        glGenBuffers(1, &quadVbo);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVert), &quadVert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &quadVbo);
    }
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &quadVao);
}

void RenderBox(const AABB& aabb)
{
    // Store previous polygon mode
    GLint prevPolMode;
    glGetIntegerv(GL_POLYGON_MODE, &prevPolMode);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLfloat vertices[] = {
        aabb.MaxX(), aabb.MaxY(), aabb.MaxZ(),
        aabb.MinX(), aabb.MaxY(), aabb.MaxZ(),
        aabb.MaxX(), aabb.MinY(), aabb.MaxZ(),
        aabb.MinX(), aabb.MinY(), aabb.MaxZ(),
        aabb.MaxX(), aabb.MaxY(), aabb.MinZ(),
        aabb.MinX(), aabb.MaxY(), aabb.MinZ(),
        aabb.MaxX(), aabb.MinY(), aabb.MinZ(),
        aabb.MinX(), aabb.MinY(), aabb.MinZ(),
    };
    GLuint indices[] = {
        0, 1, 2,
        1, 3, 2,
        4, 6, 7,
        7, 5, 4,
        1, 5 ,7,
        7, 3, 1,
        0, 2, 6,
        6, 4, 0,
        5, 1, 0,
        0, 4, 5,
        7, 6, 2,
        2, 3, 7
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);

    // Restore polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, prevPolMode);
}
