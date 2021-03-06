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
#define DegToRad 0.0174533

// player variables (position, change in pos, angle, movement speed)
float px, py, pdx, pdy, pa, ps;

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

// int mapX=16, mapY=16, mapS=8;
// int map[] = {
//     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//     1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
//     1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1,
//     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
// };


float distance(float ax, float ay, float bx, float by, float angle) {
    return sqrt(pow((bx - ax), 2) + pow((by - ay), 2));
}

void drawMap2D() {
    int x, y, x0, y0;
    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            if (map[y * mapX + x] == 1) {
                glColor3f(0.5, 7, 0.5 ); // walls
            } else {
                glColor3f(0, 0.2, 0); // background
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

void drawWalls3D(float finalDist, int rayNum, int wallDetail, bool shade) {
    float lineHeight = mapS * 320 / finalDist;
    if (lineHeight > 320) {
        lineHeight = 320;
    }
    float lineOffset = 160 - lineHeight / 2;
    if (shade) glColor3f(0.2, 0.2, 0.9);
    else glColor3f(0.2, 0.2, 0.7);
    glLineWidth(8);
    glBegin(GL_LINES);
    glVertex2i(rayNum * wallDetail + 530, lineOffset);
    glVertex2i(rayNum * wallDetail + 530, lineHeight + lineOffset);
    glEnd();
}

void drawRays2D(int rayNum) {
    bool shade; // 0 for light, 1 for shade
    int r, mx, my, mp, dof; // depth-of-field, ray multiplier
    float rx, ry, ra, xo, yo, finalDist; // ray pos, ray angle, x-offset, y-offset
    ra = pa - DegToRad * 30;
    if (ra < 0) ra += 2 * Pi;
    if (ra > 2 * Pi) ra -= 2 * Pi;
    for (r = 0; r < rayNum; r++) {
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
            // glColor3f(1, 0, 0);
            // glLineWidth(2);
            // glBegin(GL_LINES);
            // glVertex2i(px, py);
            // glVertex2i(rx, ry);
            // glEnd();
        }
        // find the shorter of the two lines
        if (hDist > vDist) {
            rx = vx;
            ry = vy;
            finalDist = vDist;
            shade = true;
            glColor3f(1, 0.9, 0);
        } if (hDist < vDist) {
            rx = hx;
            ry = hy;
            finalDist = hDist;
            shade = false;
            glColor3f(1, 0.7, 0);
        }
        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();
        // remedy fisheye effect
        float ca = pa - ra;
        if (ca < 0) ca += 2 * Pi;
        if (ca > 2 * Pi) ca -= 2 * Pi; 
        finalDist *= cos(ca);
        // draw 3d walls
        drawWalls3D(finalDist, r, 1, shade);
        // increment next ray angle
        ra += DegToRad / (rayNum / 60);
        if (ra < 0) ra += 2 * Pi;
        if (ra > 2 * Pi) ra -= 2 * Pi; 
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    drawRays2D(960);
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
    glClearColor(0.4, 0.4, 0.4, 0);
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    px=300, py=300;
    ps = 1;
    pdx = cos(pa) * ps;
    pdy = sin(pa) * ps;
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