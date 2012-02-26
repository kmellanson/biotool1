#ifndef __CELLPOLYGON_H__
#define __CELLPOLYGON_H__

#include <QtGui/QPolygonF>

class QDomElement;
class QDomDocument;

class CellPolygon : public QPolygonF
{
  public:
    enum EType
    {
      eEdition = 0,
      eFinalized
    };

  public:
    CellPolygon() : QPolygonF(), area(0.f), radius(0.f), type(eEdition) {}
    CellPolygon(const CellPolygon &other) : QPolygonF() { *this = other; }
    virtual ~CellPolygon() {}

    void computeData();
    virtual void clear();

    const QPointF &getCentroid()  const { return centroid; }
    const qreal   &getArea()      const { return area; }
    const qreal   &getRadius()    const { return radius; }

    void draw() const;

    void save(QDomDocument &doc, QDomElement &parentNode, const int level) const;
    void load(QDomElement &node);

  protected:
    QPointF centroid;
    qreal area;
    qreal radius;
    EType type;
};

#endif
