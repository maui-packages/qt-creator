/****************************************************************************
**
** Copyright (C) 2014 Tim Sander <tim@krieglstein.org>
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

#ifndef BAREMETALRUNCONFIGURATION_H
#define BAREMETALRUNCONFIGURATION_H

#include <projectexplorer/runconfiguration.h>

namespace BareMetal {
namespace Internal {

class BareMetalRunConfigurationWidget;

class BareMetalRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
    Q_DISABLE_COPY(BareMetalRunConfiguration)

    friend class BareMetalRunConfigurationFactory;
    friend class BareMetalRunConfigurationWidget;

public:
    explicit BareMetalRunConfiguration(ProjectExplorer::Target *parent, Core::Id id,
                                       const QString &projectFilePath);

    bool isEnabled() const;
    QString disabledReason() const;
    QWidget *createConfigurationWidget();
    Utils::OutputFormatter *createOutputFormatter() const;

    QString localExecutableFilePath() const;
    QString arguments() const;
    void setArguments(const QString &args);
    QString workingDirectory() const;
    void setWorkingDirectory(const QString &wd);

    QVariantMap toMap() const;

    QString projectFilePath() const;

    static const char *IdPrefix;

signals:
    void deploySpecsChanged();
    void targetInformationChanged() const;

protected:
    BareMetalRunConfiguration(ProjectExplorer::Target *parent, BareMetalRunConfiguration *source);
    bool fromMap(const QVariantMap &map);
    QString defaultDisplayName();
    void setDisabledReason(const QString &reason) const;

protected slots:
    void updateEnableState() { emit enabledChanged(); }

private slots:
    void handleBuildSystemDataUpdated();

private:
    void init();

    QString m_projectFilePath;
    QString m_gdbPath;
    QString m_arguments;
    mutable QString m_disabledReason;
    QString m_workingDirectory;
};

} // namespace Internal
} // namespace BareMetal

#endif // BAREMETALRUNCONFIGURATION_H
