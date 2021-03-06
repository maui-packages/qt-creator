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

#ifndef CPPTYPEHIERARCHY_H
#define CPPTYPEHIERARCHY_H

#include <coreplugin/inavigationwidgetfactory.h>

#include <QString>
#include <QWidget>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QModelIndex;
class QStackedLayout;
class QStandardItem;
class QStandardItemModel;
template <class> class QVector;
template <class> class QList;
QT_END_NAMESPACE

namespace Core { class IEditor; }

namespace Utils {
class NavigationTreeView;
class AnnotatedItemDelegate;
}

namespace CppEditor {
namespace Internal {

class CppEditorWidget;
class CppClass;
class CppClassLabel;

class CppTypeHierarchyWidget : public QWidget
{
    Q_OBJECT
public:
    CppTypeHierarchyWidget();
    virtual ~CppTypeHierarchyWidget();

public slots:
    void perform();

private slots:
    void onItemClicked(const QModelIndex &index);

private:
    typedef QList<CppClass> CppClass::*HierarchyMember;
    void buildHierarchy(const CppClass &cppClass, QStandardItem *parent,
                        bool isRoot, HierarchyMember member);
    void showNoTypeHierarchyLabel();
    void showTypeHierarchy();
    void clearTypeHierarchy();

    CppEditorWidget *m_cppEditor;
    Utils::NavigationTreeView *m_treeView;
    QWidget *m_hierarchyWidget;
    QStackedLayout *m_stackLayout;
    QStandardItemModel *m_model;
    Utils::AnnotatedItemDelegate *m_delegate;
    CppClassLabel *m_inspectedClass;
    QLabel *m_noTypeHierarchyAvailableLabel;
};

class CppTypeHierarchyFactory : public Core::INavigationWidgetFactory
{
    Q_OBJECT

public:
    CppTypeHierarchyFactory();

    virtual Core::NavigationView createWidget();
};

} // namespace Internal
} // namespace CppEditor

#endif // CPPTYPEHIERARCHY_H
