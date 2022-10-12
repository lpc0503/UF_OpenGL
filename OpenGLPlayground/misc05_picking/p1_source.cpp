// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <sstream>

// Include GLEW
//#include <GL/glew.h>

// glad
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <shader.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>

#include <filesystem>
namespace fs = std::filesystem;
// ATTN 1A is the general place in the program where you have to change the code base to satisfy a Task of Project 1A.
// ATTN 1B for Project 1B. ATTN 1C for Project 1C. Focus on the ones relevant for the assignment you're working on.

// ATTN: use POINT structs for cleaner code (POINT is a part of a vertex)
// allows for (1-t)*P_1+t*P_2  avoiding repeat for each coordinate (x,y,z)
#include "Geometry.h"
#include "BSplineCurve.h"
#include "BezierCurve.h"
#include "CatmullRomCurve.h"

// Function prototypes
int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex[], GLushort[], int);
void createObjects(void);
void pickVertex(void);
void moveVertex(void);
void OnRenderScene(void);
void cleanup(void);
static void mouseCallback(GLFWwindow*, int, int, int);
static void keyCallback(GLFWwindow*, int, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;
const GLuint window_width = 1024, window_height = 768;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

// Program IDs
GLuint programID;
GLuint pickingProgramID;

// Uniform IDs
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayID;
GLuint pickingColorID;
GLuint SizeID;

GLuint gPickedIndex;
bool gIsPicked = false;
std::string gMessage;

// ATTN: INCREASE THIS NUMBER AS YOU CREATE NEW OBJECTS
const GLuint NumObjects = 2; // Number of objects types in the scene

// Keeps track of IDs associated with each object
GLuint VertexArrayId[NumObjects];
GLuint VertexBufferId[NumObjects];
GLuint IndexBufferId[NumObjects];

size_t VertexBufferSize[NumObjects];
size_t IndexBufferSize[NumObjects];
size_t NumVerts[NumObjects];	// Useful for glDrawArrays command
size_t NumIdcs[NumObjects];	// Useful for glDrawElements command

// Initialize ---  global objects -- not elegant but ok for this project
const size_t IndexCount = 10;
Vertex Vertices[IndexCount];
Vertex OriginVertecies[IndexCount];
GLushort Indices[IndexCount];

// ATTN: DON'T FORGET TO INCREASE THE ARRAY SIZE IN THE PICKING VERTEX SHADER WHEN YOU ADD MORE PICKING COLORS
float pickingColor[IndexCount];

// For Imgui
bool show_demo_window = true;
bool show_another_window = true;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool gIsCycle = true;
bool gDrawLine = true;
bool gDrawControlPoint = true;
bool gDrawOrigPoint = true;
bool gIsShiftDown = false;
bool gDoubleView = false;

float gViewport[3][4] = {
    {0, window_height, window_width, window_height/2},
    {0, 0, window_width, window_height/2},
    {0, 0, window_width, window_height}
};

enum CurveType: int
{
    BSpline,
    Bezier,
    Catmull
};
int gSelectCurve = Catmull;

//
float gPointSize = 5.f;

int initWindow(void) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
    const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC

	// ATTN: Project 1A, Task 0 == Change the name of the window
	// Open a window and create its OpenGL context
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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Set up inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetCursorPos(window, window_width / 2, window_height / 2);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

	return 0;
}

