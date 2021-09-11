// Date: 06.06.2021
// Development Enviroment: Visual Studio2019

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>


using namespace std;
string delspace(string line) {
	int i = 0;
	string line1 = "";
	while (i < line.length()) {
		if (line[i] != ' ') {
			line1 += line[i];
		}
		i++;
	}
	return line1;
}
string lineseperator(string line) {
	string newline = line.substr(0, line.find_first_of(";"));
	return newline;
}

struct Operation {
	int code;
	int speed;
	int total;
};

struct Setup {
	int f_code;
	int s_code;
	int s_duration;
};


struct Order {
	int ordercode;
	int amount;
	int op_code;
	int deadline;
	int duration;
};
vector <Order> orders;
vector <Setup> setups;
vector <Operation> operations;
vector <vector<Order>> v_order;

void insert_order(string line) {
	Order order;
	order.ordercode = stoi(lineseperator(line));
	line = line.substr(line.find_first_of(";") + 1);
	order.amount = stoi(lineseperator(line));
	line = line.substr(line.find_first_of(";") + 1);
	order.op_code = stoi(lineseperator(line));
	line = line.substr(line.find_first_of(";") + 1);
	order.deadline = stoi(lineseperator(line));
	orders.push_back(order);
}
void insert_operation(string line) {
	Operation op;
	op.code = stoi(lineseperator(line));
	line = line.substr(line.find_first_of(";") + 1);
	op.speed = stoi(line);
	operations.push_back(op);
}
void insert_setup(string line) {
	Setup setup;
	setup.f_code = stoi(lineseperator(line));
	line = line.substr(line.find_first_of(";") + 1);
	setup.s_code = stoi(lineseperator(line));
	line = line.substr(line.find_first_of(";") + 1);
	setup.s_duration = stoi(line);
	setups.push_back(setup);
}
vector<Order>::iterator it;

bool compare(Order i1, Order i2)
{
	return (i1.op_code > i2.op_code);
}

int dur_cal(Order N) {
	for (auto M : operations) {
		double a = (1.0 * N.amount) / (1.0 * M.speed);
		if (N.op_code == M.code) N.duration = ceil(a);
	}
	return N.duration;
}

void total() {
	for (int i = 0;i < operations.size();i++) {
		int sum = 0;
		for (auto N : orders) { if (operations[i].code == N.op_code)sum += N.duration * 10000 / N.deadline; }
		operations[i].total = sum;
	}

}


int erase(vector<Order> vector, Order value)
{
	int a = 0;
	int i = value.ordercode;
	for (it = vector.begin();it != vector.end();it++)
	{
		if (it->ordercode == i) return a;
		else { a++; }
	}
}


void ranking() {
	sort(orders.begin(), orders.end(), compare);
	sort(operations.begin(), operations.end(), [](Operation o1, Operation o2) {return o1.total > o2.total;});
	const int j = orders.size();
	for (int i = 0;i < operations.size();i++) {
		for (auto M : orders) {
			if (operations[i].code == M.op_code) {
				orders.erase(orders.begin() + erase(orders, M));
				orders.push_back(M);
			}
		}
	}
}
int t(Order a, Order b) {
	int t1 = 0;
	if (a.op_code != b.op_code) {
		for (auto i : setups)
			if (i.f_code == a.op_code && i.s_code == b.op_code || i.f_code == b.op_code && i.s_code == a.op_code)t1 = i.s_duration;return t1;
	}
	else if (a.op_code == b.op_code)
		return t1 = 0;
}
void schedule(ofstream& output) {
	int time = 0;
	queue <Order> q;
	for (auto N : orders) {
		int setupoverhead = 0;
		if (q.size() >= 2) { q.pop(); }
		q.push(N);
		if (q.size() < 2 && orders.back().ordercode != N.ordercode) { setupoverhead = 0; }
		else {
			time += t(q.front(), q.back());
			setupoverhead = t(q.front(), q.back());
		}
		output << time << ";" << N.op_code << ";" << N.ordercode << ";" << N.amount << ";" << setupoverhead << "\n";
		time = time + N.duration;
		erase(orders, N);
	}
}

int main() {
	string line;
	ifstream input1("Operations.txt");
	while (getline(input1, line)) {
		delspace(line);
		insert_operation(line);
	}
	ifstream input2("SetupDuration.txt");
	while (getline(input2, line)) {
		delspace(line);
		insert_setup(line);
	}


	ifstream input3("Orders.txt");
	while (getline(input3, line)) {
		delspace(line);
		insert_order(line);
		int v = orders.size();
		orders[v - 1].duration = dur_cal(orders[v - 1]);
	}
	ofstream output("Schedule.txt");
	total();
	ranking();
	schedule(output);
	return 0;
}
