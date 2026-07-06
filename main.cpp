#include <functional>
#include <iostream>
#include <random>
#include <time.h>
#include "src/typepack.h"

int rand(int max, int min = 0) { if (min > max) std::swap(max, min); return min + (rand() % (max - min + 1)); }

std::string randstr(size_t len)
{
	std::random_device rd; srand(rd());
	std::string s; char a, b;
	if ((rand(1) > 0)) a = 'a', b = 'z';
	for (size_t i = 0; i < len; i++) s += rand(b, a);
	return s;
}

#ifdef QT_QTCORE_MODULE_H
void generatorData(typepack::object& t, QJsonObject& j, const size_t count)
{
	std::random_device rd;
	srand(rd());
	const int MAX_DEPTH = 5;

	std::function<std::pair<typepack::value, QJsonValue>(int)> generate = [&](int depth) -> std::pair<typepack::value, QJsonValue> {
		switch ((depth >= MAX_DEPTH) ? rand(4) : rand(6))
		{
			// null
		case 0: {
			return { {}, QJsonValue(QJsonValue::Null) };
		}
			  // bool
		case 1: {
			bool b = rand(1) == 0;
			return { typepack::value(b), QJsonValue(b) };
		}
			  // int
		case 2:
		{
			int i = rand(INT_MAX);
			return { typepack::value(i), QJsonValue(i) };
		}
		// double
		case 3:
		{
			double d = static_cast<double>(rand(1000000, -1000000)) / 1000.0;
			return { typepack::value(d), QJsonValue(d) };
		}
		// string
		case 4:
		{
			size_t len = rand(100, 1);
			std::string s = randstr(len);
			return { typepack::value(s), QJsonValue(QString::fromStdString(s)) };
		}
		// array
		case 5:
		{
			const size_t count = rand(10, 1);
			typepack::array arr;
			QJsonArray jarr;
			for (size_t i = 0; i < count; i++)
			{
				auto elem = generate(depth + 1);
				arr.push_back(std::move(elem.first));
				jarr.append(std::move(elem.second));
			}
			return { typepack::value(std::move(arr)), QJsonValue(std::move(jarr)) };
		}
		// object
		case 6:
		{
			const size_t count = rand(10, 1);
			typepack::object obj;
			QJsonObject jobj;
			for (size_t i = 0; i < count; i++) {
				std::string key = randstr(rand(20, 5));
				auto val = generate(depth + 1);
				obj.set(key, std::move(val.first));
				jobj.insert(QString::fromStdString(key), std::move(val.second));
			}
			return { typepack::value(std::move(obj)), QJsonValue(std::move(jobj)) };
		}
		default: return { {}, QJsonValue::Null };
		}
		};

	typepack::object rootObj;
	QJsonObject rootJObj;
	for (size_t i = 0; i < count; i++) {
		std::string key = randstr(rand(20, 5));
		auto val = generate(1);
		rootObj.set(key, std::move(val.first));
		rootJObj.insert(QString::fromStdString(key), std::move(val.second));
	}

	t = std::move(rootObj);
	j = std::move(rootJObj);
}
#else
void generatorData(typepack::object& t, const size_t count)
{
	std::random_device rd;
	srand(rd());
	const int MAX_DEPTH = 5;

	std::function<typepack::value(int)> generate = [&](int depth) -> typepack::value {
		switch ((depth >= MAX_DEPTH) ? rand(4) : rand(6))
		{
		// null
		case 0: {
			return {};
		}
		// bool
		case 1: {
			bool b = rand(1) == 0;
			return typepack::value(b);
		}
		// int
		case 2:
		{
			int i = rand(INT_MAX);
			return typepack::value(i);
		}
		// double
		case 3:
		{
			double d = static_cast<double>(rand(1000000, -1000000)) / 1000.0;
			return typepack::value(d);
		}
		// string
		case 4:
		{
			size_t len = rand(100, 1);
			std::string s = randstr(len);
			return typepack::value(s);
		}
		// array
		case 5:
		{
			const size_t count = rand(10, 1);
			typepack::array arr;
			for (size_t i = 0; i < count; i++) arr.push_back(generate(depth + 1));
			return typepack::value(std::move(arr));
		}
		// object
		case 6:
		{
			const size_t count = rand(10, 1);
			typepack::object obj;
			for (size_t i = 0; i < count; i++) obj.set(randstr(rand(20, 5)), generate(depth + 1));
			return typepack::value(std::move(obj));
		}
		default: return {};
		}
		};

	typepack::object rootObj;
	for (size_t i = 0; i < count; i++) rootObj.set(randstr(rand(20, 5)), generate(1));

	t = std::move(rootObj);
}
#endif

