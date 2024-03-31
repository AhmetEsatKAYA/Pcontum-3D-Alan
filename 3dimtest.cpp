#include <iostream>
#include <GL/freeglut.h>
#include <cmath>
#include "FreeImage.h"

GLuint g_textureID;

// Kamera pozisyonu
float cameraX = 0.0f;
float cameraY = 1.0f;
float cameraZ = 5.0f;

// Kameranın bakış açısı
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;

// Toprak rengi
float groundColor[3] = {0.0f, 0.6f, 0.0f}; // Yeşil

// Ev duvar rengi
float wallColor[4][3] = {
    {0.9f, 0.9f, 0.9f}, // Tavan rengi
    {0.7f, 0.7f, 0.7f}, // Duvar rengi 1
    {0.65f, 0.65f, 0.65f}, // Duvar rengi 2
    {0.7f, 0.7f, 0.7f} // Duvar rengi 3
};

// Ev boyutları
float houseWidth = 4.0f;
float houseHeight = 2.0f;
float houseLength = 6.0f;

void loadTexture() {
    const char textName[] = "hali.png"; // PNG dosyasının yolu
    FIBITMAP* dib = FreeImage_Load(FIF_PNG, textName, PNG_DEFAULT);
    dib = FreeImage_ConvertTo32Bits(dib); // 32 bit RGBA format
    if (dib != NULL) {
        glGenTextures(1, &g_textureID);
        glBindTexture(GL_TEXTURE_2D, g_textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        BYTE* bits = new BYTE[FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib) * 4];
        BYTE* pixels = (BYTE*)FreeImage_GetBits(dib);
        for (int pix = 0; pix < FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib); pix++) {
            bits[pix * 4 + 0] = pixels[pix * 4 + 2]; // Kırmızı
            bits[pix * 4 + 1] = pixels[pix * 4 + 1]; // Yeşil
            bits[pix * 4 + 2] = pixels[pix * 4 + 0]; // Mavi
            bits[pix * 4 + 3] = pixels[pix * 4 + 3]; // Alfa (saydamlık)
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
        std::cout << textName << " yüklendi." << std::endl;
        FreeImage_Unload(dib);
        delete[] bits;
    }
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
    glColor3fv(groundColor);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();
}

void drawSky() {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(0.7f, 0.7f, 1.0f); // Açık mavi gökyüzü
    glTranslatef(0.0f, 100.0f, 0.0f);
    glutSolidSphere(300.0f, 20, 20);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawHome(float ev_genislik, float ev_uzunluk, float ev_yukseklik, float ev_x, float ev_y, float ev_z) {
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureID);

    // Ev zemininin çizimi
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glEnd();

    // Ev duvarlarının çizimi
    glColor3fv(wallColor[2]); // Ön duvar
    glBegin(GL_QUADS);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);

    glColor3fv(wallColor[3]); // Sol duvar
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);

    glColor3fv(wallColor[1]); // Sağ duvar
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);

    glColor3fv(wallColor[0]); // Tavan
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawInteriorLight() {
    GLfloat light_position_interior[] = {0.0f, 1.0f, 0.0f, 1.0f}; // Iç mekan ışığı pozisyonu
    GLfloat light_ambient_interior[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Iç mekan ışığı rengi
    GLfloat light_diffuse_interior[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Iç mekan ışığı yoğunluğu

    glLightfv(GL_LIGHT2, GL_POSITION, light_position_interior);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient_interior);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse_interior);
    glEnable(GL_LIGHT2);
}

void display() {
    GLfloat light_position1[] = {0.0f, 5.0f, 0.0f, 1.0f}; // Işık kaynağı pozisyonu 1
    GLfloat light_ambient1[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Işık rengi 1
    GLfloat light_diffuse1[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Işık yoğunluğu 1

    GLfloat light_position2[] = {0.0f, 1.0f, 0.0f, 1.0f}; // Işık kaynağı pozisyonu 2
    GLfloat light_ambient2[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Işık rengi 2
    GLfloat light_diffuse2[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Işık yoğunluğu 2

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(cameraPitch, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraYaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    // Işık kaynaklarını ayarla ve etkinleştir
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1); // Işık kaynağı 1
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_POSITION, light_position2); // Işık kaynağı 2
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
    glEnable(GL_LIGHT1);

    drawInteriorLight(); // İç mekan ışığı

    drawSky();
    drawGround();
    drawTree(-3.0f, -3.0f);
    drawTree(3.0f, 3.0f);
    drawTree(-5.0f, -5.0f);
    drawTree(6.0f, 6.0f);

    drawHome(10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (GLfloat)w / (GLfloat)h, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC tuşu programı kapatır
        exit(0);
        break;
    case 'w':
        cameraPitch -= 1.0f;
        break;
    case 's':
        cameraPitch += 1.0f;
        break;
    case 'a':
        cameraYaw -= 1.0f;
        break;
    case 'd':
        cameraYaw += 1.0f;
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        cameraZ -= 0.1f;
        break;
    case GLUT_KEY_DOWN:
        cameraZ += 0.1f;
        break;
    case GLUT_KEY_LEFT:
        cameraX -= 0.1f;
        break;
    case GLUT_KEY_RIGHT:
        cameraX += 0.1f;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("3D Garden Navigation");
    glutReshapeWindow(800, 600); // Pencere boyutunu ayarla
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    loadTexture(); // Texture yükleme işlemi

    // Iç mekan ışığı ayarı
    glEnable(GL_LIGHT2);
    GLfloat light_position_interior[] = {0.0f, 1.0f, 0.0f, 1.0f}; // Iç mekan ışığı pozisyonu
    GLfloat light_ambient_interior[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Iç mekan ışığı rengi
    GLfloat light_diffuse_interior[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Iç mekan ışığı yoğunluğu
    glLightfv(GL_LIGHT2, GL_POSITION, light_position_interior);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient_interior);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse_interior);

    glutMainLoop();
    return 0;
}
