#include <QStringList>

#include "treeitem.h"

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent) {
  m_parentItem = parent;
  m_itemData = data;
}

TreeItem::~TreeItem() { qDeleteAll(m_childItems); }

void TreeItem::appendChild(TreeItem *item) { m_childItems.append(item); }

void TreeItem::insertChild(TreeItem *child, int beforeRow) {
  m_childItems.insert(beforeRow, child);
}

void TreeItem::removeChild(int row) { m_childItems.removeAt(row); }

void TreeItem::sortChildren() {
  for (int i = 0; i < childCount() - 1; i++) {
    for (int j = i + 1; j < childCount(); j++) {
      qDebug() << m_childItems[i]->data(0).toString() << " <> "
               << m_childItems[j]->data(0).toString();
      if (m_childItems[i]->data(0).toString().compare(
              m_childItems[j]->data(0).toString(), Qt::CaseInsensitive) > 0) {
        m_childItems.swap(i, j);
      }
    }
  }
}

void TreeItem::alignChildren(TreeItem *other) {
  // Find associated rows and move accordingly
  for (int i = 0; i < other->childCount(); i++) {
    // Ignore blank lines
    if (other->child(i)->data(0).toString() != "") {
      bool found = false;

      for (int j = 0; j < this->childCount(); j++) {
        if (other->child(i)->data(0).toString().compare(
                this->child(j)->data(0).toString(), Qt::CaseInsensitive) == 0) {

          // Found child in other tree, move this accordingly

          m_childItems.move(j, i);

          found = true;
        }
      }

      // Child not found in other tree, leave blank here
      if (!found) {
        QList<QVariant> data;
        data << ""
             << "";
        TreeItem *filler = new TreeItem(data, this);
        this->insertChild(filler, i);
      }
    }
  }
}

void TreeItem::fillChildren(TreeItem *other) {
  // Fill up empty rows
  QList<QVariant> data;
  data << ""
       << "";
  TreeItem *filler = new TreeItem(data, this);

  int diff = this->childCount() - other->childCount();
  if (diff < 0) {
    // Fill up children until equal with other model
    for (int i = 0; i < abs(diff); i++) {
      this->appendChild(filler);
    }
  }
}

TreeItem *TreeItem::child(int row) { return m_childItems.value(row); }

int TreeItem::childCount() const { return m_childItems.count(); }

int TreeItem::columnCount() const { return m_itemData.count(); }

QVariant TreeItem::data(int column) const { return m_itemData.value(column); }

TreeItem *TreeItem::parentItem() { return m_parentItem; }

int TreeItem::row() const {
  if (m_parentItem) {
    return m_parentItem->m_childItems.indexOf(const_cast<TreeItem *>(this));
  }

  return 0;
}