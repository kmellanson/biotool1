#pragma once

#include <QtWidgets/QDialog>

namespace Ui { class CsvExportOptions; }

class QModelIndex;

class CsvExportOptions : public QDialog
{
  Q_OBJECT

  public:
    CsvExportOptions(QWidget *parent = nullptr);

  public slots:
    void accept();

  protected slots:
    void addDataType(const QModelIndex &index);
    void delDataType(const QModelIndex &index);

  protected:
    Ui::CsvExportOptions *ui;
};