// Hello,
void initOpenGL(void) {
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for Project 1, use an ortho camera :
	gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	gViewMatrix = glm::lookAt(
		glm::vec3(0, 0, -5), // Camera is at (0,0,-5) below the origin, in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is looking up at the origin (set to 0,-1,0 to look upside-down)
	);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("./asset/shader/p1_StandardShading.vertexshader", "./asset/shader/p1_StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("./asset/shader/p1_Picking.vertexshader", "./asset/shader/p1_Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
    SizeID = glGetUniformLocation(programID, "Size");

	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayID = glGetUniformLocation(pickingProgramID, "PickingColorArray");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");

	// Define pickingColor array for picking program
	// use a for-loop here

    for(int i = 0 ; i < IndexCount ; i++) {

        pickingColor[i] = i/255.0f;
    }

    // init viewport
    int fx, fy;
    glfwGetFramebufferSize(window, &fx, &fy);
    gViewport[0][1] = float((window_height * fy/window_height)/2);
    gViewport[0][2] = float((window_width  * fx/window_width));
    gViewport[0][3] = float((window_height * fy/window_height)/2);
    gViewport[1][2] = float((window_width  * fx/window_width));
    gViewport[1][3] = float((window_height * fy/window_height)/2);
    gViewport[2][2] = float((window_width  * fx/window_width));
    gViewport[2][3] = float((window_height * fy/window_height));

    printf("windowX %d, fx %d, view %f\n", window_height, fy, gViewport[2][3]);

//    float gViewport[2][4] = {
//            {0, window_height/2, window_width, window_height/2},
//            {0, 0, window_width, window_height/2}
//    };
	// Define objects
	createObjects();

	// ATTN: create VAOs for each of the newly created objects here:
	// for several objects of the same type use a for-loop
	int obj = 0;  // initially there is only one type of object
	VertexBufferSize[obj] = sizeof(Vertices);
	IndexBufferSize[obj] = sizeof(Indices);
	NumIdcs[obj] = IndexCount;

	createVAOs(Vertices, Indices, obj);

}

// this actually creates the VAO (structure) and the VBO (vertex data buffer)
void createVAOs(Vertex Vertices[], GLushort Indices[], int ObjectId) {
	GLenum ErrorCheckValue = glGetError();
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);
	glBindVertexArray(VertexArrayId[ObjectId]);

	// Create buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, VertexBufferSize[ObjectId], Vertices, GL_STATIC_DRAW);

	// Create buffer for indices
	if (Indices != NULL) {
		glGenBuffers(1, &IndexBufferId[ObjectId]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize[ObjectId], Indices, GL_STATIC_DRAW);
	}

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color

	// Disable our Vertex Buffer Object
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
//		fprintf(
//			stderr,
//			"ERROR: Could not create a VBO: %s \n",
//			gluErrorString(ErrorCheckValue)
//		);
	}
}

// Task 1
// A, B  -> K+1, K

const float angle_ = 3.1415926 * 2.f / 5;
BSplineCurve *bspline;
BezierCurve *bezier;
CatmullRomCurve *catmull;

void createObjects(void) {

    float radius = 1.f;
	// ATTN: DERIVE YOUR NEW OBJECTS HERE:  each object has
	// an array of vertices {pos;color} and
	// an array of indices (no picking needed here) (no need for indices)
	// ATTN: Project 1A, Task 1 == Add the points in your scene
    for(int i = 0 ; i < IndexCount ; i++) {

        Vertices[i] = {{0.f, 0.f, 0.f, 1.f}, {1.f, 1.f, 1.f, 1.f}};
    }

    for(int i = 0 ; i < 5 ; i++) {

        float newX = radius * sin(angle_*i);
        float newY = -radius * cos(angle_*i) + 1;
        Vertices[i].Position[0] = newX;
        Vertices[i].Position[1] = newY;
        OriginVertecies[i].Position[0] = newX;
        OriginVertecies[i].Position[1] = newY;
    }

    for(int i = 0 ; i < 5 ; i++) {

        float newX = radius * sin(angle_*i);
        float newY = radius * cos(angle_*i)-1;
        Vertices[i+5].Position[0] = newX;
        Vertices[i+5].Position[1] = newY;
        OriginVertecies[i+5].Position[0] = newX;
        OriginVertecies[i+5].Position[1] = newY;
    }

    for(int i = 0 ; i < IndexCount ; i++) {

        Indices[i] = i;
    }

    bspline = new BSplineCurve(Vertices, IndexCount);
    bezier = new BezierCurve(Vertices, IndexCount);
    catmull = new CatmullRomCurve(Vertices, IndexCount);
}

