#include "settingsView.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSpacerItem>

#include <cell.h>
#include <settings.h>
#include <dataCtrl.h>

SettingsView::SettingsView(QWidget *parent) :
  QDialog(parent)
{
  QGridLayout *mainLayout = new QGridLayout(this);

  averageArrow = new QCheckBox(tr("Dispaly the average arrow on every cell."), this);
  averageArrow->setChecked(Cell::_averageArrow);
  averageArrowVCil = new QCheckBox(tr("Dispaly the average arrow on every VCil."), this);
  averageArrowVCil->setChecked(VCil::_averageArrow);

  maximalCSDTxt = new QSpinBox(this);
  maximalCSDTxt->setMaximum(INT_MAX);
  maximalCSDTxt->setValue(DataCtrl::maximalCSD);

  QLabel *MaxCSDLbl(new QLabel(tr("CSD maximum threshold:"), this));

  mainLayout->addWidget(MaxCSDLbl,        0, 0, 1, 1);
  mainLayout->addWidget(maximalCSDTxt,    0, 1, 1, 1);
  mainLayout->addWidget(averageArrow,     1, 0, 1, 2);
  mainLayout->addWidget(averageArrowVCil, 2, 0, 1, 2);
  mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 3,0,1,2);

  connect(maximalCSDTxt,    SIGNAL(valueChanged(int)),  SIGNAL(maximalCSD(int)));
  connect(averageArrow,     SIGNAL(toggled(bool)),      SLOT(onAverageArrow(bool)));
  connect(averageArrowVCil, SIGNAL(toggled(bool)),      SLOT(onAverageArrowVCil(bool)));
}

void SettingsView::onAverageArrow(bool display)
{
  Cell::_averageArrow = display;
  Settings::Save();
}

void SettingsView::onAverageArrowVCil(bool display)
{
  VCil::_averageArrow = display;
  Settings::Save();
}
