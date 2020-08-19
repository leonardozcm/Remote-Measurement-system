#include "DataReceiver.h"
//��ʼ������
DataReceiver::DataReceiver(QWidget* parent)
    : QMainWindow(parent)
{

     initUI();
     initTable();   
     initNetwork();
     initDataBase();
     updateTable(true);
}
//��ʼ�����
void DataReceiver::initTable()
{
    m_tableItems = ui.tableView;

    m_model = new QStandardItemModel();
    m_model->setColumnCount(6);
    m_tableItems->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}
//��ui���
void DataReceiver::initUI()
{
    ui.setupUi(this);


    m_showtp = ui.temp;
    m_showrh = ui.RH;
    m_showtime = ui.timelabel;
    
    //��չʾͼ��ť���������ͼ
    connect(ui.showchartbtm, &QPushButton::clicked, this, [=]() {DataChart* chart_page = new DataChart();
    chart_page->show();
    connect(this, SIGNAL(onUpdateTable(Data)), chart_page, SLOT(onReceiveData(Data))); });

    connect(ui.exit, &QPushButton::clicked, this, [=]() {this->close(); });


}
//��ʼ�������齨
void DataReceiver::initNetwork()
{
    //���������������ť����ʼ�������ӷ�����TCP
    connect(ui.startServer, &QPushButton::clicked, this, [=]() {StartConnection(true); });
    //ͬʱ����UDP�׽���׼�������¶ȵ�����
    connect(ui.startServer, &QPushButton::clicked, this, [=]() { m_udpDatarcvr = new QUdpSocket(this);
    m_udpDatarcvr->bind(QHostAddress::LocalHost, 6665);
    connect(m_udpDatarcvr, SIGNAL(readyRead()), this, SLOT(onDataRcvDealWith())); });

    //�������رա��������Ͽ�������
    connect(ui.stopServer, &QPushButton::clicked, this, [=]() {StartConnection(false); });
    //���������١�����
    connect(ui.stopServer, &QPushButton::clicked, this, [=]() {disconnect(m_udpDatarcvr, SIGNAL(readyRead()), this, SLOT(onDataRcvDealWith()));
    m_udpDatarcvr->close(); });
}
//��ʼ�����ݿ�
void DataReceiver::initDataBase()
{
    //���Բ������ݿ�
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
    //�������
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
//д�����ݿ�
void DataReceiver::writeToDatabase(int time, double temp, double RH)
{
    QSqlQuery query;
    query.prepare("INSERT INTO TRinfo (time, temp, rh) VALUES (:Time, :Temp, :Rh)");
    query.bindValue(":Time", time);
    query.bindValue(":Temp", temp);
    query.bindValue(":Rh", RH);
    query.exec();

}
//���±��
void DataReceiver::updateTable(bool isLoadDB)
{
    if (isLoadDB) {
        //��ȡ���ݿ�
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

    //����model
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
    //�������������
    m_tableItems->scrollToBottom();
    m_tableItems->show();
}




void DataReceiver::StartConnection(bool isrun)
{
    m_tcpSocket = new QTcpSocket(this);
    m_tcpSocket->connectToHost(QHostAddress::LocalHost, 8888);
   

    bool isConnected = m_tcpSocket->waitForConnected();
    
    if (isConnected) {
        //����Զ�̿���
        if (isrun) {
            m_tcpSocket->write("1", 1);
        }
        else
        {   
            m_tcpSocket->write("0", 1);
        }
        //��������
        m_tcpSocket->waitForBytesWritten();
        m_tcpSocket->close();
    }
    else
    {
        qDebug() << "Connect to server failed!";
    }

}

//����Զ������
void DataReceiver::onDataRcvDealWith()
{
    while (m_udpDatarcvr->hasPendingDatagrams()) {
        //��m_datagram�����ɴ��������ݵ�size
        m_datagram.resize(m_udpDatarcvr->pendingDatagramSize());
        m_udpDatarcvr->readDatagram(m_datagram.data(), m_datagram.size());
        
        //������д��data
        QBuffer buffer(&m_datagram);
        buffer.open(QIODevice::ReadOnly);
        QDataStream in(&buffer);
        Data rcvdata;
        in >> rcvdata;
        buffer.close();

        //UIչʾ
        m_showtp->setText(QString::number(rcvdata.tp, 'f', 1));
        m_showrh->setText(QString::number(rcvdata.rh, 'f', 1));
        m_showtime->setText(QTime::fromString(rcvdata.time, "hhmmss").toString("hh ('hour') mm ('mins') ss ('s')"));
        qDebug() << QTime::fromString(rcvdata.time, "hhmmss").toString("hh:mm:ss");
        //������
        writeToDatabase(rcvdata.time.toInt(), rcvdata.tp, rcvdata.rh);
        //���±��
        m_datas.append(rcvdata);
        updateTable(false);
        //��chartView�����ź�
         emit onUpdateTable(rcvdata);
        
    }
}


