#include "dataCtrl.h"

#include <QtOpenGL/QGLContext>
#include <QtXml/QDomDocument>
#include <QtCore/QFile>

#include <cmath>

#include "settings.h"

QVector<DataCtrl::CSVDataType> DataCtrl::csvDataTypes;

DataCtrl::DataCtrl(QObject *parent):
  QAbstractItemModel(parent), saved(true), cntMode(eModeView),
  averageAngleVPatch(0.), averageAngleVBeating(0.), averageCenroidRadius(0.)
{
  // csv data type callbacks

  function<QString (const Cell&)> csvStrength = [](const Cell &cell) -> QString
  {
    return QString::number(cell.getStrength());
  };

  function<QString (const Cell&)> csvAngleVPatch = [this](const Cell &cell) -> QString
  {
    qreal interval  = cell.getInterval();
    qreal angle     = cell.getAngle() - averageAngleVPatch;
    if (angle > 180.) angle -= 360.;

    return interval > averageCenroidRadius?QString::number(angle):"";
  };

  function<QString (const Cell&)> csvAngleVBeating = [this](const Cell &cell) -> QString
  {
    qreal angle = cell.getVCilBeatingAngle() - averageAngleVBeating;
    if (angle >  180.) angle -= 360.;
    if (angle < -180.) angle += 360.;

    return QString::number(angle);
  };

  function<QString (const Cell&)> csvAreaPrecentile = [](const Cell &cell) -> QString
  {
    return QString::number(cell.getAreaRatio() * 100.);
  };

  function<QString (const Cell&)> csvCircualrStandardDeviation = [](const Cell &cell) -> QString
  {
    return QString::number(cell.getVCilCircularStandardDeviation());
  };

  function<QString (const Cell&)> csvVBeatingToAngle = [this](const Cell &cell) -> QString
  {
    qreal interval  = cell.getInterval();
    qreal angle     = cell.getAngle() - cell.getVCilBeatingAngle();
    if (angle >  180.) angle -= 360.;
    if (angle < -180.) angle += 360.;

    return interval > averageCenroidRadius?QString::number(angle):"";
  };

  // init csv data types
  csvDataTypes.append(CSVDataType(tr("Strength"),                       "Str",    csvStrength));                  // strength
  csvDataTypes.append(CSVDataType(tr("Area percentile"),                "Area" ,  csvAreaPrecentile));            // area percentile
  csvDataTypes.append(CSVDataType(tr("Angle vPatch"),                   "Apatch", csvAngleVPatch));               // angle vPatch
  csvDataTypes.append(CSVDataType(tr("Angle vBeating"),                 "Abeat",  csvAngleVBeating));             // angle vBeating
  csvDataTypes.append(CSVDataType(tr("Angle vBeating / Angle vPatch"),  "Ab2p",   csvVBeatingToAngle));           // angle vBeating / angle vPatch
  csvDataTypes.append(CSVDataType(tr("Circular Standard Deviation"),    "Csd",    csvCircualrStandardDeviation)); // circular standard deviation

  Settings::Load();

  // default selection
  if (csvSelection.isEmpty())
  {
    csvSelection.append(&csvDataTypes[0]);
    csvSelection.append(&csvDataTypes[1]);
  }

  Settings::Save();
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
      if (Cell::edited())
      {
        Cell::edited()->draw(averageAngleVPatch, averageCenroidRadius);
        cell.draw();
        break;
      }
    case eModeView:
      foreach (const Cell &CellItem, cells)
        CellItem.draw(averageAngleVPatch, averageCenroidRadius);
      cell.draw();
      break;
    case eModeDefineCentroid:
      glColor3f(centroidsRefColor.redF(), centroidsRefColor.greenF(), centroidsRefColor.blueF());
      foreach (const Polygon &CellItem, centroidsRef)
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

QModelIndex DataCtrl::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  if (cells.size() > row)
    return createIndex(row, column, (void*)&cells[row]);

  return QModelIndex();
}

QModelIndex DataCtrl::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);
  return QModelIndex();
}

int DataCtrl::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;
  return cells.count();
}

int DataCtrl::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return cells.count()?1:0;
}

QVariant DataCtrl::data(const QModelIndex &index, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();
  return QString("Cell %1").arg(index.row() + 1);
}

void DataCtrl::getDataTypesNames(QStringList &names)
{
  names.clear();
  foreach(const CSVDataType &_csvDataType, csvDataTypes)
    names.append(_csvDataType.name);
}

void DataCtrl::getSelectedDataTypesNames(QStringList &names)
{
  names.clear();
  foreach(const CSVDataType *_csvDataType, csvSelection)
    names.append(_csvDataType->name);
}

