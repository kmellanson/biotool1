#include "imageView.h"

#include <QtCore/QDebug>

#include <QtGui/QMouseEvent>

ImageView::ImageView(QWidget *parent) :
  QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DoubleBuffer), parent), imageTexId(0), zoom(10), xDecal(0.), yDecal(0.), onMoveDecal(false), ratioWidthPerHeght(1.)
{
  refreshTimer.setSingleShot(false);
  connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(update()));
  refreshTimer.start(20);
}

ImageView::~ImageView()
{
  if (imageTexId) deleteTexture(imageTexId);
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    onMoveDecal = event->type() == QEvent::MouseButtonPress;
    lastMousePos = event->pos();
  }
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
  float factor = qMin(width(), height());
  if (onMoveDecal && event->type())
  {
    xDecal += (event->x() - lastMousePos.x()) / factor * zoom * 2;
    yDecal += (event->y() - lastMousePos.y()) / factor * zoom * 2;
  }
  lastMousePos = event->pos();
}

void ImageView::wheelEvent(QWheelEvent *event)
{
  zoom += event->delta() / 120;
  if (zoom > 11)  zoom = 11;
  if (zoom < 1)   zoom = 1;
  resizeGL(width(), height());
}

void ImageView::doChangeImage(const QImage &image)
{
  if (imageTexId) deleteTexture(imageTexId);
  ratioWidthPerHeght = (image.width() / (float)image.height());

  imageTexId = bindTexture(image);
}

void ImageView::initializeGL()
{
  QGLWidget::initializeGL();

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glEnable(GL_TEXTURE_2D);
}

void ImageView::paintGL()
{
  QGLWidget::paintGL();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  glTranslatef(xDecal / 10., yDecal / 10., 0);

  glBindTexture(GL_TEXTURE_2D, imageTexId);

  glBegin(GL_QUADS);
    glTexCoord2f( 0, 1); glVertex3f(-ratioWidthPerHeght, -1., 0.);
    glTexCoord2f( 0, 0); glVertex3f(-ratioWidthPerHeght,  1., 0.);
    glTexCoord2f( 1, 0); glVertex3f( ratioWidthPerHeght,  1., 0.);
    glTexCoord2f( 1, 1); glVertex3f( ratioWidthPerHeght, -1., 0.);
  glEnd();

  glPopMatrix();
}

void ImageView::resizeGL(int width, int height)
{
  QGLWidget::resizeGL(width, height);

  glViewport(0, 0, width, height);

  float x(zoom / 10.);
  float y(x);

  if (width > height)
    x *= ((float)width / (float)height);
  else
    y *= ((float)height / (float)width);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-x, +x, +y, -y, -100.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}
