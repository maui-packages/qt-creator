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

#ifndef GENERICPROJECT_H
#define GENERICPROJECT_H

#include "genericprojectmanager.h"
#include "genericprojectnodes.h"

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/target.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/buildconfiguration.h>
#include <coreplugin/idocument.h>

#include <QFuture>

namespace GenericProjectManager {
namespace Internal {

class GenericProjectFile;

class GenericProject : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    GenericProject(Manager *manager, const QString &filename);
    ~GenericProject();

    QString filesFileName() const;
    QString includesFileName() const;
    QString configFileName() const;

    QString displayName() const;
    Core::IDocument *document() const;
    ProjectExplorer::IProjectManager *projectManager() const;

    GenericProjectNode *rootProjectNode() const;
    QStringList files(FilesMode fileMode) const;

    QStringList buildTargets() const;

    bool addFiles(const QStringList &filePaths);
    bool removeFiles(const QStringList &filePaths);
    bool setFiles(const QStringList &filePaths);
    bool renameFile(const QString &filePath, const QString &newFilePath);

    enum RefreshOptions {
        Files         = 0x01,
        Configuration = 0x02,
        Everything    = Files | Configuration
    };

    void refresh(RefreshOptions options);

    QStringList projectIncludePaths() const;
    QStringList files() const;

protected:
    bool fromMap(const QVariantMap &map);

private:
    bool saveRawFileList(const QStringList &rawFileList);
    bool saveRawList(const QStringList &rawList, const QString &fileName);
    void parseProject(RefreshOptions options);
    QStringList processEntries(const QStringList &paths,
                               QHash<QString, QString> *map = 0) const;

    void refreshCppCodeModel();

    Manager *m_manager;
    QString m_fileName;
    QString m_filesFileName;
    QString m_includesFileName;
    QString m_configFileName;
    QString m_projectName;
    GenericProjectFile *m_creatorIDocument;
    GenericProjectFile *m_filesIDocument;
    GenericProjectFile *m_includesIDocument;
    GenericProjectFile *m_configIDocument;
    QStringList m_rawFileList;
    QStringList m_files;
    QHash<QString, QString> m_rawListEntries;
    QStringList m_rawProjectIncludePaths;
    QStringList m_projectIncludePaths;

    GenericProjectNode *m_rootNode;
    QFuture<void> m_codeModelFuture;
};

class GenericProjectFile : public Core::IDocument
{
    Q_OBJECT

public:
    GenericProjectFile(GenericProject *parent, QString fileName, GenericProject::RefreshOptions options);

    bool save(QString *errorString, const QString &fileName, bool autoSave);

    QString defaultPath() const;
    QString suggestedFileName() const;

    bool isModified() const;
    bool isSaveAsAllowed() const;

    ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const;
    bool reload(QString *errorString, ReloadFlag flag, ChangeType type);

private:
    GenericProject *m_project;
    GenericProject::RefreshOptions m_options;
};

} // namespace Internal
} // namespace GenericProjectManager

#endif // GENERICPROJECT_H
