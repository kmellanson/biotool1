#pragma once

#include <QtGui/QPolygonF>

#define M_PI  3.14159265358979323846

class QDomElement;
class QDomDocument;

class XPolygon : public QPolygonF
{
  public:
    enum EType
    {
      eEdition = 0,
      eFinalized
    };

  public:
    XPolygon() : QPolygonF() {}
    XPolygon(const XPolygon &other) : QPolygonF() { *this = other; }

    void computeData();
    virtual void clear();

    const QPointF &getCentroid()  const { return centroid; }
    const qreal   &getArea()      const { return area; }
    const qreal   &getRadius()    const { return radius; }

    void draw() const;
    void drawBackground() const;

    void save(QDomDocument &doc, QDomElement &parentNode, const int level) const;
    void load(QDomElement &node);

  protected:
    QPointF centroid;
    qreal area = 0.;
    qreal radius = 0.;
    EType type = eEdition;
};
