#include <GL/glut.h>

// Initialization function
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Set background color to black
    glColor3f(1.0, 1.0, 1.0);         // Set drawing color to white
    glMatrixMode(GL_PROJECTION);       // Set projection matrix mode
    glLoadIdentity();                  // Load identity matrix
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Set orthographic projection
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    glBegin(GL_QUADS);            // Start drawing a square
    glVertex2f(-0.5, -0.5);   // Bottom left corner
    glVertex2f(0.5, -0.5);    // Bottom right corner
    glVertex2f(0.5, 0.5);     // Top right corner
    glVertex2f(-0.5, 0.5);    // Top left corner
    glEnd();

    glFlush(); // Render now
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // Single buffering and RGB color
    glutInitWindowSize(500, 500);                // Set window size
    glutInitWindowPosition(100, 100);            // Set window position
    glutCreateWindow("OpenGL Initialization Example"); // Create window

    init();                    // Initialize the OpenGL settings
    glutDisplayFunc(display);  // Register display callback function

    glutMainLoop();            // Enter the main loop
    return 0;
}
