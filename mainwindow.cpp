#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QWidget *widget = new QWidget;
  setCentralWidget(widget);

  for (int i = 0; i < 2; i++) {
    views << new QTreeView;
    views[i]->setWindowTitle(QObject::tr("Configuration File Tree View"));
    views[i]->show();

    filepaths << new QLineEdit;
    filepathBrowse << new QPushButton("…");
    filepathBrowse[i]->setMaximumWidth(32);

    models << new TreeModel;
  }

  QHBoxLayout *layout = new QHBoxLayout;
  QVBoxLayout *center = new QVBoxLayout;
  QVBoxLayout *treeLayout1 = new QVBoxLayout;
  QVBoxLayout *treeLayout2 = new QVBoxLayout;
  QHBoxLayout *filepathLayout1 = new QHBoxLayout;
  QHBoxLayout *filepathLayout2 = new QHBoxLayout;

  toLeft = new QPushButton("<", this);
  toRight = new QPushButton(">", this);
  filterEqual = new QPushButton("=", this);
  filterToRight = new QPushButton(">", this);

  filterEqual->setMaximumWidth(24);
  filterToRight->setMaximumWidth(24);
  toLeft->setMaximumWidth(32);
  toRight->setMaximumWidth(32);

  center->addWidget(filterToRight);
  center->addWidget(filterEqual);
  center->addWidget(toLeft);
  center->addWidget(toRight);

  // layout->setMargin(5);

  filepathLayout1->addWidget(filepaths[0]);
  filepathLayout1->addWidget(filepathBrowse[0]);
  filepathLayout2->addWidget(filepaths[1]);
  filepathLayout2->addWidget(filepathBrowse[1]);
  treeLayout1->addLayout(filepathLayout1);
  treeLayout1->addWidget(views[0]);
  layout->addLayout(treeLayout1);
  layout->addLayout(center);
  treeLayout2->addLayout(filepathLayout2);
  treeLayout2->addWidget(views[1]);
  layout->addLayout(treeLayout2);

  widget->setLayout(layout);

  createActions();
  createMenus();

  setWindowTitle(tr("Reaper Configuration Manager"));
  setMinimumSize(160, 160);
  resize(1024, 600);

  // views[0]->setItemDelegate(new TestDelegate);
}

MainWindow::~MainWindow() {}

void MainWindow::open() {
  //  QString fileName = QFileDialog::getOpenFileName(
  //      this, tr("Open Configuration File"),
  //      "/mnt/Daten/Datentausch/homeshare/Programmierung/C/"
  //      "build-ReaperCfgManager-Desktop-Debug/data/",
  //      tr("Reaper Configuration Files (*.ini)"));

  QList<QItemSelectionModel *> selectionModels;

  files << new QFile("/mnt/Daten/Datentausch/homeshare/Programmierung/C/"
                     "build-ReaperCfgManager-Desktop-Debug/data/REAPER.ini");
  files << new QFile("/home/andre/.config/REAPER/reaper.ini");
  // connect(model, &TreeModel::dataChanged, this, &MainWindow::updateView);

  for (int i = 0; i < 2; i++) {
    files[i]->open(QIODevice::ReadOnly);
    models[i]->load(files[i]->readAll());
    files[i]->close();
  }

  for (int i = 0; i < 2; i++) {
    views[i]->setModel(models[i]);

    // Elide Paths
    QFontMetrics metrix(filepaths[i]->font());
    filepaths[i]->setText(metrix.elidedText(
        files[i]->fileName(), Qt::ElideMiddle, filepaths[i]->width()));

    models[i]->setOtherModel(models[(i + 1) % 2]);

    // Align files to eacher (sort right file content by like left one, fill
    // differing contents with blank rows)

    models[i]->align();

    // Set column width

    views[i]->resizeColumnToContents(0);

    // Set up selection

    selectionModels << new QItemSelectionModel(models[i]);
    views[i]->setSelectionModel(selectionModels[i]);
  }

  connect(
      selectionModels[0],
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      this, SLOT(selectionChanged1(QItemSelection, QItemSelection)));
  connect(
      selectionModels[1],
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      this, SLOT(selectionChanged2(QItemSelection, QItemSelection)));

  connect(toLeft, SIGNAL(clicked(bool)), this, SLOT(toLeftClicked()));
  connect(toRight, SIGNAL(clicked(bool)), this, SLOT(toRightClicked()));
  connect(filterToRight, SIGNAL(clicked(bool)), this,
          SLOT(filterToRightClicked()));
  connect(filterEqual, SIGNAL(clicked(bool)), this, SLOT(filterEqualClicked()));

  connect(views[0], SIGNAL(expanded(QModelIndex)), this,
          SLOT(expanded1(QModelIndex)));
  connect(views[1], SIGNAL(expanded(QModelIndex)), this,
          SLOT(expanded2(QModelIndex)));

  connect(views[0], SIGNAL(collapsed(QModelIndex)), this,
          SLOT(collapsed1(QModelIndex)));
  connect(views[1], SIGNAL(collapsed(QModelIndex)), this,
          SLOT(collapsed2(QModelIndex)));

  connect(views[0]->verticalScrollBar(), SIGNAL(valueChanged(int)),
          views[1]->verticalScrollBar(), SLOT(setValue(int)));

  connect(filepathBrowse[0], SIGNAL(clicked(bool)), this,
          SLOT(filepathBrowse1Clicked()));
  connect(filepathBrowse[1], SIGNAL(clicked(bool)), this,
          SLOT(filepathBrowse2Clicked()));
}

