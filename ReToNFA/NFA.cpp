#include "NFA.h"

// NFA�� �־��� ���ڿ��� �ν��ϴ��� Ȯ���ϴ� �Լ�
bool NFA::isAccept(std::string notation) {
	typedef std::pair<int, std::string> tr;

	// ���� ���̺��� �����ϴ� map�� ����
	std::map<std::pair<int, char>, std::set<int> > table; 
	// �� ���¸� �湮�ߴ��� Ȯ���ϴ� map�� ����
	std::map<tr, bool> visited; 
	// Ž���� ���¸� �����ϴ� ť�� ����
	std::queue<tr > q; 
	// ���� ���¸� �����ϴ� pair�� ����
	std::pair<int, std::string> cur = { init, "" }; 
	// ���̵��� ����
	std::sort(transitions.begin(), transitions.end()); 

	// ���� ���̺��� ����
	for (int i = 0; i < transitions.size(); i++) {
		table[{transitions[i].from, transitions[i].alpha}].insert(transitions[i].to);
	}

	// �ʺ� �켱 Ž��(BFS)�� �̿��Ͽ� �־��� ���ڿ��� �νĵǴ��� Ȯ��
	q.push(cur);
	visited[cur] = true;
	while (!q.empty()) {
		cur = q.front();
		q.pop();

		// ���� ���� ���¿� �����߰�, ������� ���ڿ��� �־��� ���ڿ��� ���ٸ� true�� ��ȯ
		if (cur.second == notation && cur.first == fin) {
			return true;
		}
		// ���� ���¿��� ��-���̷� ����� ���°� �ִٸ� �ش� ���¸� ť�� �߰�
		for (auto& t : table[{cur.first, LAMDA}]) {
			std::pair<int, std::string> next = { t, cur.second };
			if (visited[next] == false) {
				q.push(next);
				visited[next] = true;
			}
		}
		// ���� ���ڿ� ���� ���̰� �ִٸ� �ش� ���¸� ť�� �߰�
		char nextc = notation[cur.second.size()];
		if (nextc) {
			for (auto& t : table[{cur.first, nextc}]) {
				std::pair<int, std::string> next = { t, cur.second + nextc };
				if (visited[next] == false) {
					q.push(next);
					visited[next] = true;
				}
			}
		}
	}
	// BFS�� ����Ǿ����� �־��� ���ڿ��� �ν��ϴ� ���� ���¸� ã�� ���ߴٸ� false�� ��ȯ
	return false;
}

// from ���¿��� to ���·� alpha ���ڸ� ���� �����ϴ� transition�� �߰��ϴ� �Լ�
void NFA::add(int from, int to, char alpha) {
	add(TRANS(from, to, alpha));
}

// transition t�� NFA�� transition ����Ʈ�� �߰��ϴ� �Լ�
void NFA::add(TRANS t) {
	transitions.push_back(t);
}

// another NFA�� ���� NFA�� ����. 
// �� �������� another NFA�� ��� ������ id�� ���� NFA�� ���� ���� �°� ������Ŵ 
// �̸� transitions ����Ʈ�� �߰�
void NFA::append(NFA another) {
	int nextid = 0;
	//�ٸ� NFA�� transition�� ���� NFA�� state status�� ����Ͽ� �߰�
	for (int i = 0; i < another.transitions.size(); i++) {
		TRANS t = another.transitions[i];
		t.from += id;
		t.to += id;
		nextid = std::max(nextid, t.to);
		transitions.push_back(t);
	}
	id = nextid;
	fin = id;
}

// NFA ��� �Լ�
void NFA::print() {
	std::map<std::pair<int, char>, std::set<int> > table;
	std::set<char> alphabet;
	std::sort(transitions.begin(), transitions.end());
	//transition table�� ����ϴ�.
	for (int i = 0; i < transitions.size(); i++) {
		table[{transitions[i].from, transitions[i].alpha}].insert(transitions[i].to);
		alphabet.insert(transitions[i].alpha);
	}

	// ��� ���Ŀ� �°� transition ���̺��� ���
	std::cout << "_";
	for (int i = 0; i <= alphabet.size(); i++) {
		std::cout << "________";
	}
	std::cout << std::endl;
	
	std::cout << "Seq\t|" ;
	for (auto c : alphabet) {
		std::cout << (c ? std::string(1, c) : "LAMDA") << "\t|";
	}
	std::cout << std::endl;
	std::cout << "=";
	for (int i = 0; i <= alphabet.size(); i++) {
		std::cout << "========";
	}
	std::cout << std::endl;
	for (int i = 0; i <= id; i++) {
		std::cout << i << "\t|";
		for (auto c : alphabet) {
			for (auto s : table[{i, c}])
				std::cout << s << " ";
			std::cout << "\t|";
		}
		std::cout << '\n';
	}
	std::cout << "-";
	for (int i = 0; i <= alphabet.size(); i++) {
		std::cout << "--------";
	}
	std::cout << std::endl;
}

// �־��� NFA�� Ŭ������ �����ϴ� �Լ� 
// Ŭ������ �ش� NFA�� ���� ���¿��� ���� ���·� ���� �۽Ƿ� ���̸� �߰��ϰ�, 
// �� �ݴ� �������ε� �۽Ƿ� ���̸� �߰��Ͽ� ����
NFA kleene(NFA Target) {
	NFA res;
	res.add(res.get_init(), res.get_final(), LAMDA);
	Target.add(Target.get_init(), Target.get_id(), LAMDA);
	Target.add(Target.get_id(), Target.get_init(), LAMDA);
	res.append(Target);
	return res;
}

