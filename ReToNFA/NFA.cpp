#include "NFA.h"

// NFA가 주어진 문자열을 인식하는지 확인하는 함수
bool NFA::isAccept(std::string notation) {
	typedef std::pair<int, std::string> tr;

	// 전이 테이블을 저장하는 map을 선언
	std::map<std::pair<int, char>, std::set<int> > table; 
	// 각 상태를 방문했는지 확인하는 map을 선언
	std::map<tr, bool> visited; 
	// 탐색할 상태를 저장하는 큐를 선언
	std::queue<tr > q; 
	// 현재 상태를 저장하는 pair를 선언
	std::pair<int, std::string> cur = { init, "" }; 
	// 전이들을 정렬
	std::sort(transitions.begin(), transitions.end()); 

	// 전이 테이블을 생성
	for (int i = 0; i < transitions.size(); i++) {
		table[{transitions[i].from, transitions[i].alpha}].insert(transitions[i].to);
	}

	// 너비 우선 탐색(BFS)을 이용하여 주어진 문자열이 인식되는지 확인
	q.push(cur);
	visited[cur] = true;
	while (!q.empty()) {
		cur = q.front();
		q.pop();

		// 만약 종료 상태에 도달했고, 만들어진 문자열이 주어진 문자열과 같다면 true를 반환
		if (cur.second == notation && cur.first == fin) {
			return true;
		}
		// 현재 상태에서 ε-전이로 연결된 상태가 있다면 해당 상태를 큐에 추가
		for (auto& t : table[{cur.first, LAMDA}]) {
			std::pair<int, std::string> next = { t, cur.second };
			if (visited[next] == false) {
				q.push(next);
				visited[next] = true;
			}
		}
		// 다음 문자에 대한 전이가 있다면 해당 상태를 큐에 추가
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
	// BFS가 종료되었지만 주어진 문자열을 인식하는 종료 상태를 찾지 못했다면 false를 반환
	return false;
}

// from 상태에서 to 상태로 alpha 문자를 통해 전이하는 transition을 추가하는 함수
void NFA::add(int from, int to, char alpha) {
	add(TRANS(from, to, alpha));
}

// transition t를 NFA의 transition 리스트에 추가하는 함수
void NFA::add(TRANS t) {
	transitions.push_back(t);
}

// another NFA를 현재 NFA에 병합. 
// 이 과정에서 another NFA의 모든 상태의 id를 현재 NFA의 상태 수에 맞게 증가시킴 
// 이를 transitions 리스트에 추가
void NFA::append(NFA another) {
	int nextid = 0;
	//다른 NFA의 transition을 현재 NFA의 state status와 고려하여 추가
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

// NFA 출력 함수
void NFA::print() {
	std::map<std::pair<int, char>, std::set<int> > table;
	std::set<char> alphabet;
	std::sort(transitions.begin(), transitions.end());
	//transition table을 만듭니다.
	for (int i = 0; i < transitions.size(); i++) {
		table[{transitions[i].from, transitions[i].alpha}].insert(transitions[i].to);
		alphabet.insert(transitions[i].alpha);
	}

	// 출력 형식에 맞게 transition 테이블을 출력
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

// 주어진 NFA에 클로저를 적용하는 함수 
// 클로저는 해당 NFA의 시작 상태에서 종료 상태로 가는 앱실론 전이를 추가하고, 
// 그 반대 방향으로도 앱실론 전이를 추가하여 구현
NFA kleene(NFA Target) {
	NFA res;
	res.add(res.get_init(), res.get_final(), LAMDA);
	Target.add(Target.get_init(), Target.get_id(), LAMDA);
	Target.add(Target.get_id(), Target.get_init(), LAMDA);
	res.append(Target);
	return res;
}

// 두 NFA를 병합하는 함수 
// 새로운 시작 상태에서 각 NFA의 시작 상태로, 각 NFA의 종료 상태에서
// 새로운 종료 상태로 앱실론 전이를 추가하여 두 NFA를 병합
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

// 두 NFA 연결 함수
NFA Concat(NFA L, NFA R) {
	L.append(R);
	return L;
}

// 주어진 정규 표현식을 NFA로 변환하는 함수
NFA ReToNFA(std::string Re) {
	NFA res;
	NFA l, r;
	//연산 중 생기는 NFA들을 저장할 stack
	std::stack<NFA> st; 

	for (int i = 0; i < Re.size(); i++) {
		// 연산자가 아니라면 해당 문자를 인식하는 NFA를 생성
		if (!isOper(Re[i])) {
			NFA newNFA;
			newNFA.add(newNFA.get_init(), newNFA.get_final(), Re[i]);
			st.push(newNFA);
		}
		else {
			// 연산자일 경우 연산자의 종류에 따라 작업을 진행
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
	// 연산의 결과로 나온 NFA의 시작과 끝에 앱실론 전이를 추가
	res.add(res.get_init(), res.get_final(), LAMDA);
	res.append(st.top());
	res.addState();
	res.add(res.get_final(), res.get_id(), LAMDA);
	res.set_final(res.get_id());
	return res;
}

// 연산자의 우선순위를 설정하는 함수
void setPriority() {
	isp[OPENPARENTHESIS] = 0;	icp[OPENPARENTHESIS] = 999;
	isp[CONCATENATE] = 1;		icp[CONCATENATE] = 1;
	isp[OR] = 2;				icp[OR] = 2;
	isp[CLOSURE] = 3;			icp[CLOSURE] = 3;
}

// 중위 표기법을 후위 표기법으로 변환하는 함수
std::string convertInfixToPostfix(std::string infixNotation) {
	std::string postfixNotation;
	std::stack<char> operStack;
	int operandCount = 0;
	int operatorCount = 0;
	setPriority();

	for (int i = 0; i < infixNotation.size(); i++) {
		//만약 terminal(a,b,c, etc.)이라면 바로.
		if (isTerminal(infixNotation[i])) {
			postfixNotation.push_back(infixNotation[i]);
			operandCount++;
		}
		else {
			//닫는 괄호일 때 여는 괄호까지 pop
			if (infixNotation[i] == CLOSEPARENTHESIS) {
				while (operStack.top() != OPENPARENTHESIS) {
					postfixNotation.push_back(operStack.top());
					if (operStack.top() != CLOSURE) operatorCount++;
					operStack.pop();
				}
				operStack.pop();
				continue;
			}
			//top의 우선순위가 넣으려는 연산자보다 클 경우, 작거나 같은 연산자가 나올때까지 pop
			while (!operStack.empty() && (isp[operStack.top()] > icp[infixNotation[i]])) {
				postfixNotation.push_back(operStack.top());
				if (operStack.top() != CLOSURE) operatorCount++;
				operStack.pop();
			}
			operStack.push(infixNotation[i]);
		}
	}
	//stack이 빌때까지 남은 operator들을 빼냄
	while (!operStack.empty()) {
		postfixNotation.push_back(operStack.top());
		if (operStack.top() != CLOSURE) operatorCount++;
		operStack.pop();
	}

	//operator와 operator의 개수가 알맞게 되어있지 않다면 INVALIDEXP를 반환
	return (operandCount - operatorCount == 1) ? postfixNotation : INVALIDEXP;
}

// 문자열에서 주어진 문자를 제거하는 함수
std::string removeChar(std::string str, char c) {
	std::string removedstr;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == c)
			continue;
		removedstr.push_back(str[i]);
	}
	return removedstr;
}

// 주어진 정규 표현식에 연결 연산자를 추가하는 함수
std::string addConcatenation(const std::string notation) {
	std::string addNotation;

	addNotation.push_back(notation[0]);
	for (int i = 1; i < notation.size(); i++) {
		//현재 terminal혹은 여는 괄호일 때 바로 이전의 문자가 terminal 이거나 여는괄호, OR 연산자가 아니라면 CONCATENATE를 삽입
		if ((isTerminal(notation[i]) || notation[i] == OPENPARENTHESIS) && isConcate(notation[i - 1])) {
			addNotation.push_back(CONCATENATE);
		}
		addNotation.push_back(notation[i]);
	}
	return addNotation;
}