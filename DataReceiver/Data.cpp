#include "Data.h"

Data::Data()
{
}

Data::~Data()
{
}


QDataStream& operator>>(QDataStream& in, Data& data)
{
	// TODO: 在此处插入 return 语句
	in >> data.time >> data.tp >> data.rh;
	return in;
}

QDataStream& operator<<(QDataStream& out, Data& data)
{
	// TODO: 在此处插入 return 语句
	out << data.time << data.tp << data.rh;
	return out;
}


