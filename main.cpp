#include <iostream>
#include <GL/freeglut.h>
#include <cmath>
#include "FreeImage.h"
#include <vector>

GLuint g_textureID;

float cameraX = 0.0f;
float cameraY = 1.0f;
float cameraZ = 5.0f;

float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
int lastMouseX = -1;
int lastMouseY = -1;

float wallColor[3] = {1.0f, 1.0f, 1.0f};

int windowWidth;
int windowHeight;
std::vector<GLuint> g_textureIDs;

void loadTextures() {
    const char* textureNames[] = {"hali.png", "televizyon.png", "pcontum.png", "grass.png", "lake.jpeg", "tavan.png"};
    const FREE_IMAGE_FORMAT formats[] = {FIF_PNG, FIF_PNG, FIF_PNG, FIF_PNG, FIF_JPEG, FIF_PNG};
    for (int i = 0; i < sizeof(textureNames) / sizeof(textureNames[0]); ++i) {
        const char* textName = textureNames[i];
        FIBITMAP* dib = FreeImage_Load(formats[i], textName);
        if (dib == nullptr) {
            std::cerr << "Failed to load texture: " << textName << std::endl;
            continue;
        }
        
        dib = FreeImage_ConvertTo32Bits(dib);
        if (dib == nullptr) {
            std::cerr << "Failed to convert texture to 32 bits: " << textName << std::endl;
            FreeImage_Unload(dib);
            continue;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        BYTE* bits = new BYTE[FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib) * 4];
        BYTE* pixels = (BYTE*)FreeImage_GetBits(dib);
        for (int pix = 0; pix < FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib); pix++) {
            bits[pix * 4 + 0] = pixels[pix * 4 + 2]; // Red
            bits[pix * 4 + 1] = pixels[pix * 4 + 1]; // Green
            bits[pix * 4 + 2] = pixels[pix * 4 + 0]; // Blue
            bits[pix * 4 + 3] = pixels[pix * 4 + 3]; // Alpha (transparency)
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
        std::cout << textName << " OK." << std::endl;
        delete[] bits;

        // Texture ID'sini vektöre ekle
        g_textureIDs.push_back(textureID);

        // Doku işlemleri bittikten sonra FIBITMAP'i temizle
        FreeImage_Unload(dib);
    }
}

