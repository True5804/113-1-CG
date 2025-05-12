#include <GL/freeglut.h>

// initial triangle //
GLfloat v[4][3] = { {0, 0, 1}, {0, 1, 0}, {-1, -0.5, 0}, {1, -0.5, 0} };
int n; // number of recursive steps //

void triangle(GLfloat* a, GLfloat* b, GLfloat* c) {
	// display one triangle //
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
}

void divide_triangle(GLfloat* a, GLfloat* b, GLfloat* c, int m) {
	// triangle subdividion using vertex numbers //
	GLfloat v1[3], v2[3], v3[3];
	int j;

	if (m > 0) {
		for (j = 0; j < 3; j++) {
			v1[j] = (a[j] + b[j]) / 2;
			v2[j] = (a[j] + c[j]) / 2;
			v3[j] = (b[j] + c[j]) / 2;
		}

		divide_triangle(a, v1, v2, m - 1);
		divide_triangle(c, v2, v3, m - 1);
		divide_triangle(b, v3, v1, m - 1);
	}
	else {
		triangle(a, b, c);
		// draw triangle at end of recursion //
	}
}

void tetrahedron(int m) {
	glColor3f(1.0, 0.0, 0.0);
	divide_triangle(v[0], v[1], v[2], m);
	glColor3f(0.0, 1.0, 0.0);
	divide_triangle(v[3], v[2], v[1], m);
	glColor3f(0.0, 0.0, 1.0);
	divide_triangle(v[0], v[3], v[1], m);
	glColor3f(0.0, 0.0, 0.0);
	divide_triangle(v[0], v[2], v[3], m);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glBegin(GL_TRIANGLES);

	tetrahedron(n);

	glEnd();
	glFlush();  // show the output
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

void init() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
	n = 3;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("3D Gasket in surface");
	glutReshapeFunc(Reshape);

	glutDisplayFunc(display);

	init();

	glutMainLoop();

	return 0;
}