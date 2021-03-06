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

#ifndef CPPQUICKFIXASSISTANT_H
#define CPPQUICKFIXASSISTANT_H

#include <cpptools/cppsemanticinfo.h>
#include <texteditor/codeassist/defaultassistinterface.h>
#include <texteditor/codeassist/quickfixassistprovider.h>
#include <texteditor/codeassist/quickfixassistprocessor.h>

#include <cplusplus/LookupContext.h>

#include <utils/qtcoverride.h>

namespace CppTools {
class CppRefactoringFile;
typedef QSharedPointer<CppRefactoringFile> CppRefactoringFilePtr;
}

namespace CppEditor {
namespace Internal {

class CppEditorWidget;

class CppQuickFixAssistInterface : public TextEditor::DefaultAssistInterface
{
public:
    CppQuickFixAssistInterface(CppEditorWidget *editor, TextEditor::AssistReason reason);

    const QList<CPlusPlus::AST *> &path() const;
    CPlusPlus::Snapshot snapshot() const;
    CppTools::SemanticInfo semanticInfo() const;
    const CPlusPlus::LookupContext &context() const;
    CppEditorWidget *editor() const;

    CppTools::CppRefactoringFilePtr currentFile() const;

    bool isCursorOn(unsigned tokenIndex) const;
    bool isCursorOn(const CPlusPlus::AST *ast) const;

private:
    CppEditorWidget *m_editor;
    CppTools::SemanticInfo m_semanticInfo;
    CPlusPlus::Snapshot m_snapshot;
    CppTools::CppRefactoringFilePtr m_currentFile;
    CPlusPlus::LookupContext m_context;
    QList<CPlusPlus::AST *> m_path;
};

class CppQuickFixAssistProcessor : public TextEditor::QuickFixAssistProcessor
{
public:
    CppQuickFixAssistProcessor(const TextEditor::IAssistProvider *provider);

    const TextEditor::IAssistProvider *provider() const QTC_OVERRIDE;

private:
    const TextEditor::IAssistProvider *m_provider;
};

class CppQuickFixAssistProvider : public TextEditor::QuickFixAssistProvider
{
public:
    bool isAsynchronous() const QTC_OVERRIDE;
    bool supportsEditor(Core::Id editorId) const QTC_OVERRIDE;
    TextEditor::IAssistProcessor *createProcessor() const QTC_OVERRIDE;

    QList<TextEditor::QuickFixFactory *> quickFixFactories() const QTC_OVERRIDE;
};

} // Internal
} // CppEditor

#endif // CPPQUICKFIXASSISTANT_H
