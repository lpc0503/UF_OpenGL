// Include standard headers
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <shader.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>

#include "Renderer.h"
#include "Camera.h"
#include "Log.h"
#include "Model.h"
#include "Utils.h"

const int window_width = 1024, window_height = 768;

struct Vertex2 {
	float Position[4];
	float Color[4];
	float Normal[3];
	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}
};

// function prototypes
int InitWindow();
void InitOpenGL();
void CreateVAOs(Vertex2[], GLushort[], int);
void LoadObject(char*, glm::vec4, Vertex2* &, GLushort* &, int);
void CreateObjects();
void PickObject();
void OnRenderScene();
void Cleanup();
void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallback(GLFWwindow*, int, int, int, int);
void MouseCallback(GLFWwindow*, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

const GLuint NumObjects = 1;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects];
GLuint VertexBufferId[NumObjects];
GLuint IndexBufferId[NumObjects];

// TL
size_t VertexBufferSize[NumObjects];
size_t IndexBufferSize[NumObjects];
size_t NumIdcs[NumObjects];
size_t NumVerts[NumObjects];

GLint MatrixID;
GLint ModelMatrixID;
GLint ViewMatrixID;
GLint ProjMatrixID;
GLint PickingMatrixID;
GLint pickingColorID;
GLint LightID;

// hotfix
#define RED {1.f, 0.f, 0.f, 1.f}
#define GREEN {0.f, 1.f, 0.f, 1.f}
#define BLUE {0.f, 0.f, 1.f, 1.f}
#define PURPLE {1.f, 0.f, 1.f, 1.f}
#define CYAN {0.f, 1.f, 1.f, 1.f}
#define YELLOW {1.f, 1.f, 0.f, 1.f}
glm::vec4 MeshColor[6] = {PURPLE, GREEN, YELLOW, RED, BLUE, CYAN};
#define Joint 0
#define TOP 1
#define PEN 2
#define BASE 3
#define ARM1 4
#define ARM2 5
// 0 joint, 1 top, 2 pen, 3 base, 4 arm1, 5 arm2

// TODO: to ref
std::shared_ptr<Camera> g_Camera;

glm::vec4 g_ClearColor = {0.0f, 0.0f, 0.2f, 0.0f};
float g_MouseWheelFactor = 0.2;
glm::vec3 g_SunLight = {0.f, -1.f, 0.f};

// Declare global objects
// TL
const size_t CoordVertsCount = 6;
Vertex2 CoordVerts[CoordVertsCount];

int InitWindow() {
    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

#if defined(__APPLE__)
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC
#elif defined(_WIN32)
    const char* glsl_version = "#version 460";
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    window = glfwCreateWindow(window_width, window_height, "Po_Chuan,Liang(7336-5707)", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

#if defined(WIN32)
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetCursorPos(window, static_cast<float>(window_width) / 2, static_cast<float>(window_height) / 2);
    glfwSetMouseButtonCallback(window, MouseCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetScrollCallback(window, MouseWheelCallback);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Renderer::Init();
    return 0;
}

void InitOpenGL() {
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	gProjectionMatrix = glm::perspective(45.0f, window_width / (float)window_height, 0.1f, 100.0f);

	// Camera matrix
	gViewMatrix = glm::lookAt(glm::vec3(10.0, 10.0, 10.0f),	// eye
                              glm::vec3(0.0, 0.0, 0.0),	    // center
                              glm::vec3(0.0, 1.0, 0.0));	// up

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
	pickingProgramID = LoadShaders("shaders/Picking.vert", "shaders/Picking.frag");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ProjMatrixID = glGetUniformLocation(programID, "P");

	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// TL
	// Define objects
    CreateObjects();

	// ATTN: create VAOs for each of the newly created objects here:
	VertexBufferSize[0] = sizeof(CoordVerts);
	NumVerts[0] = CoordVertsCount;

	CreateVAOs(CoordVerts, NULL, 0);
}

void CreateVAOs(Vertex2 vertices[], unsigned short indices[], int objectID) {
	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(vertices[0]);
	const size_t RgbOffset = sizeof(vertices[0].Position);
	const size_t Normaloffset = sizeof(vertices[0].Color) + RgbOffset;

	// Create Vertex2 Array Object
	glGenVertexArrays(1, &VertexArrayId[objectID]);
	glBindVertexArray(VertexArrayId[objectID]);

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[objectID]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[objectID]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[objectID], vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	if (indices != NULL) {
		glGenBuffers(1, &IndexBufferId[objectID]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[objectID]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[objectID], indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)Normaloffset);	// TL

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color
	glEnableVertexAttribArray(2);	// normal

	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n"//,
//			gluErrorString(ErrorCheckValue)
		);
	}
}

