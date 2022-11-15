#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "MatrixStack.h"
#include "Program.h"
#include <stack>
#include <cmath>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

char* vertShaderPath = "../shaders/shader.vert";
char* fragShaderPath = "../shaders/shader.frag";

GLFWwindow *window;
double currentXpos, currentYpos;
glm::vec3 eye(5.0f, 5.0f, 10.0f);
glm::vec3 center(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

Program program;
MatrixStack modelViewProjectionMatrix;

// Draw cube on screen
void DrawCube(glm::mat4& modelViewProjectionMatrix)
{
	program.SendUniformData(modelViewProjectionMatrix, "mvp");
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

class RobotElements
{
private:
	
public:
	std::vector<RobotElements *> children;
	bool selected = false;
	bool visited = false;
	bool rotation = false;

	glm::vec3 to_parent;
	glm::vec3 to_joint;
	glm::vec3 joint_angles;
	glm::vec3 scale_factor;

	RobotElements() {}
	~RobotElements() {}
	void select() {selected = true;}
	void deselect() {selected = false;}
	void DrawElement(RobotElements *root, MatrixStack &modelViewProjectionMatrix) 
	{
		if (root == NULL) 
		{
			return;
		}
		modelViewProjectionMatrix.pushMatrix();
		// tranform to parent
		modelViewProjectionMatrix.translate(root->to_parent);
		modelViewProjectionMatrix.rotateX(glm::radians(root->joint_angles.x));
		modelViewProjectionMatrix.rotateY(glm::radians(root->joint_angles.y));
		modelViewProjectionMatrix.rotateZ(glm::radians(root->joint_angles.z));
		modelViewProjectionMatrix.pushMatrix();
		//translate to joint
		modelViewProjectionMatrix.translate(root->to_joint); 
		modelViewProjectionMatrix.scale(root->scale_factor);
			
		DrawCube(modelViewProjectionMatrix.topMatrix());
		modelViewProjectionMatrix.popMatrix();
		for(int i = 0; i < root->children.size(); i++) 
		{
			DrawElement(root->children[i], modelViewProjectionMatrix);
		}
		modelViewProjectionMatrix.popMatrix();
		
	}
};

RobotElements torso, head, ula, lla, ura, lra, ull, lll, url, lrl;
// global so all elements are accessible
std::vector<RobotElements *> heirarchy_elems;
std::stack<RobotElements *> stack;
int current_select = 0;
void ConstructRobot()
{
	torso.to_parent = glm::vec3(0.0f, 0.0f,0.0f);
	torso.to_joint = glm::vec3(0.0f, 0.0f,0.0f);
	torso.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	torso.scale_factor = glm::vec3(1.0f, 1.5f,1.0f);
	torso.children.push_back(&head);
	torso.children.push_back(&ula);
	torso.children.push_back(&ura);
	torso.children.push_back(&ull);
	torso.children.push_back(&url);
	torso.selected = true;

	head.to_parent = glm::vec3(0.0f, 1.5f, 0.0f);
	head.to_joint = glm::vec3(0.0f, 0.5f, 0.0f);
	head.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	head.scale_factor = glm::vec3(0.5f, 0.5f,0.5f);

	ula.to_parent = glm::vec3(-1.0f, 1.25f,0.0f);
	ula.to_joint = glm::vec3(-0.75f, 0.0f,0.0f);
	ula.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	ula.scale_factor = glm::vec3(0.75f, 0.25f,0.25f);
	ula.children.push_back(&lla);

	lla.to_parent = glm::vec3(-1.5f, 0.0f,0.0f);
	lla.to_joint = glm::vec3(-0.75f, 0.0f,0.0f);
	lla.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	lla.scale_factor = glm::vec3(0.75f, 0.2f,0.2f);

	ura.to_parent = glm::vec3(1.0f, 1.25f,0.0f);
	ura.to_joint = glm::vec3(0.75f, 0.0f,0.0f);
	ura.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	ura.scale_factor = glm::vec3(0.75f, 0.25f,0.25f);
	ura.children.push_back(&lra);

	lra.to_parent = glm::vec3(1.5f, 0.0f,0.0f);
	lra.to_joint = glm::vec3(0.75f, 0.0f,0.0f);
	lra.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	lra.scale_factor = glm::vec3(0.75f, 0.2f,0.2f);

	ull.to_parent = glm::vec3(-0.5f, -1.5f,0.0f);
	ull.to_joint = glm::vec3(0.0f, -1.5f,0.0f);
	ull.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	ull.scale_factor = glm::vec3(0.4f, 1.5f,0.4f);
	ull.children.push_back(&lll);

	lll.to_parent = glm::vec3(0.0f, -3.0f,0.0f);
	lll.to_joint = glm::vec3(0.0f, -1.5f,0.0f);
	lll.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	lll.scale_factor = glm::vec3(0.3f, 1.5f,0.3f);

	url.to_parent = glm::vec3(0.5f, -1.5f,0.0f);
	url.to_joint = glm::vec3(0.0f, -1.5f,0.0f);
	url.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	url.scale_factor = glm::vec3(0.4f, 1.5f,0.4f);
	url.children.push_back(&lrl);

	lrl.to_parent = glm::vec3(0.0f, -3.0f,0.0f);
	lrl.to_joint = glm::vec3(0.0f, -1.5f,0.0f);
	lrl.joint_angles = glm::vec3(0.0f, 0.0f,0.0f);
	lrl.scale_factor = glm::vec3(0.3f, 1.5f,0.3f);

	// build the tree in dfs order 
	stack.push(&torso);
	while(!stack.empty()) {
		RobotElements* cur = stack.top();
		stack.pop();

		heirarchy_elems.push_back(cur);
		for (int i = cur->children.size() - 1; i < cur->children.size(); i--) {
			stack.push(cur->children[i]);
		}
	}
}




void Display()
{	
	program.Bind();

	modelViewProjectionMatrix.loadIdentity();
	modelViewProjectionMatrix.pushMatrix();

	// Setting the view and Projection matrices
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	modelViewProjectionMatrix.Perspective(glm::radians(60.0f), float(width) / float(height), 0.1f, 100.0f);
	modelViewProjectionMatrix.LookAt(eye, center, up);
	
	torso.DrawElement(&torso, modelViewProjectionMatrix);
	

	modelViewProjectionMatrix.popMatrix();

	program.Unbind();
	
}
 void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	std::cout << "The y offset : " << yoffset << std::endl;
	glm::vec3 a = eye - center; // camera direction + magnitude vector
	if (yoffset < 0) {
		a *= 0.9;
	}
	if(yoffset > 0) {
		a *= 1.1;
	}
	eye = a + center;
}
// Mouse callback function
void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && GLFW_PRESS == action)
		std::cout << "Mouse left button is pressed." << std::endl;
}
// init mouse positions
double prev_x = 400, prev_y = 400;
bool first;
// Mouse position callback function
void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
	glm::vec4 position(eye.x, eye.y, eye.z,1);
	glm::vec4 pivot(center.x, center.y, center.z, 1);
	if (first) {
		prev_x = xpos;
        prev_y = ypos;
        first = false;
	}
	float d_angle_x = (2*M_PI / WINDOW_WIDTH);
	float d_angle_y = (2*M_PI / WINDOW_HEIGHT);
	

	float xoffset = prev_x - xpos;
	float yoffset = prev_y - ypos; 
	int state_left = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int state_right = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	glm::vec3 a = eye - center; // camera direction + magnitude vector
	glm::vec3 w = glm::normalize(a); // camera direction
	glm::vec3 right = glm::normalize(glm::cross(up, a)); // camera right 
	glm::vec3 v = glm::cross(w, right);
	if (state_left == GLFW_PRESS) 
	{
		float xangle = xoffset * d_angle_x;
		float yangle = yoffset * d_angle_y;
		float cosA = glm::dot(w, up);
		if (cosA * sin(d_angle_y) > 0.99f) 
		{
			d_angle_y = 0;
		}
		glm::mat4 rotateMatX(1.0f);
		rotateMatX = glm::rotate(rotateMatX, xangle, up);
		position = (rotateMatX * (position - pivot)) + pivot;

		glm::mat4 rotateMatY(1.0f);
		rotateMatY = glm::rotate(rotateMatY, yangle, right);
		glm::vec3 abs_pos = (rotateMatY * (position - pivot)) + pivot;
		eye.x = abs_pos.x;
		eye.y = abs_pos.y;
		eye.z = abs_pos.z;
		
		std::cout << "Mouse position is: x - " << xpos << ", y - " << ypos << std::endl;
	}
	if (state_right == GLFW_PRESS) {
		std::cout << "Mouse position is: x - " << xpos << ", y - " << ypos << std::endl;
	}
	prev_x = xpos;
	prev_y = ypos;
}


// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	std::cout << "Key " << (char)key << " is pressed." << std::endl;
	switch (key) {
		case '.':
			heirarchy_elems[current_select]->deselect();
			if (heirarchy_elems[current_select]->selected == false) {
				heirarchy_elems[current_select]->scale_factor.x -= 0.03;
				heirarchy_elems[current_select]->scale_factor.y -= 0.03;
				heirarchy_elems[current_select]->scale_factor.z -= 0.03;
			}
			if (current_select + 1 == heirarchy_elems.size()) {
				current_select = 0;
			} else {
				current_select++;
			}
			heirarchy_elems[current_select]->select();

			if (heirarchy_elems[current_select]->selected == true) {
				heirarchy_elems[current_select]->scale_factor.x += 0.03;
				heirarchy_elems[current_select]->scale_factor.y += 0.03;
				heirarchy_elems[current_select]->scale_factor.z += 0.03;
			}
			
			break;
		case ',':
			heirarchy_elems[current_select]->deselect();
			if (heirarchy_elems[current_select]->selected == false) {
				heirarchy_elems[current_select]->scale_factor.x -= 0.03;
				heirarchy_elems[current_select]->scale_factor.y -= 0.03;
				heirarchy_elems[current_select]->scale_factor.z -= 0.03;
			}
			if (current_select == 0) {
				current_select = heirarchy_elems.size() - 1;
			} else {
				current_select--;
			}
			heirarchy_elems[current_select]->select();
			if (heirarchy_elems[current_select]->selected == true) {
				heirarchy_elems[current_select]->scale_factor.x += 0.03;
				heirarchy_elems[current_select]->scale_factor.y += 0.03;
				heirarchy_elems[current_select]->scale_factor.z += 0.03;
			}
			break;
		case 'x':
			heirarchy_elems[current_select]->joint_angles.x += 5;
			break;
		case 'X':
			heirarchy_elems[current_select]->joint_angles.x -= 5;
			break;

		case 'y':
			heirarchy_elems[current_select]->joint_angles.y += 5;
			break;
		case 'Y':
			heirarchy_elems[current_select]->joint_angles.y -= 5;
			break;

		case 'z':
			heirarchy_elems[current_select]->joint_angles.z += 5;
			break;
		case 'Z':
			heirarchy_elems[current_select]->joint_angles.z -= 5;
			break;
	}
}

