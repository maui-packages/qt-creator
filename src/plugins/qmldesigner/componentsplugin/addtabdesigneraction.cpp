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

#include "addtabdesigneraction.h"
#include "addtabtotabviewdialog.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QUrl>
#include <QFileInfo>
#include <coreplugin/icore.h>

#include <documentmanager.h>
#include <nodemetainfo.h>
#include <modelnode.h>
#include <nodeabstractproperty.h>
#include <qmldesignerplugin.h>

namespace QmlDesigner {

bool isTabView(const ModelNode &modelNode)
{
    return modelNode.metaInfo().isSubclassOf("QtQuick.Controls.TabView", -1, -1);
}

bool isTabAndParentIsTabView(const ModelNode &modelNode)
{
    return modelNode.metaInfo().isSubclassOf("QtQuick.Controls.Tab", -1, -1)
            && modelNode.hasParentProperty()
            && modelNode.parentProperty().parentModelNode().metaInfo().isSubclassOf("QtQuick.Controls.TabView", -1, -1);
}

AddTabDesignerAction::AddTabDesignerAction()
    : AbstractAction(QCoreApplication::translate("TabViewToolAction","Add Tab..."))
{
    connect(action(), SIGNAL(triggered()), this, SLOT(addNewTab()));
}

QByteArray AddTabDesignerAction::category() const
{
    return QByteArray();
}

QByteArray AddTabDesignerAction::menuId() const
{
    return "TabViewAction";
}

int AddTabDesignerAction::priority() const
{
    return CustomActionsPriority;
}

ActionInterface::Type AddTabDesignerAction::type() const
{
    return Action;
}

bool AddTabDesignerAction::isVisible(const SelectionContext &selectionContext) const
{
    if (selectionContext.singleNodeIsSelected()) {
        ModelNode selectedModelNode = selectionContext.currentSingleSelectedNode();
        return isTabView(selectedModelNode) || isTabAndParentIsTabView(selectedModelNode);
    }

    return false;
}

bool AddTabDesignerAction::isEnabled(const SelectionContext &selectionContext) const
{
    return isVisible(selectionContext);
}

static ModelNode findTabViewModelNode(const ModelNode &currentModelNode)
{
    if (currentModelNode.metaInfo().isSubclassOf("QtQuick.Controls.TabView", -1, -1))
        return currentModelNode;
    else
        return findTabViewModelNode(currentModelNode.parentProperty().parentModelNode());
}

void AddTabDesignerAction::addNewTab()
{
    QString tabName = AddTabToTabViewDialog::create(QStringLiteral("Tab"), Core::ICore::mainWindow());

    if (!tabName.isEmpty()) {
        QString directoryPath = QFileInfo(selectionContext().view()->model()->fileUrl().toLocalFile()).absolutePath();
        QString newFilePath = directoryPath +QStringLiteral("/") + tabName + QStringLiteral(".qml");

        if (QFileInfo(newFilePath).exists()) {
            QMessageBox::warning(Core::ICore::mainWindow(), tr("Naming Error"), tr("Component already exists."));
        } else {
            const QString sourceString = QStringLiteral("import QtQuick 2.1\nimport QtQuick.Controls 1.0\n\nItem {\n    anchors.fill: parent\n}");
            bool fileCreated = DocumentManager::createFile(newFilePath, sourceString);

            if (fileCreated) {
                DocumentManager::addFileToVersionControl(directoryPath, newFilePath);

                ModelNode tabViewModelNode = findTabViewModelNode(selectionContext().currentSingleSelectedNode());

                PropertyListType propertyList;
                propertyList.append(QPair<PropertyName, QVariant>("source", QString(tabName + QStringLiteral(".qml"))));
                propertyList.append(QPair<PropertyName, QVariant>("title", tabName));

                ModelNode newTabModelNode = selectionContext().view()->createModelNode("QtQuick.Controls.Tab",
                                                                                       tabViewModelNode.majorVersion(),
                                                                                       tabViewModelNode.minorVersion(),
                                                                                       propertyList);
                newTabModelNode.setIdWithRefactoring(newTabModelNode.view()->generateNewId(tabName));
                tabViewModelNode.defaultNodeAbstractProperty().reparentHere(newTabModelNode);
            }
        }
    }
}

} // namespace QmlDesigner