void MainWindow::loadView(int treeIndex, QString filename) {
  files[treeIndex]->setFileName(filename);

  files[treeIndex]->open(QIODevice::ReadOnly);
  models[treeIndex]->load(files[treeIndex]->readAll());
  files[treeIndex]->close();
}

void MainWindow::loadView(int treeIndex) {
  QString filename =
      QFileDialog::getOpenFileName(this, tr("Open Configuration File"), "",
                                   tr("Reaper Configuration Files (*.ini)"));

  if (!filename.isEmpty()) {
    loadView(treeIndex, filename);
  }
}

void MainWindow::save1() {
  int treeIndex = 0;

  // DEBUGGING
  files[treeIndex]->setFileName("/tmp/test" + QString::number(treeIndex));

  files[treeIndex]->open(QIODevice::WriteOnly);

  QTextStream out(files[treeIndex]);
  models[treeIndex]->save(out);

  files[treeIndex]->close();
}

void MainWindow::save2() {
  int treeIndex = 1;

  // DEBUGGING
  files[treeIndex]->setFileName("/tmp/test" + QString::number(treeIndex));

  files[treeIndex]->open(QIODevice::WriteOnly);

  QTextStream out(files[treeIndex]);
  models[treeIndex]->save(out);

  files[treeIndex]->close();
}

void MainWindow::updateView() {
  /*for (int r = 0; r < model->rowCount(); r++) {
      view->setColumnWidth(r, 100);
  }*/
}

void MainWindow::selectionChanged1(const QItemSelection &selected,
                                   const QItemSelection &deselected) {
  selectOther(selected, views[1]);
}

void MainWindow::selectionChanged2(const QItemSelection &selected,
                                   const QItemSelection &deselected) {
  selectOther(selected, views[0]);
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

void MainWindow::toLeftClicked() {
  QModelIndexList indexes = views[1]->selectionModel()->selection().indexes();

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

      parentIndex = models[0]->index(parentRow, parentColumn, QModelIndex());
    }

    otherIndex = models[0]->index(row, index.column(), parentIndex);

    // models[0]->setData(otherIndex, data, Qt::DisplayRole, index.column());
    models[0]->setData(otherIndex, data, Qt::DisplayRole);
  }
}

void MainWindow::toRightClicked() {}

void MainWindow::filterToRightClicked() {}
void MainWindow::filterEqualClicked() {}

void MainWindow::expanded1(const QModelIndex &index) {
  expandOther(index, views[1]);
}

void MainWindow::expanded2(const QModelIndex &index) {
  expandOther(index, views[0]);
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
  collapseOther(index, views[1]);
}

void MainWindow::collapsed2(const QModelIndex &index) {
  collapseOther(index, views[0]);
}

void MainWindow::filepathBrowse1Clicked() { loadView(0); }

void MainWindow::filepathBrowse2Clicked() { loadView(1); }

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

  save1Act = new QAction(tr("&Save Left"), this);
  save1Act->setShortcut(QKeySequence(tr("Ctrl+S")));
  save1Act->setStatusTip(tr("Save Left"));
  connect(save1Act, &QAction::triggered, this, &MainWindow::save1);

  save2Act = new QAction(tr("&Save Right"), this);
  save2Act->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
  save2Act->setStatusTip(tr("Save Right"));
  connect(save2Act, &QAction::triggered, this, &MainWindow::save2);

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
  fileMenu->addAction(save1Act);
  fileMenu->addAction(save2Act);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);
  fileMenu->addAction(testAct);
}

void MainWindow::Test() {}
