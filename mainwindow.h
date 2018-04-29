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
  void toLeftClicked();
  void toRightClicked();

  void filterToRightClicked();
  void filterEqualClicked();

  void expanded1(const QModelIndex &index);
  void expanded2(const QModelIndex &index);
  void collapsed1(const QModelIndex &index);
  void collapsed2(const QModelIndex &index);

  void filepathBrowse1Clicked();
  void filepathBrowse2Clicked();

private slots:
  void open();
  void loadView(int treeIndex, QString filename);
  void loadView(int treeIndex);
  void save1();
  void save2();
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
  QAction *save1Act;
  QAction *save2Act;
  QAction *exitAct;

  QAction *testAct;

  QList<QTreeView *> views;
  QList<TreeModel *> models;
  QList<QLineEdit *> filepaths;
  QList<QPushButton *> filepathBrowse;
  QList<QFile *> files;

  QPushButton *toLeft;
  QPushButton *toRight;

  QPushButton *filterToRight;
  QPushButton *filterEqual;

  void Test();
};

#endif // MAINWINDOW_H
