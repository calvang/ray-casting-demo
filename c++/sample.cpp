#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

// define constants
#define PI 3.14159265358979

// player variables (position, change in pos, angle)
float px, py, pdx, pdy, pa;

// keys for multicontols
bool keys[10];

void drawPlayer() {
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px, py);
    glEnd();

    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(px + pdx * 5, py + pdy * 5);
    glEnd();
}

int mapX=8, mapY=8, mapS=64;
int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1,
    1, 1, 1, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

void drawMap2D() {
    int x, y, x0, y0;
    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            if (map[y * mapX + x] == 1) {
                glColor3f(1, 1, 1);
            }
            else {
                glColor3f(0, 0, 0);
            }
            x0 = x * mapS;
            y0 = y * mapS;
            glBegin(GL_QUADS);
            glVertex2i(x0 + 1, y0);
            glVertex2i(x0 + 1, y0 + mapS - 1);
            glVertex2i(x0 + mapS - 1, y0 + mapS - 1);
            glVertex2i(x0 + mapS - 1, y0);
            glEnd();
        }
    }
}

void drawRays3D() {
    
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    glutSwapBuffers();
}

void updateKeyStates() {
    int mvmtSpd = 5;
    if (keys[5]) mvmtSpd = 10; // l (run)
    if (keys[1]) { // w
        px += pdx;
        py += pdy;
    }
    if (keys[2]) { // s
        px -= pdx;
        py -= pdy;
    } 
    if (keys[3]) { // a
        pa -= 0.1;
        if (pa < 0) {
            pa += 2 * PI;
        }
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    };
    if (keys[4]) { // d
        pa += 0.1;
        if (pa > 2 * PI) {
            pa -= 2 * PI;
        }
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    };
}

void keyDown(unsigned char key, int x, int y) {
    switch(key) {
        case 'w':
            keys[1] = true;
            break;
        case 's':
            keys[2] = true;
            break;
        case 'a':
            keys[3] = true;
            break;
        case 'd':
            keys[4] = true;
            break;
        case 'l':
            keys[5] = true;
            break;
    }
    updateKeyStates();
    glutPostRedisplay();
}

void keyUp(unsigned char key, int x, int y) {
    switch(key) {
        case 'w':
            keys[1] = false;
            break;
        case 's':
            keys[2] = false;
            break;
        case 'a':
            keys[3] = false;
            break;
        case 'd':
            keys[4] = false;
            break;
        case 'l':
            keys[5] = false;
            break;
    }
    updateKeyStates();
    glutPostRedisplay();
}

void idle() {
    updateKeyStates();
    glutPostRedisplay();
}

void initDisplay() {
    int WIDTH = 1024, HEIGHT = 512;
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    px=300, py=300;
    pdx = cos(pa) * 5;
    pdy = sin(pa) * 5;
}

int main(int argc, char* argv[]) {
    int WIDTH = 1024, HEIGHT = 512;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Sample Ray Caster");
    
    initDisplay();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}