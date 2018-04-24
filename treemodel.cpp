#include "treemodel.h"
#include "treeitem.h"

#include <QStringList>

TreeModel::TreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent) {
  QList<QVariant> rootData;
  rootData << "Key"
           << "Value";
  rootItem = new TreeItem(rootData);
  setupModelData(data.split(QString("\n")), rootItem);
}

TreeModel::~TreeModel() { delete rootItem; }

bool TreeModel::setData(const QModelIndex &index, const QVariant &value,
                        int role, int col) {
  if (role == Qt::DisplayRole) {
    TreeItem *i = static_cast<TreeItem *>(index.internalPointer());

    i->setDataCol(value, col);
  }

  emit dataChanged(index, index);

  return true;

  // return QAbstractItemModel::setData(index, value, role);
}

int TreeModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return static_cast<TreeItem *>(parent.internalPointer())->columnCount();
  } else {
    return rootItem->columnCount();
  }
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent) {
  beginRemoveRows(QModelIndex(), row, row + count - 1);

  // For now, remove only the first row specified

  TreeItem *i = rootItem->child(row);
  i->removeChild(row);

  endRemoveRows();

  return true;
}

void TreeModel::sort(int column, Qt::SortOrder order) {
  // Sort by high level children first

  beginResetModel();

  rootItem->sortChildren();

  // Now sort all level 2 children

  for (int i = 0; i < rootItem->childCount(); i++) {
    rootItem->child(i)->sortChildren();
  }

  endResetModel();
}

void TreeModel::align(TreeModel *other) {
  beginResetModel();
  rootItem->alignChildren(other->getRootItem());

  endResetModel();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::DisplayRole) {
    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    return item->data(index.column());
  }

  if (role == Qt::FontRole) {
    if (!index.parent().isValid()) {
      QFont boldFont;
      boldFont.setBold(true);
      return boldFont;
    }
  }

  if (role == Qt::BackgroundRole) {
    // Color empty rows in other widget green

    if (otherModel) {
      // Find same field in other model

      QModelIndex otherIndex;
      QModelIndex otherParentIndex;

      if (index.parent().isValid()) {
        int otherParentRow = index.parent().row();
        int otherParentColumn = index.parent().column();

        // Find same parent in other widget
        otherParentIndex =
            otherModel->index(otherParentRow, otherParentColumn, QModelIndex());
      }

      otherIndex =
          otherModel->index(index.row(), index.column(), otherParentIndex);

      if (index.data().toString().compare(otherIndex.data().toString(),
                                          Qt::CaseInsensitive) != 0) {
        if (index.data().toString() == "") {
          return QVariant(QColor("#D0FFA3"));
        }
      }
    }
  }

  return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) {
    return 0;
  }

  return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return rootItem->data(section);
  }

  return QVariant();
}

QModelIndex TreeModel::index(int row, int column,
                             const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  TreeItem *parentItem;

  if (!parent.isValid()) {
    parentItem = rootItem;
  } else {
    parentItem = static_cast<TreeItem *>(parent.internalPointer());
  }

  TreeItem *childItem = parentItem->child(row);

  if (childItem) {
    return createIndex(row, column, childItem);
  } else {
    return QModelIndex();
  }
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) {
    return QModelIndex();
  }

  TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
  TreeItem *parentItem = childItem->parentItem();

  if (parentItem == rootItem) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const {
  TreeItem *parentItem;

  if (parent.column() > 0) {
    return 0;
  }

  if (!parent.isValid()) {
    parentItem = rootItem;
  } else {
    parentItem = static_cast<TreeItem *>(parent.internalPointer());
  }

  return parentItem->childCount();
}

void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent) {
  int number = 0;
  TreeItem *currentParent;

  while (number < lines.count()) {
    QString lineData = lines[number].trimmed();

    if (!lineData.isEmpty()) {
      QList<QVariant> data;

      QRegularExpression re("^\\[(.*)\\]$");
      QRegularExpressionMatch match = re.match(lineData);

      if (match.hasMatch()) {
        QString line = match.captured(1);

        data << line << "";

        currentParent = new TreeItem(data, parent);

        parent->appendChild(currentParent);

      } else {
        if (currentParent) {
          QRegularExpression re("^(.*)=(.*)$");
          QRegularExpressionMatch match = re.match(lineData);

          if (match.hasMatch()) {
            QString line1 = match.captured(1);
            QString line2 = match.captured(2);

            QList<QVariant> data;
            data << line1 << line2;

            currentParent->appendChild(new TreeItem(data, currentParent));
          }
        }
      }
    }

    number++;
  }
}

void TreeModel::setOtherModel(TreeModel *value) { otherModel = value; }

TreeItem *TreeModel::getRootItem() const { return rootItem; }
