#pragma once

#include <QtCharts>
#include <QPen>
#include <QPainter>
#include <QtCharts\QLegendMarker>
QT_CHARTS_USE_NAMESPACE
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QWidget>
#include <qdebug.h>
#include "Data.h"
#include "ui_DataChart.h"

class DataChart : public QWidget
{
	Q_OBJECT

public:
	DataChart(QWidget* parent = Q_NULLPTR);
	~DataChart();



private:
	Ui::DataChart ui;

	QChartView* m_tpChartView;
	QChartView* m_rhChartView;
	QChart* m_tpChart;
	QSplineSeries* m_tpSeries;
	QValueAxis* m_tpAxisX;
	QChart* m_rhChart;
	QSplineSeries* m_rhSeries;
	QValueAxis* m_rhAxisX;


	QList<QPointF> m_tpList;
	QList<QPointF> m_rhList;
	QList<Data> chartDatas;




	void initChart();
	void initAxis();

	void initSeries(bool isLoadDB);
public slots:
	void onDataRcvDealWith(Data);
};

