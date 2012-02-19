#include "dataCtrl.h"

#include <QtOpenGL/QGLContext>
#include <QtXml/QDomDocument>
#include <QtCore/QFile>

#include <QtCore/QDebug>

#include <cmath>

QVector<DataCtrl::CSVDataType> DataCtrl::csvDataTypes;

DataCtrl::DataCtrl(QObject *parent):
  QStandardItemModel(parent), saved(true), cntMode(eModeView),
  averageAngle(0.), averageCenroidRadius(0.),
  rootItem(invisibleRootItem())
{

  // csv data type callbacks
  static auto csvStrength = [](const DataCtrl *, const Cell &cell)
  {
    return QString::number(cell.getStrength());
  };

  static auto csvAngle = [](const DataCtrl *me, const Cell &cell)
  {
    qreal interval  = cell.getInterval();
    qreal angle     = cell.getAngle() - me->averageAngle;
    if (angle > 180.) angle -= 360.;

    return interval > me->averageCenroidRadius?QString::number(angle):"";
  };

  static auto csvAreaPrecentile = [](const DataCtrl *, const Cell &cell)
  {
    return QString::number(cell.getAreaRatio() * 100.);
  };

  // init csv data types
  csvDataTypes.append(CSVDataType(tr("Strength"),         "st", csvStrength));        // strength
  csvDataTypes.append(CSVDataType(tr("Angle"),            "an", csvAngle));           // angle
  csvDataTypes.append(CSVDataType(tr("Area percentile"),  "ap", csvAreaPrecentile));  // area percentile

  csvSelection.append(&csvDataTypes[0]);
  csvSelection.append(&csvDataTypes[1]);
  csvSelection.append(&csvDataTypes[2]);
}

void DataCtrl::addPoint(const QPointF &point)
{
  points.append(point);
  saved = false;
}

void DataCtrl::draw() const
{
  glLineWidth(3.);
  glPointSize(5.);

  switch (cntMode)
  {
    case eModeEdit:
    case eModeView:
      foreach (Cell CellItem, cells)
        CellItem.draw(averageAngle, averageCenroidRadius);
      cell.draw();
      break;
    case eModeDefineCentroid:
      glColor3f(centroidsRefColor.redF(), centroidsRefColor.greenF(), centroidsRefColor.blueF());
      foreach (CellPolygon CellItem, centroidsRef)
        CellItem.draw();
  }

  points.draw();
}

QVariant DataCtrl::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(orientation);

  if (role != Qt::DisplayRole)
    return QVariant();

  if (section == 0)
    return QString("Cells");

  return QAbstractItemModel::headerData(section, orientation, role);
}

void DataCtrl::removeLastPoint()
{
  if (points.count())
  {
    points.pop_back();
    saved = false;
  }
}

void DataCtrl::finalizeForm()
{
  if (!points.count()) return;
  points.computeData();
  switch (cntMode)
  {
    case eModeEdit:
    case eModeView:
      if (cell.addOneForm(points))
      {
        cells.push_back(cell);
        cell.clear();
        qDebug() << "new cell" << cells.count();

        QStandardItemModel::clear();
        for (int i = 0; ++i <= cells.count();)
          appendRow(new QStandardItem(QString("Cell %0").arg(i)));
        refresh();
      }
      break;
    case eModeDefineCentroid:
      centroidsRef.push_back(points);
      cell.clear();
      refresh();
      break;
  }
  points.clear();
  saved = false;
}

void DataCtrl::removeLastForm()
{
  switch (cntMode)
  {
    case eModeEdit:
    case eModeView:
      if (cell.isEmpty())
      {
        if (!cells.isEmpty())
        {
          cell = cells.last();
          cells.pop_back();
          cell.clearOneForm();
          refresh();
          saved = false;
        }
        return;
      }
      if (cell.clearOneForm() && !cells.isEmpty())
        refresh();
      break;
    case eModeDefineCentroid:
      if (!centroidsRef.isEmpty())
      {
        centroidsRef.pop_back();
        refresh();
      }
      break;
  }

  saved = false;
}

