#include "scene.h"

//to simplify RK4 sync
int iFPS = 50;

GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 0.0f};
GLfloat light_diffuse[] = {0.5f, 0.5f, 0.5f, 0.0f};
GLfloat light_position[] = {0.0f, 3.0f, -4.0f, 2.0f};
GLfloat light_position0[] = {1.0f, 1.0f, 1.0f, 0.0f};

Scene::Scene(QWidget *){
	QSurfaceFormat format;
	format.setVersion(2, 0);
	format.setProfile(QSurfaceFormat::CompatibilityProfile);
	QSurfaceFormat::setDefaultFormat(format);
    auto pTimer = new QTimer(this);
    connect(pTimer,SIGNAL(timeout()), this, SLOT(update()));
    connect(pTimer,SIGNAL(timeout()), this, SLOT(run()));
    pTimer->start(1000 / 60.0);
    Default();
}

Scene::~Scene(){
    teardownGL();
	deleteVBO(*lowershell);
	deleteVBO(*uppershell);
	deleteVBO(*plank);
	deleteVBO(*stand);
	deleteVBO(*swing);
	deleteVBO(*shaft);
    delete lowershell;
    delete uppershell;
    delete plank;
    delete stand;
    delete swing;
    delete shaft;
}

void Scene::initializeGL(){
    initializeOpenGLFunctions();
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

    wood = new QOpenGLTexture(QImage(":/textures/mNt/wood.jpg"));
    wood->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    wood->setMagnificationFilter(QOpenGLTexture::Linear);
    wood->setWrapMode(QOpenGLTexture::Repeat);
    wood_id = wood->textureId();

    plastic = new QOpenGLTexture(QImage(":/textures/mNt/plastic.jpg"));
    plastic->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    plastic->setMagnificationFilter(QOpenGLTexture::Linear);
    plastic->setWrapMode(QOpenGLTexture::Repeat);
    plastic_id = plastic->textureId();

    metal = new QOpenGLTexture(QImage(":/textures/mNt/metal.jpg"));
    metal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    metal->setMagnificationFilter(QOpenGLTexture::Linear);
    metal->setWrapMode(QOpenGLTexture::Repeat);
    metal_id = metal->textureId();

    lowershell = new GLobj(QStringLiteral(":/meshes/mNt/lowershell.obj"), false, true);
    uppershell = new GLobj(QStringLiteral(":/meshes/mNt/uppershell.obj"), false, true);
    shaft = new GLobj(QStringLiteral(":/meshes/mNt/shaft.obj"), false, false);
    stand = new GLobj(QStringLiteral(":/meshes/mNt/stand.obj"), false, true);
    swing = new GLobj(QStringLiteral(":/meshes/mNt/swingwithball.obj"), true, false);
    plank = new GLobj(QStringLiteral(":/meshes/mNt/plank.obj"), false, false);
    createVBO(*lowershell);
    createVBO(*uppershell);
    createVBO(*shaft);
    createVBO(*stand);
    createVBO(*swing);
    createVBO(*plank);

    lowershell->setTexture(plastic_id);
    uppershell->setTexture(plastic_id);
    //shaft->setTexture(metal_id);
    stand->setTexture(wood_id);
    //swing->setTexture(metal_id);
    //plank->setTexture(metal_id);

    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    //glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_MULTISAMPLE);
}

void Scene::resizeGL(int nWidth, int nHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat width, height, tangent;
    GLfloat ratio=(GLfloat)nHeight / (GLfloat)nWidth;
    if (nWidth>=nHeight)
        glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -10.0, 30.0);
    else
        glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -10.0, 30.0);
    tangent = tan(90/2 * 3.14159265 / 180);
    height = nSca * tangent;
    width = height * ratio;
    glFrustum(-width, width, -height, height, -10.0f, 30.0f);
    glViewport(0, 0,(GLint)nWidth, (GLint)nHeight);
}

void Scene::paintGL() {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glScalef(nSca, nSca, nSca);
     glTranslatef(0.0f, zTra, 0.0f);
     glRotatef(xRot, 1.0f, 0.0f, 0.0f);
     glRotatef(yRot, 0.0f, 1.0f, 0.0f);
     glRotatef(zRot, 0.0f, 0.0f, 1.0f);
     glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
     glTranslatef(0.0f, 0.75f, 0.0f);
	 draw();
}

void Scene::teardownGL(){
    wood->destroy();
    plastic->destroy();
    metal->destroy();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable (GL_LIGHT1);
    glDisable(GL_MULTISAMPLE);
}

void Scene::mousePressEvent(QMouseEvent *qme){
    mousePoz = qme->pos();
}

