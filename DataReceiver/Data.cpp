#include "Data.h"

Data::Data()
{
}

Data::~Data()
{
}


QDataStream& operator>>(QDataStream& in, Data& data)
{
	// TODO: �ڴ˴����� return ���
	in >> data.time >> data.tp >> data.rh;
	return in;
}

QDataStream& operator<<(QDataStream& out, Data& data)
{
	// TODO: �ڴ˴����� return ���
	out << data.time << data.tp << data.rh;
	return out;
}


