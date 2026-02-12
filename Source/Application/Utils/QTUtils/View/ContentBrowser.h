#pragma once
#include <QWidget> // Base class
#include <Application/Utils/QTUtils/Proxy/FileFilter.h>

class QLineEdit; 
class QListView; 
class QTreeView; 
class QFileSystemModel; 
class QSortFilterProxyModel; 
class QSplitter; 
class QModelIndex;

class ContentBrowser : public QWidget {
public:
	explicit ContentBrowser(QWidget* parent = nullptr);
	void setRootPath(const QString& path);
private:
	void buildUi();
	void wireSignals();


	QString rootPath;
	QLineEdit* search{ nullptr };
	QFileSystemModel* model{ nullptr }; // full FS model (files + dirs)
	ListFilterProxy* proxy{ nullptr };       // filters list, keeps root pinned
	QSortFilterProxyModel* dirProxy{ nullptr }; // shows only directories in tree
	QListView* list{ nullptr };
	QTreeView* tree{ nullptr };
	QSplitter* splitter{ nullptr };
	QModelIndex currentDirSrc;
};