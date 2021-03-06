/****************************************************************************
**
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

#include "javafilewizard.h"
#include "androidconstants.h"

#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>

using namespace Android;
using namespace Android::Internal;

JavaFileWizard::JavaFileWizard()
{
    setWizardKind(Core::IWizardFactory::FileWizard);
    setCategory(QLatin1String(Constants::JAVA_WIZARD_CATEGORY));
    setDisplayCategory(QCoreApplication::translate("Android", Constants::JAVA_DISPLAY_CATEGORY));
    setDescription(tr("Creates a Java file with boilerplate code."));
    setDisplayName(tr("Java File"));
    setId(QLatin1String(Constants::WIZARD_JAVA));
}

Core::GeneratedFiles JavaFileWizard::generateFilesFromPath(const QString &path,
                                                          const QString &name,
                                                          QString * /*errorMessage*/) const

{
    const QString mimeType = QLatin1String(Android::Constants::JAVA_MIMETYPE);
    QString capitalizedName = name;
    if (!capitalizedName.isEmpty())
        capitalizedName[0] = capitalizedName[0].toUpper();
    const QString fileName = Core::BaseFileWizardFactory::buildFileName(path, capitalizedName, preferredSuffix(mimeType));

    Core::GeneratedFile file(fileName);
    file.setContents(fileContents(fileName));
    file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);
    return Core::GeneratedFiles() << file;
}

QString JavaFileWizard::fileContents(const QString &path) const
{
    QString contents;
    QTextStream str(&contents);

    QString className = QFileInfo(path).baseName();

    str << QString::fromLatin1("public class %1 {\n").arg(className)
        << QLatin1String("\n")
        << QLatin1String("}\n")
        << QLatin1String("\n");
    return contents;
}