void pickVertex(void) {
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // initialization
		// ModelMatrix == TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		// MVP should really be PVM...
		// Send the MVP to the shader (that is currently bound)
		// as data type uniform (shared by all shader instances)
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

		// pass in the picking color array to the shader
		glUniform1fv(pickingColorArrayID, IndexCount, pickingColor);

		// --- enter vertices into VBO and draw
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(VertexArrayId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[0], Vertices);	// update buffer data
		glDrawElements(GL_POINTS, NumIdcs[0], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	glFlush();
	// --- Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow !
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// --- Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel,
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];  // 2x2 pixel region

    int winX, winY;
    int fx, fy;
    glfwGetWindowSize(window, &winX, &winY);
    glfwGetFramebufferSize(window, &fx, &fy);

	glReadPixels(xpos * (fx/winX), (window_height - ypos) * (fy/winY), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
       	// window_height - ypos;
	// OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);

	// ATTN: Project 1A, Task 2
	// Find a way to change color of selected vertex and
	// store original color
    if(gPickedIndex >= IndexCount) {

        gIsPicked = false;
        for(int i = 0 ; i < IndexCount ; i++) {

            Vertices[i].isSelected = false;
        }
    }
    else {

        gIsPicked = true;
        Vertices[gPickedIndex].isSelected = true;
    }

    for(auto &i: Vertices) {

        if(i.isSelected) {

            if(!i.swapped) {

                std::swap(i.Color, i.SelectedColor);
                i.swapped = true;
            }
        }
    }

	// Uncomment these lines if you want to see the picking shader in effect
//	 glfwSwapBuffers(window);
	 // continue; // skips the visible rendering
}

// ATTN: Project 1A, Task 3 == Retrieve your cursor position, get corresponding world coordinate, and move the point accordingly

// ATTN: Project 1C, Task 1 == Keep track of z coordinate for selected point and adjust its value accordingly based on if certain
// buttons are being pressed
void moveVertex(void) {

	glm::mat4 ModelMatrix = glm::mat4(1.0);
	GLint viewport[4] = {};
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(0, 0, window_width, window_height);
//	glm::vec4 vp = glm::vec4(gViewport[2][0], gViewport[2][1], gViewport[2][2], gViewport[2][3]);

    double mouseX;
    double mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);
    glm::vec3 tmp = glm::unProject(glm::vec3(mouseX, mouseY, 0.0), ModelMatrix, gProjectionMatrix, vp); // TODO ?????

    if (gPickedIndex >= IndexCount) {
		// Any number > vertices-indices is background!
		gMessage = "background";
	}
	else {

		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str() + std::to_string(tmp.x) + " " + std::to_string(tmp.y) + " " + std::to_string(tmp.z);

        for(auto &i : Vertices) {

            if(i.isSelected) {

                if(gIsShiftDown) // Move in z axis [-2, 2]
                {
                    i.Position[2] = -tmp.y;
                }
                else
                {
                    i.Position[0] = -tmp.x;
                    i.Position[1] = -tmp.y;
                }
            }
        }
	}
}

void OnImGuiUpdate() {

    {
        ImGui::Begin("Point Attribute");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

        ImGui::SliderFloat("Point Size", &gPointSize, 0.f, 50.f);
        ImGui::Checkbox("Draw lines", &gDrawLine);
        ImGui::Checkbox("Draw curve point", &gDrawControlPoint);
        ImGui::Checkbox("Draw control point", &gDrawOrigPoint);

        ImGui::RadioButton("B-Spline", &gSelectCurve, CurveType::BSpline);
        ImGui::RadioButton("Bezier", &gSelectCurve, CurveType::Bezier);
        ImGui::RadioButton("CatmullRom", &gSelectCurve, CurveType::Catmull);

        ImGui::Separator();

        if(gSelectCurve == CurveType::BSpline){

            ImGui::Text("B-Spline Level: %s", std::to_string(bspline->K).c_str());
            ImGui::SameLine();
            if (ImGui::Button("Go")){

                bspline->SubDivide();
            }
            ImGui::Checkbox("Cycle", &gIsCycle);
        }
        else if(gSelectCurve == CurveType::Bezier){

        }
        else if(gSelectCurve == CurveType::Catmull){

            ImGui::SliderFloat("Step", &catmull->step, 0.01f, 1.f);
        }

        ImGui::Separator();
        ImGui::Text("Last picked object %s", gMessage.c_str());
        ImGui::Text("Pick: %d, isPicked = %d", gPickedIndex, gIsPicked);
        if(ImGui::Button("Reset")) {

            for(int i = 0 ; i < IndexCount ; i++) {

                Vertices[i].Position[0] = OriginVertecies[i].Position[0];
                Vertices[i].Position[1] = OriginVertecies[i].Position[1];
            }
            bspline->Clear();
        }

        ImGui::End();
    }

    {
        ImGui::Begin("Point Control");
        if(ImGui::CollapsingHeader("Control Point")) {

            ImGuiIO &io = ImGui::GetIO();
            ImGui::Text("%s", gMessage.c_str());
            for(int i = 0 ; i < IndexCount ; i++){

                auto &pos = Vertices[i].Position;
                auto label = ("Point#" + std::to_string(i)).c_str();
                ImGui::InputFloat3(label, pos);
            }
        }

        if(ImGui::CollapsingHeader("B-Spline Point")) {

            ImGuiIO &io = ImGui::GetIO();
            for(int i = 0 ; i < bspline->V.size() ; i++) {

                auto &pos = bspline->V[i].Position;
                auto label = ("Point#" + std::to_string(i)).c_str();
                ImGui::InputFloat2(label, pos, "%.3f", ImGuiInputTextFlags_ReadOnly);
            }
        }

        if(ImGui::CollapsingHeader("Bezier Point")) {

            ImGuiIO &io = ImGui::GetIO();
            for(int i = 0 ; i < bezier->V.size() ; i++) {

                auto &pos = bezier->V[i].Position;
                auto label = ("Point#" + std::to_string(i)).c_str();
                ImGui::InputFloat2(label, pos, "%.3f", ImGuiInputTextFlags_ReadOnly);
            }
        }

        if(ImGui::CollapsingHeader("CatmullRomCurve Point")) {

            ImGuiIO &io = ImGui::GetIO();
            for(int i = 0 ; i < catmull->V.size() ; i++) {

                auto &pos = catmull->V[i].Position;
                auto label = ("Pos#" + std::to_string(i)).c_str();
                ImGui::InputFloat2(label, pos, "%.3f", ImGuiInputTextFlags_ReadOnly);
            }
        }

        ImGui::End();
    }
}

