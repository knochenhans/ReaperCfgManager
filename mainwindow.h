#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QTextStream>
#include <QTreeView>
#include <QVBoxLayout>

#include <treeitem.h>
#include <treemodel.h>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void selectionChanged1(const QItemSelection &selected,
                         const QItemSelection &deselected);
  void selectionChanged2(const QItemSelection &selected,
                         const QItemSelection &deselected);
  void btnToLeftClicked();
  void btnToRightClicked();

  void btnFilterToRightClicked();
  void btnFilterEqualClicked();

  void expanded1(const QModelIndex &index);
  void expanded2(const QModelIndex &index);
  void collapsed1(const QModelIndex &index);
  void collapsed2(const QModelIndex &index);

private slots:
  void open();
  void save();
  void updateView();

private:
  void createActions();
  void createMenus();
  void selectOther(const QItemSelection &selected, QTreeView *otherView);
  void expandOther(const QModelIndex &index, QTreeView *otherView);
  void collapseOther(const QModelIndex &index, QTreeView *otherView);

  QMenu *fileMenu;

  QActionGroup *alignmentGroup;
  QAction *openAct;
  QAction *saveAct;
  QAction *exitAct;

  QAction *testAct;

  TreeModel *model1;
  TreeModel *model2;
  QTreeView *view1;
  QTreeView *view2;

  QLineEdit *filepath1;
  QLineEdit *filepath2;

  QPushButton *btnToLeft;
  QPushButton *btnToRight;

  QPushButton *btnFilterToRight;
  QPushButton *btnFilterEqual;

  void Test();
};

#endif // MAINWINDOW_H
