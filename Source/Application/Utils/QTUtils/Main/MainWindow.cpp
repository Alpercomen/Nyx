#pragma once

#include <QDockWidget>
#include <QToolBar>
#include <QAction>
#include <QStyle>
#include <QTreeView>
#include <QStatusBar>
#include <QModelIndex>
#include <QDir>
#include <QCoreApplication>

#include <Application/Window/Window.h>
#include <Application/Core/Engine/Engine.h>
#include <Application/Utils/QTUtils/Main/MainWindow.h>
#include <Application/Utils/QTUtils/Model/Hierarchy.h>
#include <Application/Utils/QTUtils/Panel/Inspector.h>
#include <Application/Utils/QTUtils/View/ContentBrowser.h>
#include <Application/Core/Services/Editor/EditorComponentUI.h>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle("Nyx Editor");
	resize(1400, 900);

	engine = MakeShared<Engine>();
	sceneManager = MakeShared<SceneManager>();
	sceneManager->CreateScene();

	// Create the GL widget first
	sceneView = new SceneViewGL(this);
	sceneView->setEngine(engine);
	sceneView->setScene(sceneManager->GetActiveScene());
	setCentralWidget(sceneView);

	// When the GL context is ready, then create entities (textures, VAOs, etc.)
	connect(sceneView, &SceneViewGL::glInitialized, this, [&]() 
	{
		sceneManager->GenerateEntities(sceneManager->GetActiveSceneID());
		RegisterComponents();
		buildHierarchyAndInspector();
	});

	buildMenusAndToolbars();
	buildContentBrowser();

	// Point Content Browser at project source tree (locked)
#ifdef PROJECT_SOURCE_DIR
	contentBrowser->setRootPath(QString::fromUtf8(PROJECT_SOURCE_DIR));
#else
// Fallback: try repo root two levels up from exe (Build/<cfg>/Editor.exe)
	contentBrowser->setRootPath(QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../.."));
#endif

	statusBar()->showMessage("Ready");
}

void MainWindow::buildMenusAndToolbars()
{
	auto* tb = addToolBar("Playback");

	// Create expanding spacers
	QWidget* leftSpacer = new QWidget(tb);
	QWidget* rightSpacer = new QWidget(tb);
	leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	tb->addWidget(leftSpacer);

	buildFastBackward(tb);
	buildPlay(tb);
	buildStop(tb);
	buildFastForward(tb);

	tb->addWidget(rightSpacer);
	tb->setMovable(false);
}

void MainWindow::buildFastBackward(QToolBar* tb)
{
	actFB = tb->addAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), "Fast-Backward");
	connect(actFB, &QAction::triggered, this, [&]() {
	if (TIME_SCALE > 1.0f)
		TIME_SCALE = std::max(TIME_SCALE / 10.0f, TIME_SCALE_MIN);

	statusBar()->showMessage(QString("Speed: %1x").arg(TIME_SCALE));
	});
}

void MainWindow::buildFastForward(QToolBar* tb)
{
	actFF = tb->addAction(style()->standardIcon(QStyle::SP_MediaSkipForward), "Fast-Forward");
	connect(actFF, &QAction::triggered, this, [&]() {
		TIME_SCALE = std::min(TIME_SCALE * 10.0f, TIME_SCALE_MAX);
		statusBar()->showMessage(QString("Fast-Forward: %1x").arg(TIME_SCALE));
	});
}

void MainWindow::buildPlay(QToolBar* tb)
{
	actPlay = tb->addAction(style()->standardIcon(QStyle::SP_MediaPlay), "Play");
	connect(actPlay, &QAction::triggered, this, [&]() {
		TIME_SCALE = 1.0f;
		statusBar()->showMessage("Simulation running at 1x");
	});
}

void MainWindow::buildStop(QToolBar* tb)
{
	actStop = tb->addAction(style()->standardIcon(QStyle::SP_MediaStop), "Stop");
	connect(actStop, &QAction::triggered, this, [&]() {
		TIME_SCALE = 0.0f;
		statusBar()->showMessage("Simulation stopped");
	});
}

void MainWindow::buildContentBrowser()
{
	// Content Browser Dock (Bottom)
	auto* contentDock = new QDockWidget("Content Browser", this);
	contentDock->setObjectName("ContentDock");
	contentBrowser = new ContentBrowser(contentDock);
	contentDock->setWidget(contentBrowser);
	addDockWidget(Qt::BottomDockWidgetArea, contentDock);
}

void MainWindow::buildHierarchyAndInspector()
{
	// Hierarchy Dock (Left)
	auto* hierarchyDock = new QDockWidget("Hierarchy", this);
	auto* hierarchyView = new QTreeView;
	auto* hierarchyModel = new HierarchyModel(hierarchyView);
	hierarchyView->setModel(hierarchyModel);
	hierarchyView->setHeaderHidden(true);
	hierarchyDock->setWidget(hierarchyView);
	addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);

	// Inspector Dock (Right)
	auto* inspectorDock = new QDockWidget("Inspector", this);
	inspectorDock->setObjectName("InspectorDock");
	inspectorPanel = new InspectorPanel(inspectorDock);
	inspectorDock->setWidget(inspectorPanel);
	addDockWidget(Qt::RightDockWidgetArea, inspectorDock);

	// Selection handling: update Inspector when Hierarchy selection changes
	auto* sel = hierarchyView->selectionModel();
	QObject::connect(sel, &QItemSelectionModel::currentChanged,
		inspectorPanel, [=](const QModelIndex& curr, const QModelIndex&) {
			EntityID id = hierarchyModel->idAt(curr.row());
			inspectorPanel->setSelectedEntity(id);
		});
}