void OnUpdateScene(){
    glfwPollEvents();

    if(gSelectCurve == CurveType::Bezier) {

        bezier->SetVertices(Vertices, IndexCount);
        bezier->Clear();
        bezier->Calc();
        bezier->ToVertex();
    }
    else if(gSelectCurve == CurveType::BSpline) {

        bspline->SetVertices(Vertices, IndexCount);
        bspline->Recover();
    }
    else if(gSelectCurve == CurveType::Catmull){

        catmull->SetVertices(Vertices, IndexCount);
        catmull->Clear();
        catmull->Calc();
        catmull->ToVertex();
    }

    if(gIsCycle){

        if(bspline->K >= 6){

            bspline->Clear();
        }
    }
}

void OnRenderScene(void) {

	glUseProgram(programID);
	{
		// see comments in pick
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
        glUniform1f(SizeID, gPointSize);

		glEnable(GL_PROGRAM_POINT_SIZE);

		glBindVertexArray(VertexArrayId[0]);	// Draw Vertices
        if(gDrawOrigPoint){

            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, VertexBufferSize[0], Vertices);        // Update buffer data
            glDrawElements(GL_POINTS, NumIdcs[0], GL_UNSIGNED_SHORT, (void *) 0);
        }

        // hotfix

        if(gSelectCurve == CurveType::BSpline){

            if(bspline->V.size() != 0) {

                if(gDrawControlPoint){

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[0]);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bspline->I.size() * sizeof(GLushort),
                                 bspline->I.data(), GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
                    glBufferData(GL_ARRAY_BUFFER, (bspline->V.size() * sizeof(Vertex)),
                                 bspline->V.data(), GL_STATIC_DRAW);
                    glDrawElements(GL_POINTS, bspline->I.size(), GL_UNSIGNED_SHORT, (void *) 0);
                }

                if(gDrawLine) {

                    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
                    glBufferData(GL_ARRAY_BUFFER, (bspline->V.size() * sizeof(Vertex)),
                                 bspline->V.data(), GL_STATIC_DRAW);
                    std::vector<GLushort> indices;
                    for(int i = 0; i < bspline->V.size(); i++)
                        indices.push_back(i);
                    indices.push_back(0);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
                    glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
                }
            }
        }
        else if(gSelectCurve == CurveType::Bezier)
        {
            if(bezier->V.size() != 0) {

                if(gDrawControlPoint)
                {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[0]);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bezier->I.size() * sizeof(GLushort), bezier->I.data(),
                                 GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
                    glBufferData(GL_ARRAY_BUFFER, (bezier->V.size() * sizeof(Vertex)), bezier->V.data(),
                                 GL_STATIC_DRAW);
                    glDrawElements(GL_POINTS, bezier->I.size(), GL_UNSIGNED_SHORT, (void *) 0);
                }

                if(gDrawLine)
                {
                    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
                    glBufferData(GL_ARRAY_BUFFER, (bezier->V.size() * sizeof(Vertex)), bezier->V.data(),
                                 GL_STATIC_DRAW);
                    std::vector<GLushort> indices;
                    for(int i = 0 ; i < bezier->V.size() ; i++)
                        indices.push_back(i);
                    indices.push_back(0);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
                    glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
                }
            }
        }
        else if(gSelectCurve == CurveType::Catmull) {

            if(!catmull->V.empty()){

                // Draw point
                if(gDrawControlPoint){

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[0]);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, catmull->I.size() * sizeof(GLushort), catmull->I.data(),
                                 GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
                    glBufferData(GL_ARRAY_BUFFER, (catmull->V.size() * sizeof(Vertex)), catmull->V.data(),
                                 GL_STATIC_DRAW);
                    glDrawElements(GL_POINTS, catmull->I.size(), GL_UNSIGNED_SHORT, (void *) 0);
                }

                // Draw line
                if(gDrawLine) {

                    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
                    glBufferData(GL_ARRAY_BUFFER, (catmull->V.size() * sizeof(Vertex)), catmull->V.data(),
                                 GL_STATIC_DRAW);
                    std::vector<GLushort> indices;
                    for(int i = 0 ; i < catmull->V.size() ; i++)
                        indices.push_back(i);
                    indices.push_back(0);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
                    glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
                }

                // Draw curve points
                if(!catmull->V2.empty()) {

                    std::vector<GLushort> indices;
                    for(int i = 0 ; i < catmull->V2.size() ; i++)
                        indices.push_back(i);
                    indices.push_back(0);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(),
                                 GL_STATIC_DRAW);
                    glBufferData(GL_ARRAY_BUFFER, (catmull->V2.size() * sizeof(Vertex)), catmull->V2.data(),
                                 GL_STATIC_DRAW);
                    glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_SHORT, (void *) 0);
                }
            }
        }
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void cleanup(void) {
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

// Alternative way of triggering functions on mouse click and keyboard events
static void mouseCallback(GLFWwindow* window, int button, int action, int mods) {

    if(gDoubleView) return;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        pickVertex();
	}

    if(action == GLFW_RELEASE) {
        gIsPicked = false;
        for(auto &i : Vertices) {

            if(i.isSelected) {

                std::swap(i.Color, i.SelectedColor);
                i.isSelected = false;
                i.swapped = false;
            }
        }
    }
}

