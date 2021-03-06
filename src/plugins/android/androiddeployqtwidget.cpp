/**************************************************************************
**
** Copyright (c) 2014 BogDan Vatra <bog_dan_ro@yahoo.com>
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "androiddeployqtwidget.h"
#include "ui_androiddeployqtwidget.h"

#include "androiddeployqtstep.h"
#include "androidmanager.h"

#include <QFileDialog>

using namespace Android;
using namespace Internal;

AndroidDeployQtWidget::AndroidDeployQtWidget(AndroidDeployQtStep *step)
    : ProjectExplorer::BuildStepConfigWidget(),
      m_ui(new Ui::AndroidDeployQtWidget),
      m_step(step)
{
    m_ui->setupUi(this);

    m_ui->uninstallPreviousPackage->setChecked(m_step->uninstallPreviousPackage());
    connect(m_ui->installMinistroButton, SIGNAL(clicked()), SLOT(installMinistro()));
    connect(m_ui->cleanLibsPushButton, SIGNAL(clicked()), SLOT(cleanLibsOnDevice()));
    connect(m_ui->resetDefaultDevices, SIGNAL(clicked()), SLOT(resetDefaultDevices()));
    connect(m_ui->uninstallPreviousPackage, SIGNAL(toggled(bool)),
            m_step, SLOT(setUninstallPreviousPackage(bool)));

}

AndroidDeployQtWidget::~AndroidDeployQtWidget()
{
    delete m_ui;
}

QString AndroidDeployQtWidget::displayName() const
{
    return tr("<b>Deploy configurations</b>");
}

QString AndroidDeployQtWidget::summaryText() const
{
    return displayName();
}

void AndroidDeployQtWidget::installMinistro()
{
    QString packagePath =
        QFileDialog::getOpenFileName(this, tr("Qt Android Smart Installer"),
                                     QDir::homePath(), tr("Android package (*.apk)"));
    if (!packagePath.isEmpty())
        AndroidManager::installQASIPackage(m_step->target(), packagePath);
}

void AndroidDeployQtWidget::cleanLibsOnDevice()
{
    AndroidManager::cleanLibsOnDevice(m_step->target());
}

void AndroidDeployQtWidget::resetDefaultDevices()
{
    AndroidConfigurations::clearDefaultDevices(m_step->project());
}

