#include <iostream>
#include <GL/freeglut.h>
#include <cmath>
#include "FreeImage.h"
#include <vector>

GLuint g_textureID;

float cameraX = 0.0f;
float cameraY = 1.0f;
float cameraZ = 5.0f;

// Kameranın bakış açısı
float cameraYaw = 0.0f; // Kamera yatay açısı
float cameraPitch = 0.0f; // Kamera dikey açısı
int lastMouseX = -1; // Son fare konumu (x)
int lastMouseY = -1; // Son fare konumu (y)

// Ev duvar rengi
float wallColor[2][3] = {
    {1.0f, 1.0f, 1.0f}, // Tavan rengi
    {0.95f, 0.95f, 0.95f} // Duvar rengi
};

int windowWidth;
int windowHeight;
std::vector<GLuint> g_textureIDs; // Resimleri tutacak bir dizi tanımlayın

void loadTextures() {
    const char* textureNames[] = {"hali.png", "televizyon.png", "pcontum.png", "grass.png", "lake.jpeg"}; // Yüklenecek resimlerin dosya adları
    const FREE_IMAGE_FORMAT formats[] = {FIF_PNG, FIF_PNG, FIF_PNG, FIF_PNG, FIF_JPEG}; // Resim formatları
    for (int i = 0; i < sizeof(textureNames) / sizeof(textureNames[0]); ++i) {
        const char* textName = textureNames[i];
        FIBITMAP* dib = FreeImage_Load(formats[i], textName);
        dib = FreeImage_ConvertTo32Bits(dib); // 32 bit RGBA format
        if (dib != NULL) {
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
            std::cout << textName << " loaded." << std::endl;
            FreeImage_Unload(dib);
            delete[] bits;

            // Texture ID'sini vektöre ekle
            g_textureIDs.push_back(textureID);
        }
    }
}

void drawLake(float lakeWidth, float lakeLength, float lakeX, float lakeY, float lakeZ) {
    glColor3f(1.0f, 1.0f, 1.0f);

    // Göl yüzeyinin yüksekliği
    float lakeSurfaceY = 0.1f;

    // Gölü çizin (doku kullanarak)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[4]); // lake.jpeg için uygun texture ID'si

    glBegin(GL_QUADS);
    // Göldü tekrarlamak için uygun koordinatları kullanarak dokuyu çizin
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
    glTexCoord2f(100.0f, 0.0f); // Tek bir büyük kare olduğu için texture'ı tekrar etmeye gerek yok.
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

void drawZemin(float ev_genislik, float ev_uzunluk, float ev_yukseklik, float ev_x, float ev_y, float ev_z) {
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

void drawHome(float ev_genislik, float ev_uzunluk, float ev_yukseklik, float ev_x, float ev_y, float ev_z, float duvark) {
    glDisable(GL_LIGHTING);
    glPushMatrix();

    // Ön iç duvar
    glColor3fv(wallColor[1]);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[1]);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f); // Resimi düzelt
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();


    // Arka duvar
    glBegin(GL_QUADS);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glEnd();
    

    // Sol duvar
    glColor3fv(wallColor[1]); // Duvar rengi
    glBegin(GL_QUADS);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    // Sağ duvar
    glColor3fv(wallColor[1]); // Duvar rengi
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureIDs[2]);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f); // Sağ duvar
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, -ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    // Üst duvar (Tavan)
    glColor3fv(wallColor[0]); // Beyaz renk
    glBegin(GL_QUADS);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glVertex3f(-ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, ev_uzunluk / 2.0f + ev_z);
    glVertex3f(ev_genislik / 2.0f + ev_x, ev_y + ev_yukseklik, -ev_uzunluk / 2.0f + ev_z);
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
    // Zemin
    drawZemin(ev_genislik, ev_uzunluk, ev_yukseklik, ev_x, ev_y, ev_z);
}

void setupLights() {
    // Işıkları tanımlayın ve konumlandırın
    GLfloat lightPosition[] = { 0.0f, 10.0f, 0.0f, 1.0f }; // Örnek bir konum
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Işıkların renklerini ayarlayın
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Parlaklığı artırdık
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Parlaklık artırıldı

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Genel ambiyans ışığı
    GLfloat globalAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f }; // Daha yüksek bir değer
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    // Aydınlatmayı etkinleştirin
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(cameraPitch, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraYaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    // Işıkları kurun
    setupLights();

    drawSky();
    drawGround();
    drawTree(-3.0f, -3.0f);
    drawTree(3.0f, 3.0f);
    drawTree(-5.0f, -5.0f);
    drawTree(6.0f, 6.0f);

    drawHome(5.0f, 5.0f, 2.0f, 0.0f, 0.2f, 0.0f, 0.1f);

    // Evin yanına göl çizin
    drawLake(8.0f, 5.0f, 2.0f, -1.5f, 6.0f);

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
    case 27:
        exit(0);
        break;
    case 'w': // İleri hareket
        cameraX += 0.1f * sin(cameraYaw * M_PI / 180.0f);
        cameraZ -= 0.1f * cos(cameraYaw * M_PI / 180.0f);
        break;
    case 's': // Geri hareket
        cameraX -= 0.1f * sin(cameraYaw * M_PI / 180.0f);
        cameraZ += 0.1f * cos(cameraYaw * M_PI / 180.0f);
        break;
    case 'a':
        cameraYaw -= 2.0f;
        break;
    case 'd':
        cameraYaw += 2.0f;
        break;
    case 'z':
        cameraY += 0.1;
        break;
    case 'x':
        cameraY -= 0.1;
        break;
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
    glutCreateWindow("Pcontum 3D Free Software");
    glutFullScreen(); // Tam ekran yap
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMove);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    loadTextures();

    glutMainLoop();
    return 0;
}