// Ensure your .obj files are in the correct format and properly loaded by looking at the following function
void LoadObject(char* file, glm::vec4 color, Vertex2* &out_Vertices, GLushort* &out_Indices, int objectID) {
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ(file, vertices, normals);

	std::vector<GLushort> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

	const size_t vertCount = indexed_vertices.size();
	const size_t idxCount = indices.size();

	// populate output arrays
	out_Vertices = new Vertex2[vertCount];
	for (int i = 0; i < vertCount; i++) {
		out_Vertices[i].SetPosition(&indexed_vertices[i].x);
		out_Vertices[i].SetNormal(&indexed_normals[i].x);
		out_Vertices[i].SetColor(&color[0]);
	}
	out_Indices = new GLushort[idxCount];
	for (int i = 0; i < idxCount; i++) {
		out_Indices[i] = indices[i];
	}

	// set global variables!!
	NumIdcs[objectID] = idxCount;
	VertexBufferSize[objectID] = sizeof(out_Vertices[0]) * vertCount;
	IndexBufferSize[objectID] = sizeof(GLushort) * idxCount;
}

void CreateObjects() {
	//-- COORDINATE AXES --//
	CoordVerts[0] = { { 0.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } }; // x
	CoordVerts[1] = { { 5.0, 0.0, 0.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	CoordVerts[2] = { { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } }; // y
	CoordVerts[3] = { { 0.0, 5.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	CoordVerts[4] = { { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } }; // z
	CoordVerts[5] = { { 0.0, 0.0, 5.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0 } };
	
	//-- GRID --//
	
	// ATTN: Create your grid vertices here!
	
	//-- .OBJs --//

	// ATTN: Load your models here through .obj files -- example of how to do so is as shown
	// Vertex2* Verts;
	// GLushort* Idcs;
	// LoadObject("models/base.obj", glm::vec4(1.0, 0.0, 0.0, 1.0), Verts, Idcs, ObjectID);
	// CreateVAOs(Verts, Idcs, ObjectID);
}

void PickObject() {
	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

		// ATTN: DRAW YOUR PICKING SCENE HERE. REMEMBER TO SEND IN A DIFFERENT PICKING COLOR FOR EACH OBJECT BEFOREHAND
		glBindVertexArray(0);
	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);

	if (gPickedIndex == 255) { // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
	}

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

Ref<Model> BunnyModel;
Ref<Model> RobotArmModel;
Ref<Model> TestModel;
glm::vec3 Rotate[6];
void OnInitScene()
{
    g_Camera = std::make_shared<Camera>(glm::perspective(45.0f, window_width / (float)window_height, 0.1f, 100.0f));
    g_Camera->SetPosition(10.0f, 10.0f, 10.0f);
    g_Camera->LookAt(0.f, 0.f, 0.f);

    BunnyModel = Model::LoadModel("../asset/bunny.obj");
    TestModel = Model::LoadModel("../asset/Robot.obj");

    for(int i = 0 ; i < 6 ; i++) {

        Rotate[i] = glm::vec3 (0.f, 0.f, 0.f);
    }

//    RobotArmModel = Model::LoadModel("../asset/robot-arm/robot-arm.obj");
    INFO("size = {}", TestModel->GetMeshes().size());
}

float CameraMoveSpeed = 1.f;
glm::vec3 CameraRotate = {18.320f, -44.f, 0.f};
glm::vec3 CameraPos = {0.f, 0.f, 10.f};
double PrevMouseX, PrevMouseY;
glm::vec3 BunnyPos = glm::vec3{0.f};
glm::vec3 BunnyScale = glm::vec3{1.f};

// MeshMove
enum MeshMove {

    None,
    BaseMove,
    TopMove,
    Ara1Move,
    Arm2Move,
    PenMove
};

int moveType = MeshMove::None;
glm::vec3 ModelMove = {0.f, 0.f, 0.f};
glm::vec3 TopRotate = {0.f, 0.f, 0.f};
glm::vec3 Arm1Rotate = {0.f, 0.f, 0.f};
glm::vec3 Arm2Rotate = {0.f, 0.f, 0.f};


void OnUpdateScene(float dt)
{
    glfwPollEvents();

    { // Camera rotate
        { // Camera Pan
            double x, y, offX, offY;
            glfwGetCursorPos(window, &x, &y);
            x = x - window_width / 2.f;
            y = window_height / 2.f - y;
            offX = x - PrevMouseX;
            offY = y - PrevMouseY;

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
            {
                CameraRotate.x += static_cast<float>(-offY) * 5.f * dt;
                CameraRotate.y += static_cast<float>(offX) * 5.f * dt;

                if(CameraRotate.x >= 90.f)
                    CameraRotate.x = 89.9f;
                if(CameraRotate.x <= -90.f)
                    CameraRotate.x = -89.9f;
            }

            PrevMouseX = x;
            PrevMouseY = y;
        }

        if(moveType == MeshMove::None) {

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                CameraRotate.y += 5.f * CameraMoveSpeed * dt;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                CameraRotate.y -= 5.f * CameraMoveSpeed * dt;
            }
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                CameraRotate.x += 5.f * CameraMoveSpeed * dt;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                CameraRotate.x -= 5.f * CameraMoveSpeed * dt;
            }
        }

        if(moveType == MeshMove::BaseMove) {

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                ModelMove.z += .1f;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                ModelMove.z -= .1f;
            }
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                ModelMove.x += .1f;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                ModelMove.x -= .1f;
            }
        }

        if(moveType == MeshMove::TopMove) {

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                TopRotate.y += 5.f;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                TopRotate.y -= 5.f;
            }
        }

        glm::mat4 mat{1.f};
        // The order of rotation have to be x -> y or we have to deal with the gimbal lock
        mat = glm::rotate(mat, glm::radians(CameraRotate.x), glm::vec3{1.f, 0.f, 0.f});
        mat = glm::rotate(mat, glm::radians(CameraRotate.y), glm::vec3{0.f, 1.f, 0.f});
        g_Camera->SetPosition(glm::vec4{CameraPos, 1.f} * mat); // TODO: 需要理解????
    }
}

