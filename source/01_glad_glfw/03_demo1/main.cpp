/*
01.获取OpenGL版本  // https://blog.csdn.net/hhy321/article/details/122322475
02.最简单的三角形入门
03.旋转的立方体
04.glDrawArrays GL_TRIANGLES
05.不使用图元重启，切割数据
    -顶点数据 [v0x,v0y,v0z,v1x,v1y,v1z...] = 3d数据
    -拓扑数据 [n,c0,i1,i2,i3..c1,i1,i2,i3...]n为单元（图元）个数，c为每个单元含有的索引个数，i为具体的索引值，
    -流场数据 [f0,f1...]每个单元对应一个流场数据，即格心
06.图元重启
07.用例5拷贝版，构造250个顶点
08.在例7的基础上使用颜色映射表colorMap
09.通过键盘(上下左右）控制uniform的值来改变图形颜色
10.glPolygonMode设置线框模式,使用glDrawElements绘制
11.两个三角使用不同的片段着色器，可以实现立方体不同的面设置不同的颜色（根据本人目前的经验不推荐这种方式）
*/

#define TEST2

#ifdef TEST1

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(int argc, char** argv)
{
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    if (!glfwInit())
    {
        return -1;
    }
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 设置窗口隐藏
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLFW", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    printf("OpenGL vendor string: %s\n", glGetString(GL_VENDOR));
    printf("OpenGL renderer string: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version string: %s\n", glGetString(GL_VERSION));

    auto version = glGetString(GL_VERSION);
    std::cout << '\n' << version[0] << ',' << version[2] << '\n';

    glfwTerminate();
    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* FS = "#version 330 core\n"
"void main()\n"
"{\n"
"   gl_FragColor = vec4(0.0f,0.8f,0.0f,1.0f);\n"
"}\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader & Program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  // left  
         0.5f, -0.5f, 0.0f,  // right 
         0.0f,  0.5f, 0.0f,  // top   
    };

    // Buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST2

#ifdef TEST3

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor =  vec4(fColor.r, fColor.g, fColor.b, 1.0f);\n"
"}\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // 设置为线框模式
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //3d模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime()/*glm::radians(-45.0f)*/, glm::vec3(1.0f, 1.0f, 0.0f)); //旋转，绕x，y轴顺时针旋转45°（可以同时看到多个立方体的表面），第二个参数设置为时间可以一直旋转
        //3d观察矩阵
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)); //平移 向z轴反方向平移5个单位（即向屏幕里面平移，图形会越来越小）
        //3d投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  //投影

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glBindVertexArray(0); // no need to unbind it every time 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST3

#ifdef TEST4

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   //vec4 color = vec4(fColor,0.0);\n"
"   //if(color.a<1.0) discard;\n"
"   FragColor = vec4(fColor, 0.5f);\n"
"}\n\0";

