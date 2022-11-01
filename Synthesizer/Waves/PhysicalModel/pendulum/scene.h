#ifndef _RSCENE_H
#define _RSCENE_H
#include "globj.h"
#include "phyzxmodel.h"
#include "timer.h"

//#include <iostream>
#include <QOpenGLWidget>
//Qt before 5.5? Never heard of
#include <QTimer>
#include <QtOpenGL/QtOpenGL>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLTexture>
#include <QVector>
#include <QTextStream>
#include <QVector3D>
#include <QVector2D>
#include <ctime>

class Scene : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

private:
    Timer ltime;
    int nbFrames = 0;

    GLobj *lowershell, *plank, *shaft, *stand, *swing, *uppershell;
    bool isRunning = false;

    GLfloat xRot;
    GLfloat yRot;
    GLfloat zRot;
    GLfloat zTra;
    GLfloat nSca;

    QPoint mousePoz;

    enum scaleDirection{PLUS, MINUS};
    enum rotateDirection{UP, DOWN, LEFT, RIGHT};
    enum translateDirection{FORWARDS, BACKWARDS};

    void Scale(scaleDirection);
    void Rotate(rotateDirection);
    void Translate(translateDirection);
    void Default();
    void LightUpdate();

    QOpenGLTexture *wood, *plastic, *metal;

    GLuint wood_id, plastic_id, metal_id;
	GLuint elementbuffer;

    time_t time;
    double dt;
    unsigned int cycles;
    Phyzxmodel model;

    void RenderGLobj(GLobj &to_render);
    void createVBO(GLobj &to_load);
	void deleteVBO(GLobj &to_remove);

    //QVector<GLobj &> objects;
	void draw();
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int, int) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void teardownGL();
    void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent*) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent*) Q_DECL_OVERRIDE;
public:
    explicit Scene(QWidget *parent = 0);
    ~Scene();
signals:
    void diffEnergy(double current_energy);
    void diffFreq(double const_freq);
    void diffAngle(double current_angle);
public slots:
    void toggleRunning(bool);
    void run();
    void setAngle(double new_angle);
    void setMass(double new_mass);
    void setImpulse(double new_impulse);
    void setLength(double new_length);
    //void updateModel(double time);
    void setType(bool isBallUsed);
    void setDelta(double new_delta);
    void setDamping(double damp_data);
    void setSpeed(int new_speed);
};

#endif // RSCENE_H