void drawLake(float lakeWidth, float lakeLength, float lakeX, float lakeY, float lakeZ) {
    glColor3f(1.0f, 1.0f, 1.0f);

    float lakeSurfaceY = 0.1f;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[4]); // lake.jpeg için uygun texture ID'si

    glBegin(GL_QUADS);
    for (float x = -lakeWidth / 2.0f + lakeX; x < lakeWidth / 2.0f + lakeX; x += 1.0f) {
        for (float z = -lakeLength / 2.0f + lakeZ; z < lakeLength / 2.0f + lakeZ; z += 1.0f) {
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(x, lakeSurfaceY, z);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(x + 1.0f, lakeSurfaceY, z);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(x + 1.0f, lakeSurfaceY, z + 1.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(x, lakeSurfaceY, z + 1.0f);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawTree(float x, float z) {
    glPushMatrix();
    glColor3f(0.5f, 0.35f, 0.05f); // Kahverengi
    glTranslatef(x, 0.0f, z);
    // Gövde (silindir)
    glPushMatrix();
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    GLUquadricObj *obj = gluNewQuadric();
    gluCylinder(obj, 0.1f, 0.1f, 2.0f, 20, 20);
    glPopMatrix();
    // Yapraklar (küre)
    glColor3f(0.0f, 0.5f, 0.0f); // Yeşil
    glTranslatef(0.0f, 2.0f, 0.0f);
    glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();
}

void drawGround() {
    // Yer rengini beyaz olarak belirleyelim
    glColor3f(1.0f, 1.0f, 1.0f); // Yer rengi (beyaz)

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[3]); // grass.png için uygun texture ID'si
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glTexCoord2f(100.0f, 0.0f); 
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glTexCoord2f(100.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glTexCoord2f(0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void drawSky() {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(36.0f / 255.0f, 137.0f / 255.0f, 207.0f / 255.0f); // Gökyüzü rengi (mavi)
    glTranslatef(0.0f, 100.0f, 0.0f);
    glutSolidSphere(300.0f, 20, 20);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawHome(float ev_genislik, float ev_uzunluk, float ev_yukseklik, float ev_x, float ev_y, float ev_z, float duvar_kalinlik) {
    glDisable(GL_LIGHTING);
    glPushMatrix();

    // Ön iç duvar
    glColor3fv(wallColor);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[1]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    // Arka balkon duvarı
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-ev_genislik / 2.0f + ev_x + 1.0f, ev_y, ev_uzunluk / 2.0f + ev_z); // Arka balkon duvarını sola kaydır
    glVertex3f(-ev_genislik / 2.0f + ev_x + 1.0f, ev_y + ev_yukseklik - 1.9f, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik - 1.9f, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glEnd();

    // Arka balkon sağ direği
    glColor3f(0.9f, 0.9f, 0.9f); // Normal duvar rengi
    glBegin(GL_QUADS);
    glVertex3f(2.3f, 0.2f, 2.5f); // Sol üst
    glVertex3f(2.3f, 2.2f, 2.5f); // Sol alt
    glVertex3f(2.5f, 2.2f, 2.5f); // Sağ alt
    glVertex3f(2.5f, 0.2f, 2.5f); // Sağ üst
    glEnd();

    // Arka balkon sol direği
    glColor3f(0.9f, 0.9f, 0.9f); // Normal duvar rengi
    glBegin(GL_QUADS);
    glVertex3f(-2.4f, 0.2f, 2.5f); // Sol üst
    glVertex3f(-2.4f, 2.2f, 2.5f); // Sol alt
    glVertex3f(-2.5f, 2.2f, 2.5f); // Sağ alt
    glVertex3f(-2.5f, 0.2f, 2.5f); // Sağ üst
    glEnd();

    // Arka balkon üst duvarı
    glColor3f(0.9f, 0.9f, 0.9f); // Normal duvar rengi
    glBegin(GL_QUADS);
    glVertex3f(-2.5f, 2.1f, 2.5f); // Sol üst
    glVertex3f(-2.5f, 2.2f, 2.5f); // Sol alt
    glVertex3f(2.5f, 2.2f, 2.5f); // Sağ alt
    glVertex3f(2.5f, 2.1f, 2.5f); // Sağ üst
    glEnd();

    // Arka balkon direği
    glColor3f(0.9f, 0.9f, 0.9f); // Normal duvar rengi
    glBegin(GL_QUADS);
    glVertex3f(-1.5f, 0.2f, 2.5f); // Sol üst
    glVertex3f(-1.5f, 2.2f, 2.5f); // Sol alt
    glVertex3f(-1.3f, 2.2f, 2.5f); // Sağ alt
    glVertex3f(-1.3f, 0.2f, 2.5f); // Sağ üst
    glEnd();

    // Sol duvar
    glColor3f(1.0f, 1.0f, 1.0f); // Tam beyaz renk
    glBegin(GL_QUADS);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    // Sağ duvar
    glColor3fv(wallColor);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[2]);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    // Tavan
    glColor3fv(wallColor);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[5]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);

    // Zemin
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[0]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Resimi düzelt
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void setupLights() {
    GLfloat lightPosition[] = { 0.0f, 10.0f, 0.0f, 1.0f }; 
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    GLfloat lightAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // Hafif ambiyans ekle
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Daha güçlü yayılan ışık
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Yansıyan ışık güçlendirildi

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    GLfloat globalAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(cameraPitch, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraYaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    setupLights();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glShadeModel(GL_SMOOTH);

    drawSky();
    drawGround();
    drawTree(-3.0f, -3.0f);
    drawTree(10.0f, 10.0f);
    drawTree(-15.0f, -15.0f);
    drawTree(6.0f, 6.0f);
    drawHome(5.0f, 5.0f, 2.0f, 0.0f, 0.2f, 0.0f, 0.1f);
    drawLake(8.0f, 5.0f, 0.0f, -1.5f, 7.0f);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (GLfloat)w / (GLfloat)h, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void ileri() {
    cameraX += 0.1f * sin(cameraYaw * M_PI / 180.0f);
    cameraZ -= 0.1f * cos(cameraYaw * M_PI / 180.0f);
}
void geri() {
    cameraX -= 0.1f * sin(cameraYaw * M_PI / 180.0f);
    cameraZ += 0.1f * cos(cameraYaw * M_PI / 180.0f);
}
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'w':
        ileri();
        break;
    case 's':
        geri();
        break;
    case 'a':
        cameraYaw -= 2.0f;
        break;
    case 'd':
        cameraYaw += 2.0f;
        break;
    case 'z':
        cameraY += 0.1f;
        break;
    case 'x':
        cameraY -= 0.1f;
        break;
    }
    glutPostRedisplay();
}

void fareTekerlegi(int button, int dir, int x, int y) {
    if (dir > 0) {
        ileri();
    } else {
        geri();
    }
    glutPostRedisplay();
}

void mouseMove(int x, int y) {
    windowWidth = glutGet(GLUT_SCREEN_WIDTH);
    windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
    if (x < 0) x = 0;
    if (x >= windowWidth) x = windowWidth - 1;
    if (y < 0) y = 0;
    if (y >= windowHeight) y = windowHeight - 1;
    float deltaXAngle = 360.0f * ((float)(x - lastMouseX) / windowWidth);
    deltaXAngle = deltaXAngle * 2;
    float deltaYAngle = 360.0f * ((float)(y - lastMouseY) / windowHeight);

    if (abs(deltaXAngle) > abs(deltaYAngle)) {
        cameraYaw += deltaXAngle;
        if (cameraYaw > 360.0f) cameraYaw -= 360.0f;
        else if (cameraYaw < 0.0f) cameraYaw += 360.0f;
    } else {
        cameraPitch += deltaYAngle; 
        if (cameraPitch > 90.0f) cameraPitch = 90.0f; // Yatayda tam dik açıya kadar döndür
        else if (cameraPitch < -90.0f) cameraPitch = -90.0f; // Yatayda tam dik açıya kadar döndür
    }
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Pcontum 3D");
    glutFullScreen();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMove);
    glutMouseWheelFunc(fareTekerlegi);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    loadTextures();

    glutMainLoop();
    return 0;
}