// �� NFA�� �����ϴ� �Լ� 
// ���ο� ���� ���¿��� �� NFA�� ���� ���·�, �� NFA�� ���� ���¿���
// ���ο� ���� ���·� �۽Ƿ� ���̸� �߰��Ͽ� �� NFA�� ����
NFA Or(NFA L, NFA R) {
	NFA res;
	int temp;
	res.add(res.get_init(), res.get_final(), LAMDA);
	res.append(L);
	temp = res.get_final();
	res.addState();
	res.add(res.get_init(), res.get_id(), LAMDA);
	res.append(R);
	res.addState();
	res.add(temp, res.get_id(), LAMDA);
	res.add(res.get_final(), res.get_id(), LAMDA);
	return res;
}

// �� NFA ���� �Լ�
NFA Concat(NFA L, NFA R) {
	L.append(R);
	return L;
}

// �־��� ���� ǥ������ NFA�� ��ȯ�ϴ� �Լ�
NFA ReToNFA(std::string Re) {
	NFA res;
	NFA l, r;
	//���� �� ����� NFA���� ������ stack
	std::stack<NFA> st; 

	for (int i = 0; i < Re.size(); i++) {
		// �����ڰ� �ƴ϶�� �ش� ���ڸ� �ν��ϴ� NFA�� ����
		if (!isOper(Re[i])) {
			NFA newNFA;
			newNFA.add(newNFA.get_init(), newNFA.get_final(), Re[i]);
			st.push(newNFA);
		}
		else {
			// �������� ��� �������� ������ ���� �۾��� ����
			switch (Re[i]) {
			case CONCATENATE:
				r = st.top(); st.pop();
				l = st.top(); st.pop();
				st.push(Concat(l, r));
				break;
			case OR:
				r = st.top(); st.pop();
				l = st.top(); st.pop();
				st.push(Or(l, r));
				break;
			case CLOSURE:
				l = st.top(); st.pop();
				st.push(kleene(l));
				break;
			}
		}
	}
	// ������ ����� ���� NFA�� ���۰� ���� �۽Ƿ� ���̸� �߰�
	res.add(res.get_init(), res.get_final(), LAMDA);
	res.append(st.top());
	res.addState();
	res.add(res.get_final(), res.get_id(), LAMDA);
	res.set_final(res.get_id());
	return res;
}

// �������� �켱������ �����ϴ� �Լ�
void setPriority() {
	isp[OPENPARENTHESIS] = 0;	icp[OPENPARENTHESIS] = 999;
	isp[CONCATENATE] = 1;		icp[CONCATENATE] = 1;
	isp[OR] = 2;				icp[OR] = 2;
	isp[CLOSURE] = 3;			icp[CLOSURE] = 3;
}

// ���� ǥ����� ���� ǥ������� ��ȯ�ϴ� �Լ�
std::string convertInfixToPostfix(std::string infixNotation) {
	std::string postfixNotation;
	std::stack<char> operStack;
	int operandCount = 0;
	int operatorCount = 0;
	setPriority();

	for (int i = 0; i < infixNotation.size(); i++) {
		//���� terminal(a,b,c, etc.)�̶�� �ٷ�.
		if (isTerminal(infixNotation[i])) {
			postfixNotation.push_back(infixNotation[i]);
			operandCount++;
		}
		else {
			//�ݴ� ��ȣ�� �� ���� ��ȣ���� pop
			if (infixNotation[i] == CLOSEPARENTHESIS) {
				while (operStack.top() != OPENPARENTHESIS) {
					postfixNotation.push_back(operStack.top());
					if (operStack.top() != CLOSURE) operatorCount++;
					operStack.pop();
				}
				operStack.pop();
				continue;
			}
			//top�� �켱������ �������� �����ں��� Ŭ ���, �۰ų� ���� �����ڰ� ���ö����� pop
			while (!operStack.empty() && (isp[operStack.top()] > icp[infixNotation[i]])) {
				postfixNotation.push_back(operStack.top());
				if (operStack.top() != CLOSURE) operatorCount++;
				operStack.pop();
			}
			operStack.push(infixNotation[i]);
		}
	}
	//stack�� �������� ���� operator���� ����
	while (!operStack.empty()) {
		postfixNotation.push_back(operStack.top());
		if (operStack.top() != CLOSURE) operatorCount++;
		operStack.pop();
	}

	//operator�� operator�� ������ �˸°� �Ǿ����� �ʴٸ� INVALIDEXP�� ��ȯ
	return (operandCount - operatorCount == 1) ? postfixNotation : INVALIDEXP;
}

// ���ڿ����� �־��� ���ڸ� �����ϴ� �Լ�
std::string removeChar(std::string str, char c) {
	std::string removedstr;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == c)
			continue;
		removedstr.push_back(str[i]);
	}
	return removedstr;
}

// �־��� ���� ǥ���Ŀ� ���� �����ڸ� �߰��ϴ� �Լ�
std::string addConcatenation(const std::string notation) {
	std::string addNotation;

	addNotation.push_back(notation[0]);
	for (int i = 1; i < notation.size(); i++) {
		//���� terminalȤ�� ���� ��ȣ�� �� �ٷ� ������ ���ڰ� terminal �̰ų� ���°�ȣ, OR �����ڰ� �ƴ϶�� CONCATENATE�� ����
		if ((isTerminal(notation[i]) || notation[i] == OPENPARENTHESIS) && isConcate(notation[i - 1])) {
			addNotation.push_back(CONCATENATE);
		}
		addNotation.push_back(notation[i]);
	}
	return addNotation;
}