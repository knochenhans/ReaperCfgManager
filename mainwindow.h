#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QActionGroup>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
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

private slots:
  void open();
  void save();
  void updateView();

private:
  void createActions();
  void createMenus();

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

  void Test();
};

#endif // MAINWINDOW_H
