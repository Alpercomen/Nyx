#pragma once
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QTreeView>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QSplitter>
#include <QListView>
#include <QHeaderView>
#include <QTooltip>

#include <Application/Utils/QTUtils/View/ContentBrowser.h>
#include <Application/Utils/QTUtils/Proxy/FileFilter.h>

ContentBrowser::ContentBrowser(QWidget* parent) : QWidget(parent)
{
	buildUi();
	wireSignals();
}

void ContentBrowser::buildUi()
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	// Filter row
	auto* hl = new QHBoxLayout();
	search = new QLineEdit(this);
	search->setPlaceholderText("Filter (*.png;*.json; name substring)");
	hl->addWidget(new QLabel("Filter:"));
	hl->addWidget(search, 1);
	layout->addLayout(hl);

	// Data model (read-only)
	model = new QFileSystemModel(this);
	model->setReadOnly(true);
	model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);

	// Left tree: only directories via proxy
	dirProxy = new DirectoryOnlyProxy(this);
	dirProxy->setSourceModel(model);

	// Right list: files + dirs with wildcard filter
	proxy = new ListFilterProxy(this);
	proxy->setSourceModel(model);
	proxy->setRecursiveFilteringEnabled(true);
	proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

	splitter = new QSplitter(this);
	tree = new QTreeView(splitter);
	tree->setModel(dirProxy);
	tree->setHeaderHidden(false);
	tree->header()->setStretchLastSection(true);
	tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	tree->setExpandsOnDoubleClick(true);
	tree->setSelectionMode(QAbstractItemView::SingleSelection);


	list = new QListView(splitter);
	list->setModel(proxy);
	list->setViewMode(QListView::IconMode);
	list->setMovement(QListView::Static); // fixed tile positions
	list->setResizeMode(QListView::Adjust);
	list->setUniformItemSizes(true); // consistent metrics
	list->setWordWrap(false); // single-line labels
	list->setTextElideMode(Qt::ElideRight); // crop long names with …
	list->setIconSize(QSize(48, 48));
	list->setGridSize(QSize(64, 96)); // fixed tile size (Explorer-like)
	list->setSpacing(8);
	list->setSelectionMode(QAbstractItemView::SingleSelection);
	list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	list->setDragDropMode(QAbstractItemView::NoDragDrop);
	list->setAcceptDrops(false);
	list->setDropIndicatorShown(false);
	list->setMouseTracking(true); // needed for hover tooltips

	splitter->setStretchFactor(0, 1);
	layout->addWidget(splitter, 1);
}

void ContentBrowser::wireSignals() 
{
	connect(search, &QLineEdit::textChanged, this, [this](const QString& t) {
		proxy->setFilterWildcard(t);
		if (currentDirSrc.isValid())
		{
			proxy->setPinnedRoot(currentDirSrc);
			list->setRootIndex(proxy->mapFromSource(currentDirSrc));
		}
	});

	connect(tree->selectionModel(), &QItemSelectionModel::currentChanged,
		this, [this](const QModelIndex& curr, const QModelIndex&) {
			currentDirSrc = dirProxy->mapToSource(curr);
			proxy->setPinnedRoot(currentDirSrc);
			list->setRootIndex(proxy->mapFromSource(currentDirSrc));
	});

	// Double-click list to open files with OS default app; double-click dir to drill down via tree
	connect(list, &QListView::doubleClicked, this, [this](const QModelIndex& proxyIndex) {
		const QModelIndex srcIndex = proxy->mapToSource(proxyIndex);
		const QFileInfo fi = model->fileInfo(srcIndex);
		if (fi.isDir()) {
			const QModelIndex treeIdx = dirProxy->mapFromSource(srcIndex);
			tree->setCurrentIndex(treeIdx);
			tree->expand(treeIdx);
		}
		else {
			QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
		}
	});

	// Show full name on hover as tooltip
	connect(list, &QListView::entered, this, [this](const QModelIndex& proxyIndex) {
		const QModelIndex srcIndex = proxy->mapToSource(proxyIndex);
		const QFileInfo fi = model->fileInfo(srcIndex);
		QToolTip::showText(QCursor::pos(), fi.fileName());
	});


	// Show full name on hover as tooltip
	connect(list, &QListView::entered, this, [this](const QModelIndex& proxyIndex) {
		const QModelIndex srcIndex = proxy->mapToSource(proxyIndex);
		const QFileInfo fi = model->fileInfo(srcIndex);
		QToolTip::showText(QCursor::pos(), fi.fileName());
	});
}

void ContentBrowser::setRootPath(const QString& path)
{
	rootPath = QDir::cleanPath(path);
	const QModelIndex srcRoot = model->setRootPath(rootPath);

	// Lock tree to directories-only proxy
	const QModelIndex treeRoot = dirProxy->mapFromSource(srcRoot);
	tree->setRootIndex(treeRoot);

	// Initialize current directory to root and pin it
	currentDirSrc = srcRoot;
	proxy->setPinnedRoot(currentDirSrc);
	list->setRootIndex(proxy->mapFromSource(currentDirSrc));
}