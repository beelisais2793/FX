#ifndef GLOBJ_H
#define GLOBJ_H

#ifndef NOARRAYS
enum VBOtype{VERTEX, NORMAL, TEXTURE};
#endif

#define FOURCC_DXT1 0x31545844 //(MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT3 0x33545844 //(MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT5 0x35545844 //(MAKEFOURCC('D','X','T','5'))

#include <QFile>
#include <QMainWindow>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QVector>
#include <QTextStream>
#include <QVector3D>
#include <QVector2D>

struct Face {
    QVector<QVector3D> Vertices;
    QVector<QVector3D> Normales;
    QVector<QVector2D> UVs;
    Face(){
        Vertices.resize(3);
        Normales.resize(3);
        UVs.resize(3);
    }
};

struct Material {
    GLfloat ambient [4] = {};
    GLfloat diffuse [4] = {};
    GLfloat specular[4] = {};
    GLfloat emission[4] = {};
    GLfloat shininess   = {};
    Material() {}
    Material(const Material& that) {
        memcpy(this->ambient,  that.ambient,  sizeof(GLfloat) * 4);
        memcpy(this->diffuse,  that.diffuse,  sizeof(GLfloat) * 4);
        memcpy(this->specular, that.specular, sizeof(GLfloat) * 4);
        memcpy(this->emission, that.emission, sizeof(GLfloat) * 4);
        this->shininess = that.shininess;
    }
};

//Wavefront .obj parser
class GLobj {
public:
    void setTexture(GLuint texture);
    void setMaterial(Material &type);
    void setPosition(QVector3D n_p);
    void setScale(QVector3D n_s);
    void setRotation(QVector3D deg);
    QVector3D getRotation();
    QVector3D getPosition();
    QVector3D getScale();
    Material getMaterial();
    GLuint getTexture();
    bool isRotatable();
    bool isTextured();
	void setVBO(GLuint vbo, VBOtype type);
	GLuint getVBO(VBOtype type);
	~GLobj();
    GLobj(bool, bool);
    GLobj(QString filename, bool, bool);
    QVector<QVector3D>& mVertices();
    QVector<QVector3D>& mNormales();
    QVector<QVector2D>& mUVs();
    QVector<Face>& mFaces();
    void LoadMesh(QString &mesh);
private:
    QVector<QVector3D> Vertices;
    QVector<QVector3D> Normales;
    QVector<QVector2D> UVs;
    QVector<Face> Faces;

	GLuint VBOv, VBOu, VBOn;

    QVector3D m_position;
    GLuint m_texture;
    Material m_material;
    QVector3D m_scale = {1.0f, 1.0f, 1.0f};
    QVector3D m_rotation = {0.0f, 0.0f, 0.0f};
    bool rotate_on = false;
    bool textures_on = false;
};

#endif // GLOBJ_H