void DataCtrl::clear()
{
  QStandardItemModel::clear();
  points.clear();
  cell.clear();
  cells.clear();
  centroidsRef.clear();
  refresh();
  saved = true;
}

void DataCtrl::save(const QString &filename)
{
  QDomDocument Doc("document");
  QDomElement Root = Doc.createElement("document");
  Doc.appendChild(Root);

  QDomElement Cells = Doc.createElement("cells");
  Root.appendChild(Cells);
  foreach(Cell _cell, cells) _cell.save(Doc, Cells);

  QDomElement CentroidsRed = Doc.createElement("centroid_references");
  Root.appendChild(CentroidsRed);
  foreach(CellPolygon _cellPoly, centroidsRef) _cellPoly.save(Doc, CentroidsRed, 0);

  QString FileName(filename);
  if (!FileName.endsWith(".xml"))
    FileName.append(".xml");
  QFile File(FileName);
  if (File.open(QFile::WriteOnly))
  {
    File.write(Doc.toString(2).toUtf8());
    File.close();
  }

  saved = true;
}

void DataCtrl::exportCsv(const QString &filename)
{
  QByteArray CSV;

  QStringList Values;
  foreach(Cell _cell, cells)
  {
    Values.clear();
    foreach(CSVDataType *_csvDataType, csvSelection)
      Values.append(_csvDataType->value(this, _cell));
    CSV.append(QString("%1\n").arg(Values.join(";")));
  }

  QString FileName(filename);
  if (!FileName.endsWith(".csv"))
    FileName.append(".csv");
  QFile File(FileName);
  if (File.open(QFile::WriteOnly))
  {
    File.write(CSV);
    File.close();
  }
}

void DataCtrl::load(const QString &filename)
{
  QDomDocument Doc("document");
  QFile File(filename);
  if (!File.open(QIODevice::ReadOnly))
    return;

  if (!Doc.setContent(&File))
  {
    File.close();
    return;
  }
  File.close();

  clear();

  QDomElement DocElem = Doc.documentElement();

  QDomElement Element = DocElem.firstChildElement();
  while (!Element.isNull())
  {
    if (Element.tagName() == "cells")
    {
      QDomElement CellElement = Element.firstChildElement("cell");
      while (!CellElement.isNull())
      {
        Cell LoadedCell;
        if (LoadedCell.load(CellElement))
          cells.push_back(LoadedCell);
        CellElement = CellElement.nextSiblingElement("cell");
      }
    }
    if (Element.tagName() == "centroid_references")
    {
      QDomElement CellPolyElement = Element.firstChildElement("polygon");
      while (!CellPolyElement.isNull())
      {
        CellPolygon LoadedCellPoly;
        LoadedCellPoly.load(CellPolyElement);
        LoadedCellPoly.computeData();
        centroidsRef.push_back(LoadedCellPoly);
        CellPolyElement = CellPolyElement.nextSiblingElement("polygon");
      }
    }
    Element = Element.nextSiblingElement();
  }

  refresh();
  saved = true;
}

void DataCtrl::refresh()
{
  averageAngle = 0.f;
  averageCenroidRadius = 0.f;
  const int cellsCount = cells.count();
  if (!cellsCount)
  {
    emit angleChanged(0.f);
    emit countChanged(0, 0);
    return;
  }

  qreal sinsum(0.f), cossum(0.f);

  int ignored = 0;

  const int centroidsRefCount = centroidsRef.count();
  if (centroidsRefCount)
  {
    foreach (CellPolygon _poly, centroidsRef)
      averageCenroidRadius += _poly.getRadius();
    averageCenroidRadius /= centroidsRefCount;
  }

  foreach(Cell _cell, cells)
    if (_cell.getInterval() > averageCenroidRadius)
    {
      const qreal angle = _cell.getAngle() * M_PI / 180.f;
      sinsum += sin(angle);
      cossum += cos(angle);
    }
    else
      ++ignored;

  averageAngle = atan2(sinsum, cossum) * 180.f / M_PI;
  emit angleChanged(averageAngle);
  emit countChanged(ignored, cellsCount);
}
