#include <GL/glut.h>

// initial triangle //
GLfloat v[4][3] = { {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {-1.0, -0.5, 0.0}, {1.0, -0.5, 0.0} };
int n; // number of recursive steps //

void triangle(GLfloat* a, GLfloat* b, GLfloat* c) {
	// display one triangle //
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
}

void tetra(GLfloat* a, GLfloat* b, GLfloat* c, GLfloat* d) {
	glColor3f(1.0, 0.0, 0.0);
	triangle(a, b, c);
	glColor3f(0.0, 1.0, 0.0);
	triangle(a, c, d);
	glColor3f(0.0, 0.0, 1.0);
	triangle(a, d, b);
	glColor3f(0.0, 0.0, 0.0);
	triangle(b, d, c);
}

void divide_tetra(GLfloat* a, GLfloat* b, GLfloat* c, GLfloat* d, int m) {
	// triangle subdividion using vertex numbers //
	GLfloat mid[6][3];
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glBegin(GL_TRIANGLES);

	divide_tetra(v[0], v[1], v[2], v[3], n);
	
	glEnd();
	glFlush();
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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
	n = 3;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("3D Gasket");
	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);

	init();

	glutMainLoop();
	return 0;
}
