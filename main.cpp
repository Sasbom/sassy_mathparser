#include <iostream>
#include <vector>
#include <string_view>
#include <variant>
#include <deque>
#include <algorithm>
#include <cmath>
#include <unordered_map>

enum class OPenum{
    NOOP,
    PLUS,
    MIN,
    MULT,
    DIV,
    EXP,
};

enum class NUMenum{
    NONUM,
    NUM,
    DOT
};

NUMenum is_char_numeric(char const & c){
    int num = int(c);
    
    if (num>=48 && num <= 57){
        return NUMenum::NUM;
    }
    else if (num == 46){
        return NUMenum::DOT;
    }
    return NUMenum::NONUM; 
};

OPenum is_char_operator(char const& c){
    switch(c){
        case '+': return OPenum::PLUS; break;
        case '-': return OPenum::MIN; break;
        case '*': return OPenum::MULT; break;
        case '/': return OPenum::PLUS; break;
        case '^': return OPenum::EXP; break;
    }
    return OPenum::NOOP;
}

struct OP{
    OPenum op = OPenum::NOOP;
    OP(char const & c) {
        switch(c){
            case '+': op = OPenum::PLUS; break;
            case '-': op = OPenum::MIN; break;
            case '*': op = OPenum::MULT; break;
            case '/': op = OPenum::DIV; break;
            case '^': op = OPenum::EXP; break;
            default : op = OPenum::NOOP; break;
        }
    }

    friend std::ostream& operator<<(std::ostream& o, OP const & opp){
        char out;
        
        switch(opp.op){
            case OPenum::PLUS: out = '+'; break;
            case OPenum::MIN: out = '-'; break;
            case OPenum::MULT: out = '*'; break;
            case OPenum::DIV: out = '/'; break;
            case OPenum::EXP: out = '^'; break;
            default : out = 'n'; break;
        }
        
        return o << out;
    }
};

struct VAL{
    float value{0};

    VAL(std::string const& valuestring){
        value = atof(valuestring.c_str());
    }

    VAL() = default;

    friend std::ostream& operator<<(std::ostream& o, VAL const & v){
        return o << v.value;
    }

}; 

void evaluate(VAL* left, OP * op, VAL * right){
    float value{};
    switch(op->op){
        case OPenum::EXP : value = std::pow(left->value,right->value); break;
        case OPenum::MULT : value = left->value * right->value; break;
        case OPenum::DIV : value = left->value / right->value; break;
        case OPenum::PLUS : value = left->value + right->value; break;
        case OPenum::MIN : value = left->value - right->value; break;
        default : value = left->value; break;
    }
    left->value = value;
    //after evaluation call delete for op and right elements
}

struct Expression{
    using expTree = std::vector<std::variant<std::string,Expression*>>;
    expTree data{};

    using parsedTree = std::vector<std::variant<OP*,VAL*,Expression*>>;
    parsedTree p_data{};

    static std::unordered_map<OPenum,int> op_order;

    void parse();
    void fill_implicit_mult();
    void calculate();
};

// order of operations
std::unordered_map<OPenum,int> Expression::op_order{{OPenum::NOOP, -1},
                                                    {OPenum::PLUS, 0},
                                                    {OPenum::MIN, 0},
                                                    {OPenum::MULT, 1},
                                                    {OPenum::DIV,1},
                                                    {OPenum::EXP,2}, };

void print_exp_simple(Expression * exp){
    for(auto const & el : exp->p_data){
        int i = el.index();
        if (i == 0){
            std::cout << "[OP " << *(std::get<OP*>(el)) << "]";
        }
        else if (i == 1){
            std::cout << "[VAL " << *(std::get<VAL*>(el)) << "]";
        } else if (i == 2){
            std::cout<< "[EXP]";
        }
        std::cout << ' ';
    }
    std::cout << "\n";
}

void Expression::fill_implicit_mult(){
    std::deque<Expression::parsedTree::iterator> insert_pos;
    int offset{0};

    for(auto i = p_data.begin(); i != p_data.end(); i++){
        
        auto cur_idx = (*i).index();
        auto nxt_idx = (*(i+1)).index();

        if( (cur_idx == 1 && nxt_idx == 2) || (cur_idx == 1 && nxt_idx == 2) || (cur_idx == 2 && nxt_idx == 2) ){
            std::cout << "inserting at " << i-p_data.begin() << "\n";
            insert_pos.push_back(i+1);
        }
    }
    while(!insert_pos.empty()){
        OP * mult = new OP{'*'};
        p_data.insert(insert_pos.back()+offset,mult);
        offset++;
        insert_pos.pop_back();
    }
}

void parse_substring(Expression::parsedTree& data, std::string_view const & parse_string){
    std::string catcher{};     
    bool handling_number = false;
    bool num_hasdot = false;
    int length = parse_string.length();
    int pos = 0;
    bool negate = false;
    int last_pushed = -1;

    for (char const & c : parse_string){
        ++pos;
        if (is_char_numeric(c)==NUMenum::NUM || (is_char_numeric(c)==NUMenum::DOT && !num_hasdot)){
            catcher += c;
            if (!handling_number)
                handling_number = true;
            if (!num_hasdot && is_char_numeric(c)==NUMenum::DOT)
                num_hasdot = true;
            if (pos < length && is_char_numeric(parse_string[pos])!=NUMenum::NONUM){
                continue;}
        }
        if (is_char_operator(c)!=OPenum::NOOP){
            catcher += c;
        }
        if(pos == length || (handling_number && is_char_numeric(parse_string[pos])==NUMenum::NONUM) || (!handling_number && is_char_operator(c)!=OPenum::NOOP) ){
            if (handling_number){
                auto symbol = new VAL{catcher};
                if (negate){
                    symbol->value *= -1;
                    negate = false;
                }
                data.push_back(symbol);
                last_pushed = 0;
            } else if (is_char_operator(c)!=OPenum::NOOP && ((last_pushed == -1 || last_pushed == 1)&& is_char_operator(c)==OPenum::MIN)){
                negate = (negate) ? false : true;
            } else if (is_char_operator(c)!=OPenum::NOOP){
                auto symbol = new OP{c};
                data.push_back(symbol);
                last_pushed = 1;
            }
            
            handling_number = false;
            num_hasdot = false;
            catcher = "";
        }
    }
}

