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

import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.0

Rectangle {
    color: styleConstants.lighterBackgroundColor

    border.color: styleConstants.backgroundColor
    border.width: 1


    Image {
        id: itemIcon // to be set by model

        anchors.top: parent.top
        anchors.topMargin: styleConstants.cellVerticalMargin
        anchors.horizontalCenter: parent.horizontalCenter

        width: itemLibraryIconWidth  // to be set in Qml context
        height: itemLibraryIconHeight   // to be set in Qml context
        source: itemLibraryIconPath     // to be set by model
    }

    Text {
        id: text
        font.pixelSize: 11
        elide: Text.ElideMiddle
        wrapMode: Text.WordWrap
        anchors.top: itemIcon.bottom
        anchors.topMargin: styleConstants.cellVerticalSpacing
        anchors.left: parent.left
        anchors.leftMargin: styleConstants.cellHorizontalMargin
        anchors.right: parent.right
        anchors.rightMargin: styleConstants.cellHorizontalMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: styleConstants.cellHorizontalMargin

        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter
        text: itemName  // to be set by model
        color: "#FFFFFF"
        renderType: Text.NativeRendering
    }

    MouseArea {
        id: mouseRegion
        anchors.fill: parent

        property bool reallyPressed: false
        property int pressedX
        property int pressedY

        onPressed: {
            rootView.startDragAndDrop(itemLibraryEntry)
        }
    }
}
