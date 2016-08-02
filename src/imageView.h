#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__

#include <QtOpenGL/QGLWidget>

#include <QtCore/QTimer>

#include "dataCtrl.h"

class ImageView : public QGLWidget
{
  Q_OBJECT

  public:
    ImageView(QWidget *parent = NULL);
    ~ImageView();

    void changeMode(DataCtrl::EMode mode) { dataCtrl->setCurrentMode(mode); onMoveDecal = false; }
    void setImageRealWidth(const qreal &width);
    void setImageRealHeight(const qreal &height);

    DataCtrl& data() { return *dataCtrl; }
    const DataCtrl& data() const { return *dataCtrl; }

    const qreal &imageRealWidth()  const { return dataCtrl->imageRealWidth(); }
    const qreal &imageRealHeight() const { return dataCtrl->imageRealHeigth(); }

  public slots:
    void doChangeImage(const QImage &image);
    void doCloseImage();

    void doZoomIn()     { if (zoom > 1) --zoom; }
    void doZoomOut()    { if (zoom < 11) ++zoom; }
    void doResetView()  { zoom = 10; xDecal = yDecal = 0.; resizeGL(width(), height()); }

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

  protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

  protected:
    bool onMoveDecal = false;
    int zoom = 10;
    DataCtrl *dataCtrl;
    GLuint imageTexId = 0;
    GLfloat xDecal = 0.f, yDecal = 0.f;
    GLfloat ratioWidthPerHeght = 1.f;
    QTimer refreshTimer;
    QPoint lastMousePos;
};

#endif
