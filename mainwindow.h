#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void fetchData();

private:
    void setupUI();
    void setupDatabase();

    QSqlDatabase db;
    QComboBox *tableSelector;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QPushButton *plotButton;
    QwtPlot *plot;
    QwtPlotCurve *curve;
};

#endif // MAINWINDOW_H
