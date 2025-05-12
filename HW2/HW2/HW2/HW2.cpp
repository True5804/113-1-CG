#include <GL/glut.h>

// initial triangle //
GLfloat v[4][3] = { {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {-1.0, -0.5, 0.0}, {1.0, -0.5, 0.0} };
int n = 0; // number of recursive steps //


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

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen and depth buffer
	glutSwapBuffers();

	glBegin(GL_TRIANGLES);

	divide_tetra(v[0], v[1], v[2], v[3], n);

	glEnd();
	glFlush();
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

void createMenu() {
	int submenu = glutCreateMenu(sub);
	glutAddMenuEntry("0", 1);
	glutAddMenuEntry("1", 2);
	glutAddMenuEntry("2", 3);
	glutAddMenuEntry("3", 4);

	int menu = glutCreateMenu(main_menu);
	glutAddSubMenu("Subdivision Level", submenu);
	glutAddMenuEntry("Exit", 1);

	glutAttachMenu(GLUT_RIGHT_BUTTON); // trigger the menu
	glutDetachMenu(GLUT_LEFT_BUTTON); // leave the menu
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
	glClearColor(1.0, 1.0, 1.0, 1.0); // set the color of background in white 
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("S11159020");

	createMenu();

	glutKeyboardFunc(key);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);
	
	init();

	glutMainLoop();
	return 0;
}