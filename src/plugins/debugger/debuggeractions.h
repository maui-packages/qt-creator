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

#ifndef DEBUGGER_ACTIONS_H
#define DEBUGGER_ACTIONS_H

#include <QObject>
#include <QHash>
#include <QMap>

namespace Utils { class SavedAction; }

namespace Debugger {
namespace Internal {

// Global debugger options that are not stored as saved action.
class GlobalDebuggerOptions
{
public:
    typedef QMap<QString, QString> SourcePathMap;

    void toSettings() const;
    void fromSettings();
    bool operator==(const GlobalDebuggerOptions &rhs) const
        { return sourcePathMap == rhs.sourcePathMap; }
    bool operator!=(const GlobalDebuggerOptions &rhs) const
        { return sourcePathMap != rhs.sourcePathMap; }

    SourcePathMap sourcePathMap;
};

class DebuggerSettings : public QObject
{
    Q_OBJECT // For tr().

public:
    explicit DebuggerSettings();
    ~DebuggerSettings();

    void insertItem(int code, Utils::SavedAction *item);
    Utils::SavedAction *item(int code) const;

    QString dump() const;

    void readSettings();
    void writeSettings() const;

private:
    QHash<int, Utils::SavedAction *> m_items;
};

///////////////////////////////////////////////////////////

enum DebuggerActionCode
{
    // General
    SettingsDialog,
    UseAlternatingRowColors,
    FontSizeFollowsEditor,
    UseMessageBoxForSignals,
    AutoQuit,
    LockView,
    LogTimeStamps,
    VerboseLog,
    OperateByInstruction,
    CloseBuffersOnExit,
    SwitchModeOnExit,
    BreakpointsFullPathByDefault,
    RaiseOnInterrupt,
    StationaryEditorWhileStepping,

    UseDebuggingHelpers,

    UseCodeModel,
    ShowThreadNames,

    UseToolTipsInMainEditor,
    UseToolTipsInLocalsView,
    UseToolTipsInBreakpointsView,
    UseToolTipsInStackView,
    UseAddressInBreakpointsView,
    UseAddressInStackView,

    RegisterForPostMortem,
    AlwaysAdjustColumnWidths,

    ExtraDumperFile,     // For loading a file. Recommended.
    ExtraDumperCommands, // To modify an existing setup.

    // Cdb
    CdbAdditionalArguments,
    CdbSymbolPaths,
    CdbSourcePaths,
    CdbBreakEvents,
    CdbBreakOnCrtDbgReport,
    UseCdbConsole,
    CdbBreakPointCorrection,
    IgnoreFirstChanceAccessViolation,

    // Gdb
    LoadGdbInit,
    LoadGdbDumpers,
    AttemptQuickStart,
    GdbStartupCommands,
    GdbPostAttachCommands,
    GdbWatchdogTimeout,
    AutoEnrichParameters,
    UseDynamicType,
    TargetAsync,
    WarnOnReleaseBuilds,
    MultiInferior,
    IntelFlavor,
    IdentifyDebugInfoPackages,

    // Stack
    MaximalStackDepth,
    ExpandStack,
    CreateFullBacktrace,

    // Watchers & Locals
    ShowStdNamespace,
    ShowQtNamespace,
    SortStructMembers,
    AutoDerefPointers,
    MaximalStringLength,
    DisplayStringLimit,

    // Source List
    ListSourceFiles,

    // Running
    SkipKnownFrames,
    EnableReverseDebugging,

    // Breakpoints
    SynchronizeBreakpoints,
    AllPluginBreakpoints,
    SelectedPluginBreakpoints,
    AdjustBreakpointLocations,
    NoPluginBreakpoints,
    SelectedPluginBreakpointsPattern,
    BreakOnThrow,
    BreakOnCatch,
    BreakOnWarning,
    BreakOnFatal,
    BreakOnAbort,

    // QML Tools
    ShowQmlObjectTree,
    ShowAppOnTop,
    QmlUpdateOnSave
};

} // namespace Internal
} // namespace Debugger

#endif // DEBUGGER_WATCHWINDOW_H
