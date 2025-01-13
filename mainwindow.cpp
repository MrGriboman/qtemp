#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), plot(nullptr), curve(nullptr) {
    setupUI();
    setupDatabase();
}

MainWindow::~MainWindow() {
    db.close();
}

void MainWindow::setupUI() {
    // Setup controls
    tableSelector = new QComboBox(this);
    tableSelector->addItem("Все измерения", "TEMP_LOGS");
    tableSelector->addItem("Средняя по часам", "HOURLY_TEMP");
    tableSelector->addItem("Средняя по дням", "DAILY_TEMP");

    startDateEdit = new QDateEdit(this);
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDate(QDate::currentDate().addDays(-7));

    endDateEdit = new QDateEdit(this);
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(QDate::currentDate());

    plotButton = new QPushButton("Построить график", this);
    connect(plotButton, &QPushButton::clicked, this, &MainWindow::fetchData);

    // Setup plot
    plot = new QwtPlot(this);
    plot->setTitle("Temperature Data");
    plot->setAxisTitle(QwtPlot::xBottom, "Time");
    plot->setAxisTitle(QwtPlot::yLeft, "Temperature");

    curve = new QwtPlotCurve();
    curve->attach(plot);

    // Layout
    QVBoxLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(tableSelector);
    controlsLayout->addWidget(startDateEdit);
    controlsLayout->addWidget(endDateEdit);
    controlsLayout->addWidget(plotButton);

    QWidget *controlPanel = new QWidget(this);
    controlPanel->setLayout(controlsLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(plot);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::setupDatabase() {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("TEMP_DATABASE");
    db.setUserName("mrgriboman");
    db.setPassword("frog1324");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
    }
}

void MainWindow::fetchData() {
    QString table = tableSelector->currentData().toString();
    QString startDate = startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = endDateEdit->date().toString("yyyy-MM-dd");

    QSqlQuery query;
    query.prepare(QString("SELECT TIMESTAMP, TEMPERATURE FROM %1 WHERE TIMESTAMP BETWEEN :start AND :end").arg(table));
    query.bindValue(":start", startDate);
    query.bindValue(":end", endDate);

    if (!query.exec()) {
        QMessageBox::critical(this, "Query Error", query.lastError().text());
        return;
    }

    QVector<double> x, y;
    while (query.next()) {
        QDateTime timestamp = query.value("TIMESTAMP").toDateTime();
        double temperature = query.value("TEMPERATURE").toDouble();
        x.append(timestamp.toMSecsSinceEpoch());
        y.append(temperature);
    }

    if (x.isEmpty()) {
        QMessageBox::information(this, "No Data", "No data found for the selected range.");
        return;
    }

    curve->setSamples(x.data(), y.data(), x.size());
    plot->replot();
}
