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

#include "modelnodecontextmenu.h"
#include "modelnodecontextmenu_helper.h"
#include "designeractionmanager.h"
#include <qmldesignerplugin.h>

#include <modelnode.h>

#include <utils/algorithm.h>

#include <QSet>

namespace QmlDesigner {

ModelNodeContextMenu::ModelNodeContextMenu(AbstractView *view) :
    m_selectionContext(view)
{
}

static QSet<ActionInterface* > findMembers(QSet<ActionInterface* > actionInterface,
                                                          const QString &category)
{
    QSet<ActionInterface* > ret;

     foreach (ActionInterface* factory, actionInterface) {
         if (factory->category() == category)
             ret.insert(factory);
     }
     return ret;
}


void populateMenu(QSet<ActionInterface* > &actionInterfaces,
                  const QString &category,
                  QMenu* menu,
                  const SelectionContext &selectionContext)
{
    QSet<ActionInterface* > matchingFactories = findMembers(actionInterfaces, category);

    actionInterfaces.subtract(matchingFactories);

    QList<ActionInterface* > matchingFactoriesList = matchingFactories.toList();
    Utils::sort(matchingFactoriesList, [](ActionInterface *l, ActionInterface *r) {
        return l->priority() > r->priority();
    });

    foreach (ActionInterface* actionInterface, matchingFactoriesList) {
       if (actionInterface->type() == ActionInterface::Menu) {
           actionInterface->currentContextChanged(selectionContext);
           QMenu *newMenu = actionInterface->action()->menu();
           menu->addMenu(newMenu);

           //recurse

           populateMenu(actionInterfaces, actionInterface->menuId(), newMenu, selectionContext);
       } else if (actionInterface->type() == ActionInterface::Action) {
           QAction* action = actionInterface->action();
           actionInterface->currentContextChanged(selectionContext);
           menu->addAction(action);
       }
    }
}

void ModelNodeContextMenu::execute(const QPoint &position, bool selectionMenuBool)
{
    QMenu* mainMenu = new QMenu();

    m_selectionContext.setShowSelectionTools(selectionMenuBool);
    m_selectionContext.setScenePosition(m_scenePos);


     QSet<ActionInterface* > factories =
             QSet<ActionInterface* >::fromList(QmlDesignerPlugin::instance()->designerActionManager().designerActions());

     populateMenu(factories, QString(""), mainMenu, m_selectionContext);

     mainMenu->exec(position);
     mainMenu->deleteLater();
}

void ModelNodeContextMenu::setScenePos(const QPoint &position)
{
    m_scenePos = position;
}

void ModelNodeContextMenu::showContextMenu(AbstractView *view,
                                           const QPoint &globalPosition,
                                           const QPoint &scenePosition,
                                           bool showSelection)
{
    ModelNodeContextMenu contextMenu(view);
    contextMenu.setScenePos(scenePosition);
    contextMenu.execute(globalPosition, showSelection);
}

} //QmlDesigner
