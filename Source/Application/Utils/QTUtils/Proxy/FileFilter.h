#pragma once
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QString>
#include <QModelIndex>

// Proxy that shows only directories (for the tree on the left)
class DirectoryOnlyProxy : public QSortFilterProxyModel {
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override 
    {
        const auto* fsm = qobject_cast<const QFileSystemModel*>(sourceModel());

        if (!fsm) 
            return true;

        const QModelIndex idx = fsm->index(source_row, 0, source_parent);
        return fsm->isDir(idx);
    }
};

// Proxy for the right list that pins the current directory as visible
class ListFilterProxy : public QSortFilterProxyModel 
{
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;
    void setPinnedRoot(const QModelIndex& srcRoot) 
    { 
        m_pinnedRoot = srcRoot;
    }

protected:
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override 
    {
        const QModelIndex idx = sourceModel()->index(row, 0, parent);
        if (m_pinnedRoot.isValid() && idx == m_pinnedRoot)
            return true;  // keep current directory visible
        return QSortFilterProxyModel::filterAcceptsRow(row, parent);
    }
private:
    QModelIndex m_pinnedRoot;
};

class FileFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
        Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
public:
    explicit FileFilterProxyModel(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
        setRecursiveFilteringEnabled(true);
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

    QString filterText() const { return m_filterText; }

public slots:
    void setFilterText(const QString& text)
    {
        if (m_filterText == text) return;
        m_filterText = text;

        setFilterWildcard(m_filterText);
        emit filterTextChanged(m_filterText);
        invalidateFilter();
    }

signals:
    void filterTextChanged(const QString&);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override
    {
        if (m_filterText.trimmed().isEmpty())
            return true;

        const QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);
        const QString name = sourceModel()->data(idx, Qt::DisplayRole).toString();
        const QString path = sourceModel()->data(idx, QFileSystemModel::FilePathRole).toString();

        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent) ||
            name.contains(m_filterText, Qt::CaseInsensitive) ||
            path.contains(m_filterText, Qt::CaseInsensitive);
    }

private:
    QString m_filterText;
};