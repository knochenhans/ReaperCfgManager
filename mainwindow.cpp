#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QWidget *widget = new QWidget;
  setCentralWidget(widget);

  view1 = new QTreeView();
  view1->setWindowTitle(QObject::tr("Simple Tree Model"));
  view1->show();

  view2 = new QTreeView();
  view2->setWindowTitle(QObject::tr("Simple Tree Model"));
  view2->show();

  filepath1 = new QLineEdit();
  filepath2 = new QLineEdit();

  QHBoxLayout *layout = new QHBoxLayout;
  QVBoxLayout *center = new QVBoxLayout;
  QVBoxLayout *tree1 = new QVBoxLayout;
  QVBoxLayout *tree2 = new QVBoxLayout;

  btnToLeft = new QPushButton("<", this);
  btnToRight = new QPushButton(">", this);
  btnFilterEqual = new QPushButton("=", this);
  btnFilterToRight = new QPushButton(">", this);

  btnFilterEqual->setMaximumWidth(24);
  btnFilterToRight->setMaximumWidth(24);
  btnToLeft->setMaximumWidth(32);
  btnToRight->setMaximumWidth(32);

  center->addWidget(btnFilterToRight);
  center->addWidget(btnFilterEqual);
  center->addWidget(btnToLeft);
  center->addWidget(btnToRight);

  // layout->setMargin(5);

  tree1->addWidget(filepath1);
  tree1->addWidget(view1);
  layout->addLayout(tree1);
  layout->addLayout(center);
  tree2->addWidget(filepath2);
  tree2->addWidget(view2);
  layout->addLayout(tree2);

  widget->setLayout(layout);

  createActions();
  createMenus();

  setWindowTitle(tr("Reaper Configuration Manager"));
  setMinimumSize(160, 160);
  resize(1024, 600);

  // view1->setItemDelegate(new TestDelegate);
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
  // view1->setHeaderHidden(true);
  file1.close();

  // Elide Paths
  QFontMetrics metrix1(filepath1->font());
  filepath1->setText(metrix1.elidedText(file1.fileName(), Qt::ElideMiddle,
                                        filepath1->width()));

  QFile file2("/home/andre/.config/REAPER/reaper.ini");

  file2.open(QIODevice::ReadOnly);
  model2 = new TreeModel(file2.readAll());
  view2->setModel(model2);
  // view2->setHeaderHidden(true);
  file2.close();

  // Elide Paths
  QFontMetrics metrix2(filepath2->font());
  filepath2->setText(metrix2.elidedText(file2.fileName(), Qt::ElideMiddle,
                                        filepath2->width()));

  model1->setOtherModel(model2);
  model2->setOtherModel(model1);

  // Set column width
  for (int c = 0; c < model1->columnCount(); c++) {
    view1->setColumnWidth(c, 200);
  }

  for (int c = 0; c < model2->columnCount(); c++) {
    view2->setColumnWidth(c, 200);
  }

  // Align files to eacher (sort right file content by like left one, fill
  // differing contents with blank rows)

  model1->align(model2);
  model2->align(model1);

  // Set up selection

  QItemSelectionModel *selectionModel1 = new QItemSelectionModel(model1);
  QItemSelectionModel *selectionModel2 = new QItemSelectionModel(model2);
  view1->setSelectionModel(selectionModel1);
  view2->setSelectionModel(selectionModel2);

  connect(
      selectionModel1,
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      this, SLOT(selectionChanged1(QItemSelection, QItemSelection)));
  connect(
      selectionModel2,
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      this, SLOT(selectionChanged2(QItemSelection, QItemSelection)));

  connect(btnToLeft, SIGNAL(clicked(bool)), this, SLOT(btnToLeftClicked()));
  connect(btnToRight, SIGNAL(clicked(bool)), this, SLOT(btnToRightClicked()));
  connect(btnFilterToRight, SIGNAL(clicked(bool)), this,
          SLOT(btnFilterToRightClicked()));
  connect(btnFilterEqual, SIGNAL(clicked(bool)), this,
          SLOT(btnFilterEqualClicked()));

  connect(view1, SIGNAL(expanded(QModelIndex)), this,
          SLOT(expanded1(QModelIndex)));
  connect(view2, SIGNAL(expanded(QModelIndex)), this,
          SLOT(expanded2(QModelIndex)));

  connect(view1, SIGNAL(collapsed(QModelIndex)), this,
          SLOT(collapsed1(QModelIndex)));
  connect(view2, SIGNAL(collapsed(QModelIndex)), this,
          SLOT(collapsed2(QModelIndex)));

  connect(view1->verticalScrollBar(), SIGNAL(valueChanged(int)),
          view2->verticalScrollBar(), SLOT(setValue(int)));
}

