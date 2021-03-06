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

#include "qmakeandroidrunfactories.h"
#include "qmakeandroidrunconfiguration.h"

#include <android/androidmanager.h>

#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <debugger/debuggerconstants.h>
#include <qmakeprojectmanager/qmakeproject.h>
#include <qmakeprojectmanager/qmakenodes.h>
#include <qtsupport/customexecutablerunconfiguration.h>
#include <qtsupport/qtkitinformation.h>
#include <qtsupport/qtsupportconstants.h>

using namespace Android;
using namespace ProjectExplorer;
using namespace QmakeProjectManager;

namespace QmakeAndroidSupport {
namespace Internal {

static const char ANDROID_RC_ID_PREFIX[] = "Qt4ProjectManager.AndroidRunConfiguration:";

static QString pathFromId(const Core::Id id)
{
    return id.suffixAfter(ANDROID_RC_ID_PREFIX);
}

QmakeAndroidRunConfigurationFactory::QmakeAndroidRunConfigurationFactory(QObject *parent)
    : ProjectExplorer::IRunConfigurationFactory(parent)
{
}

QString QmakeAndroidRunConfigurationFactory::displayNameForId(Core::Id id) const
{
    return QFileInfo(pathFromId(id)).completeBaseName();
}

bool QmakeAndroidRunConfigurationFactory::canCreate(Target *parent, Core::Id id) const
{
    if (!canHandle(parent))
        return false;
    return availableCreationIds(parent).contains(id);
}

bool QmakeAndroidRunConfigurationFactory::canRestore(Target *parent, const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

bool QmakeAndroidRunConfigurationFactory::canClone(Target *parent, RunConfiguration *source) const
{
    return canCreate(parent, source->id());
}

QList<Core::Id> QmakeAndroidRunConfigurationFactory::availableCreationIds(Target *parent, CreationMode mode) const
{
    if (!canHandle(parent))
        return QList<Core::Id>();

    QmakeProject *project = static_cast<QmakeProject *>(parent->project());
    QList<QmakeProFileNode *> nodes = project->allProFiles(QList<QmakeProjectType>()
                                                           << ApplicationTemplate
                                                           << LibraryTemplate);

    if (mode == AutoCreate)
        nodes = QmakeProject::nodesWithQtcRunnable(nodes);

    const Core::Id base = Core::Id(ANDROID_RC_ID_PREFIX);
    return QmakeProject::idsForNodes(base, nodes);
}

RunConfiguration *QmakeAndroidRunConfigurationFactory::doCreate(Target *parent, Core::Id id)
{
    if (parent->project()->rootProjectNode())
        return new QmakeAndroidRunConfiguration(parent, id, parent->project()->rootProjectNode()->path());
    return new QmakeAndroidRunConfiguration(parent, id);
}

RunConfiguration *QmakeAndroidRunConfigurationFactory::doRestore(Target *parent,
                                                            const QVariantMap &map)
{
    Core::Id id = ProjectExplorer::idFromMap(map);
    if (parent->project()->rootProjectNode())
        return new QmakeAndroidRunConfiguration(parent, id, parent->project()->rootProjectNode()->path());
    return new QmakeAndroidRunConfiguration(parent, id);
}

RunConfiguration *QmakeAndroidRunConfigurationFactory::clone(Target *parent, RunConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;

    QmakeAndroidRunConfiguration *old = static_cast<QmakeAndroidRunConfiguration *>(source);
    return new QmakeAndroidRunConfiguration(parent, old);
}

bool QmakeAndroidRunConfigurationFactory::canHandle(Target *t) const
{
    return t->project()->supportsKit(t->kit())
            && AndroidManager::supportsAndroid(t)
            && qobject_cast<QmakeProject *>(t->project());
}

#ifdef Q_CC_GCC
#  warning FIX ME !!!
#endif
QList<RunConfiguration *> QmakeAndroidRunConfigurationFactory::runConfigurationsForNode(Target *t, ProjectExplorer::Node *n)
{
    QList<ProjectExplorer::RunConfiguration *> result;
    foreach (ProjectExplorer::RunConfiguration *rc, t->runConfigurations())
        if (QmakeAndroidRunConfiguration *qt4c = qobject_cast<QmakeAndroidRunConfiguration *>(rc))
                if (qt4c->proFilePath() == n->path())
                    result << rc;
    return result;
}

} // namespace Internal
} // namespace Android
