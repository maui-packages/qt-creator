/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
**************************************************************************/

#include "genericproject.h"
#include "genericprojectconstants.h"
#include "genericprojectnodes.h"
#include "makestep.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <cpptools/cppmodelmanagerinterface.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/qtcassert.h>
#include <coreplugin/icore.h>

#include <cpptools/cppmodelmanagerinterface.h>

#include <QtCore/QtDebug>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QProcess>

#include <QtGui/QFormLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QComboBox>
#include <QtGui/QStringListModel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

using namespace GenericProjectManager;
using namespace GenericProjectManager::Internal;

////////////////////////////////////////////////////////////////////////////////////
// GenericProject
////////////////////////////////////////////////////////////////////////////////////

namespace {

class ListModel: public QStringListModel
{
public:
    ListModel(QObject *parent)
        : QStringListModel(parent) {}

    virtual ~ListModel() {}

    virtual int rowCount(const QModelIndex &parent) const
    { return 1 + QStringListModel::rowCount(parent); }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    { return QStringListModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled; }

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const
    {
        if (row == stringList().size())
            return createIndex(row, column);

        return QStringListModel::index(row, column, parent);
    }

    virtual QVariant data(const QModelIndex &index, int role) const
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            if (index.row() == stringList().size())
                return tr("<new>");
        }

        return QStringListModel::data(index, role);
    }

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (role == Qt::EditRole && index.row() == stringList().size())
            insertRow(index.row(), QModelIndex());

        return QStringListModel::setData(index, value, role);
    }
};

} // end of anonymous namespace

GenericProject::GenericProject(Manager *manager, const QString &fileName)
    : _manager(manager),
      _fileName(fileName),
      _toolChain(0)
{
    QFileInfo fileInfo(_fileName);
    const QString projectBaseName = fileInfo.baseName();
    QDir dir = fileInfo.dir();

    _filesFileName = QFileInfo(dir, projectBaseName + QLatin1String(".files")).absoluteFilePath();
    _includesFileName = QFileInfo(dir, projectBaseName + QLatin1String(".includes")).absoluteFilePath();
    _configFileName = QFileInfo(dir, projectBaseName + QLatin1String(".config")).absoluteFilePath();

    _file = new GenericProjectFile(this, fileName);
    _rootNode = new GenericProjectNode(this, _file);

    _manager->registerProject(this);
}

GenericProject::~GenericProject()
{
    _manager->unregisterProject(this);

    delete _rootNode;
    delete _toolChain;
}

QString GenericProject::filesFileName() const
{ return _filesFileName; }

QString GenericProject::includesFileName() const
{ return _includesFileName; }

QString GenericProject::configFileName() const
{ return _configFileName; }

QStringList GenericProject::readLines(const QString &absoluteFileName) const
{
    QStringList lines;

    QFile file(absoluteFileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);

        forever {
            QString line = stream.readLine();
            if (line.isNull())
                break;

            line = line.trimmed();
            if (line.isEmpty())
                continue;

            lines.append(line);
        }
    }

    return lines;
}


void GenericProject::parseProject()
{
    const QFileInfo projectFileInfo(_fileName);

    QSettings projectInfo(_fileName, QSettings::IniFormat);

    _files = convertToAbsoluteFiles(readLines(filesFileName()));
    _files.removeDuplicates();

    _projectIncludePaths = readLines(includesFileName());
    _projectIncludePaths.removeDuplicates();

    _generated = convertToAbsoluteFiles(projectInfo.value(QLatin1String("generated")).toStringList());
    _defines   = projectInfo.value(QLatin1String("defines")).toStringList();

    emit fileListChanged();
}

void GenericProject::refresh()
{
    parseProject();

    _rootNode->refresh();

    CppTools::CppModelManagerInterface *modelManager =
        ExtensionSystem::PluginManager::instance()->getObject<CppTools::CppModelManagerInterface>();

    if (_toolChain && modelManager) {
        const QByteArray predefinedMacros = _toolChain->predefinedMacros();
        const QList<ProjectExplorer::HeaderPath> systemHeaderPaths = _toolChain->systemHeaderPaths();

        CppTools::CppModelManagerInterface::ProjectInfo pinfo = modelManager->projectInfo(this);
        pinfo.defines = predefinedMacros;

        QStringList allIncludePaths, allFrameworkPaths;

        foreach (const ProjectExplorer::HeaderPath &headerPath, _toolChain->systemHeaderPaths()) {
            if (headerPath.kind() == ProjectExplorer::HeaderPath::FrameworkHeaderPath)
                allFrameworkPaths.append(headerPath.path());

            else
                allIncludePaths.append(headerPath.path());
        }

        allIncludePaths += this->allIncludePaths();

        pinfo.frameworkPaths = allFrameworkPaths;
        pinfo.includePaths = allIncludePaths;

        // ### add _defines.
        pinfo.sourceFiles = files();
        pinfo.sourceFiles += generated();

        modelManager->updateProjectInfo(pinfo);
        modelManager->updateSourceFiles(pinfo.sourceFiles);
    }
}

