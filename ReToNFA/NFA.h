//Regular Expression���� NFA�� ��ȯ�ϱ� ���� �Լ����Դϴ�.
//Regular Expression�� std::string���� �־����� ��
//1. ������ ���ԵǾ� ���� �� �ִٸ� removeChar(std::string, char)�� ���� ������ �����մϴ�.
//2. concatenate�� �����Ǿ� �ִٸ� addConcatenation(std::string)���� �� ��ġ�� concate��ȣ�� �߰����ݴϴ�.
//3. convertInfixToPostfix(std::string)���� Infix������ Regular Expression�� Postfix�� ��ȯ�մϴ�.
//
//Postfix ������ regular expression�� ReToNFA(std::string)�� �̿��Ͽ� class NFA�� ��ȯ�մϴ�.
//1. print()�� ��ȯ�� NFA�� transition table�� Ȯ���մϴ�.
//2. isAccept(std::string)�� �־��� language�� NFA���� accept�Ǵ��� Ȯ���մϴ�.
#pragma once
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>
#include "stafx.h"

//������ ��ȯ�� ���� in-stack-priority, in-comming-prioirtiy
static std::map<char, int> isp;
static std::map<char, int> icp;

typedef struct _TRANSITION {
	int from;
	int to;
	char alpha;
	struct _TRANSITION(int _from, int _to, char _alpha) :
		from(_from),
		to(_to),
		alpha(_alpha) {}
	bool operator==(const struct _TRANSITION& ot) {
		return to == ot.to && alpha == ot.alpha;
	}
	bool operator>(const struct _TRANSITION& ot) {
		return from == ot.from ? to == ot.to ? alpha > ot.alpha : to > ot.to : from > ot.from;
	}
	bool operator<(const struct _TRANSITION& ot) {
		return from == ot.from ? to == ot.to ? alpha < ot.alpha : to < ot.to : from < ot.from;
	}
}TRANS;

class NFA {
	std::vector<TRANS> transitions;
	//initial state, final state, new state index
	int init;
	int fin;
	int id;
public:
	NFA() {
		init = 0;
		fin = 1;
		id = 1;
	}
	NFA(int _init, int _fin) :
		init(_init),
		fin(_fin),
		id(_fin) {}
	NFA(const NFA& ot) :
		init(ot.init),
		fin(ot.fin),
		id(ot.id),
		transitions(ot.transitions) {}
	//NFA�� transition�� �߰��մϴ�.
	void add(int from, int to, char alpha);
	void add(TRANS t);
	//�ٸ� NFA�� ��� transitions�� �߰��մϴ�.
	void append(NFA another);
	//NFA�� transition Table�� ����մϴ�.
	void print();
	int get_init() { return init; }
	int get_final() { return fin; }
	int get_id() { return id; }
	void set_final(int _id) { fin = _id; }
	void addState() { id++; }
	//notation�� NFA�� accept�Ǵ��� Ȯ���մϴ�.
	bool isAccept(std::string notation);
};

//ISP, ICP�� priority�� �����մϴ�.
void setPriority();

inline bool isTerminal(const char oneletter) {
	return (oneletter >= 'a' && oneletter <= 'z');
}

inline bool isConcate(const char oneletter) {
	return isTerminal(oneletter) || (oneletter != OR && oneletter != BLANK && oneletter != OPENPARENTHESIS);
}

inline bool isOper(char c) {
	return c == CLOSURE || c == CONCATENATE || c == OR || c == OPENPARENTHESIS;
}

std::string convertInfixToPostfix(std::string infixNotation);
//str���� c�� ��� �����Ͽ� ��ȯ�մϴ�.
std::string removeChar(std::string str, char c);
//notation�� concate��ȣ�� �߰��մϴ�.
std::string addConcatenation(const std::string notation);

//Target NFA�� kleene clousre�� �߰��մϴ�.
NFA kleene(NFA Target);
//L, R NFA�� OR�մϴ�.
NFA Or(NFA L, NFA R);
//L, R NFA�� Concatenate�մϴ�
NFA Concat(NFA L, NFA R);
NFA ReToNFA(std::string Re);