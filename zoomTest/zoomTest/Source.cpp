#include <GL/glut.h>

// Zoom factor
float zoomFactor = 1.0f;
int prevMouseY = 0; // To track the previous mouse Y position

// Function to draw the square
void drawSquare() {
    glBegin(GL_QUADS);
    glVertex2f(-0.5f * zoomFactor, -0.5f * zoomFactor);
    glVertex2f(0.5f * zoomFactor, -0.5f * zoomFactor);
    glVertex2f(0.5f * zoomFactor, 0.5f * zoomFactor);
    glVertex2f(-0.5f * zoomFactor, 0.5f * zoomFactor);
    glEnd();
}

// Display callback
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.5f, 0.8f); // Blue color for the square
    drawSquare();
    glutSwapBuffers();
}

// Mouse motion callback (for vertical movement detection)
void mouseMotion(int x, int y) {
    // Calculate the vertical movement
    int deltaY = y - prevMouseY;

    // Adjust the zoom factor based on vertical movement
    if (deltaY > 0) {
        zoomFactor -= 0.01f; // Zoom out
    }
    else if (deltaY < 0) {
        zoomFactor += 0.01f; // Zoom in
    }

    // Clamp zoomFactor to avoid flipping or extreme zoom
    if (zoomFactor < 0.1f) zoomFactor = 0.1f;

    // Update the previous mouse Y position
    prevMouseY = y;

    glutPostRedisplay();
}

// Mouse button callback
void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        prevMouseY = y; // Initialize prevMouseY when the left button is pressed
    }
}

// Reshape callback
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Zoom Object with Mouse Movement");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion); // Register motion callback
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