QStringList GenericProject::convertToAbsoluteFiles(const QStringList &paths) const
{
    const QDir projectDir(QFileInfo(_fileName).dir());
    QStringList absolutePaths;
    foreach (const QString &file, paths) {
        QFileInfo fileInfo(projectDir, file);
        absolutePaths.append(fileInfo.absoluteFilePath());
    }
    absolutePaths.removeDuplicates();
    return absolutePaths;
}

QStringList GenericProject::allIncludePaths() const
{
    QStringList paths;
    paths += _includePaths;
    paths += _projectIncludePaths;
    paths.removeDuplicates();
    return paths;
}

QStringList GenericProject::projectIncludePaths() const
{ return _projectIncludePaths; }

QStringList GenericProject::files() const
{ return _files; }

QStringList GenericProject::generated() const
{ return _generated; }

QStringList GenericProject::includePaths() const
{ return _includePaths; }

void GenericProject::setIncludePaths(const QStringList &includePaths)
{ _includePaths = includePaths; }

QStringList GenericProject::defines() const
{ return _defines; }

void GenericProject::setDefines(const QStringList &defines)
{ _defines = defines; }

void GenericProject::setToolChainId(const QString &toolChainId)
{
    using namespace ProjectExplorer;

    _toolChainId = toolChainId;

    delete _toolChain;
    _toolChain = 0;

    if (toolChainId == QLatin1String("mingw")) {
        const QLatin1String qmake_cxx("g++"); // ### FIXME
        const QString mingwDirectory; // ### FIXME

        _toolChain = ToolChain::createMinGWToolChain(qmake_cxx, mingwDirectory);

    } else if (toolChainId == QLatin1String("msvc")) {
        const QString msvcVersion; // ### FIXME
        _toolChain = ToolChain::createMSVCToolChain(msvcVersion);

    } else if (toolChainId == QLatin1String("wince")) {
        const QString msvcVersion, wincePlatform; // ### FIXME
        _toolChain = ToolChain::createWinCEToolChain(msvcVersion, wincePlatform);

    } else if (toolChainId == QLatin1String("gcc") || toolChainId == QLatin1String("icc")) {
        const QLatin1String qmake_cxx("g++"); // ### FIXME
        _toolChain = ToolChain::createGccToolChain(qmake_cxx);
    }
}

QString GenericProject::buildParser(const QString &buildConfiguration) const
{
    if (_toolChain) {
        switch (_toolChain->type()) {
        case ProjectExplorer::ToolChain::GCC:
        case ProjectExplorer::ToolChain::LinuxICC:
        case ProjectExplorer::ToolChain::MinGW:
            return QLatin1String(ProjectExplorer::Constants::BUILD_PARSER_GCC);

        case ProjectExplorer::ToolChain::MSVC:
        case ProjectExplorer::ToolChain::WINCE:
            return ProjectExplorer::Constants::BUILD_PARSER_MSVC;

        default:
            break;
        } // switch
    }

    return QString();
}

QString GenericProject::toolChainId() const
{ return _toolChainId; }

QString GenericProject::name() const
{
    return _projectName;
}

Core::IFile *GenericProject::file() const
{
    return _file;
}

ProjectExplorer::IProjectManager *GenericProject::projectManager() const
{
    return _manager;
}

QList<ProjectExplorer::Project *> GenericProject::dependsOn()
{
    return QList<Project *>();
}

bool GenericProject::isApplication() const
{
    return true;
}

ProjectExplorer::Environment GenericProject::environment(const QString &) const
{
    return ProjectExplorer::Environment::systemEnvironment();
}

QString GenericProject::buildDirectory(const QString &buildConfiguration) const
{
    QString buildDirectory = value(buildConfiguration, "buildDirectory").toString();

    if (buildDirectory.isEmpty()) {
        QFileInfo fileInfo(_fileName);

        buildDirectory = fileInfo.absolutePath();
    }

    return buildDirectory;
}

ProjectExplorer::BuildStepConfigWidget *GenericProject::createConfigWidget()
{
    return new GenericBuildSettingsWidget(this);
}

QList<ProjectExplorer::BuildStepConfigWidget*> GenericProject::subConfigWidgets()
{
    return QList<ProjectExplorer::BuildStepConfigWidget*>();
}

 void GenericProject::newBuildConfiguration(const QString &buildConfiguration)
 {
     makeStep()->setBuildTarget(buildConfiguration, "all", true);
 }