void Scene::mouseReleaseEvent(QMouseEvent *){
    draw();
    update();
}

void Scene::wheelEvent(QWheelEvent *qwe){
    if (qwe->delta() > 0)
        Scale(PLUS);
    else if (qwe->delta() < 0)
        Scale(MINUS);
    update();
}

void Scene::keyPressEvent(QKeyEvent *qke){
    switch(qke->key()){
    case Qt::Key_Q:
        swing->setRotation(swing->getRotation() +
                    QVector3D(1.0f, 0.0f, 0.0f));
        break;
    case Qt::Key_E:
        swing->setRotation(swing->getRotation() +
                    QVector3D(-1.0f, 0.0f, 0.0f));
        break;
	case Qt::Key_G:
        //rodReload(false);
		break;
	case Qt::Key_H:
        //rodReload(true);
		break;
    case Qt::Key_Plus:
        Scale(PLUS);
        break;
    case Qt::Key_Minus:
        Scale(MINUS);
        break;
    case Qt::Key_Up:
        Rotate(UP);
        break;
    case Qt::Key_Down:
        Rotate(DOWN);
        break;
    case Qt::Key_Left:
        Rotate(LEFT);
        break;
    case Qt::Key_Right:
        Rotate(RIGHT);
        break;
    case Qt::Key_Space:
        Default();
        break;
    case Qt::Key_Escape:
        this->close();
        break;
    }
    update();
}

void Scene::Scale(scaleDirection direction){
    switch (direction){
    case PLUS:
        if(nSca < 2.5f){
            nSca *= 1.1f;
            LightUpdate();
        }
        break;
    case MINUS:
        if(nSca > 0.1f){
            nSca /= 1.1f;
            LightUpdate();
        }
        break;
    }
}

void Scene::Rotate(rotateDirection direction){
    switch (direction){
    case UP:
        xRot -= 1.0f;
        break;
    case DOWN:
        xRot +=1.0f;
        break;
    case LEFT:
        zRot += 1.0f;
        break;
    case RIGHT:
        zRot -= 1.0f;
        break;
    }
}

void Scene::Translate(translateDirection direction){
    switch(direction){
    case FORWARDS:
        zTra += 0.05f;
        break;
    case BACKWARDS:
        zTra -= 0.05f;
        break;
    }
}

void Scene::Default(){
    xRot = -90.0f;
    yRot = 0.0f;
    zRot = -90.0f;
    zTra = -1.5f;
    nSca = 0.5f;
    LightUpdate();
}

void Scene::LightUpdate(){
    GLfloat light_diffuse_new[] =
    {
        light_diffuse[0] * 2.0f * nSca,
        light_diffuse[1] * 2.0f * nSca,
        light_diffuse[2] * 2.0f * nSca,
        light_diffuse[3]
    };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_new);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse_new);
}
 
void Scene::draw() {
    //Draw grid
    glColor3f(1.0, 0.0, 0.0);
    for(int x = 1; x < 3; x++){
      glBegin(GL_LINE_STRIP);
      glVertex3f(0,x,-3);
      glVertex3f(0,x,3);
      glEnd();
    }
    for(int y = -2; y < 3; y++){
      glBegin(GL_LINE_STRIP);
      glVertex3f(0,0,y);
      glVertex3f(0,3,y);
      glEnd();
    }
    glColor3f(1.0, 1.0, 1.0);
    RenderGLobj(*stand);
    RenderGLobj(*plank);
    RenderGLobj(*shaft);
    RenderGLobj(*lowershell);
    RenderGLobj(*uppershell);
    RenderGLobj(*swing);
}

void Scene::mouseMoveEvent(QMouseEvent* pe) {
    xRot += 180 / 1.5*(GLfloat)(pe->y() - mousePoz.y()) / height(); //no X rotato pleas
	zRot += 180 / 1.5*(GLfloat)(pe->x() - mousePoz.x()) / width();
	mousePoz = pe->pos();
    draw();
    update();
}

void Scene::RenderGLobj(GLobj &to_render) {
	if (to_render.isTextured()) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, to_render.getTexture());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glPushMatrix();
	if (to_render.isRotatable()) {
		glTranslatef(-to_render.getPosition().x(),
			-to_render.getPosition().y() + 1.81f,
			-to_render.getPosition().z());
		glRotatef(to_render.getRotation().x(), 1.0f, 0.0f, 0.0f);
		glRotatef(to_render.getRotation().y(), 0.0f, 1.0f, 0.0f);
		glRotatef(to_render.getRotation().z(), 0.0f, 0.0f, 1.0f);
		glTranslatef(to_render.getPosition().x(),
			to_render.getPosition().y() - 1.81f,
			to_render.getPosition().z());
	}
	glTranslatef(
		to_render.getPosition().x(),
		to_render.getPosition().y(),
		to_render.getPosition().z());
