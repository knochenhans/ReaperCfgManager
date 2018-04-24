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

  // Search in other tree
  for (int i = 0; i < other->childCount(); i++) {

    // Ignore blank lines
    if (other->child(i)->data(0).toString() != "") {
      bool found = false;

      // Look for similar row in this tree
      for (int j = 0; j < this->childCount(); j++) {

        // Compare this to other row
        if (other->child(i)->data(0).toString().compare(
                this->child(j)->data(0).toString(), Qt::CaseInsensitive) == 0) {

          // First sort all sub children
          this->child(j)->alignChildren(other->child(i));

          // Found child in other tree, move this accordingly
          m_childItems.move(j, i);

          found = true;
        }
      }

      // Child not found in other tree, leave blank row here
      if (!found) {
        QList<QVariant> data;
        data << ""
             << "";
        TreeItem *filler = new TreeItem(data, this);

        // Fill sub children
        for (int c = 0; c < other->child(i)->childCount(); c++) {
          filler->appendChild(new TreeItem(data, filler));
        }

        this->insertChild(filler, i);
      }
    }
  }
}

void TreeItem::setData(const QList<QVariant> &data) { m_itemData = data; }
void TreeItem::setDataCol(const QVariant &data, int col) {
  m_itemData[col] = data;
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
