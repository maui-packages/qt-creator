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

#ifndef BASETEXTEDITOR_P_H
#define BASETEXTEDITOR_P_H

#include "basetexteditor.h"
#include "behaviorsettings.h"
#include "displaysettings.h"
#include "marginsettings.h"
#include "fontsettings.h"
#include "refactoroverlay.h"

#include <coreplugin/id.h>
#include <utils/changeset.h>

#include <QBasicTimer>
#include <QSharedPointer>
#include <QPointer>
#include <QScopedPointer>
#include <QTextBlock>
#include <QTimer>

namespace TextEditor {

class BaseTextDocument;
class CodeAssistant;

namespace Internal {
class TextEditorOverlay;
class ClipboardAssistProvider;

class TEXTEDITOR_EXPORT BaseTextBlockSelection
{
public:
    BaseTextBlockSelection()
        : positionBlock(0), positionColumn(0)
        , anchorBlock(0) , anchorColumn(0){}
    BaseTextBlockSelection(const BaseTextBlockSelection &other);

    void clear();
    QTextCursor selection(const BaseTextDocument *baseTextDocument) const;
    QTextCursor cursor(const BaseTextDocument *baseTextDocument) const;
    void fromPostition(int positionBlock, int positionColumn, int anchorBlock, int anchorColumn);
    bool hasSelection() { return !(positionBlock == anchorBlock && positionColumn == anchorColumn); }

    // defines the first block
    inline int firstBlockNumber() const { return qMin(positionBlock, anchorBlock); }
     // defines the last block
    inline int lastBlockNumber() const { return qMax(positionBlock, anchorBlock); }
    // defines the first visual column of the selection
    inline int firstVisualColumn() const { return qMin(positionColumn, anchorColumn); }
    // defines the last visual column of the selection
    inline int lastVisualColumn() const { return qMax(positionColumn, anchorColumn); }

public:
    int positionBlock;
    int positionColumn;
    int anchorBlock;
    int anchorColumn;

private:
    QTextCursor cursor(const BaseTextDocument *baseTextDocument, bool fullSelection) const;
};

//================BaseTextEditorPrivate==============

struct BaseTextEditorPrivateHighlightBlocks
{
    QList<int> open;
    QList<int> close;
    QList<int> visualIndent;
    inline int count() const { return visualIndent.size(); }
    inline bool isEmpty() const { return open.isEmpty() || close.isEmpty() || visualIndent.isEmpty(); }
    inline bool operator==(const BaseTextEditorPrivateHighlightBlocks &o) const {
        return (open == o.open && close == o.close && visualIndent == o.visualIndent);
    }
    inline bool operator!=(const BaseTextEditorPrivateHighlightBlocks &o) const { return !(*this == o); }
};


} // namespace Internal
} // namespace TextEditor

#endif // BASETEXTEDITOR_P_H
