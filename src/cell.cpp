#include "cell.h"

#include <QtOpenGL/QGLContext>

#include <QtXml/QDomDocument>

QColor Cell::insideColor (QColor(0xff, 0x1f, 0x1f));
QColor Cell::outsideColor(QColor(0x1f, 0xff, 0x1f));

void Cell::clear()
{
  insideForm.clear();
  outsideForm.clear();
}

bool Cell::isEmpty() const
{
  return insideForm.isEmpty() && outsideForm.isEmpty();
}

bool Cell::isFull() const
{
  return !(insideForm.isEmpty() || outsideForm.isEmpty());
}

bool Cell::clearOneForm()
{
  if (!insideForm.isEmpty())
  {
    insideForm.clear();
    return false;
  }
  outsideForm.clear();
  return true;
}

bool Cell::addOneForm(const Polygon &form)
{
  if (outsideForm.isEmpty())
  {
    outsideForm = form;
    return false;
  }
  insideForm = form;
  computeVector();
  return true;
}

void Cell::computeVector()
{
  QLineF xAbsis(QPointF(0., 0.), QPointF(1., 0.));
  QLineF line(outsideForm.getCentroid(), insideForm.getCentroid());
  angle = line.angle(xAbsis);
}

void Cell::draw() const
{
  // draw outside form
  glColor3f(outsideColor.redF(), outsideColor.greenF(), outsideColor.blueF());
  outsideForm.draw();

  // draw inside form
  glColor3f(insideColor.redF(), insideColor.greenF(), insideColor.blueF());
  insideForm.draw();
}

void Cell::save(QDomDocument &doc, QDomElement &parentNode) const
{
  QDomElement CellNode = doc.createElement("cell");
  parentNode.appendChild(CellNode);

  insideForm.save(doc, CellNode, 0);
  outsideForm.save(doc, CellNode, 1);
}

bool Cell::load(QDomElement &node)
{
  // load data
  QDomElement FormElement = node.firstChildElement("polygon");
  while (!FormElement.isNull())
  {
    bool Ok = false;
    int Level = FormElement.attribute("level", "-1").toInt(&Ok, 10);
    if (Ok && Level >= 0)
    {
      if (Level == 0) insideForm.load(FormElement);
      if (Level == 1) outsideForm.load(FormElement);
    }

    FormElement = FormElement.nextSiblingElement("polygon");
  }

  // (re-)compute other data if loaded correctly
  if (isFull())
  {
    insideForm.computeData();
    outsideForm.computeData();
    computeVector();
    return true;
  }

  return false;
}