void MainWindow::save() {}

void MainWindow::updateView() {
  /*for (int r = 0; r < model->rowCount(); r++) {
      view->setColumnWidth(r, 100);
  }*/
}

void MainWindow::selectionChanged1(const QItemSelection &selected,
                                   const QItemSelection &deselected) {
  selectOther(selected, view2);
}

void MainWindow::selectionChanged2(const QItemSelection &selected,
                                   const QItemSelection &deselected) {
  selectOther(selected, view1);
}

void MainWindow::selectOther(const QItemSelection &selected,
                             QTreeView *otherView) {
  QItemSelectionModel *selection = new QItemSelectionModel(otherView->model());

  otherView->setSelectionModel(selection);

  foreach (const QModelIndex &index, selected.indexes()) {
    // Check for parent

    QModelIndex otherIndex;
    QModelIndex otherParentIndex;

    if (index.parent().isValid()) {
      int otherParentRow = index.parent().row();
      int otherParentColumn = index.parent().column();

      // Find same parent in other widget
      otherParentIndex = otherView->model()->index(
          otherParentRow, otherParentColumn, QModelIndex());
    }

    otherIndex = otherView->model()->index(index.row(), index.column(),
                                           otherParentIndex);
    selection->select(otherIndex, QItemSelectionModel::Select);
  }
}

void MainWindow::btnToLeftClicked() {
  QModelIndexList indexes = view2->selectionModel()->selection().indexes();

  foreach (const QModelIndex &index, indexes) {
    // Copy data from right to left

    // Create index for left widget

    QVariant data = index.data();
    int row = index.row();
    int column = index.column();

    QModelIndex otherIndex;
    QModelIndex parentIndex;

    //  Check for parent

    if (index.parent().isValid()) {
      int parentRow = index.parent().row();
      int parentColumn = index.parent().column();

      parentIndex = model1->index(parentRow, parentColumn, QModelIndex());
    }

    otherIndex = model1->index(row, index.column(), parentIndex);

    // model1->setData(otherIndex, data, Qt::DisplayRole, index.column());
    model1->setData(otherIndex, data, Qt::DisplayRole);
  }
}

void MainWindow::btnToRightClicked() {}

void MainWindow::btnFilterToRightClicked() {}
void MainWindow::btnFilterEqualClicked() {}

void MainWindow::expanded1(const QModelIndex &index) {
  expandOther(index, view2);
}

void MainWindow::expanded2(const QModelIndex &index) {
  expandOther(index, view1);
}

void MainWindow::expandOther(const QModelIndex &index, QTreeView *otherView) {
  QModelIndex otherIndex;

  otherIndex =
      otherView->model()->index(index.row(), index.column(), QModelIndex());

  // Check if childs of other index are empty

  if (otherIndex.data().toString() == "") {
    // Get number of lines we have to add to other widget temporarily

    // otherIndex.model()->insertRows(otherIndex.row(),index.model()->rowCount(index),
    // otherIndex);
  }

  emit otherView->expand(otherIndex);
}

void MainWindow::collapsed1(const QModelIndex &index) {
  collapseOther(index, view2);
}

void MainWindow::collapsed2(const QModelIndex &index) {
  collapseOther(index, view1);
}

void MainWindow::collapseOther(const QModelIndex &index, QTreeView *otherView) {
  QModelIndex otherIndex;

  otherIndex =
      otherView->model()->index(index.row(), index.column(), QModelIndex());

  emit otherView->collapse(otherIndex);
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