void CreateCube()
{
	// x, y, z, r, g, b, ...
	float cubeVerts[] = {
		// Face x-
		-1.0f,	+1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		-1.0f,	-1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		// Face x+
		+1.0f,	+1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	-1.0f,	+1.0f,	0.8f,	0.2f,	0.2f,
		+1.0f,	-1.0f,	-1.0f,	0.8f,	0.2f,	0.2f,
		// Face y-
		+1.0f,	-1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	-1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	-1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		// Face y+
		+1.0f,	+1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.8f,	0.2f,
		+1.0f,	+1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		-1.0f,	+1.0f,	-1.0f,	0.2f,	0.8f,	0.2f,
		// Face z-
		+1.0f,	+1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	-1.0f,	-1.0f,	0.2f,	0.2f,	0.8f,
		// Face z+
		+1.0f,	+1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		+1.0f,	-1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	+1.0f,	+1.0f,	0.2f,	0.2f,	0.8f,
		-1.0f,	-1.0f,	+1.0f,	0.2f,	0.2f,	0.8f
	};

	GLuint vertBufferID;
	glGenBuffers(1, &vertBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	GLint posID = glGetAttribLocation(program.GetPID(), "position");
	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	GLint colID = glGetAttribLocation(program.GetPID(), "color");
	glEnableVertexAttribArray(colID);
	glVertexAttribPointer(colID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

}

void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment2 - Andrew Plant", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	program.SetShadersFileName(vertShaderPath, fragShaderPath);
	program.Init();

	CreateCube();
	ConstructRobot();
}


int main()
{	
	Init();
	while ( glfwWindowShouldClose(window) == 0) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}