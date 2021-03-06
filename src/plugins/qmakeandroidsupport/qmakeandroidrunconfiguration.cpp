/**************************************************************************
**
** Copyright (c) 2014 BogDan Vatra <bog_dan_ro@yahoo.com>
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

#include "qmakeandroidrunconfiguration.h"

#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
#include <qtsupport/qtoutputformatter.h>
#include <qtsupport/qtkitinformation.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>

#include <utils/qtcassert.h>

namespace {
    QLatin1String PRO_FILE_KEY("QMakeProjectManager.QmakeAndroidRunConfiguration.ProFile");
}

using namespace ProjectExplorer;
using QmakeProjectManager::QmakeProject;

namespace QmakeAndroidSupport {
namespace Internal {

QmakeAndroidRunConfiguration::QmakeAndroidRunConfiguration(Target *parent, Core::Id id, const QString &path)
    : AndroidRunConfiguration(parent, id)
    , m_proFilePath(path)
{
    QmakeProject *project = static_cast<QmakeProject *>(parent->project());
    m_parseSuccess = project->validParse(m_proFilePath);
    m_parseInProgress = project->parseInProgress(m_proFilePath);
    init();
}

QmakeAndroidRunConfiguration::QmakeAndroidRunConfiguration(Target *parent, QmakeAndroidRunConfiguration *source)
    : AndroidRunConfiguration(parent, source)
    , m_proFilePath(source->m_proFilePath)
    , m_parseSuccess(source->m_parseSuccess)
    , m_parseInProgress(source->m_parseInProgress)
{
    init();
}

void QmakeAndroidRunConfiguration::init()
{
    connect(target()->project(), SIGNAL(proFileUpdated(QmakeProjectManager::QmakeProFileNode*,bool,bool)),
            this, SLOT(proFileUpdated(QmakeProjectManager::QmakeProFileNode*,bool,bool)));
}

bool QmakeAndroidRunConfiguration::fromMap(const QVariantMap &map)
{
    const QDir projectDir = QDir(target()->project()->projectDirectory().toString());
    m_proFilePath = QDir::cleanPath(projectDir.filePath(map.value(PRO_FILE_KEY).toString()));
    m_parseSuccess = static_cast<QmakeProject *>(target()->project())->validParse(m_proFilePath);
    m_parseInProgress = static_cast<QmakeProject *>(target()->project())->parseInProgress(m_proFilePath);

    return RunConfiguration::fromMap(map);
}

QVariantMap QmakeAndroidRunConfiguration::toMap() const
{
    if (m_proFilePath.isEmpty()) {
        if (!target()->project()->rootProjectNode())
            return QVariantMap();
        m_proFilePath = target()->project()->rootProjectNode()->path();
    }

    const QDir projectDir = QDir(target()->project()->projectDirectory().toString());
    QVariantMap map(RunConfiguration::toMap());
    map.insert(PRO_FILE_KEY, projectDir.relativeFilePath(m_proFilePath));
    return map;
}

bool QmakeAndroidRunConfiguration::isEnabled() const
{
    return m_parseSuccess && !m_parseInProgress;
}

QString QmakeAndroidRunConfiguration::disabledReason() const
{
    if (m_parseInProgress)
        return tr("The .pro file \"%1\" is currently being parsed.")
                .arg(QFileInfo(m_proFilePath).fileName());

    if (!m_parseSuccess)
        return static_cast<QmakeProject *>(target()->project())->disabledReasonForRunConfiguration(m_proFilePath);
    return QString();
}

void QmakeAndroidRunConfiguration::proFileUpdated(QmakeProjectManager::QmakeProFileNode *pro, bool success, bool parseInProgress)
{
    if (m_proFilePath.isEmpty() && target()->project()->rootProjectNode()) {
        m_proFilePath = target()->project()->rootProjectNode()->path();
    }

    if (m_proFilePath != pro->path())
        return;

    bool enabled = isEnabled();
    QString reason = disabledReason();
    m_parseSuccess = success;
    m_parseInProgress = parseInProgress;
    if (enabled != isEnabled() || reason != disabledReason())
        emit enabledChanged();
}

QString QmakeAndroidRunConfiguration::proFilePath() const
{
    return m_proFilePath;
}

} // namespace Internal
} // namespace Android
