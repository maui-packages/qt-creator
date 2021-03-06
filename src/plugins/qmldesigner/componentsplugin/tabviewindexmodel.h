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

#ifndef TABVIEWINDEXMODEL_H
#define TABVIEWINDEXMODEL_H

#include <qmlitemnode.h>

#include <QObject>
#include <QtQml>

class TabViewIndexModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant modelNodeBackendProperty READ modelNodeBackend WRITE setModelNodeBackend NOTIFY modelNodeBackendChanged)
    Q_PROPERTY(QStringList tabViewIndexModel READ tabViewIndexModel NOTIFY modelNodeBackendChanged)

public:
    explicit TabViewIndexModel(QObject *parent = 0);

    void setModelNodeBackend(const QVariant &modelNodeBackend);
    void setModelNode(const QmlDesigner::ModelNode &modelNode);
    QStringList tabViewIndexModel() const;
    void setupModel();

    static void registerDeclarativeType();

signals:
    void modelNodeBackendChanged();

private:
    QVariant modelNodeBackend() const;

    QmlDesigner::ModelNode m_modelNode;
    QStringList m_tabViewIndexModel;

};

QML_DECLARE_TYPE(TabViewIndexModel)

#endif // TABVIEWINDEXMODEL_H
