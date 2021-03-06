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

#include "winrtruncontrol.h"

#include "winrtdevice.h"
#include "winrtrunconfiguration.h"
#include "winrtrunnerhelper.h"

#include <coreplugin/idocument.h>
#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildtargetinfo.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/kitinformation.h>
#include <qtsupport/qtkitinformation.h>

#include <QTimer>

using ProjectExplorer::DeviceKitInformation;
using ProjectExplorer::IDevice;
using ProjectExplorer::RunControl;
using ProjectExplorer::RunMode;
using ProjectExplorer::Target;

namespace WinRt {
namespace Internal {

WinRtRunControl::WinRtRunControl(WinRtRunConfiguration *runConfiguration, RunMode mode)
    : RunControl(runConfiguration, mode)
    , m_runConfiguration(runConfiguration)
    , m_state(StoppedState)
    , m_runner(0)
{
    setIcon(QLatin1String(ProjectExplorer::Constants::ICON_RUN_SMALL));
}

void WinRtRunControl::start()
{
    if (m_state != StoppedState)
        return;
    if (!startWinRtRunner())
        m_state = StoppedState;
}

RunControl::StopResult WinRtRunControl::stop()
{
    if (m_state == StoppedState)
        return StoppedSynchronously;

    m_runner->stop();
    return AsynchronousStop;
}

bool WinRtRunControl::isRunning() const
{
    return m_state == StartedState;
}

void WinRtRunControl::onProcessStarted()
{
    QTC_CHECK(m_state == StartingState);
    m_state = StartedState;
    emit started();
}

void WinRtRunControl::onProcessFinished()
{
    QTC_CHECK(m_state == StartedState);
    onProcessError();
}

void WinRtRunControl::onProcessError()
{
    QTC_ASSERT(m_runner, return);
    m_runner->disconnect();
    m_runner->deleteLater();
    m_runner = 0;
    m_state = StoppedState;
    emit finished();
}

bool WinRtRunControl::startWinRtRunner()
{
    QTC_ASSERT(!m_runner, return false);
    m_runner = new WinRtRunnerHelper(this);
    connect(m_runner, SIGNAL(started()), SLOT(onProcessStarted()));
    connect(m_runner, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onProcessFinished()));
    connect(m_runner, SIGNAL(error(QProcess::ProcessError)), SLOT(onProcessError()));
    m_state = StartingState;
    m_runner->start();
    return true;
}

} // namespace Internal
} // namespace WinRt