// ATTN: Project 1B, Task 2 and 4 == account for key presses to activate subdivision and hiding/showing functionality
// for respective tasks
bool isKeyPressed = false;
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        gIsShiftDown = true;
    else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        gIsShiftDown = false;

    if(key == GLFW_KEY_1 && action == GLFW_PRESS) {

        if (!isKeyPressed) {

            bspline->SubDivide();
            gSelectCurve = CurveType::BSpline;
            isKeyPressed = true;
        }
    }
    else if(key == GLFW_KEY_2 && action == GLFW_PRESS) {

        if(!isKeyPressed) {

            gSelectCurve = CurveType::Bezier;
            isKeyPressed = true;
        }
    }
    else if(key == GLFW_KEY_3 && action == GLFW_PRESS){

        if(!isKeyPressed) {

            gSelectCurve = CurveType::Catmull;
            isKeyPressed = true;
        }
    }
    else if(key == GLFW_KEY_4 && action == GLFW_PRESS) {

        if(!isKeyPressed) {

            glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            gDoubleView = !gDoubleView;
            std::cout << gDoubleView << std::endl;
        }
    }
    else {

        isKeyPressed = false;
    }
}

int main(void) {
	// ATTN: REFER TO https://learnopengl.com/Getting-started/Creating-a-window
	// AND https://learnopengl.com/Getting-started/Hello-Window to familiarize yourself with the initialization of a window in OpenGL

	// Initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// ATTN: REFER TO https://learnopengl.com/Getting-started/Hello-Triangle to familiarize yourself with the graphics pipeline
	// from setting up your vertex data in vertex shaders to rendering the data on screen (everything that follows)
    std::cout << "Current path is " << fs::current_path() << '\n';
	// Initialize OpenGL pipeline
	initOpenGL();

	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		// Timing
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last printf() was more than 1sec ago
//			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// DRAGGING: move current (picked) vertex with cursor
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			moveVertex();
		}

        OnUpdateScene();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        OnImGuiUpdate();

        // upper view

        if (gDoubleView) {

            glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(gViewport[0][0], gViewport[0][1], gViewport[0][2], gViewport[0][3]);
            gViewMatrix = glm::lookAt(glm::vec3{0.f, 0.f, -5.f}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f});
            OnRenderScene();

            // lower view
            glViewport(gViewport[1][0], gViewport[1][1], gViewport[1][2], gViewport[1][3]);
            gViewMatrix = glm::lookAt(glm::vec3{-5.f, 0.f, 0.f}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f});
            OnRenderScene();
            glfwSwapBuffers(window);
        }
        else {

            glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(gViewport[2][0], gViewport[2][1], gViewport[2][2], gViewport[2][3]);
            gViewMatrix = glm::lookAt(glm::vec3{0.f, 0.f, -5.f}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f});
            OnRenderScene();
            glfwSwapBuffers(window);
        }
        // ImGui Render
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

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	cleanup();

	return 0;
}
