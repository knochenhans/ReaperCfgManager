#ifndef TREEITEM_H
#define TREEITEM_H

#include <QDebug>
#include <QList>
#include <QVariant>

class TreeItem {
public:
  explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0);
  ~TreeItem();

  void appendChild(TreeItem *child);
  void insertChild(TreeItem *child, int beforeRow);
  void removeChild(int row);
  void sortChildren();
  void alignChildren(TreeItem *other);

  TreeItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  TreeItem *parentItem();

private:
  QList<TreeItem *> m_childItems;
  QList<QVariant> m_itemData;
  TreeItem *m_parentItem;
};

#endif // TREEITEM_H
