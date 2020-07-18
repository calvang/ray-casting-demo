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

// application window dimensions
const int WIDTH = 1024, HEIGHT = 512;

/* Draw player and directional indicator */
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

// X and Y grid sizes and individual blocksize
int mapX=16, mapY=16, mapS=8;
// Map grid: 1=obstacle, 0=free space
int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1,
    1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* Calculate distance */
float distance(float ax, float ay, float bx, float by, float angle) {
    return sqrt(pow((bx - ax), 2) + pow((by - ay), 2));
}

/* Draw 2D Demo Sidebar */
void drawBar2D() {
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);

    glVertex2i(mapS * mapX, 0);
    glVertex2i(mapS * mapX, HEIGHT);
    glVertex2i(0, HEIGHT);
    glEnd();
}

/* Draw 2D Map Grid for Preview */
void drawMap2D() {
    int x, y, x0, y0;
    float gap = 0.2;
    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            if (map[y * mapX + x] == 1) {
                glColor3f(0.5, 7, 0.5);
            } else {
                glColor3f(0, 0.2, 0);
            }
            x0 = x * mapS;
            y0 = y * mapS;
            glBegin(GL_QUADS);
            glVertex2i(x0 + gap, y0);
            glVertex2i(x0 + gap, y0 + mapS - gap);
            glVertex2i(x0 + mapS - gap, y0 + mapS - gap);
            glVertex2i(x0 + mapS - gap, y0);
            glEnd();
        }
    }
}

/* Render walls using ray casted values */
void drawWalls3D(float finalDist, int rayNum, int wallDetail, bool shade) {
    float lineHeight = mapS * HEIGHT / finalDist;
    if (lineHeight > HEIGHT) {
        lineHeight = HEIGHT;
    }
    float lineOffset = HEIGHT / 2 - lineHeight / 2;
    if (shade) glColor3f(0.2, 0.2, 0.9);
    else glColor3f(0.1, 0.1, 0.5);
    glLineWidth(8);
    glBegin(GL_LINES);
    glVertex2i(rayNum * wallDetail + 129 /*arbitrary alignment val */, lineOffset);
    glVertex2i(rayNum * wallDetail + 129, lineHeight + lineOffset);
    glEnd();
}

/* Cast rays and render objects */
void drawRays2D(int rayNum) {
    bool shade; // 0 for light, 1 for shade
    int r, mx, my, mp, dof; // depth-of-field, ray multiplier
    float rx, ry, ra, xo, yo, finalDist; // ray pos, ray angle, x-offset, y-offset
    int b2Shifter = (int) log2(mapS); // determine B2 val for rounding to nearest block
    // set ray angles
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
            ry = (((int)py >> b2Shifter ) << b2Shifter) - 0.0001;
            rx = (py - ry) * aTan + px;
            yo = -mapS;
            xo = -yo * aTan;
        }
        if (ra < Pi) { // ray facing up
            ry = (((int)py >> b2Shifter) << b2Shifter) + mapS;
            rx = (py - ry) * aTan + px;
            yo = mapS;
            xo = -yo * aTan;
        } 
        if (ra == 0 || ra == Pi) { // ray facing left/right
            rx = px;
            ry = py;
            dof = mapY;
        }
        // dof compared to mapY so enough iterations are done to hit the furthest possible wall
        while(dof < mapY) {
            mx = (int)(rx) >> 3;
            my = (int)(ry) >> 3;
            mp = my * mapX + mx;
            // hit object
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                dof = mapY;
                hx = rx;
                hy = ry;
                hDist = distance(px, py, hx, hy, ra);
            } else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        /* Vertical line check */
        dof = 0;
        float nTan = -tan(ra);
        float vDist = std::numeric_limits<float>::max();
        float vx = px, vy = py;
        if (ra > Pi2 && ra < Pi3) { // ray facing left
            rx = (((int)px >> b2Shifter) << b2Shifter) - 0.0001;
            ry = (px - rx) * nTan + py;
            xo = -mapS;
            yo = -xo * nTan;
        }
        if (ra < Pi2 || ra > Pi3) { // ray facing right
            rx = (((int)px >> b2Shifter) << b2Shifter) + mapS;
            ry = (px - rx) * nTan + py;
            xo = mapS;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == Pi) { // ray facing up/down
            rx = px;
            ry = py;
            dof = mapX;
        }
        // dof compared to mapY so enough iterations are done to hit the furthest possible wall
        while(dof < mapX) {
            mx = (int)(rx) >> 3;
            my = (int)(ry) >> 3;
            mp = my * mapX + mx;
            // hit object
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                dof = mapX;
                vx = rx;
                vy = ry;
                vDist = distance(px, py, vx, vy, ra);
            } else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
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
        glLineWidth(10);
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
        ra += DegToRad / (rayNum / 70);
        if (ra < 0) ra += 2 * Pi;
        if (ra > 2 * Pi) ra -= 2 * Pi; 
    }
}

/* Render all display elements */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBar2D();
    drawMap2D();
    drawPlayer();
    drawRays2D(910);
    glutSwapBuffers();
}

/* Updates backend variables based on key states */
void updateKeyStates() {
    float mvmtSpd = 0.1; 
    if (keys[5]) mvmtSpd *= 2; // l (run)
    if (keys[1]) { // w
        px += mvmtSpd * pdx;
        py += mvmtSpd * pdy;
    }
    if (keys[2]) { // s
        px -= mvmtSpd * pdx;
        py -= mvmtSpd * pdy;
    } 
    if (keys[3]) { // a
        pa -= 0.4 * mvmtSpd;
        if (pa < 0) {
            pa += 2 * Pi;
        }
        pdx = cos(pa) * ps;
        pdy = sin(pa) * ps;
    };
    if (keys[4]) { // d
        pa += 0.4 * mvmtSpd;
        if (pa > 2 * Pi) {
            pa -= 2 * Pi;
        }
        pdx = cos(pa) * ps;
        pdy = sin(pa) * ps;
    };
}

/* Handle key holds */
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

/* Handle key releases */
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

/* Maintain variable updates in idle */
void idle() {
    updateKeyStates();
    glutPostRedisplay();
}

/* Initialize display background and variables */
void initDisplay() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    // starting player position
    px=2, py=12;
    ps = 4;
    pdx = cos(pa) * ps;
    pdy = sin(pa) * ps;
}

int main(int argc, char* argv[]) {
    int WIDTH = 1024, HEIGHT = 512;
    // Initialize application
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Sample Ray Caster");
    // Loop controls and graphics
    initDisplay();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}