void Expression::parse(){
    for(auto i = data.begin(); i !=data.end() ; i++){
                //check type
                auto el = *i;
                if (el.index() == 0){
                        //parse contents and add to list
                        std::string_view new_parse_string = std::get<std::string>(el);
                        parse_substring(p_data,new_parse_string);
                }
                else if (el.index() == 1){
                    Expression* exp = std::get<Expression*>(el);
                    p_data.push_back(exp);
                    exp->parse();
                };
            }
    fill_implicit_mult();
}

void Expression::calculate(){
    using eltype = std::variant<OP*,VAL*,Expression*>;
    auto hasOps = [](eltype& el){return el.index()==0;};

    while(std::any_of(p_data.begin(),p_data.end(),hasOps)){
        int max_p{-1};
        int eval_idx{-1};
        int idx{};
        for(auto const& el : p_data){
            if (el.index()==0){
                OPenum cur = std::get<OP*>(el)->op;
                int prescedence = op_order.at(cur);
                if (prescedence > max_p){
                    eval_idx = idx;
                    max_p = prescedence;
                }
            }
            idx++;
        }
        if (max_p != -1 && eval_idx != -1){
            VAL* left = std::get<VAL*>(p_data[eval_idx-1]);
            VAL* right = std::get<VAL*>(p_data[eval_idx+1]);
            OP* operand = std::get<OP*>(p_data[eval_idx]);
            std::cout << *left << *operand << *right << "\n";
            evaluate(left,operand,right);
            std::cout << " result: " << *left << "\n";
            delete right;
            delete operand;
            p_data.erase(p_data.begin()+(eval_idx));
            p_data.erase(p_data.begin()+(eval_idx));
        }
    }
}

void calculate_expression(Expression* head){
    using eltype = std::variant<OP*,VAL*,Expression*>;
    auto hasExp = [](eltype& el){return el.index()==2;};
    std::deque<Expression *> stack {};
    std::deque<Expression::parsedTree::iterator> it_stack;
    bool br = false;
    stack.push_back(head);

    while(!stack.empty()){
        bool exprLeft = std::any_of(stack.back()->p_data.begin(),stack.back()->p_data.end(),hasExp);
        if (exprLeft) {
            auto it_pt = std::find_if(stack.back()->p_data.begin(),stack.back()->p_data.end(),hasExp);
            auto exp = std::get<Expression*>(*it_pt);
            stack.push_back(exp);
            it_stack.push_back(it_pt);
        }
        else {
            stack.back()->calculate();
            auto result = stack.back()->p_data[0];
            if (it_stack.size()>0) {
                *(it_stack.back()) = result;
                it_stack.pop_back();
            } else {
                stack.front()->p_data[0] = result;
            }
            std::cout << "resolved: "<< std::get<VAL*>(result)->value  <<"\n";
            
            if (stack.size()>0) {
                stack.pop_back();
            }
        }
        if (stack.size()>0)
            print_exp_simple(stack.back()); // useless printing but it is fun
    }
}

Expression * parse_parens(std::string_view const & str){
    auto head = new Expression();
    std::deque<Expression *> stack {};
    stack.push_back(head);
    int open{0};
    std::string collect{""};

    for(auto i = str.begin();i!=str.end();i++){
        char c = *i;
        auto curExpr = stack.back();

        if (c == '('){
            auto newExpr = new Expression();
            
            if (collect != ""){
                curExpr->data.emplace_back(collect);
                collect = "";
            }
            open++;
            curExpr->data.push_back(newExpr);
            stack.push_back(newExpr);
            continue;
        }

        if (c == ')'){
            if (collect != ""){
                curExpr->data.push_back(collect);
                collect = "";
            }
            open--;
            if(open < 0){
                std::cout << "too many closing parentheses.\n";
                return nullptr;
            }
            stack.pop_back();  
            continue;
        }
        collect += c;
    }

    if (collect != ""){
        stack.back()->data.push_back(collect);
    }

    if (open>0){
        std::cout << "too many opening parentheses.\n";
        return nullptr;
    }

    return head;
}

void parse_expression(std::string_view const & math_str){
    std::cout << "parsing: " <<math_str << "\n";
    auto x = parse_parens(math_str);
    x->parse();
    std::cout << "starting calculation...\n-----\n";
    calculate_expression(x);
    auto idx = x->p_data.front().index();
    if (idx==2){
        auto unpack = std::get<Expression*>(x->p_data.front());
        std::cout << "FINAL RESULT: " << std::get<VAL*>(unpack->p_data.front())->value << "\n";
    } else if (idx==1){
        std::cout << "FINAL RESULT: " << std::get<VAL*>(x->p_data.front())->value << "\n";
    }
};

int main(){
    std::string_view str = "(5 + (4 + 2) + (5 + 10)(20) * (1 * (4+3)) / (2^3))";
    parse_expression(str); // expected result = 273.5
} 