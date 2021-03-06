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

#ifndef QMAKE_ANDROIDRUNCONFIGURATION_H
#define QMAKE_ANDROIDRUNCONFIGURATION_H

#include <android/androidrunconfiguration.h>

namespace QmakeProjectManager { class QmakeProFileNode; }

namespace QmakeAndroidSupport {
namespace Internal {

class QmakeAndroidRunConfiguration : public Android::AndroidRunConfiguration
{
    Q_OBJECT
    friend class QmakeAndroidRunConfigurationFactory;

public:
    QmakeAndroidRunConfiguration(ProjectExplorer::Target *parent, Core::Id id, const QString &path = QString());

    QString proFilePath() const;

    bool isEnabled() const;
    QString disabledReason() const;

protected:
    QmakeAndroidRunConfiguration(ProjectExplorer::Target *parent, QmakeAndroidRunConfiguration *source);

    bool fromMap(const QVariantMap &map);
    QVariantMap toMap() const;

private slots:
    void proFileUpdated(QmakeProjectManager::QmakeProFileNode *pro, bool success, bool parseInProgress);

private:
    void init();

    mutable QString m_proFilePath;
    bool m_parseSuccess;
    bool m_parseInProgress;
};

} // namespace Internal
} // namespace QmakeAndroidSupport

#endif // QMAKE_ANDROIDRUNCONFIGURATION_H
