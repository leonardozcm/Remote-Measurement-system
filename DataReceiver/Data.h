#pragma once

#include <QString>
#include <QDataStream>

class Data 
{

public:
	Data();
	~Data();
	QString  time;
	double tp;
	double rh;

	
	friend QDataStream& operator>>(QDataStream& in, Data& data);
	friend QDataStream& operator<<(QDataStream& out, Data& data);
};
