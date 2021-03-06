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

#ifndef SELECTABLEFILESMODEL_H
#define SELECTABLEFILESMODEL_H

#include <QAbstractItemModel>
#include <QSet>
#include <QFutureInterface>
#include <QFutureWatcher>
#include <QDialog>
#include <QTreeView>
#include <QLabel>
#include "projectexplorer_export.h"

namespace Utils { class PathChooser; }

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace ProjectExplorer {

struct Tree
{
    QString name;
    Qt::CheckState checked;
    bool isDir;
    QList<Tree *> childDirectories;
    QList<Tree *> files;
    QList<Tree *> visibleFiles;
    QIcon icon;
    QString fullPath;
    Tree *parent;
};

struct Glob
{
    enum Mode { EXACT, ENDSWITH, REGEXP };
    Mode mode;
    QString matchString;
    mutable QRegExp matchRegexp;

    bool isMatch(const QString &text) const
    {
        if (mode == Glob::EXACT) {
            if (text == matchString)
                return true;
        } else if (mode == Glob::ENDSWITH) {
            if (text.endsWith(matchString))
                return true;
        } else if (mode == Glob::REGEXP) {
            if (matchRegexp.exactMatch(text))
                return true;
        }
        return false;
    }
};

class PROJECTEXPLORER_EXPORT SelectableFilesModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    SelectableFilesModel(QObject *parent);
    ~SelectableFilesModel();

    void setInitialMarkedFiles(const QStringList &files);

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QStringList selectedFiles() const;
    QStringList selectedPaths() const;
    QStringList preservedFiles() const;

    void startParsing(const QString &baseDir);
    void cancel();
    void applyFilter(const QString &selectFilesfilter, const QString &hideFilesfilter);

    void selectAllFiles();

signals:
    void parsingFinished();
    void parsingProgress(const QString &filename);

private slots:
    void buildTreeFinished();

private:
    QList<Glob> parseFilter(const QString &filter);
    Qt::CheckState applyFilter(const QModelIndex &index);
    bool filter(Tree *t);
    void run(QFutureInterface<void> &fi);
    void collectFiles(Tree *root, QStringList *result) const;
    void collectPaths(Tree *root, QStringList *result) const;
    void buildTree(const QString &baseDir, Tree *tree, QFutureInterface<void> &fi, int symlinkDepth);
    void deleteTree(Tree *tree);
    void propagateUp(const QModelIndex &index);
    void propagateDown(const QModelIndex &index);
    void selectAllFiles(Tree *root);
    Tree *m_root;
    // Used in the future thread need to all not used after calling startParsing
    QString m_baseDir;
    QSet<QString> m_files;
    QStringList m_outOfBaseDirFiles;
    QFutureWatcher<void> m_watcher;
    Tree *m_rootForFuture;
    int m_futureCount;
    bool m_allFiles;

    QList<Glob> m_hideFilesFilter;
    QList<Glob> m_showFilesFilter;
};

class PROJECTEXPLORER_EXPORT SelectableFilesDialogEditFiles : public QDialog
{
    Q_OBJECT

public:
    SelectableFilesDialogEditFiles(const QString &path, const QStringList &files, QWidget *parent);
    ~SelectableFilesDialogEditFiles();
    QStringList selectedFiles() const;

private slots:
    void applyFilter();
    void parsingProgress(const QString &fileName);
    void parsingFinished();

protected:
    void smartExpand(const QModelIndex &index);
    void createShowFileFilterControls(QVBoxLayout *layout);
    void createHideFileFilterControls(QVBoxLayout *layout);
    void createApplyButton(QVBoxLayout *layout);

    SelectableFilesModel *m_selectableFilesModel;

    QLabel *m_hideFilesFilterLabel;
    QLineEdit *m_hideFilesfilterLineEdit;

    QLabel *m_showFilesFilterLabel;
    QLineEdit *m_showFilesfilterLineEdit;

    QPushButton *m_applyFilterButton;

    QTreeView *m_view;
    QLabel *m_preservedFiles;
    QLabel *m_progressLabel;
};

class SelectableFilesDialogAddDirectory : public SelectableFilesDialogEditFiles
{
    Q_OBJECT

public:
    SelectableFilesDialogAddDirectory(const QString &path, const QStringList &files, QWidget *parent);

private slots:
    void validityOfDirectoryChanged(bool validState);
    void parsingFinished();
    void startParsing();

private:
    Utils::PathChooser *m_pathChooser;
    QLabel *m_sourceDirectoryLabel;
    QPushButton *m_startParsingButton;

    void setWidgetsEnabled(bool enabled);
    void createPathChooser(QVBoxLayout *layout, const QString &path);
};

} // namespace ProjectExplorer

#endif // SELECTABLEFILESMODEL_H