int main()
{
    //初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //设置当前上下文，回调函数（键盘，鼠标等）
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //gladLoadGL();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //着色器 -> 创建 加载 编译 链接 绑定
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //顶点数据
    //float vertices[] = {
    //    -0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,// left  
    //     0.5f, -0.5f, 0.0f,  0.0f,  1.0f,  0.0f,// right 
    //     0.0f,  0.5f, 0.0f,  0.0f,  0.0f,  1.0f,// top   
    //};

    //float vertices[] = {
    //    -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,
    //     0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,
    //    -0.5f,  0.2f,  0.0f,  1.0f,  1.0f,  0.0f,
    //     0.5f,  0.2f,  0.0f,  1.0f,  1.0f,  0.0f,
    //    -0.5f, -0.2f,  0.0f,  0.0f,  1.0f,  0.0f,
    //     0.5f, -0.2f,  0.0f,  0.0f,  1.0f,  0.0f,
    //    -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,
    //};

    //float vertices[] = {
    //-0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,
    // 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,
    ////-0.5f,  0.2f,  0.0f,  1.0f,  1.0f,  0.0f,
    //// 0.5f,  0.2f,  0.0f,  1.0f,  1.0f,  0.0f,
    ////-0.5f, -0.2f,  0.0f,  0.0f,  1.0f,  0.0f,
    //// 0.5f, -0.2f,  0.0f,  0.0f,  1.0f,  0.0f,
    //0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,
    //-0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,
    //};



    float vertices[] = {
        -0.8f,  0.8f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.8f,  0.8f,  0.0f,  1.0f,  0.0f,  0.0f,
        -0.8f,  0.4f,  0.0f,  1.0f,  1.0f,  0.0f,

        -0.3f,  0.6f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.3f,  0.4f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.8f,  0.6f,  0.0f,  1.0f,  1.0f,  0.0f,

        -0.8f, -0.2f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.3f,  0.2f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.3f, -0.2f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.8f,  0.2f,  0.0f,  0.0f,   1.0f,  0.0f,
        -0.8f,  -0.6f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.3f,  -0.4f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.3f,  -0.6f,  0.0f,  0.0f,  1.0f,  1.0f,

         0.8f,  -0.4f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.8f,  -0.8f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.8f,  -0.8f,  0.0f,  0.0f,  0.0f,  1.0f,

    };

    //缓冲对象：创建 绑定类型 链接数据 配置数据
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式


    //循环绘制
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
        glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


#endif // TEST4

#ifdef TEST5


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in float aColor;\n"
"out vec3 myColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   if(aColor<0.4f)"
"        myColor = vec3(1.0f,0.0f,0.0f);\n"
"   else if(aColor<0.7f)"
"        myColor = vec3(0.0f,1.0f,0.0f);\n"
"   else if(aColor<1.0f)"
"        myColor = vec3(1.0f,1.0f,0.0f);\n"
"   else"
"        myColor = vec3(0.0f,0.0f,1.0f);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 myColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(myColor, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    //顶点数据
    float vertices[] = {
        -0.894737,-0.9,    0.0f,
        -0.894737,0.9,    0.0f,
        -0.794737,-0.9,    0.0f,
        -0.794737,0.9,    0.0f,
        -0.689474,-0.9,    0.0f,
        -0.689474,0.9,    0.0f,
        -0.584211,-0.9,    0.0f,
        -0.584211,0.9,    0.0f,
        -0.478947,-0.9,    0.0f,
        -0.478947,0.9,    0.0f,
        -0.373684,-0.9,    0.0f,
        -0.373684,0.9,    0.0f,
        -0.268421,-0.9,    0.0f,
        -0.268421,0.9,    0.0f,
        -0.163158,-0.9,    0.0f,
        -0.163158,0.9,    0.0f,
        -0.0578947,-0.9,0.0f,
        -0.0578947,0.9,    0.0f,
        0.0473685,-0.9,    0.0f,
        0.0473685,0.9,    0.0f,
        0.152632,-0.9,    0.0f,
        0.152632,0.9,    0.0f,
        0.257895,-0.9,    0.0f,
        0.257895,0.9,    0.0f,
        0.363158,-0.9,    0.0f,
        0.363158,0.9,    0.0f,
        0.468421,-0.9,    0.0f,
        0.468421,0.9,    0.0f,
        0.573684,-0.9,    0.0f,
        0.573684,0.9,    0.0f,
        0.678947,-0.9,    0.0f,
        0.678947,0.9,    0.0f,
        0.784211,-0.9,    0.0f,
        0.784211,0.9,    0.0f,
        0.889474,-0.9,    0.0f,
        0.889474,0.9,    0.0f,

    };

    const int verCount = 36;
    int indCount = 0;

    std::vector<float> vecVertices;
    vecVertices.resize(verCount * 4);

    for (size_t i = 0; i < verCount; i++)
    {
        vecVertices[i * 4 + 0] = vertices[i * 3 + 0];  //x
        vecVertices[i * 4 + 1] = vertices[i * 3 + 1];  //y
        vecVertices[i * 4 + 2] = vertices[i * 3 + 2];  //z
    }

    //拓扑数据
    int topo[] = {
        17,
4,0,1,3,2,
4,2,3,5,4,
4,4,5,7,6,
4,6,7,9,8,
4,8,9,11,10,
4,10,11,13,12,
4,12,13,15,14,
4,14,15,17,16,
4,16,17,19,18,
4,18,19,21,20,
4,20,21,23,22,
4,22,23,25,24,
4,24,25,27,26,
4,26,27,29,28,
4,28,29,31,30,
4,30,31,33,32,
4,32,33,35,34,

    };

    //流场数据
    float vecFieldData[]
    {
                0,0.030303,0.0606061,0.0909091,0.121212,0.151515,0.181818,0.212121,0.242424,0.272727,0.30303,0.333333,
        0.363636,0.393939,0.424242,0.454545,0.484848,0.515152,0.545455,0.575758,0.606061,0.636364,0.666667,
        0.69697,0.727273,0.757576,0.787879,0.818182,0.848485,0.878788,0.909091,0.939394,0.969697,
    };

    int start = 1;
    int end = topo[1] + 2;

    std::vector<uint32_t> vecIndices;  //所有的索引

    for (int i = 0; i < topo[0]; i++)  //  拓扑的个数
    {
        std::vector<int> vecIndex;  //拓扑数据中的单个拓扑
        for (int j = start; j < end; j++)
        {
            vecIndex.emplace_back(topo[j]);
        }
        start = end;
        end += topo[end] + 1;

        int x = *(vecIndex.begin() + 1);
        size_t startField = vecIndices.size();  //0

        for (int k = 2; k < vecIndex.size() - 1; k++)
        {
            vecIndices.emplace_back(x);
            vecIndices.emplace_back(vecIndex[k]);
            vecIndices.emplace_back(vecIndex[k + 1]);
        }

        //流场数据
        for (size_t n = startField; n < vecIndices.size(); n++)
        {
            std::cout << (vecIndices[n] + 1) * 4 - 1 << " ";
            vecVertices[(vecIndices[n] + 1) * 4 - 1] = vecFieldData[i];  //w,先用索引数组找到是第几个点，然后计算w的下标
        }
    }
    indCount = vecIndices.size();
    //索引数组
    auto ind = vecIndices.data();
    //顶点数组
    auto ver = vecVertices.data();

    //uint32_t indices[]
    //{
    //    0,1,5,4,3,2,
    //    2,3,4,5,9,8,7,6,
    //    6,7,8,9,13,12,11,10,
    //    10,11,12,13,15,14,
    //};
    //float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f   // top left 
    //};
    //unsigned int indices[] = {  // note that we start from 0!
    //    0, 1, 3,  // first Triangle
    //    1, 2, 3   // second Triangle
    //};
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verCount * 4 * sizeof(float), ver, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(int), ind, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


#endif // TEST5

#ifdef TEST6


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in float aColor;\n"
"out vec3 myColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   myColor = vec3(aColor,0.0f,0.0f);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 myColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(myColor, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    //顶点数据
    float vertices[] = {
    -0.8f,  0.8f,  0.0f,  0.5f,  //.0f,  0.0f,  0.0f,
     0.8f,  0.8f,  0.0f,  0.5f,  //.0f,  0.0f,  0.0f,

    -0.8f,  0.4f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,
    -0.3f,  0.6f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,
     0.3f,  0.4f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,
     0.8f,  0.6f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,

    -0.8f, -0.2f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,
    -0.3f,  0.2f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,
     0.3f, -0.2f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,
     0.8f,  0.2f,  0.0f,  0.5f,  //.0f,  1.0f,  0.0f,

    -0.8f,  -0.6f,  0.0f, 0.5f,  //0.0f,  1.0f,  1.0f,
    -0.3f,  -0.4f,  0.0f, 0.5f,  //0.0f,  1.0f,  1.0f,
     0.3f,  -0.6f,  0.0f, 0.5f,  //0.0f,  1.0f,  1.0f,
     0.8f,  -0.4f,  0.0f, 0.5f,  //0.0f,  1.0f,  1.0f,

    -0.8f,  -0.8f,  0.0f, 0.5f,  //0.0f,  0.0f,  1.0f,
     0.8f,  -0.8f,  0.0f, 0.5f,  //0.0f,  0.0f,  1.0f,

    };

    //拓扑数据,0xFFFFFFFF是重启标志，uint数据是32位。如果索引数据类型是ushort，标志用0xFFFF
    unsigned int indices[] = {
        0,1,5,4,3,2,0xFFFFFFFF,
        2,3,4,5,9,8,7,6,0xFFFFFFFF,
        6,7,8,9,13,12,11,10,0xFFFFFFFF,
        10,11,12,13,15,14,
    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    //开启图元重启，好像默认是开启的
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLE_FAN, 31, GL_UNSIGNED_INT, 0);  //31是索引数组元素的个数
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


#endif // TEST6

#ifdef TEST7


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in float aColor;\n"
"out vec3 myColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   if(aColor<0.2f)"
"        myColor = vec3(1.0f,0.0f,0.0f);\n"
"   else if(aColor<0.4f)"
"        myColor = vec3(0.0f,1.0f,0.0f);\n"
"   else if(aColor<0.6f)"
"        myColor = vec3(1.0f,1.0f,0.0f);\n"
"   else"
"        myColor = vec3(0.0f,0.0f,1.0f);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 myColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(myColor, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //顶点数据
    float vertices[] = {

-0.85,-0.8,0,
-0.8,-0.4,0,
-0.85,-0,0,
-0.8,0.4,0,
-0.85,0.8,0,
-0.816667,-0.8,0,
-0.766667,-0.4,0,
-0.816667,-0,0,
-0.766667,0.4,0,
-0.816667,0.8,0,
-0.783333,-0.8,0,
-0.733333,-0.4,0,
-0.783333,-0,0,
-0.733333,0.4,0,
-0.783333,0.8,0,
-0.75,-0.8,0,
-0.7,-0.4,0,
-0.75,-0,0,
-0.7,0.4,0,
-0.75,0.8,0,
-0.716667,-0.8,0,
-0.666667,-0.4,0,
-0.716667,-0,0,
-0.666667,0.4,0,
-0.716667,0.8,0,
-0.683333,-0.8,0,
-0.633333,-0.4,0,
-0.683333,-0,0,
-0.633333,0.4,0,
-0.683333,0.8,0,
-0.65,-0.8,0,
-0.6,-0.4,0,
-0.65,-0,0,
-0.6,0.4,0,
-0.65,0.8,0,
-0.616667,-0.8,0,
-0.566667,-0.4,0,
-0.616667,-0,0,
-0.566667,0.4,0,
-0.616667,0.8,0,
-0.583333,-0.8,0,
-0.533333,-0.4,0,
-0.583333,-0,0,
-0.533333,0.4,0,
-0.583333,0.8,0,
-0.55,-0.8,0,
-0.5,-0.4,0,
-0.55,-0,0,
-0.5,0.4,0,
-0.55,0.8,0,
-0.516667,-0.8,0,
-0.466667,-0.4,0,
-0.516667,-0,0,
-0.466667,0.4,0,
-0.516667,0.8,0,
-0.483333,-0.8,0,
-0.433333,-0.4,0,
-0.483333,-0,0,
-0.433333,0.4,0,
-0.483333,0.8,0,
-0.45,-0.8,0,
-0.4,-0.4,0,
-0.45,-0,0,
-0.4,0.4,0,
-0.45,0.8,0,
-0.416667,-0.8,0,
-0.366667,-0.4,0,
-0.416667,-0,0,
-0.366667,0.4,0,
-0.416667,0.8,0,
-0.383333,-0.8,0,
-0.333333,-0.4,0,
-0.383333,-0,0,
-0.333333,0.4,0,
-0.383333,0.8,0,
-0.35,-0.8,0,
-0.3,-0.4,0,
-0.35,-0,0,
-0.3,0.4,0,
-0.35,0.8,0,
-0.316667,-0.8,0,
-0.266667,-0.4,0,
-0.316667,-0,0,
-0.266667,0.4,0,
-0.316667,0.8,0,
-0.283333,-0.8,0,
-0.233333,-0.4,0,
-0.283333,-0,0,
-0.233333,0.4,0,
-0.283333,0.8,0,
-0.25,-0.8,0,
-0.2,-0.4,0,
-0.25,-0,0,
-0.2,0.4,0,
-0.25,0.8,0,
-0.216667,-0.8,0,
-0.166667,-0.4,0,
-0.216667,-0,0,
-0.166667,0.4,0,
-0.216667,0.8,0,
-0.183333,-0.8,0,
-0.133333,-0.4,0,
-0.183333,-0,0,
-0.133333,0.4,0,
-0.183333,0.8,0,
-0.15,-0.8,0,
-0.1,-0.4,0,
-0.15,-0,0,
-0.1,0.4,0,
-0.15,0.8,0,
-0.116667,-0.8,0,
-0.0666667,-0.4,0,
-0.116667,-0,0,
-0.0666667,0.4,0,
-0.116667,0.8,0,
-0.0833334,-0.8,0,
-0.0333334,-0.4,0,
-0.0833334,-0,0,
-0.0333334,0.4,0,
-0.0833334,0.8,0,
-0.05,-0.8,0,
0,-0.4,0,
-0.05,-0,0,
0,0.4,0,
-0.05,0.8,0,
-0.0166667,-0.8,0,
0.0333333,-0.4,0,
-0.0166667,-0,0,
0.0333333,0.4,0,
-0.0166667,0.8,0,
0.0166667,-0.8,0,
0.0666667,-0.4,0,
0.0166667,-0,0,
0.0666667,0.4,0,
0.0166667,0.8,0,
0.05,-0.8,0,
0.1,-0.4,0,
0.05,-0,0,
0.1,0.4,0,
0.05,0.8,0,
0.0833333,-0.8,0,
0.133333,-0.4,0,
0.0833333,-0,0,
0.133333,0.4,0,
0.0833333,0.8,0,
0.116667,-0.8,0,
0.166667,-0.4,0,
0.116667,-0,0,
0.166667,0.4,0,
0.116667,0.8,0,
0.15,-0.8,0,
0.2,-0.4,0,
0.15,-0,0,
0.2,0.4,0,
0.15,0.8,0,
0.183333,-0.8,0,
0.233333,-0.4,0,
0.183333,-0,0,
0.233333,0.4,0,
0.183333,0.8,0,
0.216667,-0.8,0,
0.266667,-0.4,0,
0.216667,-0,0,
0.266667,0.4,0,
0.216667,0.8,0,
0.25,-0.8,0,
0.3,-0.4,0,
0.25,-0,0,
0.3,0.4,0,
0.25,0.8,0,
0.283333,-0.8,0,
0.333333,-0.4,0,
0.283333,-0,0,
0.333333,0.4,0,
0.283333,0.8,0,
0.316667,-0.8,0,
0.366667,-0.4,0,
0.316667,-0,0,
0.366667,0.4,0,
0.316667,0.8,0,
0.35,-0.8,0,
0.4,-0.4,0,
0.35,-0,0,
0.4,0.4,0,
0.35,0.8,0,
0.383333,-0.8,0,
0.433333,-0.4,0,
0.383333,-0,0,
0.433333,0.4,0,
0.383333,0.8,0,
0.416667,-0.8,0,
0.466667,-0.4,0,
0.416667,-0,0,
0.466667,0.4,0,
0.416667,0.8,0,
0.45,-0.8,0,
0.5,-0.4,0,
0.45,-0,0,
0.5,0.4,0,
0.45,0.8,0,
0.483333,-0.8,0,
0.533333,-0.4,0,
0.483333,-0,0,
0.533333,0.4,0,
0.483333,0.8,0,
0.516667,-0.8,0,
0.566667,-0.4,0,
0.516667,-0,0,
0.566667,0.4,0,
0.516667,0.8,0,
0.55,-0.8,0,
0.6,-0.4,0,
0.55,-0,0,
0.6,0.4,0,
0.55,0.8,0,
0.583333,-0.8,0,
0.633333,-0.4,0,
0.583333,-0,0,
0.633333,0.4,0,
0.583333,0.8,0,
0.616667,-0.8,0,
0.666667,-0.4,0,
0.616667,-0,0,
0.666667,0.4,0,
0.616667,0.8,0,
0.65,-0.8,0,
0.7,-0.4,0,
0.65,-0,0,
0.7,0.4,0,
0.65,0.8,0,
0.683333,-0.8,0,
0.733333,-0.4,0,
0.683333,-0,0,
0.733333,0.4,0,
0.683333,0.8,0,
0.716667,-0.8,0,
0.766667,-0.4,0,
0.716667,-0,0,
0.766667,0.4,0,
0.716667,0.8,0,
0.75,-0.8,0,
0.8,-0.4,0,
0.75,-0,0,
0.8,0.4,0,
0.75,0.8,0,
0.783333,-0.8,0,
0.833333,-0.4,0,
0.783333,-0,0,
0.833333,0.4,0,
0.783333,0.8,0,

//0.5F,0.5F,0.0F,
//-0.5F,-0.5F,0.0F,
//0.5F,-0.5F,0.0F,
//-0.5F,0.5F,0.0F,

//-0.894737,-0.9,    0.0f,
//-0.894737,0.9,    0.0f,
//-0.794737,-0.9,    0.0f,
//-0.794737,0.9,    0.0f,
//-0.689474,-0.9,    0.0f,
//-0.689474,0.9,    0.0f,
//-0.584211,-0.9,    0.0f,
//-0.584211,0.9,    0.0f,
//-0.478947,-0.9,    0.0f,
//-0.478947,0.9,    0.0f,
//-0.373684,-0.9,    0.0f,
//-0.373684,0.9,    0.0f,
//-0.268421,-0.9,    0.0f,
//-0.268421,0.9,    0.0f,
//-0.163158,-0.9,    0.0f,
//-0.163158,0.9,    0.0f,
//-0.0578947,-0.9,0.0f,
//-0.0578947,0.9,    0.0f,
//0.0473685,-0.9,    0.0f,
//0.0473685,0.9,    0.0f,
//0.152632,-0.9,    0.0f,
//0.152632,0.9,    0.0f,
//0.257895,-0.9,    0.0f,
//0.257895,0.9,    0.0f,
//0.363158,-0.9,    0.0f,
//0.363158,0.9,    0.0f,
//0.468421,-0.9,    0.0f,
//0.468421,0.9,    0.0f,
//0.573684,-0.9,    0.0f,
//0.573684,0.9,    0.0f,
//0.678947,-0.9,    0.0f,
//0.678947,0.9,    0.0f,
//0.784211,-0.9,    0.0f,
//0.784211,0.9,    0.0f,
//0.889474,-0.9,    0.0f,
//0.889474,0.9,    0.0f,
    };

    const int verCount = 250 * 3;
    int indCount = 0;

    std::vector<float> vecVertices;
    vecVertices.resize(verCount / 3 * 4);

    for (size_t i = 0; i < verCount / 3; i++)
    {
        vecVertices[i * 4 + 0] = vertices[i * 3 + 0];  //x
        vecVertices[i * 4 + 1] = vertices[i * 3 + 1];  //y
        vecVertices[i * 4 + 2] = vertices[i * 3 + 2];  //z
    }

    //拓扑数据
    int topo[] = {
98,
6,1,0,5,6,7,2,
6,3,2,7,8,9,4,
6,6,5,10,11,12,7,
6,8,7,12,13,14,9,
6,11,10,15,16,17,12,
6,13,12,17,18,19,14,
6,16,15,20,21,22,17,
6,18,17,22,23,24,19,
6,21,20,25,26,27,22,
6,23,22,27,28,29,24,
6,26,25,30,31,32,27,
6,28,27,32,33,34,29,
6,31,30,35,36,37,32,
6,33,32,37,38,39,34,
6,36,35,40,41,42,37,
6,38,37,42,43,44,39,
6,41,40,45,46,47,42,
6,43,42,47,48,49,44,
6,46,45,50,51,52,47,
6,48,47,52,53,54,49,
6,51,50,55,56,57,52,
6,53,52,57,58,59,54,
6,56,55,60,61,62,57,
6,58,57,62,63,64,59,
6,61,60,65,66,67,62,
6,63,62,67,68,69,64,
6,66,65,70,71,72,67,
6,68,67,72,73,74,69,
6,71,70,75,76,77,72,
6,73,72,77,78,79,74,
6,76,75,80,81,82,77,
6,78,77,82,83,84,79,
6,81,80,85,86,87,82,
6,83,82,87,88,89,84,
6,86,85,90,91,92,87,
6,88,87,92,93,94,89,
6,91,90,95,96,97,92,
6,93,92,97,98,99,94,
6,96,95,100,101,102,97,
6,98,97,102,103,104,99,
6,101,100,105,106,107,102,
6,103,102,107,108,109,104,
6,106,105,110,111,112,107,
6,108,107,112,113,114,109,
6,111,110,115,116,117,112,
6,113,112,117,118,119,114,
6,116,115,120,121,122,117,
6,118,117,122,123,124,119,
6,121,120,125,126,127,122,
6,123,122,127,128,129,124,
6,126,125,130,131,132,127,
6,128,127,132,133,134,129,
6,131,130,135,136,137,132,
6,133,132,137,138,139,134,
6,136,135,140,141,142,137,
6,138,137,142,143,144,139,
6,141,140,145,146,147,142,
6,143,142,147,148,149,144,
6,146,145,150,151,152,147,
6,148,147,152,153,154,149,
6,151,150,155,156,157,152,
6,153,152,157,158,159,154,
6,156,155,160,161,162,157,
6,158,157,162,163,164,159,
6,161,160,165,166,167,162,
6,163,162,167,168,169,164,
6,166,165,170,171,172,167,
6,168,167,172,173,174,169,
6,171,170,175,176,177,172,
6,173,172,177,178,179,174,
6,176,175,180,181,182,177,
6,178,177,182,183,184,179,
6,181,180,185,186,187,182,
6,183,182,187,188,189,184,
6,186,185,190,191,192,187,
6,188,187,192,193,194,189,
6,191,190,195,196,197,192,
6,193,192,197,198,199,194,
6,196,195,200,201,202,197,
6,198,197,202,203,204,199,
6,201,200,205,206,207,202,
6,203,202,207,208,209,204,
6,206,205,210,211,212,207,
6,208,207,212,213,214,209,
6,211,210,215,216,217,212,
6,213,212,217,218,219,214,
6,216,215,220,221,222,217,
6,218,217,222,223,224,219,
6,221,220,225,226,227,222,
6,223,222,227,228,229,224,
6,226,225,230,231,232,227,
6,228,227,232,233,234,229,
6,231,230,235,236,237,232,
6,233,232,237,238,239,234,
6,236,235,240,241,242,237,
6,238,237,242,243,244,239,
6,241,240,245,246,247,242,
6,243,242,247,248,249,244,

//2,
//3,0,1,2,
//3,0,1,3,

//        17,
//4,0,1,3,2,
//4,2,3,5,4,
//4,4,5,7,6,
//4,6,7,9,8,
//4,8,9,11,10,
//4,10,11,13,12,
//4,12,13,15,14,
//4,14,15,17,16,
//4,16,17,19,18,
//4,18,19,21,20,
//4,20,21,23,22,
//4,22,23,25,24,
//4,24,25,27,26,
//4,26,27,29,28,
//4,28,29,31,30,
//4,30,31,33,32,
//4,32,33,35,34,

    };

    //流场数据
    std::default_random_engine e;
    //std::uniform_real_distribution<float> r(0.0, 1.0);
    std::normal_distribution<float> r(0.0, 1.0);
    //float vecFieldData[2] = { 0.2f,0.8f };
    float vecFieldData[98] = { 0.0 };
    for (size_t i = 0; i < 98; i++)
    {
        vecFieldData[i] = r(e);
    }

    int start = 1;
    int end = topo[1] + 2;

    std::vector<uint32_t> vecIndices;  //所有的索引

    for (int i = 0; i < topo[0]; i++)  //  拓扑的个数
    {
        std::vector<int> vecIndex;  //拓扑数据中的单个拓扑
        for (int j = start; j < end; j++)
        {
            vecIndex.emplace_back(topo[j]);
        }
        start = end;
        end += topo[end] + 1;

        int x = *(vecIndex.begin() + 1);
        size_t startField = vecIndices.size(); //本次需要赋值流场数据的单元（cell)所包含的点，在vecIndices中的位置

        for (int k = 2; k < vecIndex.size() - 1; k++)
        {
            vecIndices.emplace_back(x);
            vecIndices.emplace_back(vecIndex[k]);
            vecIndices.emplace_back(vecIndex[k + 1]);
        }

        //流场数据
        for (size_t n = startField; n < vecIndices.size(); n++)
        {
            vecVertices[(vecIndices[n] + 1) * 4 - 1] = vecFieldData[i];  //w 先用索引数组找到是第几个点，然后计算w的下标
        }
    }
    indCount = vecIndices.size();
    //索引数组
    auto ind = vecIndices.data();
    //顶点数组
    auto ver = vecVertices.data();

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verCount / 3 * 4 * sizeof(float), ver, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(uint32_t), ind, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


#endif // TEST7

#ifdef TEST8

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <random>
#include <vector>
#include <iostream>
#include <list>

struct Color
{
    float r, g, b, a; // 每个分量的取值范围为[0.0, 1.0]
};

/// 颜色映射表
class ColorMap
{
public:
    bool IsEmpty() const;

    // 根据param，将节点插入mColorNodes中
    void AddNode(float param, Color color) { mColorNodes.emplace_back(ColorNode{ param,color }); }
    void Pop() {}

    Color Lookup(float param) const {}

    const size_t size() const { return mColorNodes.size(); }

    void Clear() {}
private:
    struct ColorNode
    {
        float param;
        Color color;
    };
public:
    std::list<ColorNode> mColorNodes;
};


unsigned char* GenTexture(ColorMap cm, uint32_t n)
{
    auto cmSize = cm.size();

    if (cmSize <= 0)
        return nullptr;


}

unsigned char* GenTexture(ColorMap myCM)
{
    //归一化
    ColorMap cm = myCM;
    auto start = cm.mColorNodes.front().param;
    auto end = cm.mColorNodes.back().param;

    for (auto& elem : cm.mColorNodes)
    {
        elem.param = elem.param / (end - start);
    }


    ///< 生成纹理的结果
    unsigned char* result = new unsigned char[1024 * 4];
    ///< 用来保存前一个特征点的序号
    int preIndex = 0;
    ///< 用来保存前一个特征点的颜色
    Color preColor = cm.mColorNodes.begin()->color;
    for (auto it = cm.mColorNodes.begin(); it != cm.mColorNodes.end(); it++)
    {
        ///< 输出颜色的序号
        const int index = static_cast<const int>(it->param * 1023);
        ///< 给定的特征点直接使用，不需要计算
        result[index * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
        result[index * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
        result[index * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
        result[index * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);

        if (it == cm.mColorNodes.begin())
            continue;

        ///< 两个特征点之间序号的间隔
        int distance = index - preIndex - 1;
        ///< 两个特征点之间颜色的间隔
        float rStep = it->color.r - preColor.r;
        float gStep = it->color.g - preColor.g;
        float bStep = it->color.b - preColor.b;
        float aStep = it->color.a - preColor.a;
        ///< 两个特征点之间的所有插值
        for (int i = preIndex + 1; i < index; i++)
        {
            if (true)
            {
                result[i * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
                result[i * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
                result[i * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
                result[i * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);
            }
            else
            {
                result[i * 4 + 0] = static_cast<unsigned char>((preColor.r + (i - preIndex) * rStep / distance) * 255);
                result[i * 4 + 1] = static_cast<unsigned char>((preColor.g + (i - preIndex) * gStep / distance) * 255);
                result[i * 4 + 2] = static_cast<unsigned char>((preColor.b + (i - preIndex) * bStep / distance) * 255);
                result[i * 4 + 3] = static_cast<unsigned char>((preColor.a + (i - preIndex) * aStep / distance) * 255);
            }

        }
        preIndex = index;
        preColor = it->color;
    }
    return result;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec4 aPos;\n"
//"layout (location = 2) in float aTexCoord;\n"
//"layout (location = 2) in vec2 aTexCoord;\n"
"uniform float maxP;\n"
"uniform float minP;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   TexCoord = vec2((aPos.w-minP)/(maxP-minP),0.0);\n"
//"   TexCoord = aTexCoord;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture,TexCoord);\n"
//"   FragColor = vec4(0.0f,1.0f,0.0f,1.0f);\n"
"}\n\0";
#include <direct.h>
int main()
{
    auto ret = _getcwd(NULL, 0);
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader zprogram
    // ------------------------------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vs_FBO, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //顶点数据
    float vertices[] = {
#if(1)
0.5F,0.5F,0.0F,
-0.5F,-0.5F,0.0F,
0.5F,-0.5F,0.0F,
-0.5F,0.5F,0.0F,
#else
- 0.85,-0.8,0,
-0.8,-0.4,0,
-0.85,-0,0,
-0.8,0.4,0,
-0.85,0.8,0,
-0.816667,-0.8,0,
-0.766667,-0.4,0,
-0.816667,-0,0,
-0.766667,0.4,0,
-0.816667,0.8,0,
-0.783333,-0.8,0,
-0.733333,-0.4,0,
-0.783333,-0,0,
-0.733333,0.4,0,
-0.783333,0.8,0,
-0.75,-0.8,0,
-0.7,-0.4,0,
-0.75,-0,0,
-0.7,0.4,0,
-0.75,0.8,0,
-0.716667,-0.8,0,
-0.666667,-0.4,0,
-0.716667,-0,0,
-0.666667,0.4,0,
-0.716667,0.8,0,
-0.683333,-0.8,0,
-0.633333,-0.4,0,
-0.683333,-0,0,
-0.633333,0.4,0,
-0.683333,0.8,0,
-0.65,-0.8,0,
-0.6,-0.4,0,
-0.65,-0,0,
-0.6,0.4,0,
-0.65,0.8,0,
-0.616667,-0.8,0,
-0.566667,-0.4,0,
-0.616667,-0,0,
-0.566667,0.4,0,
-0.616667,0.8,0,
-0.583333,-0.8,0,
-0.533333,-0.4,0,
-0.583333,-0,0,
-0.533333,0.4,0,
-0.583333,0.8,0,
-0.55,-0.8,0,
-0.5,-0.4,0,
-0.55,-0,0,
-0.5,0.4,0,
-0.55,0.8,0,
-0.516667,-0.8,0,
-0.466667,-0.4,0,
-0.516667,-0,0,
-0.466667,0.4,0,
-0.516667,0.8,0,
-0.483333,-0.8,0,
-0.433333,-0.4,0,
-0.483333,-0,0,
-0.433333,0.4,0,
-0.483333,0.8,0,
-0.45,-0.8,0,
-0.4,-0.4,0,
-0.45,-0,0,
-0.4,0.4,0,
-0.45,0.8,0,
-0.416667,-0.8,0,
-0.366667,-0.4,0,
-0.416667,-0,0,
-0.366667,0.4,0,
-0.416667,0.8,0,
-0.383333,-0.8,0,
-0.333333,-0.4,0,
-0.383333,-0,0,
-0.333333,0.4,0,
-0.383333,0.8,0,
-0.35,-0.8,0,
-0.3,-0.4,0,
-0.35,-0,0,
-0.3,0.4,0,
-0.35,0.8,0,
-0.316667,-0.8,0,
-0.266667,-0.4,0,
-0.316667,-0,0,
-0.266667,0.4,0,
-0.316667,0.8,0,
-0.283333,-0.8,0,
-0.233333,-0.4,0,
-0.283333,-0,0,
-0.233333,0.4,0,
-0.283333,0.8,0,
-0.25,-0.8,0,
-0.2,-0.4,0,
-0.25,-0,0,
-0.2,0.4,0,
-0.25,0.8,0,
-0.216667,-0.8,0,
-0.166667,-0.4,0,
-0.216667,-0,0,
-0.166667,0.4,0,
-0.216667,0.8,0,
-0.183333,-0.8,0,
-0.133333,-0.4,0,
-0.183333,-0,0,
-0.133333,0.4,0,
-0.183333,0.8,0,
-0.15,-0.8,0,
-0.1,-0.4,0,
-0.15,-0,0,
-0.1,0.4,0,
-0.15,0.8,0,
-0.116667,-0.8,0,
-0.0666667,-0.4,0,
-0.116667,-0,0,
-0.0666667,0.4,0,
-0.116667,0.8,0,
-0.0833334,-0.8,0,
-0.0333334,-0.4,0,
-0.0833334,-0,0,
-0.0333334,0.4,0,
-0.0833334,0.8,0,
-0.05,-0.8,0,
0,-0.4,0,
-0.05,-0,0,
0,0.4,0,
-0.05,0.8,0,
-0.0166667,-0.8,0,
0.0333333,-0.4,0,
-0.0166667,-0,0,
0.0333333,0.4,0,
-0.0166667,0.8,0,
0.0166667,-0.8,0,
0.0666667,-0.4,0,
0.0166667,-0,0,
0.0666667,0.4,0,
0.0166667,0.8,0,
0.05,-0.8,0,
0.1,-0.4,0,
0.05,-0,0,
0.1,0.4,0,
0.05,0.8,0,
0.0833333,-0.8,0,
0.133333,-0.4,0,
0.0833333,-0,0,
0.133333,0.4,0,
0.0833333,0.8,0,
0.116667,-0.8,0,
0.166667,-0.4,0,
0.116667,-0,0,
0.166667,0.4,0,
0.116667,0.8,0,
0.15,-0.8,0,
0.2,-0.4,0,
0.15,-0,0,
0.2,0.4,0,
0.15,0.8,0,
0.183333,-0.8,0,
0.233333,-0.4,0,
0.183333,-0,0,
0.233333,0.4,0,
0.183333,0.8,0,
0.216667,-0.8,0,
0.266667,-0.4,0,
0.216667,-0,0,
0.266667,0.4,0,
0.216667,0.8,0,
0.25,-0.8,0,
0.3,-0.4,0,
0.25,-0,0,
0.3,0.4,0,
0.25,0.8,0,
0.283333,-0.8,0,
0.333333,-0.4,0,
0.283333,-0,0,
0.333333,0.4,0,
0.283333,0.8,0,
0.316667,-0.8,0,
0.366667,-0.4,0,
0.316667,-0,0,
0.366667,0.4,0,
0.316667,0.8,0,
0.35,-0.8,0,
0.4,-0.4,0,
0.35,-0,0,
0.4,0.4,0,
0.35,0.8,0,
0.383333,-0.8,0,
0.433333,-0.4,0,
0.383333,-0,0,
0.433333,0.4,0,
0.383333,0.8,0,
0.416667,-0.8,0,
0.466667,-0.4,0,
0.416667,-0,0,
0.466667,0.4,0,
0.416667,0.8,0,
0.45,-0.8,0,
0.5,-0.4,0,
0.45,-0,0,
0.5,0.4,0,
0.45,0.8,0,
0.483333,-0.8,0,
0.533333,-0.4,0,
0.483333,-0,0,
0.533333,0.4,0,
0.483333,0.8,0,
0.516667,-0.8,0,
0.566667,-0.4,0,
0.516667,-0,0,
0.566667,0.4,0,
0.516667,0.8,0,
0.55,-0.8,0,
0.6,-0.4,0,
0.55,-0,0,
0.6,0.4,0,
0.55,0.8,0,
0.583333,-0.8,0,
0.633333,-0.4,0,
0.583333,-0,0,
0.633333,0.4,0,
0.583333,0.8,0,
0.616667,-0.8,0,
0.666667,-0.4,0,
0.616667,-0,0,
0.666667,0.4,0,
0.616667,0.8,0,
0.65,-0.8,0,
0.7,-0.4,0,
0.65,-0,0,
0.7,0.4,0,
0.65,0.8,0,
0.683333,-0.8,0,
0.733333,-0.4,0,
0.683333,-0,0,
0.733333,0.4,0,
0.683333,0.8,0,
0.716667,-0.8,0,
0.766667,-0.4,0,
0.716667,-0,0,
0.766667,0.4,0,
0.716667,0.8,0,
0.75,-0.8,0,
0.8,-0.4,0,
0.75,-0,0,
0.8,0.4,0,
0.75,0.8,0,
0.783333,-0.8,0,
0.833333,-0.4,0,
0.783333,-0,0,
0.833333,0.4,0,
0.783333,0.8,0,
#endif
    };

    const int verCount = 4 * 3;
    int indCount = 0;

    std::vector<float> vecVertices;
    vecVertices.resize(verCount / 3 * 4);

    for (size_t i = 0; i < verCount / 3; i++)
    {
        vecVertices[i * 4 + 0] = vertices[i * 3 + 0];  //x
        vecVertices[i * 4 + 1] = vertices[i * 3 + 1];  //y
        vecVertices[i * 4 + 2] = vertices[i * 3 + 2];  //z
    }

    //拓扑数据
    int indices[] = {
#if(1)
2,
3,0,1,2,
3,0,1,3,
#else
98,
6,1,0,5,6,7,2,
6,3,2,7,8,9,4,
6,6,5,10,11,12,7,
6,8,7,12,13,14,9,
6,11,10,15,16,17,12,
6,13,12,17,18,19,14,
6,16,15,20,21,22,17,
6,18,17,22,23,24,19,
6,21,20,25,26,27,22,
6,23,22,27,28,29,24,
6,26,25,30,31,32,27,
6,28,27,32,33,34,29,
6,31,30,35,36,37,32,
6,33,32,37,38,39,34,
6,36,35,40,41,42,37,
6,38,37,42,43,44,39,
6,41,40,45,46,47,42,
6,43,42,47,48,49,44,
6,46,45,50,51,52,47,
6,48,47,52,53,54,49,
6,51,50,55,56,57,52,
6,53,52,57,58,59,54,
6,56,55,60,61,62,57,
6,58,57,62,63,64,59,
6,61,60,65,66,67,62,
6,63,62,67,68,69,64,
6,66,65,70,71,72,67,
6,68,67,72,73,74,69,
6,71,70,75,76,77,72,
6,73,72,77,78,79,74,
6,76,75,80,81,82,77,
6,78,77,82,83,84,79,
6,81,80,85,86,87,82,
6,83,82,87,88,89,84,
6,86,85,90,91,92,87,
6,88,87,92,93,94,89,
6,91,90,95,96,97,92,
6,93,92,97,98,99,94,
6,96,95,100,101,102,97,
6,98,97,102,103,104,99,
6,101,100,105,106,107,102,
6,103,102,107,108,109,104,
6,106,105,110,111,112,107,
6,108,107,112,113,114,109,
6,111,110,115,116,117,112,
6,113,112,117,118,119,114,
6,116,115,120,121,122,117,
6,118,117,122,123,124,119,
6,121,120,125,126,127,122,
6,123,122,127,128,129,124,
6,126,125,130,131,132,127,
6,128,127,132,133,134,129,
6,131,130,135,136,137,132,
6,133,132,137,138,139,134,
6,136,135,140,141,142,137,
6,138,137,142,143,144,139,
6,141,140,145,146,147,142,
6,143,142,147,148,149,144,
6,146,145,150,151,152,147,
6,148,147,152,153,154,149,
6,151,150,155,156,157,152,
6,153,152,157,158,159,154,
6,156,155,160,161,162,157,
6,158,157,162,163,164,159,
6,161,160,165,166,167,162,
6,163,162,167,168,169,164,
6,166,165,170,171,172,167,
6,168,167,172,173,174,169,
6,171,170,175,176,177,172,
6,173,172,177,178,179,174,
6,176,175,180,181,182,177,
6,178,177,182,183,184,179,
6,181,180,185,186,187,182,
6,183,182,187,188,189,184,
6,186,185,190,191,192,187,
6,188,187,192,193,194,189,
6,191,190,195,196,197,192,
6,193,192,197,198,199,194,
6,196,195,200,201,202,197,
6,198,197,202,203,204,199,
6,201,200,205,206,207,202,
6,203,202,207,208,209,204,
6,206,205,210,211,212,207,
6,208,207,212,213,214,209,
6,211,210,215,216,217,212,
6,213,212,217,218,219,214,
6,216,215,220,221,222,217,
6,218,217,222,223,224,219,
6,221,220,225,226,227,222,
6,223,222,227,228,229,224,
6,226,225,230,231,232,227,
6,228,227,232,233,234,229,
6,231,230,235,236,237,232,
6,233,232,237,238,239,234,
6,236,235,240,241,242,237,
6,238,237,242,243,244,239,
6,241,240,245,246,247,242,
6,243,242,247,248,249,244,
#endif
    };

    //流场数据
    std::default_random_engine e/*(time(nullptr))*/;
    std::uniform_real_distribution<float> r(-10, 10);
    //std::lognormal_distribution<double> r(0.0, 1.0);
    //float vecFieldData[2] = { 0.0f,1.0f };
    float vecFieldData[98] = { 0.0 };
    for (size_t i = 0; i < 98; i++)
    {
        vecFieldData[i] = r(e);
    }

    int start = 1;
    int end = indices[1] + 2;

    std::vector<uint32_t> vecIndices;  //所有的索引

    for (int i = 0; i < indices[0]; i++)  //  拓扑的个数
    {
        std::vector<int> vecIndex;  //拓扑数据中的单个拓扑
        for (int j = start; j < end; j++)
        {
            vecIndex.emplace_back(indices[j]);
        }
        start = end;
        end += indices[end] + 1;

        int core = *(vecIndex.begin() + 1);
        size_t startField = vecIndices.size(); //本次需要赋值流场数据的单元（cell)所包含的点，在vecIndices中的位置

        for (int k = 2; k < vecIndex.size() - 1; k++)
        {
            vecIndices.emplace_back(core);
            vecIndices.emplace_back(vecIndex[k]);
            vecIndices.emplace_back(vecIndex[k + 1]);
        }

        //流场数据
        for (size_t n = startField; n < vecIndices.size(); n++)
        {
            vecVertices[(vecIndices[n] + 1) * 4 - 1] = vecFieldData[i];  //w 先用索引数组找到是第几个点，然后计算w的下标
        }
    }
    indCount = vecIndices.size();
    //索引数组
    auto ind = vecIndices.data();
    //顶点数组
    auto ver = vecVertices.data();

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verCount / 3 * 4 * sizeof(float), ver, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(uint32_t), ind, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    //纹理1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glShade(GL_SMOOTH);
    //glShadeModel
    //加载纹理图片1
    int width, height, nrChannels;

    //注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
    //unsigned char* data2 = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

    ColorMap cm;

    cm.AddNode(0.0, Color{ 1.0, 0.0, 0.0, 1.0 });
    cm.AddNode(0.3, Color{ 1.0, 1.0, 0.0, 1.0 });
    cm.AddNode(0.5, Color{ 0.0, 1.0, 0.0, 1.0 });
    cm.AddNode(0.6, Color{ 0.0, 1.0, 1.0, 1.0 });
    cm.AddNode(0.8, Color{ 0.0, 0.0, 1.0, 1.0 });

    unsigned char* retx = GenTexture(cm);

    unsigned char data2[] = {
        255,    0,    0,    255,
          0,  255,    0,    255,
          0,    0,  255,    255,
        255,    0,    0,    255,
    };

    if (retx)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, retx);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "maxP"), 10.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "minP"), -10.0f);

        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 12);
        glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST8

#ifdef TEST9
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x, double y);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//vec4(myColor.r,myColor.g,myColor.b, 1.0f)
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 myColor;\n"
"void main()\n"
"{\n"
"   FragColor = myColor;\n"
"}\n\0";

int shaderProgram;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit(); //glfw 初始化
    //设置OpenGL版本等信息
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    //创建GLFW窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //设置当前上下文
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //设置鼠标回调函数
    glfwSetCursorPosCallback(window, mouse_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    //把OpenGL的函数指针导入给GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.5f, 0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f, 0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(unsigned int), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    //循环渲染
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        //glUseProgram(shaderProgram);
        //最好不要在此调用glUseProgram
        //glUniform4f(glGetUniformLocation(shaderProgram, "myColor"), 1.0f, 0.0f, 0.0f, 1.0f);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        //交换缓存
        glfwSwapBuffers(window);
        //事件处理
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    //停止
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
//输入处理函数
void processInput(GLFWwindow* window) {
    unsigned int ID = 0;
    char name[] = "myColor";
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //通过键盘控制uniform的值改变图形颜色
    //注意：使用glUniform**之前需要调用glUseProgram，不要在注循环while里面调用glUseProgram
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram, "myColor"), 0.0f, 1.0f, 0.0f, 1.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram, "myColor"), 0.0f, 0.0f, 1.0f, 1.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram, "myColor"), 0.0f, 1.0f, 1.0f, 1.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram, "myColor"), 1.0f, 1.0f, 0.0f, 1.0f);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//窗口大小设置回调函数
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//
void mouse_callback(GLFWwindow* window, double x, double y)
{
    //std::cout << x << "," << y << '\n';
    float data[100] = { 0 };
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(data));

    std::cout << data << '\n';
}