typepack::object t_root;
void TypePack()
{
	using namespace std;

	cout << "========== typepack ==========" << endl;

	clock_t a, b;

	typepack::binary bin = t_root.toBinary();
	cout << "storage usage: " << bin.size() << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) t_root.toBinary();
	b = clock() - a;
	cout << "serialize time: " << b << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) typepack::object::fromBinary(bin);
	b = clock() - a;
	cout << "deserialize time: " << b << endl;
	puts("\n");

}
#ifdef QT_QTCORE_MODULE_H
QJsonObject j_root;
void QJson()
{
	using namespace std;

	cout << "========== qjson ==========" << endl;

	clock_t a, b;

	QByteArray bin = QJsonDocument(j_root).toJson();
	cout << "storage usage: " << bin.size() << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) QJsonDocument(j_root).toJson();
	b = clock() - a;
	cout << "serialize time: " << b << endl;

	a = clock();
	for (size_t i = 0; i < 10000; i++) QJsonDocument::fromJson(bin);
	b = clock() - a;
	cout << "deserialize time: " << b << endl;
	puts("\n");
}
#endif

int main()
{
	using namespace std;

#ifdef QT_QTCORE_MODULE_H
	generatorData(t_root, j_root, 100);
#else
	generatorData(t_root, 100);
#endif

#ifdef QT_QTCORE_MODULE_H
	time_t j_total = 0;
#endif
	time_t t_total = 0;
	clock_t a, b;
	const size_t count = 5;
	for (size_t i = 0; i < count; i++)
	{
		cout << "test " << i + 1 << "/" << count << "\n\n";
#ifdef QT_QTCORE_MODULE_H
		a = clock();
		QJson();
		b = clock();
		j_total += b - a;
#endif
		a = clock();
		TypePack();
		b = clock();
		t_total += b - a;
	}

#ifdef QT_QTCORE_MODULE_H
	cout << "qjson total time: " << j_total / count << endl;
#endif
	cout << "typepack total time: " << t_total / count << endl;

	system("pause");
	return 0;

	using namespace typepack;

	object obj;
	// bool
	obj.set("bool", true);
	obj.get("bool").toBool(); // get a value, return false if not exists
	obj.get("bool", true).toBool(); // get a value, return true if not exists

	// int
	obj.set("int", 100);
	obj.get("int").toInt(); // get a value, return 0 if not exists
	obj.get("int", -1).toInt(); // get a value, return -1 if not exists

	// flaot
	obj.set("float", 1.5);
	obj.get("float").toFloat64();
	obj.get("float", -1.0).toFloat64();

	// string
	obj.set("string", "value");
	obj.get("string").toString();
	obj.get("string", "invalid value").toString();

	// binary
	obj.set("binary", binary(1000, 0xFF));
	obj.get("binary").toBinary();

	// sub object
	object sobj;
	obj.set("object", sobj);
	obj.get("object").toObject();

	// array
	typepack::array sarr;
	obj.set("array", sarr);
	obj.get("array", sarr).toArray();

	if (obj.exist("invalid-key")); // check if there exists a value
	if (obj.get("int").isInt()); // check a value type

	binary bin = obj.toBinary(); // serialize to binary
	object::fromBinary(bin); // deserialize from binary

	typepack::array arr;
	arr.push_back(true);
	arr.push_back(100);
	arr.push_back(1.5);
	arr.push_back("value");
	arr.push_back(binary(1000, 0xFF));

	bin = arr.toBinary(); // serialize to binary
	typepack::array::fromBinary(bin); // deserialize from binary

	bin = pack::toBinary(obj); // object to binary
	bin = pack::toBinary(arr); // array to binary
	bin = pack::toBinary(true); // bool to binary
	bin = pack::toBinary("hello"); // string to binary

	value val = pack::fromBinary(bin); // deserialize and check types
	if (val.isObject());
	if (val.isArray());
	if (val.isInt());

	return 0;
}