#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DataReceiver.h"

#include <QWidget>
#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QTableView>
#include <QtNetwork>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <qdebug.h>
#include <QSqlError>
#include <QStandardItemModel>
#include <iostream>
#include "DataChart.h"
#include "Data.h"

class DataReceiver : public QMainWindow
{
    Q_OBJECT

public:
    DataReceiver(QWidget* parent = Q_NULLPTR);
    

private:
    Ui::DataReceiverClass ui;
    QTcpSocket* m_tcpSocket;
    QUdpSocket* m_udpDatarcvr;
    QSqlDatabase m_database;
    QByteArray m_datagram;
    QTableView* m_tableItems;
    QStandardItemModel* m_model;
    QList<Data> m_datas;
    QLabel* m_showtp;
    QLabel* m_showrh;
    QLabel* m_showtime;


    
    void initTable();
    void initUI();
    void initNetwork();
    void initDataBase();
    void writeToDatabase(int time, double temp, double RH);
    void updateTable(bool isLoadDB);

private slots:
    void StartConnection(bool isrun);
    void onDataRcvDealWith();

signals:
    void onUpdateTable(Data);
};
