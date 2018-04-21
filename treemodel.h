#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QColor>
#include <QDebug>
#include <QModelIndex>
#include <QRegularExpression>
#include <QVariant>

class TreeItem;

class TreeModel : public QAbstractItemModel {
  Q_OBJECT

public:
  explicit TreeModel(const QString &data, QObject *parent = 0);
  ~TreeModel();

  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole);
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex());
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
  void align(TreeModel *other);

  TreeItem *getRootItem() const;

private:
  void setupModelData(const QStringList &lines, TreeItem *parent);

  TreeItem *rootItem;
};

#endif // TREEMODEL_H
