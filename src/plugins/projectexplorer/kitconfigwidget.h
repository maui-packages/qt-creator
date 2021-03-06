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

#ifndef KITCONFIGWIDGET_H
#define KITCONFIGWIDGET_H

#include "projectexplorer_export.h"

#include <coreplugin/id.h>

#include <QWidget>

namespace ProjectExplorer {

class Kit;
class KitInformation;

// --------------------------------------------------------------------------
// KitConfigWidget
// --------------------------------------------------------------------------

class PROJECTEXPLORER_EXPORT KitConfigWidget : public QObject
{
    Q_OBJECT

public:
    KitConfigWidget(Kit *kit, const KitInformation *ki);

    Core::Id kitInformationId() const;

    virtual QString displayName() const = 0;
    virtual QString toolTip() const { return QString(); }
    virtual void makeReadOnly() = 0;
    virtual void refresh() = 0;
    virtual bool visibleInKit() { return true; }

    virtual QWidget *mainWidget() const = 0;
    virtual QWidget *buttonWidget() const { return 0; }

    bool isSticky() const { return m_isSticky; }
    bool isMutable() const;
    void setMutable(bool b);

    static QString msgManage();

signals:
    void dirty();

protected:
    Kit *m_kit;
    const KitInformation *m_kitInformation;
    bool m_isSticky;
};

} // namespace ProjectExplorer

#endif // KITCONFIGWIDGET_H
