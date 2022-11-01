#include "globj.h"

GLobj::~GLobj() {
	Faces.clear();
	Faces.squeeze();
	Vertices.clear();
	Vertices.squeeze();
	Normales.clear();
	Normales.squeeze();
	UVs.clear();
	UVs.squeeze();
}

GLobj::GLobj(bool rotation, bool texture){
    rotate_on = rotation;
    textures_on = texture;
}

GLobj::GLobj(QString filename, bool rotation, bool texture){
    rotate_on = rotation;
    textures_on = texture;
    LoadMesh(filename);
}

void GLobj::LoadMesh(QString &filename) {
    QVector3D temp3;
    QVector2D temp2;
    if (!filename.isEmpty()){
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream fileCont(&file);
            while (!fileCont.atEnd()){
                QString fileLine = fileCont.readLine();
                if (fileLine.startsWith("vn ")){
                    QStringList lineToken = fileLine.split(" ");
                    temp3.setX(lineToken[1].toFloat());
                    temp3.setY(lineToken[2].toFloat());
                    temp3.setZ(lineToken[3].toFloat());
                    Normales.push_back(temp3);
                }
                else if(fileLine.startsWith("vt ")){
                    QStringList lineToken = fileLine.split(" ");
                    temp2.setX(lineToken[1].toFloat());
                    temp2.setY(lineToken[2].toFloat());
                    UVs.push_back(temp2);
                }
                else if (fileLine.startsWith("v ")){
                    QStringList lineToken = fileLine.split(" ");
                    temp3.setX(lineToken[1].toFloat());
                    temp3.setY(lineToken[2].toFloat());
                    temp3.setZ(lineToken[3].toFloat());
                    Vertices.push_back(temp3);
                }
                else if (fileLine.startsWith("f ")){
                    QStringList lineToken = fileLine.split(" ");
                    Face *F = new Face;
                    for (size_t i = 1; i <= 3; i++){
                        QStringList args = lineToken[i].split("/");
                        F->Vertices[i-1] = Vertices[args[0].toInt()-1];
                        F->UVs[i-1] = UVs[args[1].toInt()-1];
                        F->Normales[i-1] = Normales[args[2].toInt()-1];
                    }
                    Faces.push_back(*F);
                }
            }
        }
        file.close();
    }
#ifndef NOARRAYS
	Vertices.clear();
	UVs.clear();
	Normales.clear();
    for (auto i = 0; i < Faces.size(); ++i) {
		Vertices.append(Faces[i].Vertices);
		UVs.append(Faces[i].UVs);
		Normales.append(Faces[i].Normales);
	}
#endif
}

void GLobj::setTexture(GLuint texture){
    m_texture = texture;
}

void GLobj::setMaterial(Material &type){
    m_material = type;
}

void GLobj::setPosition(QVector3D n_p){
    if (n_p == m_position)
        return;
    m_position = n_p;
}

void GLobj::setScale(QVector3D n_s){
    if (n_s == m_scale)
        return;
    m_scale = n_s;
}

void GLobj::setRotation(QVector3D n_r){
    if (n_r == m_rotation)
        return;
    m_rotation = n_r;
}

QVector3D GLobj::getScale(){
    return m_scale;
}

Material GLobj::getMaterial(){
    return m_material;
}

QVector3D GLobj::getPosition(){
    return m_position;
}

QVector3D GLobj::getRotation(){
    return m_rotation;
}

GLuint GLobj::getTexture(){
    return m_texture;
}

QVector<QVector3D>& GLobj::mVertices(){
    return Vertices;
}

QVector<QVector3D>& GLobj::mNormales(){
    return Normales;
}

QVector<QVector2D>& GLobj::mUVs(){
    return UVs;
}

bool GLobj::isRotatable(){
    return rotate_on;
}

bool GLobj::isTextured(){
    return textures_on;
}

QVector<Face>& GLobj::mFaces(){
    return Faces;
}

void GLobj::setVBO(GLuint vbo, VBOtype type) {
	if (vbo == 0)
		throw 1;
	switch (type) {
	case VERTEX:
		VBOv = vbo;
		break;
	case NORMAL:
		VBOn = vbo;
		break;
	case TEXTURE:
		if (!textures_on)
			throw 2;
		VBOu = vbo;
		break;
    default:
        throw 3;
	}
}

GLuint GLobj::getVBO(VBOtype type) {
	switch (type) {
	case VERTEX:
		return VBOv;
	case NORMAL:
		return VBOn;
	case TEXTURE:
		if (!textures_on)
			throw 2;
		return VBOu;
    default:
        throw 3;
	}
}