#endif // TEST9

#ifdef TEST10

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 oColor;\n"
"void main()\n"
"{\n"
"   oColor = aColor;\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 oColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(oColor, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,     1.0f,  0.0f, 0.0f, // top right
         0.5f, -0.5f, 0.0f,     0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,     0.0f,  1.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,     1.0f,  0.0f, 0.0f, // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
        0, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
#endif // TEST10

#ifdef TEST11

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShader1Source = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";
const char* fragmentShader2Source = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
"}\n\0";


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // we skipped compile log checks this time for readability (if you do encounter issues, add the compile-checks! see previous code samples)
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderOrange = glCreateShader(GL_FRAGMENT_SHADER); // the first fragment shader that outputs the color orange
    unsigned int fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER); // the second fragment shader that outputs the color yellow
    unsigned int shaderProgramOrange = glCreateProgram();
    unsigned int shaderProgramYellow = glCreateProgram(); // the second shader program
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShaderOrange, 1, &fragmentShader1Source, NULL);
    glCompileShader(fragmentShaderOrange);
    glShaderSource(fragmentShaderYellow, 1, &fragmentShader2Source, NULL);
    glCompileShader(fragmentShaderYellow);
    // link the first program object
    glAttachShader(shaderProgramOrange, vertexShader);
    glAttachShader(shaderProgramOrange, fragmentShaderOrange);
    glLinkProgram(shaderProgramOrange);
    // then link the second program object using a different fragment shader (but same vertex shader)
    // this is perfectly allowed since the inputs and outputs of both the vertex and fragment shaders are equally matched.
    glAttachShader(shaderProgramYellow, vertexShader);
    glAttachShader(shaderProgramYellow, fragmentShaderYellow);
    glLinkProgram(shaderProgramYellow);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,  // left 
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top 
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top 
    };
    unsigned int VBOs[2], VAOs[2];
    glGenVertexArrays(2, VAOs); // we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(2, VBOs);
    // first triangle setup
    // --------------------
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);    // Vertex attributes stay the same
    glEnableVertexAttribArray(0);
    // glBindVertexArray(0); // no need to unbind at all as we directly bind a different VAO the next few lines
    // second triangle setup
    // ---------------------
    glBindVertexArray(VAOs[1]);    // note that we bind to a different VAO now
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);    // and a different VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
    glEnableVertexAttribArray(0);
    // glBindVertexArray(0); // not really necessary as well, but beware of calls that could affect VAOs while this one is bound (like binding element buffer objects, or enabling/disabling vertex attributes)


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // now when we draw the triangle we first use the vertex and orange fragment shader from the first program
        glUseProgram(shaderProgramOrange);
        // draw the first triangle using the data from our first VAO
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);    // this call should output an orange triangle
        // then we draw the second triangle using the data from the second VAO
        // when we draw the second triangle we want to use a different shader program so we switch to the shader program with our yellow fragment shader.
        glUseProgram(shaderProgramYellow);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);    // this call should output a yellow triangle

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteProgram(shaderProgramOrange);
    glDeleteProgram(shaderProgramYellow);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

#endif // TEST11

