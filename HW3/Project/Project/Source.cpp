#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#define M_PI 3.1415

// initial triangle //
GLfloat v[4][3] = { {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {-1.0, -0.5, 0.0}, {1.0, -0.5, 0.0} };
int n = 0; // number of recursive steps //
char fixedAxis = 'x'; // Default fixed axis for rotation
int rotationDirection = 0; // 0 = stop, 1 = clockwise, -1 = counterclockwise
GLfloat zoomFactor = 1.0f; // Zoom level

// Add to existing variables:
int lastMousePosX = 0, lastMousePosY = 0; // Previous mouse positions
bool leftButtonPressed = false;
bool leftRightButtonPressed = false;

// Rotation angles
GLfloat cumulativeRotationX = 0.0f;
GLfloat cumulativeRotationY = 0.0f;
GLfloat cumulativeRotationZ = 0.0f;

// Camera spherical coordinates
GLfloat r = 5.0f; // Distance from the origin
GLfloat theta = M_PI / 4.0f;; // Angle from the Z-axis (in radians)
GLfloat phi = M_PI / 4.0f;; // Angle from the X-axis in the XY-plane (in radians)


void triangle(GLfloat* a, GLfloat* b, GLfloat* c) {
	// display one triangle //
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
}

// Function to draw a tetrahedron using four triangles with a different color
void tetra(GLfloat* a, GLfloat* b, GLfloat* c, GLfloat* d) {
	glColor3f(1.0, 0.753, 0.796); //Pink
	triangle(a, b, c);
	glColor3f(0.486, 0.988, 0.0); //grass green
	triangle(a, c, d);
	glColor3f(0.678, 0.847, 0.902); //light blue
	triangle(a, d, b);
	glColor3f(1.0, 0.647, 0.0); //Orange
	triangle(b, d, c);
}

// Recursive function to subdivide the tetrahedron
void divide_tetra(GLfloat* a, GLfloat* b, GLfloat* c, GLfloat* d, int m) {
	// triangle subdividion using vertex numbers //
	GLfloat mid[6][3]; // Array to store midpoints of edges
	int j;

	if (m > 0) {
		for (j = 0; j < 3; j++) {
			mid[0][j] = (a[j] + b[j]) / 2;
			mid[1][j] = (a[j] + c[j]) / 2;
			mid[2][j] = (a[j] + d[j]) / 2;
			mid[3][j] = (b[j] + c[j]) / 2;
			mid[4][j] = (c[j] + d[j]) / 2;
			mid[5][j] = (b[j] + d[j]) / 2;
		}

		divide_tetra(a, mid[0], mid[1], mid[2], m - 1);
		divide_tetra(mid[0], b, mid[3], mid[5], m - 1);
		divide_tetra(mid[1], mid[3], c, mid[4], m - 1);
		divide_tetra(mid[2], mid[5], mid[4], d, m - 1);
	}
	else {
		tetra(a, b, c, d);
		// draw triangle at end of recursion //
	}
}

// Function to update camera position in spherical coordinates
void updateCamera() {
	// Convert spherical to Cartesian coordinates
	GLfloat eyeX = r * sin(theta) * cos(phi);
	GLfloat eyeY = r * sin(theta) * sin(phi);
	GLfloat eyeZ = r * cos(theta);

	// Set the camera view
	gluLookAt(eyeX, eyeY, eyeZ, // Camera position
		0.0f, 0.0f, 0.0f, // Look-at point (origin)
		0.0f, 1.0f, 0.0f); // Up vector
}

void rotateTetrahedron() {
	if (rotationDirection != 0) {
		// Increment the cumulative rotation for the fixed axis
		switch (fixedAxis) {
		case 'x':
			cumulativeRotationX += rotationDirection * 0.05f;
			break;
		case 'y':
			cumulativeRotationY += rotationDirection * 0.05f;
			break;
		case 'z':
			cumulativeRotationZ += rotationDirection * 0.05f;
			break;
		}
		glutPostRedisplay();
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // Reset before applying transformations

	// Update the camera position
	updateCamera();

	// Apply zoomFactor to scale the entire scene
	glScalef(zoomFactor, zoomFactor, zoomFactor);

	glPushMatrix();
	// Apply cumulative rotation based on the fixed axis
	glRotatef(cumulativeRotationX, 1.0f, 0.0f, 0.0f); // X-axis rotation
	glRotatef(cumulativeRotationY, 0.0f, 1.0f, 0.0f); // Y-axis rotation
	glRotatef(cumulativeRotationZ, 0.0f, 0.0f, 1.0f); // Z-axis rotation

	glBegin(GL_TRIANGLES);
	divide_tetra(v[0], v[1], v[2], v[3], n);
	glEnd();

	glPopMatrix();
	glutSwapBuffers();
}

// Mouse motion function
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		leftButtonPressed = true;
	}

	lastMousePosX = x;
	lastMousePosY = y;
}

