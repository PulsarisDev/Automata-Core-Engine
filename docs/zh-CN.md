# 写在前面

## 背景
2022年秋季学期，本人在修读北京邮电大学计算机学院（国家示范性软件学院）软件工程课程《形式语言与自动机》时，按照期中实验要求完成了一个从正则表达式到ε-NFA，再到NFA、DFA，最后对该DFA进行最小化并输出对应正规文法的程序。2023年秋季学期，本人在修读《编译原理》时意识到其中的部分知识点与实验用到了DFA的知识。因此，一个将本人自动机实验中实现的DFA独立出来进行封装以便后续使用的想法油然而生，进而诞生了本项目。

## 意义
本项目本质上是把不同类型的自动机抽象出来封装成类。这样做的好处是用户可以方便的创建与使用自动机，而省去了维护自动机本身的麻烦。但同时，这样做也有以下缺点：
- 对于大部分的使用场景，一个抽象出来的完整的DFA是没有必要的。比如，对于DFA在编译原理中的应用，用纯C语言列一堆条件判断语句反而会更简单些。
- 对于少数的需要完整自动机的场景，本项目大概率不能满足这些场景对安全性和性能的要求。
- 对于初学《形式语言与自动机》与《编译原理》的学生，直接使用封装库不利于学生对自动机本质的理解。

——但这个项目依然存在在这里，等待着或许可以帮上谁。至少，这个类的存在满足了我的强迫症。

# 文档

## 概述
本项目提供了一个对有穷自动机(Finite state Automata, FA)的C++类实现。它对输入字符串的每个字符做识别和判断，以确定其能到达的最终状态。

有穷自动机分为两类，即不确定的有穷自动机(Non-deterministic Finite Automata, NFA)和确定的有穷自动机(Deterministic Finite Automata, DFA)。截止本项目的最新版本，本项目中只包含了对确定的有穷自动机(DFA)的实现。

确定的有穷自动机(DFA)由五个部分组成：A = ( Σ, S, s0, F, N )

- Σ: 输入字母表，或称输入字符的集合。
- S: 状态的集合
- s0: 初始状态
- F: 终止状态集合 F ⊆ S
- N: 转换公式 N:S×Σ → S

本项目对以上五个部分分别实现了设置接口：

- Σ: `set_alphabet()`
- S: `DFA()`、`add_state()`、`remove_state()`
- s0: `set_start_state()`
- F: `set_end_states()`
- N: `insert_edge()`

在使用本项目提供的DFA引擎时，您应该先实例化一个空的`DFA`类，分别设置该实例的五个部分，随后使用`verify()`或`analyze()`来运行这个DFA实例。

*以上方法并非函数原型，因为它们没有包含类型和信息。具体函数的定义和使用方法可在下文中找到。

## 安装
在源码中下载automata.hpp，在自己的代码中包含该头文件即可。

注意：要使用该头文件，您的编译器需要支持`C++11`标准。

## 公有类型