GenericProjectNode *GenericProject::rootProjectNode() const
{
    return _rootNode;
}

QStringList GenericProject::files(FilesMode fileMode) const
{
    return _files; // ### TODO: handle generated files here.
}

QStringList GenericProject::targets() const
{
    QStringList targets;
    targets.append(QLatin1String("all"));
    targets.append(QLatin1String("clean"));
    return targets;
}

MakeStep *GenericProject::makeStep() const
{
    foreach (ProjectExplorer::BuildStep *bs, buildSteps()) {
        if (MakeStep *ms = qobject_cast<MakeStep *>(bs))
            return ms;
    }

    return 0;
}

void GenericProject::restoreSettingsImpl(ProjectExplorer::PersistentSettingsReader &reader)
{
    Project::restoreSettingsImpl(reader);

    if (buildConfigurations().isEmpty()) {
        MakeStep *makeStep = new MakeStep(this);
        insertBuildStep(0, makeStep);

        const QLatin1String all("all");

        addBuildConfiguration(all);
        setActiveBuildConfiguration(all);
        makeStep->setBuildTarget(all, all, /* on = */ true);

        const QLatin1String buildDirectory("buildDirectory");

        const QFileInfo fileInfo(file()->fileName());
        setValue(all, buildDirectory, fileInfo.absolutePath());
    }

    QString toolChainId = reader.restoreValue(QLatin1String("toolChain")).toString();
    if (toolChainId.isEmpty())
        toolChainId = QLatin1String("gcc");

    setToolChainId(toolChainId.toLower()); // ### move

    const QStringList userIncludePaths =
            reader.restoreValue(QLatin1String("includePaths")).toStringList();

    setIncludePaths(allIncludePaths());

    parseProject();
    refresh();
}

void GenericProject::saveSettingsImpl(ProjectExplorer::PersistentSettingsWriter &writer)
{
    Project::saveSettingsImpl(writer);

    writer.saveValue(QLatin1String("toolChain"), _toolChainId);
    writer.saveValue(QLatin1String("includePaths"), _includePaths);
}

////////////////////////////////////////////////////////////////////////////////////
// GenericBuildSettingsWidget
////////////////////////////////////////////////////////////////////////////////////
GenericBuildSettingsWidget::GenericBuildSettingsWidget(GenericProject *project)
    : _project(project)
{
    QFormLayout *fl = new QFormLayout(this);

    // build directory
    _pathChooser = new Core::Utils::PathChooser(this);
    _pathChooser->setEnabled(true);
    fl->addRow(tr("Build directory:"), _pathChooser);
    connect(_pathChooser, SIGNAL(changed()), this, SLOT(buildDirectoryChanged()));

    // tool chain
    QComboBox *toolChainChooser = new QComboBox;
    toolChainChooser->addItems(ProjectExplorer::ToolChain::supportedToolChains());
    toolChainChooser->setCurrentIndex(toolChainChooser->findText(_project->toolChainId()));
    fl->addRow(tr("Tool chain:"), toolChainChooser);
    connect(toolChainChooser, SIGNAL(activated(QString)), _project, SLOT(setToolChainId(QString)));
}

GenericBuildSettingsWidget::~GenericBuildSettingsWidget()
{ }

QString GenericBuildSettingsWidget::displayName() const
{ return tr("Generic Manager"); }

void GenericBuildSettingsWidget::init(const QString &buildConfiguration)
{
    _buildConfiguration = buildConfiguration;
    _pathChooser->setPath(_project->buildDirectory(buildConfiguration));
}

void GenericBuildSettingsWidget::buildDirectoryChanged()
{
    _project->setValue(_buildConfiguration, "buildDirectory", _pathChooser->path());
}

////////////////////////////////////////////////////////////////////////////////////
// GenericProjectFile
////////////////////////////////////////////////////////////////////////////////////
GenericProjectFile::GenericProjectFile(GenericProject *parent, QString fileName)
    : Core::IFile(parent),
      _project(parent),
      _fileName(fileName)
{ }

GenericProjectFile::~GenericProjectFile()
{ }

bool GenericProjectFile::save(const QString &)
{
    return false;
}

QString GenericProjectFile::fileName() const
{
    return _fileName;
}

QString GenericProjectFile::defaultPath() const
{
    return QString();
}

QString GenericProjectFile::suggestedFileName() const
{
    return QString();
}

QString GenericProjectFile::mimeType() const
{
    return Constants::GENERICMIMETYPE;
}

bool GenericProjectFile::isModified() const
{
    return false;
}

bool GenericProjectFile::isReadOnly() const
{
    return true;
}

bool GenericProjectFile::isSaveAsAllowed() const
{
    return false;
}

void GenericProjectFile::modified(ReloadBehavior *)
{
}
