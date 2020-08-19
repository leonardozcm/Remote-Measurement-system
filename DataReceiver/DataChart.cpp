#include "DataChart.h"

DataChart::DataChart(QWidget* parent)
	: QWidget(parent)
{
	//初始化界面
	initChart();
	initAxis();

	//载入数据，初始化曲线
	initSeries(true);
}

DataChart::~DataChart()
{
}

void DataChart::initChart()
{
	ui.setupUi(this);
	m_tpChartView = ui.Tempchart;
	m_tpChartView->setRubberBand(QChartView::RectangleRubberBand);
	m_tpAxisX = new QValueAxis;
	m_tpAxisX->setTitleText(QString::fromLocal8Bit("时间"));
	m_tpAxisX->setLabelFormat("%d");

}

void DataChart::initAxis()
{
	QValueAxis* tp_axisY = new QValueAxis;
	tp_axisY->setRange(0, 100);
	tp_axisY->setLabelFormat("%d");
	tp_axisY->setTickCount(10);
	tp_axisY->setTitleText(QString::fromLocal8Bit("温度/℃"));


	m_tpChart = new QChart;
	m_tpChartView->setChart(m_tpChart);
	m_tpChart->addAxis(m_tpAxisX, Qt::AlignBottom);
	m_tpChart->addAxis(tp_axisY, Qt::AlignLeft);
	m_tpChart->setTitle(QString::fromLocal8Bit("温度实时监测"));
	m_tpChart->setAnimationOptions(QChart::SeriesAnimations);
	m_tpChart->setBackgroundVisible(false);


	m_rhChartView = ui.RHchart;
	m_rhChartView->setRubberBand(QChartView::RectangleRubberBand);
	m_rhAxisX = new QValueAxis;
	m_rhAxisX->setTitleText(QString::fromLocal8Bit("时间"));
	m_rhAxisX->setLabelFormat("%d");

	QValueAxis* rh_axisY = new QValueAxis;
	rh_axisY->setRange(0, 100);
	rh_axisY->setLabelFormat("%d");
	rh_axisY->setTickCount(10);
	rh_axisY->setTitleText(QString::fromLocal8Bit("湿度/%"));


	m_rhChart = new QChart;
	m_rhChartView->setChart(m_rhChart);
	m_rhChart->addAxis(m_rhAxisX, Qt::AlignBottom);
	m_rhChart->addAxis(rh_axisY, Qt::AlignLeft);
	m_rhChart->setTitle(QString::fromLocal8Bit("湿度实时监测"));
	m_rhChart->setAnimationOptions(QChart::SeriesAnimations);
	m_rhChart->setBackgroundVisible(false);

}

void DataChart::initSeries(bool isLoadDB)
{
	
	if (isLoadDB) {
		m_tpList.clear();
		m_rhList.clear();
		chartDatas.clear();
		QSqlQuery query;

		query.prepare("SELECT * FROM TRinfo");
		query.exec();

		while (query.next())
		{
			Data tmp;
			tmp.time = query.value("time").toString();
			tmp.tp = query.value("temp").toDouble();
			tmp.rh = query.value("rh").toDouble();
			chartDatas.append(tmp);
			m_tpList.append(QPointF(chartDatas.size(), tmp.tp));
			m_rhList.append(QPointF(chartDatas.size(), tmp.rh));
		}
	}


	m_tpSeries = new QSplineSeries();
	m_tpSeries->append(m_tpList);
	m_tpChart->addSeries(m_tpSeries);
	double maxnum = chartDatas.size() > 50 ? chartDatas.size() : 50;
	m_tpAxisX->setRange(0, maxnum);
	m_tpChart->setAxisX(m_tpAxisX, m_tpSeries);


	m_rhSeries = new QSplineSeries();
	m_rhSeries->append(m_rhList);
	m_rhChart->addSeries(m_rhSeries);
	m_rhAxisX->setRange(0, maxnum);
	m_rhChart->setAxisX(m_rhAxisX, m_rhSeries);
	
}

//更新数据
void DataChart::onDataRcvDealWith(Data data)
{
	chartDatas.append(data);
	m_tpList.append(QPointF(chartDatas.size(), data.tp));
	//调整横坐标
	double maxnum = chartDatas.size() > 50 ? chartDatas.size() : 50;
	m_tpAxisX->setRange(m_tpList.takeFirst().rx(), maxnum);
	m_tpSeries->replace(m_tpList);
	m_rhList.append(QPointF(chartDatas.size(), data.rh));
	m_rhAxisX->setRange(m_rhList.takeFirst().rx(), maxnum);
	m_rhSeries->replace(m_rhList);
}