|类型|名称|
|-|-|
|size_t|**StateNo**|
|std::set\<StateNo\>|**StateNoSet**|
|std::set\<char\>|**CharSet**|
|std::pair\<char,StateNo\>|**Edge**|
|std::map\<char,StateNo\>|**EdgeSet**|
|struct|[ReportMsg](#struct-reportmsg)|

## 公有方法

|类型|内容|
|-|-|
||[DFA](#dfasizt_t-size)(sizt_t size)|
|std::string|[to_string](#stdstring-to_string-const)() const|
|bool|[verify](#bool-verifyconst-stdstring-expr)(const std::string& expr)|
|std::pair<bool,ReportMsg>|[analyze](#stdpairboolreportmsg-analyzeconst-stdstring-expr)(const std::string& expr)|
|void|[add_state](#void-add_state)()|
|void|[remove_state](#void-remove_state)()|
|void|[set_start_state](#void-set_start_statestateno-s)(StateNo s)|
|void|[set_end_state](#void-set_end_stateconst-statenoset-s_set)(const StateNoSet &s_set)|
|void|[set_alphabet](#void-set_alphabetconst-charset-set)(const CharSet &set)|
|void|[insert_edge](#void-insert_edgestateno-from_state-char-on_edge-stateno-to_state)(StateNo from_state, char on_edge, StateNo to_state)|
|void|[insert_edge](#void-insert_edgestateno-from_state-const-charset-on_edge-stateno-to_state)(StateNo from_state, const CharSet &on_edge, StateNo to_state)|
|void|[insert_edge](#void-insert_edgestateno-from_state-const-edge-edge)(StateNo from_state, const Edge &edge)|
|void|[insert_edge](#void-insert_edgestateno-from_state-const-edgeset-edge_set)(StateNo from_state, const EdgeSet &edge_set)|

## 详细信息

### 公有类型文档

#### struct **ReportMsg**

该结构体含有自动机运行时产生的信息，会在自动机遇到无法处理的状态转移或成功运行结束时自动生成，不建议用户自行实例化。
|类型|名称|简介|
|-|-|-|
|short|error_type|出现错误时的错误类型，详见下表|
|size_t|last_pos|出现错误时，最后一个被正确处理的字符在输入字符串中的位置|
|char|char_reading|出现错误时正在处理的字符|
|std::string|str_read|出现错误时，已经成功处理的子字符串|
|StateNo|last_state|出现错误时，DFA位于哪一个状态|
|std::string|to_string()|将该信息实例以字符串形式返回|

|错误代码|错误类型|
|-|-|
|0|转移到陷阱态（或称无效的状态转移/在当前状态不存在一条输入字符的转移）|
|1|无效的字符输入（或称输入字符不在设定好的字母表中）|
|2|在非终止态停机|

### 共有方法文档

#### **DFA**(sizt_t size)
类的构造函数。接受一个参数为DFA的尺寸，或称DFA的状态的数量。这个值是可选的，若不提供则默认为0。你可以随时使用[add_state](#void-add_state)()或[remove_state](#void-remove_state)()来更改DFA中含有的状态数，但建议直接在构造函数中指定数量并进行初始化。

#### std::string **to_string**() const
将该DFA实例以便于阅读的字符串形式返回。字符串中遍历每个状态和该状态包含的转移，并会在相应状态中标注开始态或/和终止态。字符串中不包括字母表。

#### bool **verify**(const std::string& expr)
验证传入字符串是否符合该DFA，或者说是否符合该DFA对应的正规文法。只返回一个布尔值，代表是或否。此方法适合于仅验证字符串，若在如词法分析等大部分情况下输入字符串都不符合DFA实例的情况，考虑使用[analyze](#stdpairboolreportmsg-analyzeconst-stdstring-expr)()方法，以分析状态转移信息。

#### std::pair<bool,ReportMsg> **analyze**(const std::string& expr)
分析传入字符串在DFA实例中的状态转移情况。在[verify](#bool-verifyconst-stdstring-expr)()方法的基础上多了返回报告信息。通过分析报告可以达成对DFA的进阶使用。

#### void **add_state**()
添加一个状态。只能添加在现有状态的末尾。

#### void **remove_state**()
移除一个状态。只能移除最后一个状态。因此，这一组加减状态的方法只推荐在还没有设置状态转移公式时使用。*考虑在未来版本中提供移除指定状态的方法。*

#### void **set_start_state**(StateNo s)
接受一个参数作为初始状态的下标。下标从0开始计数，不可超过DFA实例中的状态数量。

#### void **set_end_state**(const StateNoSet &s_set)
接受一个参数作为终止状态的下标集合。下标从0开始计数，集合中任意一个元素均不可超过DFA实例中的状态数量。

#### void **set_alphabet**(const CharSet &set)
接受一个参数作为DFA的字符集。

#### void **insert_edge**(StateNo from_state, char on_edge, StateNo to_state)
插入一条状态转移公式。在状态from_state中读取到字符on_edge时，转换到状态to_state。

#### void **insert_edge**(StateNo from_state, const CharSet &on_edge, StateNo to_state)
插入多条状态转移公式。在状态from_state中读取到任意属于字符集on_edge中的字符时，转换到状态to_state。

#### void **insert_edge**(StateNo from_state, const Edge &edge)
插入一条状态转移公式。为状态from_state中插入一条边edge。边edge的本质是封装好的std::pair\<char,StateNo\>{on_edge, to_state}。

#### void **insert_edge**(StateNo from_state, const EdgeSet &edge_set)
插入多条状态转移公式。为状态from_state中插入一组由边构成的集合edge_set。边集edge_set的本质是std::map\<char,StateNo\>，其中每一项都是封装好的std::pair\<char,StateNo\>{on_edge, to_state}。

## 用例

### 例1 验证给定字符串是否符合正则表达式a(b+c)*的DFA
原正则表达式对应的一种DFA如下：

注意，以上DFA只是众多能解释原正则表达式的一种，且并非是最小化DFA。由于本例旨在展示本DFA引擎的使用方法，故不对以上DFA是否最小展开讨论。

实现该DFA的代码如下：
```C++
#include <iostream>
#include "automata.hpp"

int main(){
    // Init a DFA instance with 4 states
    DFA dfa(4);
    // Set alphabet of the instance
    CharSet alphabet = {'a','b','c'};
    dfa.set_alphabet(alphabet);
    // Set start and end states of the instance
    dfa.set_start_state(0);
    dfa.set_end_state({1,2,3});
    // Set transitions
    dfa.insert_edge(0,'a',1);
    dfa.insert_edge(1,'b',2);
    dfa.insert_edge(1,'c',3);
    dfa.insert_edge(2,'b',2);
    dfa.insert_edge(3,'c',3);
    dfa.insert_edge(2,'c',3);
    dfa.insert_edge(3,'b',2);
    // Run this instance
    std::cout<<(dfa.verify("abcbccc")?"Verified":"Unverified")<<std::endl;
    return 0;
}
```

得到输出如下：
```
Verified
```

### 例2 判断字符串中有多少个符合10+语句的DFA
如图构造DFA：

```C++
#include <iostream>
#include "automata.hpp"

int main() {
    // Init a DFA instance
    DFA dfa(3);
    dfa.set_alphabet({'0', '1'});
    dfa.set_start_state(0);
    dfa.set_end_state({2});
    dfa.insert_edge(0, '1', 1);
    dfa.insert_edge(1, '0', 2);
    dfa.insert_edge(2, '0', 2);

    // Run this instance
    std::string str = "1002101311000";
    size_t count = 0;
    while (!str.empty()) {
        auto result = dfa.analyze(str);
        if(result.second.last_pos!=0) count++;
        str = str.substr(result.second.last_pos+1);
    }
    std::cout<<count;
    return 0;
}
```
得到输出如下：
```
3
```

# 写在后面

大概就是列一些flag。

或许将来会有一天把自动机期中实验完整地重写一遍并开源——再说吧。

或许将来还有一天会把使用这个类的编译原理实验开源——等我这学期修完这门课拿到分吧。