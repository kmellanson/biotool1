#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dataCtrl.h"
#include "settingsView.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QLabel>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->actLoadImage, SIGNAL(triggered()), this, SLOT(doLoadImage()));
  connect(this, SIGNAL(onLoadWorkImage(QImage)), ui->imageView, SLOT(doChangeImage(QImage)));
  connect(ui->actCloseImage, SIGNAL(triggered()), ui->imageView, SLOT(doCloseImage()));
  connect(ui->actModeEdit, SIGNAL(triggered(bool)), this, SLOT(doChangeMode(bool)));
  connect(ui->actModeView, SIGNAL(triggered(bool)), this, SLOT(doChangeMode(bool)));
  connect(ui->actResetView, SIGNAL(triggered()), ui->imageView, SLOT(doResetView()));
  connect(ui->actNew, SIGNAL(triggered()), this, SLOT(doNew()));
  connect(ui->actSave, SIGNAL(triggered()), this, SLOT(doSave()));
  connect(ui->actSaveAs, SIGNAL(triggered()), this, SLOT(doSaveAs()));
  connect(ui->actOpen, SIGNAL(triggered()), this, SLOT(doOpen()));
  connect(&ui->imageView->data(), SIGNAL(countChanged(int, int)), this, SLOT(doCellCountChanged(int, int)));
  connect(&ui->imageView->data(), SIGNAL(angleChanged(int)), this, SLOT(doAngleChanged(int)));
  connect(ui->actExport, SIGNAL(triggered()), this, SLOT(doExport()));
  connect(ui->actSettings, SIGNAL(triggered()), this, SLOT(doSettings()));


  ui->actModeView->blockSignals(true);
  ui->actModeView->trigger();
  ui->actModeView->blockSignals(false);

  cellsLabel = new QLabel(" [000000 cells] ");
  cellsLabel->setAlignment(Qt::AlignLeft);
  cellsLabel->setMinimumSize(cellsLabel->sizeHint());
  doCellCountChanged(0, 0);

  angleLabel = new QLabel(" [-000 deg] ");
  angleLabel->setAlignment(Qt::AlignLeft);
  angleLabel->setMinimumSize(angleLabel->sizeHint());
  angleLabel->setVisible(false);
  doAngleChanged(0);

  statusBar()->addWidget(cellsLabel);
  statusBar()->addWidget(angleLabel);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type())
  {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

bool MainWindow::askForUnsavedChanges(const QString &title)
{
  return (ui->imageView->data().isSaved() ||
          QMessageBox::question(this,
                                title,
                                tr("Le travail en cours comporte des modification non sauvegardées.\n"
                                   "Etes-vous sûr de vouloir continuer?"),
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes);
}

void MainWindow::doLoadImage()
{
  QString filename(QFileDialog::getOpenFileName(this, tr("Choisir une image"), tr("."), tr("Images (*.png *.jpg *.jpeg *.tif *.tiff);;Tous (*)")));
  if (filename.isEmpty()) return;
  if (!QFileInfo(filename).isReadable())
  {
    QMessageBox::warning(this, tr("Erreur"), tr("Impossible de lire le fichier."));
    return;
  }
  QImage Image(filename);
  if (Image.isNull())
  {
    QMessageBox::warning(this, tr("Erreur"), tr("Format d'image incorrect."));
    return;
  }

  emit onLoadWorkImage(Image);
}

void MainWindow::doChangeMode(bool activated)
{
  QAction *action = (QAction*)sender();
  if (activated)
  {
    if (action == ui->actModeView)
    {
      ui->actModeEdit->blockSignals(true);
      ui->actModeEdit->trigger();
      ui->actModeEdit->blockSignals(false);
      ui->imageView->changeMode(ImageView::eModeView);
    }
    if (action == ui->actModeEdit)
    {
      ui->actModeView->blockSignals(true);
      ui->actModeView->trigger();
      ui->actModeView->blockSignals(false);
      ui->imageView->changeMode(ImageView::eModeEdit);
    }
  }
  else
  {
    action->blockSignals(true);
    action->trigger();
    action->blockSignals(false);
  }
}

void MainWindow::doNew()
{
  if (askForUnsavedChanges(tr("Nouveau document")))
  {
    ui->imageView->data().clear();
    fileName.clear();
  }
}

void MainWindow::doSave()
{
  if (fileName.isEmpty())
    doSaveAs();
  else
    ui->imageView->data().save(fileName);
}

void MainWindow::doSaveAs()
{
  const QString filename(QFileDialog::getSaveFileName(this, tr("Enregistrer sous"), tr("."), tr("Documents (*.xml);;Tous (*)")));
  if (!filename.isEmpty())
  {
    fileName = filename;
    doSave();
  }
}

void MainWindow::doOpen()
{
  if (askForUnsavedChanges(tr("Ouvrir un document")))
  {
    const QString filename(QFileDialog::getOpenFileName(this, tr("Ouvrir un document"), tr("."), tr("Documents (*.xml);;Tous (*)")));
    if (!filename.isEmpty())
    {
      fileName = filename;
      ui->imageView->data().load(fileName);
    }
  }
}

void MainWindow::doCellCountChanged(int ignored, int count)
{
  cellsLabel->setText(QString(" [%1/%2 cell%3] ").
                      arg(count - ignored).arg(count).arg(count?"s":""));
}

void MainWindow::doAngleChanged(int angle)
{
  angleLabel->setText(QString(" [%1 deg] ").arg(angle));
}

void MainWindow::doExport()
{
  const QString filename(QFileDialog::getSaveFileName(this, tr("Exporter sous"), tr("."), tr("Fichiers csv (*.csv)")));
  if (!filename.isEmpty())
    ui->imageView->data().exportCsv(filename);
}

void MainWindow::doSettings()
{
  SettingsView Settings(this);
  connect(&Settings, SIGNAL(minimalStrength(qreal)), &ui->imageView->data(), SLOT(setMinimalStrength(qreal)));
  Settings.exec();
}
