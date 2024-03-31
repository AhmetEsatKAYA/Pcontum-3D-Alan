#include <iostream>
#include <GL/freeglut.h>
#include <FreeImage.h>

GLuint g_textureID;

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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_textureID);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Texture Example");

    loadTexture();

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
