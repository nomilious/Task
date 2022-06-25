#include <iostream>
#include <queue>
#include <vector>
#include <iomanip>
#include <fstream>
#define s1 "В момент времени "
#define s2 " транзакт с идентификатором "
using namespace std;
fstream file("out.log", ios::out);
struct Transact {
	int id;
	float duration, t_first;
	Transact(int i, float t1) {
		id = i; t_first = duration = 0;
		file << fixed << setprecision(3) << s1 << setw(7) << t1 << s2 << setw(3) << id << " вошел в модель." << endl;
	}
	Transact() { }
};
class Device {
	int id, t_min, t_max;
	queue<Transact> q;
public:
	static float time;
	Device(int i, int t1, int t2) :id(i), t_min(t1), t_max(t2) {}
	int q_el() { return q.size(); }
	void add(Transact);
	float t_serve();
	void serve();
	void set_first();
};
void Device::set_first() {
	if (q.size() && !q.front().t_first) {
		q.front().t_first = time;
		file << fixed << setprecision(3) << s1 << setw(7) << time << s2 << setw(3) << q.front().id << " занял устройство " << id << endl;
	}
}
float Device::t_serve() {
	if (q.size() && q.front().t_first)
		return (q.front().t_first + q.front().duration);
	return 0;
}
void Device::serve() {
	file << fixed << setprecision(3) << s1 << setw(7) << time << s2 << setw(3) << q.front().id << " освободил устройство " << id << endl;
	file << fixed << setprecision(3) << s1 << setw(7) << time << s2 << setw(3) << q.front().id << " вышел из модели" << endl;
	q.pop();
	set_first();
}
void Device::add(Transact x) {
	file << fixed << setprecision(3) << s1 << setw(7) << time << s2 << setw(3) << x.id << " встал в очередь " << id << endl;
	x.duration = ((rand() % ((t_min - t_max) * 1000)) / 1000.0 + (float)t_min);
	q.push(x);
}
float Device::time = 0.0;
vector<float>::iterator next_event(vector<float>&);
void add_if_unique(vector<float>&, float);
int main() {
	int tmax = 22, ind = 0;
	float t_end = 150.0;
	vector<float> Event;
	Device* d1 = new Device(1, 5, tmax), * d2 = new Device(2, 6, tmax);
	Transact x;
	bool f = 1;

	srand(time(0));
	Event.push_back(Device::time + (rand() % (tmax * 1000)) / 1000.0);
	while (next_event(Event) < Event.end() && f)
	{// пока время меньше ИЛИ обслуживаются оставшиеся
		f = 0;
		Device::time = *(next_event(Event));
		if (Device::time == d1->t_serve()) {
			f = 1;
			d1->serve();
		}
		else if (Device::time == d2->t_serve()) {
			f = 1;
			d2->serve();
		}
		else if (Device::time < t_end) {
			f = 1;
			x = Transact(++ind, Device::time);
			if (d1->q_el() <= d2->q_el())
				d1->add(x);
			else
				d2->add(x);
			d1->set_first();
			d2->set_first();
			// пополнение очереди событий
			Event.push_back(Device::time + (rand() % (tmax * 1000)) / 1000.0);
		} 
		else
			f = 1;
		Event.erase(find(Event.begin(), Event.end(), Device::time));
		add_if_unique(Event, d1->t_serve());
		add_if_unique(Event, d2->t_serve());
	}
	delete d1, d2;
	file.close();
	return 0;
}
void add_if_unique(vector<float>& name, float n) {
	if (find(name.begin(), name.end(), n) == name.end() && n > 0.0)
		name.push_back(n);
}
vector<float>::iterator next_event(vector<float>& vec) {
	float t = 10000.0;
	for (auto i : vec)
		if (i > 0.0 && i < t)
			t = i;
	return find(vec.begin(), vec.end(), t);
}