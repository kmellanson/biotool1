#include "settings.h"

#include "cell.h"
#include "cellItem.h"
#include "dataCtrl.h"

#include <QtCore/QSettings>

bool   Cell::_averageArrow            (true);
QColor Cell::_insideColor             (QColor(0xff, 0x1f, 0x1f));
QColor Cell::_outsideColor            (QColor(0x1f, 0xff, 0x1f));
QColor Cell::_insideSelectedColor     (QColor(0x7f, 0x00, 0xff));
QColor Cell::_outsideSelectedColor    (QColor(0x00, 0x7f, 0xff));
QColor Cell::_vectorColor             (QColor(0x1f, 0x1f, 0xff));
QColor Cell::_averageVectorColor      (QColor(0x7f, 0x7f, 0xff));
bool   CellItem::_averageArrow        (false);
QColor CellItem::_insideColor         (QColor(0xdf, 0x9f, 0x9f));
QColor CellItem::_outsideColor        (QColor(0x9f, 0xdf, 0x9f));
QColor CellItem::_vectorColor         (QColor(0x00, 0x00, 0x00));
QColor CellItem::_averageVectorColor  (QColor(0x00, 0x00, 0x00));
qreal  CellItem::_arrowLength         (.1);
qreal  CellItem::_arrowHeadLength     (.025);
qreal  CellItem::_arrowHeadHalfWidth  (.0075);
qreal  CellItem::_arrowScale          (0.75);

QColor DataCtrl::centroidsRefColor  (QColor(0x7f, 0x7f, 0xff));
QVector<const DataCtrl::CSVDataType*> DataCtrl::csvSelection;

void Settings::Load()
{
  QSettings settings;

  Cell::_averageArrow            = settings.value("averageArrow",        Cell::_averageArrow).value<bool>();
  Cell::_insideColor             = settings.value("insideColor",         Cell::_insideColor).value<QColor>();
  Cell::_outsideColor            = settings.value("outsideColor",        Cell::_outsideColor).value<QColor>();
  Cell::_vectorColor             = settings.value("vectorColor",         Cell::_vectorColor).value<QColor>();
  Cell::_averageVectorColor      = settings.value("averageVectorColor",  Cell::_averageVectorColor).value<QColor>();
  CellItem::_arrowLength         = settings.value("arrowLength",         CellItem::_arrowLength).value<qreal>();
  CellItem::_arrowHeadLength     = settings.value("arrowHeadLength",     CellItem::_arrowHeadLength).value<qreal>();
  CellItem::_arrowHeadHalfWidth  = settings.value("arrowHeadHalfWidth",  CellItem::_arrowHeadHalfWidth).value<qreal>();
  CellItem::_arrowScale          = settings.value("arrowScale",          CellItem::_arrowScale).value<qreal>();

  DataCtrl::centroidsRefColor = settings.value("centroidsRefColor",   DataCtrl::centroidsRefColor).value<QColor>();
  DataCtrl::setSelectedDataTypesNames(settings.value("csvSelectedDataType", QStringList()).value<QStringList>());
}

void Settings::Save()
{
  QSettings settings;

  settings.setValue("averageArrow",         Cell::_averageArrow);
  settings.setValue("insideColor",          Cell::_insideColor);
  settings.setValue("outsideColor",         Cell::_outsideColor);
  settings.setValue("vectorColor",          Cell::_vectorColor);
  settings.setValue("averageVectorColor",   Cell::_averageVectorColor);
  settings.setValue("arrowLength",          CellItem::_arrowLength);
  settings.setValue("arrowHeadLength",      CellItem::_arrowHeadLength);
  settings.setValue("arrowHeadHalfWidth",   CellItem::_arrowHeadHalfWidth);
  settings.setValue("arrowScale",           CellItem::_arrowScale);

  settings.setValue("centroidsRefColor",    DataCtrl::centroidsRefColor);

  QStringList SelectedDataNames;
  DataCtrl::getSelectedDataTypesNames(SelectedDataNames);
  settings.setValue("csvSelectedDataType",  SelectedDataNames);
}