// Function to handle mouse movement while dragging
void motion(int x, int y) {
	int deltaX = x - lastMousePosX;
	int deltaY = y - lastMousePosY;

	if (leftButtonPressed) {
		// Adjust theta and phi based on mouse movement
		theta -= deltaY * 0.005f; // Vertical movement adjusts theta
		phi -= deltaX * 0.005f;   // Horizontal movement adjusts phi

		// Clamp theta to avoid flipping
		if (theta < 0.01f) theta = 0.01f;
		if (theta > M_PI - 0.01f) theta = M_PI - 0.01f;
	}

	lastMousePosX = x;
	lastMousePosY = y;

	glutPostRedisplay(); // Redraw the scene
}

void mouseWheel(int wheel, int direction, int x, int y) {
	// Adjust zoom factor based on wheel scroll direction
	if (direction > 0) {
		zoomFactor += 0.05f; // Scroll up: Zoom in
	}
	else if (direction < 0) {
		zoomFactor -= 0.05f; // Scroll down: Zoom out
	}

	// Clamp zoom factor to avoid extreme zooming
	if (zoomFactor < 0.1f) zoomFactor = 0.1f; // Minimum zoom
	if (zoomFactor > 10.0f) zoomFactor = 10.0f; // Maximum zoom

	glutPostRedisplay(); // Redraw the scene
}

void main_menu(int id) {
	if (id == 1) {
		exit(0);
	}
}

void sub(int id) {
	switch (id) {
	case 1:
		n = 0;
		break;
	case 2:
		n = 1;
		break;
	case 3:
		n = 2;
		break;
	case 4:
		n = 3;
		break;
	default:
		n = 0;
		break;
	}

	// display()
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glBegin(GL_TRIANGLES);

	divide_tetra(v[0], v[1], v[2], v[3], n);

	glEnd();
	glFlush();
}

void direct(int id) {
	switch (id) {
	case 1: // Stop
		rotationDirection = 0; // Stop automatic rotation
		glutIdleFunc(NULL); // Disable idle function
		break;
	case 2: // Clockwise
		rotationDirection = 1; // Enable clockwise rotation
		glutIdleFunc(rotateTetrahedron); // Enable idle function
		break;
	case 3: // Counter-Clockwise
		rotationDirection = -1; // Enable counter-clockwise rotation
		glutIdleFunc(rotateTetrahedron); // Enable idle function
		break;
	default:
		rotationDirection = 0; // Stop automatic rotation
		glutIdleFunc(NULL); // Disable idle function
		break;
	}
}

// Submenu for rotation axis
void ax(int id) {
	switch (id) {
	case 1: // Fix rotation on X-axis
		fixedAxis = 'x';
		break;
	case 2: // Fix rotation on Y-axis
		fixedAxis = 'y';
		break;
	case 3: // Fix rotation on Z-axis
		fixedAxis = 'z';
		break;
	}
	glutIdleFunc(NULL); // Disable automatic rotation
	glutPostRedisplay(); // Redraw the scene
}

void createMenu() {
	int level = glutCreateMenu(sub);
	glutAddMenuEntry("0", 1);
	glutAddMenuEntry("1", 2);
	glutAddMenuEntry("2", 3);
	glutAddMenuEntry("3", 4);

	int axis = glutCreateMenu(ax);
	glutAddMenuEntry("X", 1);
	glutAddMenuEntry("Y", 2);
	glutAddMenuEntry("Z", 3);

	int dir = glutCreateMenu(direct);
	glutAddMenuEntry("Stop", 1);
	glutAddMenuEntry("Clockwise", 2);
	glutAddMenuEntry("Counter-Clockwise", 3);

	int rotate = glutCreateMenu(NULL);
	glutAddSubMenu("Axis", axis);
	glutAddSubMenu("Direction", dir);

	int menu = glutCreateMenu(main_menu);
	glutAddSubMenu("Subdivision Level", level);
	glutAddSubMenu("Rotation", rotate);
	glutAddMenuEntry("Exit", 1);

	glutDetachMenu(GLUT_LEFT_BUTTON);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h) {
		glOrtho(-2, 2, -2 * (GLfloat)h / (GLfloat)w, 2 * (GLfloat)h / (GLfloat)w, -10, 10);
	}
	else {
		glOrtho(-2 * (GLfloat)w / (GLfloat)h, 2 * (GLfloat)w / (GLfloat)h, -2, 2, -10, 10);
	}

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void key(unsigned char k, int x, int y) { // keyboard trigger
	switch (k) {
	case 'Q':
		exit(0);
		break;
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
}

void init() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set the color of background in white 
	glEnable(GL_DEPTH_TEST); // Enable depth testing
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("S11159020");

	createMenu();

	glutKeyboardFunc(key);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);   // Handle mouse motion while dragging
	glutMouseWheelFunc(mouseWheel);

	init();

	glutMainLoop();
	return 0;
}