#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__

#include <QtOpenGL/QGLWidget>

#include <QtCore/QTimer>

class ImageView : public QGLWidget
{
  Q_OBJECT

  public:
    enum EMode
    {
      eModeView,
      eModeEdit,
    };

  public:
    ImageView(QWidget *parent = NULL);
    ~ImageView();

    void changeMode(EMode mode) { currentMode = mode; onMoveDecal = false; }

  public slots:
    void doZoomIn()     { if (zoom > 1) --zoom; }
    void doZoomOut()    { if (zoom < 11) ++zoom; }
    void doResetView()  { zoom = 10; xDecal = yDecal = 0.; resizeGL(width(), height()); }

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *);

  protected slots:
    void doChangeImage(const QImage &image);
    void doCloseImage();

  protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

  protected:
    bool onMoveDecal;
    EMode currentMode;
    int zoom;
    GLuint imageTexId;
    GLfloat xDecal, yDecal;
    GLfloat ratioWidthPerHeght;
    QTimer refreshTimer;
    QPoint lastMousePos;
};

#endif
