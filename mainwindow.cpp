#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QWidget *widget = new QWidget;
  setCentralWidget(widget);

  /*QWidget* topFiller = new QWidget;
  topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QWidget* bottomFiller = new QWidget;
  bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);*/

  view1 = new QTreeView();
  view1->setWindowTitle(QObject::tr("Simple Tree Model"));
  view1->show();

  view2 = new QTreeView();
  view2->setWindowTitle(QObject::tr("Simple Tree Model"));
  view2->show();

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setMargin(5);
  // layout->addWidget(topFiller);
  layout->addWidget(view1);
  layout->addWidget(view2);
  // layout->addWidget(bottomFiller);
  widget->setLayout(layout);

  createActions();
  createMenus();

  /*QString message = tr("A context menu is available by right-clicking");
  statusBar()->showMessage(message);*/

  setWindowTitle(tr("Menus"));
  setMinimumSize(160, 160);
  resize(1024, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::open() {
  //  QString fileName = QFileDialog::getOpenFileName(
  //      this, tr("Open Configuration File"),
  //      "/mnt/Daten/Datentausch/homeshare/Programmierung/C/"
  //      "build-ReaperCfgManager-Desktop-Debug/data/",
  //      tr("Reaper Configuration Files (*.ini)"));

  QFile file1("/mnt/Daten/Datentausch/homeshare/Programmierung/C/"
              "build-ReaperCfgManager-Desktop-Debug/data/REAPER.ini");

  // connect(model, &TreeModel::dataChanged, this, &MainWindow::updateView);

  file1.open(QIODevice::ReadOnly);
  model1 = new TreeModel(file1.readAll());
  view1->setModel(model1);
  file1.close();

  QFile file2("/home/andre/.config/REAPER/reaper.ini");

  file2.open(QIODevice::ReadOnly);
  model2 = new TreeModel(file2.readAll());
  view2->setModel(model2);
  file2.close();

  // Set column width
  for (int c = 0; c < model1->columnCount(); c++) {
    view1->setColumnWidth(c, 200);
  }

  for (int c = 0; c < model2->columnCount(); c++) {
    view2->setColumnWidth(c, 200);
  }

  model2->align(model1);
  model1->align(model2);
}

void MainWindow::save() {}

void MainWindow::updateView() {
  /*for (int r = 0; r < model->rowCount(); r++) {
      view->setColumnWidth(r, 100);
  }*/
}

void MainWindow::createActions() {
  openAct = new QAction(tr("&Open"), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open"));
  connect(openAct, &QAction::triggered, this, &MainWindow::open);

  saveAct = new QAction(tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save"));
  connect(saveAct, &QAction::triggered, this, &MainWindow::save);

  exitAct = new QAction(tr("&Quit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Quit"));
  connect(exitAct, &QAction::triggered, this, &MainWindow::close);

  testAct = new QAction(tr("&Test"), this);
  testAct->setStatusTip(tr("Test"));
  connect(testAct, &QAction::triggered, this, &MainWindow::Test);
}

void MainWindow::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);
  fileMenu->addAction(testAct);
}

void MainWindow::Test() {}