void DataCtrl::setSelectedDataTypesNames(const QStringList &names)
{
  csvSelection.clear();
  foreach(const QString &Name, names)
    foreach(const CSVDataType &_csvDataType, csvDataTypes)
      if (_csvDataType.name == Name)
      {
        csvSelection.append(&_csvDataType);
        break;
      }
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
        if (!Cell::edited())
        {
          beginInsertRows(QModelIndex(), cells.count()-1, cells.count()-1);
          cells.push_back(cell);
          cell.clear();
          refresh();
          endInsertRows();
          endResetModel();
        }
        else
        {
          Cell::edited()->addVCil(cell);
          refresh();
          cell.clear();
        }
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
        if (!Cell::edited())
        {
          if (!cells.isEmpty())
          {
            beginRemoveRows(QModelIndex(), cells.count()-1, cells.count()-1);
            cell = cells.last();
            cells.pop_back();
            cell.clearOneForm();
            refresh();
            saved = false;
            endRemoveRows();
          }
        }
        else
        {
          Cell::edited()->removeLastForm(cell);
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

void DataCtrl::removeSelectedForm()
{
  const Cell *Selected = Cell::selected();
  if (Selected)
    for (int i = cells.count(); --i >= 0; )
      if (&cells.at(i) == Cell::selected())
      {
        cells.remove(i);
        refresh();
        break;
      }
}

void DataCtrl::startEditSelectedForm()
{
  if (Cell::selected()) Cell::selected()->setEdited();
}

void DataCtrl::stopEditSelectedForm()
{
  Cell::stopEdition();
}

void DataCtrl::setSelection(const QModelIndex &selected)
{
  ((Cell*)selected.internalPointer())->setSelected();
}

void DataCtrl::clear()
{
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
  foreach(Polygon _cellPoly, centroidsRef) _cellPoly.save(Doc, CentroidsRed, 0);

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
  refresh();

  QByteArray CSV;

  QStringList Values;
  foreach(Cell _cell, cells)
  {
    Values.clear();
    foreach(const CSVDataType *_csvDataType, csvSelection)
      Values.append(_csvDataType->value(_cell));
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

QString DataCtrl::getCsvSuffix() const
{
  QStringList Suffixes;
  foreach(const CSVDataType *_csvDataType, csvSelection)
    Suffixes << _csvDataType->suffix;
  return "-" + Suffixes.join("_");
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

  beginInsertRows(QModelIndex(), -1, -1);

  QDomElement Element = Doc.documentElement().firstChildElement();
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
        Polygon LoadedCellPoly;
        LoadedCellPoly.load(CellPolyElement);
        LoadedCellPoly.computeData();
        centroidsRef.push_back(LoadedCellPoly);
        CellPolyElement = CellPolyElement.nextSiblingElement("polygon");
      }
    }
    Element = Element.nextSiblingElement();
  }

  refresh();

  endInsertRows();
  endResetModel();

  saved = true;
}

void DataCtrl::refresh()
{
  averageAngleVPatch = 0.f;
  averageAngleVBeating = 0.f;
  averageCenroidRadius = 0.f;
  const int cellsCount = cells.count();
  if (!cellsCount)
  {
    emit angleVPatchChanged(0);
    emit angleVBeatingChanged(0);
    emit countChanged(0, 0);
    return;
  }

  qreal pSinSum(0.f), pCosSum(0.f), bSinSum(0.f), bCosSum(0.f);

  int ignored = 0;

  const int centroidsRefCount = centroidsRef.count();
  if (centroidsRefCount)
  {
    foreach (Polygon _poly, centroidsRef)
      averageCenroidRadius += _poly.getRadius();
    averageCenroidRadius /= centroidsRefCount;
  }

  foreach(Cell _cell, cells)
  {
    // vPatch
    if (_cell.getInterval() > averageCenroidRadius)
    {
      const qreal pAngle = _cell.getAngle() * M_PI / 180.f;
      pSinSum += sin(pAngle);
      pCosSum += cos(pAngle);
    }
    else
      ++ignored;

    // vBeating
    const qreal bAngle = _cell.getVCilBeatingAngle() * M_PI / 180.f;
    bSinSum += sin(bAngle);
    bCosSum += cos(bAngle);
  }


  averageAngleVPatch    = atan2(pSinSum, pCosSum) * 180.f / M_PI;
  averageAngleVBeating  = atan2(bSinSum, bCosSum) * 180.f / M_PI;

  emit angleVPatchChanged(averageAngleVPatch);
  emit angleVBeatingChanged(averageAngleVBeating);
  emit countChanged(ignored, cellsCount);
}