void OnImGuiUpdate()
{
//    ImGui::ShowDemoWindow();

    ImGui::Begin("Settings");

    ImGui::DragFloat("Mouse Wheel", &g_MouseWheelFactor, 0.1f);

    auto dir = g_Camera->GetDir();
    ImGui::Text("Camera Direction = %.2f %.2f %.2f", dir.x, dir.y, dir.z);
    ImGui::Text("Sun Direction = %.2f %.2f %.2f", g_SunLight.x, g_SunLight.y, g_SunLight.z);
    if(ImGui::Button("Set Sun"))
    {
        g_SunLight = dir;
    }

    ImGui::ColorEdit4("Background", glm::value_ptr(g_ClearColor));

    ImGui::SliderFloat("Speed", &CameraMoveSpeed, 1.f, 10.f);
    ImGui::DragFloat3("Pos", &CameraPos);
    ImGui::DragFloat3("Rotation", &CameraRotate);
    ImGui::DragFloat3("Bunny Pos", &BunnyPos);
    ImGui::DragFloat3("Bunny Scale", &BunnyScale);

    /*for(int i = 0; i < BunnyModel->GetMeshCount(); i++)
    {
        auto &mesh = BunnyModel->GetMeshes()[i];

        if(ImGui::CollapsingHeader("Vertices"))
        {
            for (int j = 0; j < mesh->m_Vertices.size(); j++)
            {
                ImGui::Text("vertex %d %.2f, %.2f, %.2f", j, mesh->m_Vertices[j].pos.x, mesh->m_Vertices[j].pos.y,
                            mesh->m_Vertices[j].pos.z);
            }
        }

        if(ImGui::CollapsingHeader("Indices"))
        {
            for (int j = 0; j < mesh->m_Indices.size(); j++)
            {
                ImGui::Text("indice %d", mesh->m_Indices[j]);
            }
        }
    }*/

    ImGui::End();
}

