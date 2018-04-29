#include "treemodel.h"
#include "treeitem.h"

#include <QStringList>

TreeModel::TreeModel(QObject *parent) : QAbstractItemModel(parent) {}

TreeModel::TreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent) {
  load(data);
}

TreeModel::~TreeModel() { delete rootItem; }

bool TreeModel::setData(const QModelIndex &index, const QVariant &value,
                        int role) {

  if (role == Qt::EditRole) {
    TreeItem *i = static_cast<TreeItem *>(index.internalPointer());

    i->setDataCol(value, index.column());
    emit dataChanged(index, index);
  }

  return true;
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

void TreeModel::align() {
  beginResetModel();
  rootItem->alignChildren(otherModel->getRootItem());

  endResetModel();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
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
        } else {
          return QVariant(QColor("#BDDDFF"));
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

  return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
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

void TreeModel::load(const QString &data) {
  beginResetModel();
  QList<QVariant> rootData;
  rootData << "Key"
           << "Value";
  rootItem = new TreeItem(rootData);
  setupModelData(data.split(QString("\n")), rootItem);
  endResetModel();
}

void TreeModel::save(QTextStream &out) {
  for (int i = 0; i < rootItem->childCount(); i++) {
    TreeItem *item = rootItem->child(i);

    if (!item->data(0).toString().isEmpty()) {

      out << "[" << item->data(0).toString() << "]" << endl;

      for (int j = 0; j < item->childCount(); j++) {
        TreeItem *subItem = item->child(j);

        if (!subItem->data(0).toString().isEmpty()) {

          out << subItem->data(0).toString() << "="
              << subItem->data(1).toString() << endl;
        }
      }

      out << endl;
    }
  }
}

TreeItem *TreeModel::getRootItem() const { return rootItem; }
