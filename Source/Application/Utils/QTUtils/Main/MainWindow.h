#pragma once
#include <Application/Utils/QTUtils/View/SceneViewGL.h>
#include <QMainWindow> // Base class

class SceneView; // fwd
class HierarchyModel; // fwd
class InspectorPanel; // fwd
class ContentBrowser; // fwd
class QAction; // fwd

class MainWindow : public QMainWindow {
public:
	explicit MainWindow(QWidget* parent = nullptr);

private:
	// helpers
	void buildMenusAndToolbars();
	void buildContentBrowser();
	void buildHierarchyAndInspector();

	// Central view
	SceneViewGL* sceneView = nullptr;

	// Dock widgets content
	HierarchyModel* hierarchyModel = nullptr;
	InspectorPanel* inspectorPanel = nullptr;
	ContentBrowser* contentBrowser = nullptr;

	// Actions
	QAction* actPlay = nullptr;
	QAction* actStop = nullptr;
	QAction* actFF = nullptr;

	// Nyx
	SharedPtr<Engine> engine;
	SharedPtr<SceneManager> sceneManager;
};