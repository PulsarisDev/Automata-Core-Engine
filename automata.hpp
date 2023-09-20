#pragma once
#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include <vector>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <sstream>

using StateNo = size_t;
using StateNoSet = std::set<StateNo>;
using CharSet = std::set<char>;
using Edge = std::pair<char,StateNo>;
using State = std::map<char,StateNo>;
using EdgeSet = State;
using Graph = std::vector<State>;

struct ReportMsg{
    short error_type = -1;
    size_t last_pos = 0; // Pos inside string when the msg generated
    char char_reading = 0;
    std::string str_read; // Substring that already be read
    StateNo last_state = 0; // Last state when the msg generated
    std::string to_string() const {
        std::stringstream ss;
        ss << "On reading char '" << char_reading << "':\n";
        switch (error_type) {
            case 0:
                ss << "Transferred into a trap state.\n";
                break;
            case 1:
                ss << "Invalid character detected.\n";
                break;
            case 2:
                ss << "Not stopped in any end state.\n";
                break;
            default:
                ss << "<Unknown error type>\n";
                break;
        }
        ss << "Last Valid Pos: " << last_pos << '\n'
           << "Read Str: " << str_read << '\n'
           << "Last State: " << last_state << '\n';
        return ss.str();
    }
} ;

class DFA {
public:
    /* Automata Operations */
    explicit DFA(size_t size = 0);
    std::string to_string() const;
    bool verify(const std::string& expr);
    std::pair<bool,ReportMsg> analyze(const std::string& expr);

    /* State Operations */
    void add_state();
    void remove_state();
    void set_start_state(StateNo s);
    void set_end_state(const StateNoSet &s_set);
    void set_alphabet(const CharSet &set);

    /* Edge Operations */
    void insert_edge(StateNo from_state, char on_edge, StateNo to_state);
    void insert_edge(StateNo from_state, const CharSet &on_edge, StateNo to_state);
    void insert_edge(StateNo from_state, const Edge &edge);
    void insert_edge(StateNo from_state, const EdgeSet &edge_set);

private:
    Graph _graph;
    StateNo _p_start_state;
    StateNoSet _p_end_states;
    CharSet _alphabet_set;
};

DFA::DFA(size_t size):_p_start_state(0) {_graph.resize(size);}
void DFA::add_state() {_graph.emplace_back(State());}
void DFA::remove_state() {_graph.pop_back();}
void DFA::set_start_state(const StateNo s) {_p_start_state = s;}
void DFA::set_end_state(const StateNoSet &s_set) {_p_end_states = s_set;}
void DFA::set_alphabet(const CharSet &set) {_alphabet_set = set;}

void DFA::insert_edge(StateNo from_state, char on_edge, StateNo to_state) {
    _graph.at(from_state).insert(Edge(on_edge,to_state));
}
void DFA::insert_edge(StateNo from_state, const CharSet &on_edge, StateNo to_state) {
    for(char ch:on_edge) {
        _graph.at(from_state).insert(Edge(ch,to_state));
    }
}
void DFA::insert_edge(StateNo from_state, const Edge &edge) {
    _graph.at(from_state).insert(edge);
}
void DFA::insert_edge(StateNo from_state, const EdgeSet &edge_set) {
    for(Edge each:edge_set){
        _graph.at(from_state).insert(each);
    }
}

std::string DFA::to_string() const{
    std::stringstream ss;
    for (StateNo state_curr = 0; state_curr < _graph.size(); state_curr++) {
        ss << "State " << state_curr << ':';
        if (state_curr == _p_start_state) {
            ss << " (start";
            if (_p_end_states.count(state_curr)) ss << ",end";
            ss << ')';
        } else if (_p_end_states.count(state_curr)) ss << " (end)";
        if (!_graph.at(state_curr).empty()) {
            std::vector<bool> truth_table(_graph.size(), false);
            for (StateNo state_next = 0; state_next < _graph.size(); state_next++) {
                for (const auto &each: _graph.at(state_curr)) {
                    if (each.second==state_next) {
                        if (!truth_table[state_next]) {
                            truth_table[state_next] = true;
                            ss << "\n  ->" << state_next << ": ";
                        }
                        ss << each.first << ' ';
                    }
                }
            }
        }
        ss << '\n';
    }
    return ss.str();
}
bool DFA::verify(const std::string& expr) {
    bool flag = true;
    StateNo state_pointer = _p_start_state;
    for (size_t i = 0; i < expr.size(); i++) {
        const char &ch = expr[i];
        const State &current_state = _graph.at(state_pointer);
        if (_alphabet_set.find(ch) == _alphabet_set.end()) flag = false; //不合法原因：存在非字母表输入
        auto it = current_state.find(ch);
        if (it != current_state.end())
            state_pointer = it->second;
        else flag = false; //不合法原因：转移至陷阱态
        if ((i == expr.size() - 1) && (!_p_end_states.count(state_pointer))) flag = false; //不合法原因：字符串未在终止状态处结束
    }
    return flag;
}

std::pair<bool, ReportMsg> DFA::analyze(const std::string &expr) {
    bool flag = true;
    ReportMsg msg;
    StateNo state_pointer = _p_start_state;
    for (size_t i = 0; i < expr.size(); i++) {
        const char &ch = msg.char_reading = expr[i];
        const State &current_state = _graph.at(state_pointer);
        if (_alphabet_set.find(ch) == _alphabet_set.end()) {
            msg.error_type = 1;
            flag = false; //不合法原因：存在非字母表输入
            break;
        }
        auto it = current_state.find(ch);
        if (it != current_state.end())
            state_pointer = it->second;
        else {
            msg.error_type = 0;
            flag = false; //不合法原因：转移至陷阱态
            break;
        }
        if ((i == expr.size() - 1) && (!_p_end_states.count(state_pointer))) {
            msg.error_type = 2;
            flag = false; //不合法原因：字符串未在终止状态处结束
            break;
        }
        msg.str_read += ch;
        msg.last_pos = i;
    }
    msg.last_state = state_pointer;
    return {flag,msg};
}

#endif //AUTOMATA_HPP