void OnRenderScene()
{

    Renderer::BeginScene(g_Camera);
    Renderer::DrawGrid(5, 5);
    Renderer::DrawDirectionalLight(g_SunLight, {1.f, 1.f, 1.f, 1.f});
    Renderer::DrawMesh(BunnyModel->GetMeshes().front(), BunnyPos, {0.f, 0.f, 0.f}, BunnyScale);

    // hotcode

    //Base
//    Renderer::DrawMesh(TestModel->GetMeshes()[0], {0.f, 0.f, 0.f}, BaseRotate, {.5f, .5f, .5f}, MeshColor[0]);
//    // Top
//    Renderer::DrawMesh(TestModel->GetMeshes()[1], {0.f, 0.f, 0.f}, BaseRotate, {.5f, .5f, .5f}, MeshColor[1]);
//    // Arm1
//    Renderer::DrawMesh(TestModel->GetMeshes()[2], {0.f, 0.f, 0.f}, BaseRotate+TopRotate, {.5f, .5f, .5f}, MeshColor[2]);
//    // Joint
//    Renderer::DrawMesh(TestModel->GetMeshes()[3], {0.f, 0.f, 0.f}, BaseRotate+TopRotate, {.5f, .5f, .5f}, MeshColor[3]);
//    // Arm2
//    Renderer::DrawMesh(TestModel->GetMeshes()[4], {0.f, 0.f, 0.f}, BaseRotate+TopRotate+Arm1Rotate, {.5f, .5f, .5f}, MeshColor[4]);


// 0 joint, 1 top, 2 pen, 3 base, 4 arm1, 5 arm2

    for(int i = 0 ; i < TestModel->GetMeshCount() ; i++) {

        if(i == BASE)
            Renderer::DrawMesh(TestModel->GetMeshes()[i], ModelMove, {0.f, 0.f, 0.f}, {.5f, .5f, .5f}, MeshColor[i]);
        else if(moveType == MeshMove::TopMove) {
            Rotate[i] += TopRotate;
            Renderer::DrawMesh(TestModel->GetMeshes()[i], ModelMove, Rotate[i], {.5f, .5f, .5f}, MeshColor[i]);
        }
        else
            Renderer::DrawMesh(TestModel->GetMeshes()[i], ModelMove, Rotate[i], {.5f, .5f, .5f}, MeshColor[i]);

    }
    TopRotate = {0.f, 0.f, 0.f};

//    for(auto &mesh : TestModel->GetMeshes())
//    {
//        Renderer::DrawMesh(mesh, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {.5f, .5f, .5f});
//    }
//
    Renderer::DrawLine(glm::vec3 {0, 0, 0}, glm::vec3 {5.f, 0.f, 0.f}, glm::vec4 {1.f, 0.f, 0.f, 1.f});
    Renderer::DrawLine(glm::vec3 {0, 0, 0}, glm::vec3 {0.f, 5.f, 0.f}, glm::vec4 {0.f, 1.f, 0.f, 1.f});
    Renderer::DrawLine(glm::vec3 {0, 0, 0}, glm::vec3 {0.f, 0.f, 5.f}, glm::vec4 {0.f, 0.f, 1.f, 1.f});

    auto sunDir = glm::normalize(g_Camera->GetDir());
    Renderer::DrawLine({1.f, 1.f, 1.f}, glm::vec3{1.f, 1.f, 1.f} + sunDir * 0.5f, {1.f, 1.f, 0.f, 1.f});

//    for(auto &mesh : RobotArmModel->GetMeshes())
//    {
//        if(mesh->m_Name == "b")
//        {
//            Renderer::DrawMesh(mesh, {1.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});
//        }
//        else
//            Renderer::DrawMesh(mesh, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});
//    }
    Renderer::EndScene();

    // TODO: Use Renderer to draw this

//    glUseProgram(programID);
//    {
//        glm::vec3 lightPos = glm::vec3(4, 4, 4);
//        glm::mat4x4 ModelMatrix = glm::mat4(1.0);
//        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
//        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, glm::value_ptr(g_Camera->GetView()));
//        glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, glm::value_ptr(g_Camera->GetProjection()));
//        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
//
//        glBindVertexArray(VertexArrayId[0]);	// Draw CoordAxes
//        glDrawArrays(GL_LINES, 0, NumVerts[0]);
//
//        glBindVertexArray(0);
//    }
//    glUseProgram(0);
}

// TODO: remove
void Cleanup() {
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    CameraPos.z += (float)-yoffset * g_MouseWheelFactor;
    if(CameraPos.z < 0)
        CameraPos.z = 0.f;
}

bool hold = false;
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS) {

        if(!hold) {

            switch (key){

                case GLFW_KEY_B:
                    moveType = moveType == MeshMove::BaseMove? MeshMove::None : MeshMove::BaseMove;
                    hold = true;
                    break;
                case GLFW_KEY_T:
                    moveType = moveType == MeshMove::TopMove? MeshMove::None : MeshMove::TopMove;
                    hold = true;
                    break;
                case GLFW_KEY_A:
                    break;
                case GLFW_KEY_D:
                    break;
                case GLFW_KEY_W:
                    break;
                case GLFW_KEY_S:
                    break;
                case GLFW_KEY_SPACE:
                    break;
                default:
                    break;
            }
        }
	}
    if(action == GLFW_RELEASE) {

        hold = false;
    }
}

// Alternative way of triggering functions on mouse click events
void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        PickObject();
	}
}

int main() {

//    printf(">>");
//    getchar();

	// Initialize window
	int errorCode = InitWindow();
	if (errorCode != 0)
		return errorCode;

	// Initialize OpenGL pipeline
    InitOpenGL();
    OnInitScene();

	// For speed computation
	double lastTime = glfwGetTime(), preTime = lastTime;
	int nbFrames = 0;
	do {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

        glClearColor(g_ClearColor.x, g_ClearColor.y, g_ClearColor.z, g_ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        OnImGuiUpdate();
        OnUpdateScene(currentTime-preTime);
        preTime = currentTime;
        OnRenderScene();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
#endif
        // Swap buffers
        glfwSwapBuffers(window);
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Renderer::Shutdown();

    Cleanup();
}