#ifdef NOARRAYS
    glBegin(GL_TRIANGLES);
	for(int i = 0; i < to_render.mFaces().size(); i++) {
		for(int j = 0; j < 3; j++) {
				glNormal3f(to_render.mFaces()[i].Normales[j].x(), to_render.mFaces()[i].Normales[j].y(), to_render.mFaces()[i].Normales[j].z());
				glTexCoord2f(to_render.mFaces()[i].UVs[j].x(), to_render.mFaces()[i].UVs[j].y());
				glVertex3f(to_render.mFaces()[i].Vertices[j].x(), to_render.mFaces()[i].Vertices[j].y(), to_render.mFaces()[i].Vertices[j].z());
		}
    }
    glEnd();
#else
	glBindBuffer(GL_ARRAY_BUFFER, to_render.getVBO(VERTEX));
	glVertexPointer(3, GL_FLOAT, sizeof(QVector3D), NULL);

	glBindBuffer(GL_ARRAY_BUFFER, to_render.getVBO(NORMAL));
	glNormalPointer(GL_FLOAT, sizeof(QVector3D), NULL);
	if (to_render.isTextured()) {
		glBindBuffer(GL_ARRAY_BUFFER, to_render.getVBO(TEXTURE));
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}
	glDrawArrays(GL_TRIANGLES, 0, to_render.mVertices().size());
#endif
	glTranslatef(0.0f, 0.0f, 0.0f);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

/*(void Scene::rodReload(bool isBallUsed){

}*/

void Scene::createVBO(GLobj &to_load){
    GLuint *temp_VBO = new GLuint[3];
    glGenBuffers(3, temp_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, temp_VBO[0]); //vertices
    glBufferData(GL_ARRAY_BUFFER, to_load.mVertices().size() * sizeof(QVector3D), to_load.mVertices().constData(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, temp_VBO[1]); //normales
    glBufferData(GL_ARRAY_BUFFER, to_load.mNormales().size() * sizeof(QVector3D), to_load.mNormales().constData(), GL_STATIC_DRAW);
	if (to_load.isTextured()) {
		glBindBuffer(GL_ARRAY_BUFFER, temp_VBO[2]); //UVs
		glBufferData(GL_ARRAY_BUFFER, to_load.mUVs().size() * sizeof(QVector2D), to_load.mUVs().constData(), GL_STATIC_DRAW);
		to_load.setVBO(temp_VBO[2], TEXTURE);
	}
    to_load.setVBO(temp_VBO[0], VERTEX);
    to_load.setVBO(temp_VBO[1], NORMAL);
    delete[] temp_VBO;
}

void Scene::deleteVBO(GLobj &to_remove) {
	GLuint buffers[3];
	buffers[0] = to_remove.getVBO(VERTEX);
	buffers[1] = to_remove.getVBO(NORMAL);
	if (to_remove.isTextured())
		buffers[2] = to_remove.getVBO(TEXTURE);
	glDeleteBuffers(3, buffers);
}

void Scene::run(){
    //throw 2;
    if (isRunning) {
        time += dt;
        //model.currentTime(time);
        model.updateData();
        swing->setRotation(QVector3D(model.getTheta(), 0.0f, 0.0f));

    }
    emit diffAngle(model.getTheta());
    emit diffFreq(model.getPeriod());

}

void Scene::toggleRunning(bool){
    isRunning = !isRunning;
    if (isRunning){
        model.setUp();
    }
}

void Scene::setAngle(double new_angle){
    swing->setRotation(QVector3D(new_angle, 0.0f, 0.0f));
    model.setTht0(new_angle);
    draw();
}

void Scene::setMass(double new_mass){
    model.setRMass(new_mass);
}

void Scene::setType(bool isBallUsed){
    deleteVBO(*swing);
    delete swing;
    if (isBallUsed)
        swing = new GLobj(QStringLiteral(":/meshes/mNt/swingwithball.obj"), true, false);
    else
        swing = new GLobj(QStringLiteral(":/meshes/mNt/swingnoball.obj"), true, false);
    createVBO(*swing);
    draw();
    model.setComp(isBallUsed ? BALL : NONE);
}

void Scene::setDelta(double new_delta){
    model.setSpd(new_delta);
}

void Scene::setDamping(double damp_data){
    model.setDamp(damp_data);
}

void Scene::setSpeed(int new_speed){
    iFPS = new_speed;
}
void Scene::setImpulse(double new_impulse){
    model.setImp(new_impulse);
}

void Scene::setLength(double new_length){
    model.setLen(new_length);
}
