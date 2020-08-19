#include "DataReceiver.h"
//初始化操作
DataReceiver::DataReceiver(QWidget* parent)
    : QMainWindow(parent)
{

     initUI();
     initTable();   
     initNetwork();
     initDataBase();
     updateTable(true);
}
//初始化表格
void DataReceiver::initTable()
{
    m_tableItems = ui.tableView;

    m_model = new QStandardItemModel();
    m_model->setColumnCount(6);
    m_tableItems->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}
//绑定ui组件
void DataReceiver::initUI()
{
    ui.setupUi(this);


    m_showtp = ui.temp;
    m_showrh = ui.RH;
    m_showtime = ui.timelabel;
    
    //绑定展示图按钮，点击生成图
    connect(ui.showchartbtm, &QPushButton::clicked, this, [=]() {DataChart* chart_page = new DataChart();
    chart_page->show();
    connect(this, SIGNAL(onUpdateTable(Data)), chart_page, SLOT(onReceiveData(Data))); });

    connect(ui.exit, &QPushButton::clicked, this, [=]() {this->close(); });


}
//初始化网络组建
void DataReceiver::initNetwork()
{
    //点击启动服务器按钮，开始尝试连接服务器TCP
    connect(ui.startServer, &QPushButton::clicked, this, [=]() {StartConnection(true); });
    //同时生成UDP套接字准备接受温度等数据
    connect(ui.startServer, &QPushButton::clicked, this, [=]() { m_udpDatarcvr = new QUdpSocket(this);
    m_udpDatarcvr->bind(QHostAddress::LocalHost, 6665);
    connect(m_udpDatarcvr, SIGNAL(readyRead()), this, SLOT(onDataRcvDealWith())); });

    //。。。关闭。。。。断开。。。
    connect(ui.stopServer, &QPushButton::clicked, this, [=]() {StartConnection(false); });
    //。。。销毁。。。
    connect(ui.stopServer, &QPushButton::clicked, this, [=]() {disconnect(m_udpDatarcvr, SIGNAL(readyRead()), this, SLOT(onDataRcvDealWith()));
    m_udpDatarcvr->close(); });
}
//初始化数据库
void DataReceiver::initDataBase()
{
    //尝试查找数据库
    if (QSqlDatabase::contains("TRdatabase.db")) {
        m_database = QSqlDatabase::database("TRdatabase.db");
    }
    else {
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName("TRdatabase.db");
    }
    if (!m_database.open())
    {
        qDebug() << m_database.lastError();
    }
    else
    {
        qDebug() << "Succeed to connect database.";
    }
    //创建表格
    QSqlQuery sql_query;

    if (!sql_query.exec("create table TRinfo(time int primary key, temp double, rh double)"))
    {
        qDebug() <<  sql_query.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
}
//写入数据库
void DataReceiver::writeToDatabase(int time, double temp, double RH)
{
    QSqlQuery query;
    query.prepare("INSERT INTO TRinfo (time, temp, rh) VALUES (:Time, :Temp, :Rh)");
    query.bindValue(":Time", time);
    query.bindValue(":Temp", temp);
    query.bindValue(":Rh", RH);
    query.exec();

}
//更新表格
void DataReceiver::updateTable(bool isLoadDB)
{
    if (isLoadDB) {
        //读取数据库
        m_datas.clear();
        QSqlQuery query;

        query.prepare("SELECT * FROM TRinfo");
        query.exec();
        QSqlRecord recode = query.record();
        int column = recode.count();
        QString s1 = recode.fieldName(0);

        while (query.next())
        {
            Data tmp;
            tmp.time = query.value("time").toString();
            tmp.tp = query.value("temp").toDouble();
            tmp.rh = query.value("rh").toDouble();
            qDebug() << tmp.time;

            m_datas.push_back(tmp);
        }
   }

    //载入model
    m_model->clear();
    for (int i = 0; i < m_datas.size(); i++)
    {
        QStandardItem* tp_item = new QStandardItem(QString::number(m_datas[i].tp, 'f', 2));
        QStandardItem* rh_item = new QStandardItem(QString::number(m_datas[i].rh, 'f', 2));
        tp_item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        rh_item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        m_model->setItem(i, 0, tp_item);
        m_model->setItem(i, 1, rh_item);
    }
    m_tableItems->setModel(m_model);
    //滚动条滚到最后
    m_tableItems->scrollToBottom();
    m_tableItems->show();
}




void DataReceiver::StartConnection(bool isrun)
{
    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->connectToHost(QHostAddress::LocalHost, 8888);
   

    bool isConnected = m_tcpSocket->waitForConnected();
    
    if (isConnected) {
        //控制远程开关
        if (isrun) {
            m_tcpSocket->write("1", 1);
        }
        else
        {   
            m_tcpSocket->write("0", 1);
        }
        //传送数据
        m_tcpSocket->waitForBytesWritten();
        m_tcpSocket->close();
    }
    else
    {
        qDebug() << "Connect to server failed!";
    }

}

//接收远程数据
void DataReceiver::onDataRcvDealWith()
{
    while (m_udpDatarcvr->hasPendingDatagrams()) {
        //吧m_datagram调整成待接收数据的size
        m_datagram.resize(m_udpDatarcvr->pendingDatagramSize());
        m_udpDatarcvr->readDatagram(m_datagram.data(), m_datagram.size());
        
        //把数据写入data
        QBuffer buffer(&m_datagram);
        buffer.open(QIODevice::ReadOnly);
        QDataStream in(&buffer);
        Data rcvdata;
        in >> rcvdata;
        buffer.close();

        //UI展示
        m_showtp->setText(QString::number(rcvdata.tp, 'f', 1));
        m_showrh->setText(QString::number(rcvdata.rh, 'f', 1));
        m_showtime->setText(QTime::fromString(rcvdata.time, "hhmmss").toString("hh ('hour') mm ('mins') ss ('s')"));
        qDebug() << QTime::fromString(rcvdata.time, "hhmmss").toString("hh:mm:ss");
        //存数据
        writeToDatabase(rcvdata.time.toInt(), rcvdata.tp, rcvdata.rh);
        //更新表格
        m_datas.append(rcvdata);
        updateTable(false);
        //向chartView发送信号
         emit onUpdateTable(rcvdata);
        
    }
}


