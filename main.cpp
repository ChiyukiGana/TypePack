#include <iostream>
#include <time.h>
#include "src/typepack.h"

int rand(int max, int min = 0) { if (min > max) std::swap(max, min); return min + (rand() % (max - min + 1)); }

std::string randstr(size_t len)
{
	srand(time(nullptr));
	std::string s;
	for (size_t i = 0; i < len; i++) s += rand('z', '0');
	return s;
}

void TypePack()
{
	using namespace std;
	namespace tp = typepack;

	clock_t a, b;

	tp::object root;
	{
		tp::object o;
		tp::array a;
		for (size_t i = 0; i < 1000; i++) o.set(std::to_string(i), randstr(20));
		for (size_t i = 0; i < 1000; i++) a.emplace_back(randstr(20));
		root.set("object", std::move(o));
		root.set("array", std::move(a));
	}

	tp::binary bin = root.toBinary();
	cout << "storage usage: " << bin.size() << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) root.toBinary();
	b = clock() - a;
	cout << "serialize time: " << b << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) tp::object::fromBinary(bin);
	b = clock() - a;
	cout << "deserialize time: " << b << endl;
}
#ifdef QT_QTCORE_MODULE_H
void QJson()
{
	using namespace std;

	clock_t a, b;

	QJsonObject root;
	{
		QJsonObject o;
		QJsonArray a;
		for (size_t i = 0; i < 1000; i++) o.insert(QString::number(i), QString::fromStdString(randstr(20)));
		for (size_t i = 0; i < 1000; i++) a.append(QString::fromStdString(randstr(20)));
		root.insert("object", std::move(o));
		root.insert("array", std::move(a));
	}

	QByteArray bin = QJsonDocument(root).toJson();
	cout << "storage usage: " << bin.size() << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) QJsonDocument(root).toJson();
	b = clock() - a;
	cout << "serialize time: " << b << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) QJsonDocument::fromJson(bin);
	b = clock() - a;
	cout << "deserialize time: " << b << endl;
}
#endif

int main()
{
	using namespace std;
	namespace tp = typepack;

	TypePack();
#ifdef QT_QTCORE_MODULE_H
	QJson();
#endif

	return 0;

	tp::object o;
	// bool
	o.set("bool", true);
	o.get("bool").toBool(); // get a value, return false if not exists
	o.get("bool", true).toBool(); // get a value, return true if not exists

	// int
	o.set("int", 100);
	o.get("int").toInt(); // get a value, return 0 if not exists
	o.get("int", -1).toInt(); // get a value, return -1 if not exists

	// flaot
	o.set("float", 1.5);
	o.get("float").toNumber();
	o.get("float", -1.0).toNumber();

	// string
	o.set("string", "value");
	o.get("string").toString();
	o.get("string", "invalid value").toString();

	// binary
	o.set("binary", tp::binary(1000, 0xFF));
	o.get("binary").toBinary();

	// sub object
	tp::object so;
	o.set("object", so);
	o.get("object").toObject();

	// array
	typepack::array sa;
	o.set("array", sa);
	o.get("array", sa).toArray();

	if (o.exist("invalid-key")); // check if there exists a value
	if (o.get("int").isInt()); // check a value type

	tp::binary bin = o.toBinary(); // serialize to binary
	tp::object::fromBinary(bin); // deserialize from binary

	typepack::array a;
	a.push_back(true);
	a.push_back(100);
	a.push_back(1.5);
	a.push_back("value");
	a.push_back(tp::binary(1000, 0xFF));

	bin = a.toBinary(); // serialize to binary
	typepack::array::fromBinary(bin); // deserialize from binary

	bin = tp::pack::toBinary(o); // object to binary
	bin = tp::pack::toBinary(a); // array to binary
	bin = tp::pack::toBinary(true); // bool to binary
	bin = tp::pack::toBinary("hello"); // string to binary

	tp::value val = tp::pack::fromBinary(bin); // deserialize and check types
	if (val.isObject());
	if (val.isArray());
	if (val.isInt());

	return 0;
}