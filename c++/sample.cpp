#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <limits>
#include <iostream>

// define constants
#define Pi 3.1415926535
#define Pi2 Pi / 2
#define Pi3 3 * Pi / 2

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
            } else {
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

// void logRays(int r, int mx, int my, int mp, int dof,
//     float rx, float ry, float ra, float xo, float yo) {
//     std::cout
// }

float distance(float ax, float ay, float bx, float by, float angle) {
    return sqrt(pow((bx - ax), 2) + pow((by - ay), 2));
}

void drawRays3D() {
    int r, mx, my, mp, dof; // depth-of-field
    float rx, ry, ra, xo, yo; // ray pos, ray angle, x-offset, y-offset
    ra=pa;
    for (r = 0; r < 1; r++) {
        /* Horizontal line check */
        dof = 0;
        float aTan = -1/tan(ra);
        float hDist = std::numeric_limits<float>::max();
        float hx = px, hy = py;
        if (ra > Pi) { // ray facing down
            ry = (((int)py >> 6 ) << 6) - 0.0001;
            rx = (py - ry) * aTan + px;
            yo = -64;
            xo = -yo * aTan;
        }
        if (ra < Pi) { // ray facing down
            ry = (((int)py >> 6 ) << 6) + 64;
            rx = (py - ry) * aTan + px;
            yo = 64;
            xo = -yo * aTan;
        }
        if (ra == 0 || ra == Pi) { // ray facing left/right
            rx = px;
            ry = py;
            dof = 8;
        }
        while(dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            // hit object
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                dof = 8;
                hx = rx;
                hy = ry;
                hDist = distance(px, py, hx, hy, ra);
            } else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
            // draw example ray
            // glColor3f(1, 0, 0);
            // glLineWidth(4);
            // glBegin(GL_LINES);
            // glVertex2i(px, py);
            // glVertex2i(rx, ry);
            // glEnd();
        }

        /* Vertical line check */
        dof = 0;
        float nTan = -tan(ra);
        float vDist = std::numeric_limits<float>::max();
        float vx = px, vy = py;
        if (ra > Pi2 && ra < Pi3) { // ray facing left
            rx = (((int)px >> 6 ) << 6) - 0.0001;
            ry = (px - rx) * nTan + py;
            xo = -64;
            yo = -xo * nTan;
        }
        if (ra < Pi2 || ra > Pi3) { // ray facing right
            rx = (((int)px >> 6 ) << 6) + 64;
            ry = (px - rx) * nTan + py;
            xo = 64;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == Pi) { // ray facing up/down
            rx = px;
            ry = py;
            dof = 8;
        }
        while(dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            // hit object
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                dof = 8;
                vx = rx;
                vy = ry;
                vDist = distance(px, py, vx, vy, ra);
            } else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
            // draw example ray
            glColor3f(1, 0, 0);
            glLineWidth(2);
            glBegin(GL_LINES);
            glVertex2i(px, py);
            glVertex2i(rx, ry);
            glEnd();

            // find the shorter of the two lines
            if (hDist > vDist) {
                rx = vx;
                ry = vy;
            } if (hDist < vDist) {
                rx = hx;
                ry = hy;
            }
            std::cout << hDist << ", " << vDist << "\n";
            // glColor3f(0, 1, 0);
            // glLineWidth(2);
            // glBegin(GL_LINES);
            // glVertex2i(px, py);
            // glVertex2i(rx, ry);
            // glEnd();
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    drawRays3D();
    glutSwapBuffers();
}

void updateKeyStates() {
    int mvmtSpd = 1;
    if (keys[5]) mvmtSpd = 2; // l (run)
    if (keys[1]) { // w
        px += mvmtSpd * pdx;
        py += mvmtSpd * pdy;
    }
    if (keys[2]) { // s
        px -= mvmtSpd * pdx;
        py -= mvmtSpd * pdy;
    } 
    if (keys[3]) { // a
        pa -= 0.1;
        if (pa < 0) {
            pa += 2 * Pi;
        }
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    };
    if (keys[4]) { // d
        pa += 0.1;
        if (pa > 2 * Pi) {
            pa -= 2 * Pi;
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