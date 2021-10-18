#ifndef __NASAL_VM_H__
#define __NASAL_VM_H__

class nasal_vm
{
private:
    /* reference from nasal_gc */
    nasal_ref*&              top;      // stack top
    /* values of nasal_vm */
    uint32_t                 pc;       // program counter
    uint32_t                 offset;   // used to load default parameter to a new function
    std::stack<uint32_t>     ret;      // ptr stack stores address for function to return
    std::stack<nasal_func*>  func_stk; // stack to store function,this is used when getting upvalues
    std::stack<int>          counter;  // iterator stack for forindex/foreach
    const double*            num_table;// numbers used in process(const calculation)
    std::vector<std::string> str_table;// symbols used in process
    std::vector<uint32_t>    imm;      // immediate number
    nasal_ref*               mem_addr; // used for mem_call
    nasal_gc                 gc;       // garbage collector
    /* values used for debug */
    std::vector<opcode>      bytecode; // bytecode
    std::vector<std::string> files;    // files

    void init(
        const std::vector<std::string>&,
        const std::vector<double>&,
        const std::vector<std::string>&);
    void clear();
    /* debug functions */
    bool detail_info;
    void valinfo(nasal_ref&);
    void bytecodeinfo(const uint32_t);
    void traceback();
    void stackinfo(const uint32_t);
    void detail();
    void opcallsort(const uint64_t*);
    void die(std::string);
    /* vm calculation functions*/
    bool condition(nasal_ref);
    void opr_nop();
    void opr_intg();
    void opr_intl();
    void opr_loadg();
    void opr_loadl();
    void opr_loadu();
    void opr_pnum();
    void opr_pone();
    void opr_pzero();
    void opr_pnil();
    void opr_pstr();
    void opr_newv();
    void opr_newh();
    void opr_newf();
    void opr_happ();
    void opr_para();
    void opr_defpara();
    void opr_dynpara();
    void opr_unot();
    void opr_usub();
    void opr_add();
    void opr_sub();
    void opr_mul();
    void opr_div();
    void opr_lnk();
    void opr_addc();
    void opr_subc();
    void opr_mulc();
    void opr_divc();
    void opr_lnkc();
    void opr_addeq();
    void opr_subeq();
    void opr_muleq();
    void opr_diveq();
    void opr_lnkeq();
    void opr_addeqc();
    void opr_subeqc();
    void opr_muleqc();
    void opr_diveqc();
    void opr_lnkeqc();
    void opr_meq();
    void opr_eq();
    void opr_neq();
    void opr_less();
    void opr_leq();
    void opr_grt();
    void opr_geq();
    void opr_lessc();
    void opr_leqc();
    void opr_grtc();
    void opr_geqc();
    void opr_pop();
    void opr_jmp();
    void opr_jt();
    void opr_jf();
    void opr_counter();
    void opr_cntpop();
    void opr_findex();
    void opr_feach();
    void opr_callg();
    void opr_calll();
    void opr_upval();
    void opr_callv();
    void opr_callvi();
    void opr_callh();
    void opr_callfv();
    void opr_callfh();
    void opr_callb();
    void opr_slcbegin();
    void opr_slcend();
    void opr_slc();
    void opr_slc2();
    void opr_mcallg();
    void opr_mcalll();
    void opr_mupval();
    void opr_mcallv();
    void opr_mcallh();
    void opr_ret();
public:
    nasal_vm():top(gc.top){}
    ~nasal_vm(){clear();}
    void run(
        const nasal_codegen&,
        const nasal_import&,
        const bool,
        const bool);
};

void nasal_vm::init(
    const std::vector<std::string>& strs,
    const std::vector<double>&      nums,
    const std::vector<std::string>& filenames)
{
    gc.init(strs);
    num_table=nums.data(); // get constant numbers
    str_table=strs; // get constant strings & symbols
    files=filenames;// get filenames for debugger
}
void nasal_vm::clear()
{
    gc.clear();
    while(!ret.empty())
        ret.pop();
    while(!counter.empty())
        counter.pop();
    str_table.clear();
    imm.clear();
}
void nasal_vm::valinfo(nasal_ref& val)
{
    const nasal_val* p=val.value.gcobj;
    switch(val.type)
    {
        case vm_none: printf("\tnull |\n");break;
        case vm_nil:  printf("\tnil  |\n");break;
        case vm_num:  printf("\tnum  | %lf\n",val.num());break;
        case vm_str:  printf("\tstr  | <%p> %s\n",p,raw_string(*val.str()).c_str());break;
        case vm_func: printf("\tfunc | <%p> func{entry=0x%x}\n",p,val.func()->entry);break;
        case vm_vec:  printf("\tvec  | <%p> [%lu val]\n",p,val.vec()->elems.size());break;
        case vm_hash: printf("\thash | <%p> {%lu member}\n",p,val.hash()->elems.size());break;
        case vm_obj:  printf("\tobj  | <%p>\n",p);break;
    }
}
void nasal_vm::bytecodeinfo(const uint32_t p)
{
    const opcode& code=bytecode[p];
    printf("\t0x%.8x: %s 0x%x",p,code_table[code.op].name,code.num);
    if(code.op==op_callb)
        printf(" <%s>",builtin_func[code.num].name);
    printf(" (<%s> line %d)\n",files[code.fidx].c_str(),code.line);
}
void nasal_vm::traceback()
{
    // push pc to ret stack to store the position program crashed
    ret.push(pc);
    printf("trace back:\n");
    uint32_t same=0,last=0xffffffff;
    for(uint32_t point=0;!ret.empty();last=point,ret.pop())
    {
        point=ret.top();
        if(point==last)
        {
            ++same;
            continue;
        }
        if(same)
        {
            printf("\t0x%.8x: %d same call(s) ...\n",last,same);
            same=0;
        }
        bytecodeinfo(point);
    }
    if(same)
        printf("\t0x%.8x: %d same call(s) ...\n",last,same);
}
void nasal_vm::stackinfo(const uint32_t limit=10)
{
    printf("vm stack(limit %d):\n",limit);
    uint32_t same=0,global_size=bytecode[0].num;
    nasal_ref last={vm_none,0xffffffff};
    for(uint32_t i=0;i<limit && top>=gc.stack+global_size;++i,--top)
    {
        if(top[0]==last)
        {
            ++same;
            continue;
        }
        if(same)
        {
            printf("\t...  | %d same value(s)\n",same);
            same=0;
        }
        last=top[0];
        valinfo(top[0]);
    }
    if(same)
       printf("\t...  | %d same value(s)\n",same);
}
void nasal_vm::detail()
{
    printf("mcall address: %p\n",mem_addr);
    if(bytecode[0].num)// bytecode[0] is op_intg
    {
        printf("global:\n");
        for(uint32_t i=0;i<bytecode[0].num;++i)
        {
            printf("[%d]",i);
            valinfo(gc.stack[i]);
        }
    }
    if(!gc.local.empty())
    {
        printf("local:\n");
        auto& vec=gc.local.back().vec()->elems;
        for(uint32_t i=0;i<vec.size();++i)
        {
            printf("[%d]",i);
            valinfo(vec[i]);
        }
    }
    if(!func_stk.empty() && !func_stk.top()->upvalue.empty())
    {
        printf("upvalue:\n");
        auto& upval=func_stk.top()->upvalue;
        for(uint32_t i=0;i<upval.size();++i)
        {
            auto& vec=upval[i].vec()->elems;
            for(uint32_t j=0;j<vec.size();++j)
            {
                printf("[%d][%d]",i,j);
                valinfo(vec[j]);
            }
        }
    }
}
void nasal_vm::opcallsort(const uint64_t* arr)
{
    typedef std::pair<uint32_t,uint64_t> op;
    std::vector<op> opcall;
    for(uint32_t i=0;i<=op_exit;++i)
        opcall.push_back({i,arr[i]});
    std::sort(
        opcall.begin(),
        opcall.end(),
        [](op& a,op& b){return a.second>b.second;}
    );
    std::cout<<'\n';
    for(auto& i:opcall)
    {
        if(!i.second)
            break;
        std::cout<<code_table[i.first].name<<": "<<i.second<<'\n';
    }
}
void nasal_vm::die(std::string str)
{
    printf("[vm] %s\n",str.c_str());
    traceback();
    stackinfo();
    if(detail_info)
        detail();
    std::exit(1);
}
inline bool nasal_vm::condition(nasal_ref val)
{
    if(val.type==vm_num)
        return val.value.num;
    else if(val.type==vm_str)
    {
        const std::string& str=*val.str();
        double num=str2num(str.c_str());
        if(std::isnan(num))
            return str.empty();
        return num;
    }
    return false;
}
inline void nasal_vm::opr_nop(){}
inline void nasal_vm::opr_intg()
{
    // global values store on stack
    for(uint32_t i=0;i<imm[pc];++i)
        (top++)[0].type=vm_nil;
    --top;// point to the top
}
inline void nasal_vm::opr_intl()
{
    top[0].func()->local.resize(imm[pc],gc.nil);
}
inline void nasal_vm::opr_loadg()
{
    gc.stack[imm[pc]]=(top--)[0];
}
inline void nasal_vm::opr_loadl()
{
    gc.local.back().vec()->elems[imm[pc]]=(top--)[0];
}
inline void nasal_vm::opr_loadu()
{
    func_stk.top()->upvalue[(imm[pc]>>16)&0xffff].vec()->elems[imm[pc]&0xffff]=(top--)[0];
}
inline void nasal_vm::opr_pnum()
{
    (++top)[0]={vm_num,num_table[imm[pc]]};
}
inline void nasal_vm::opr_pone()
{
    (++top)[0]={vm_num,(double)1};
}
inline void nasal_vm::opr_pzero()
{
    (++top)[0]={vm_num,(double)0};
}
inline void nasal_vm::opr_pnil()
{
    (++top)[0].type=vm_nil;
}
inline void nasal_vm::opr_pstr()
{
    (++top)[0]=gc.strs[imm[pc]];
}
inline void nasal_vm::opr_newv()
{
    nasal_ref newv=gc.alloc(vm_vec);
    auto& vec=newv.vec()->elems;// top-imm[pc] stores the vector
    vec.resize(imm[pc]);
    // use top-=imm[pc]-1 here will cause error if imm[pc] is 0
    top=top-imm[pc]+1;
    for(uint32_t i=0;i<imm[pc];++i)
        vec[i]=top[i];
    top[0]=newv;
}
inline void nasal_vm::opr_newh()
{
    (++top)[0]=gc.alloc(vm_hash);
}
inline void nasal_vm::opr_newf()
{
    offset=1;
    (++top)[0]=gc.alloc(vm_func);
    top[0].func()->entry=imm[pc];
    if(!gc.local.empty())
    {
        top[0].func()->upvalue=func_stk.top()->upvalue;
        top[0].func()->upvalue.push_back(gc.local.back());
    }
}
inline void nasal_vm::opr_happ()
{
    top[-1].hash()->elems[str_table[imm[pc]]]=top[0];
    --top;
}
inline void nasal_vm::opr_para()
{
    nasal_func* func=top[0].func();
    size_t size=func->keys.size();
    func->keys[str_table[imm[pc]]]=size;
    func->local[offset++]={vm_none};
}
inline void nasal_vm::opr_defpara()
{
    nasal_ref val=top[0];
    nasal_func* func=(--top)[0].func();
    size_t size=func->keys.size();
    func->keys[str_table[imm[pc]]]=size;
    func->local[offset++]=val;
}
inline void nasal_vm::opr_dynpara()
{
    top[0].func()->dynpara=imm[pc];
}
inline void nasal_vm::opr_unot()
{
    nasal_ref val=top[0];
    switch(val.type)
    {
        case vm_nil:top[0]=gc.zero;break;
        case vm_num:top[0]=val.num()?gc.zero:gc.one;break;
        case vm_str:
        {
            double num=str2num(val.str()->c_str());
            if(std::isnan(num))
                top[0]=val.str()->empty()?gc.one:gc.zero;
            else
                top[0]=num?gc.zero:gc.one;
        }
        break;
        default:die("unot: incorrect value type");break;
    }
}
inline void nasal_vm::opr_usub()
{
    top[0]={vm_num,-top[0].to_number()};
}

#define op_calc(type)\
    nasal_ref val(vm_num,top[-1].to_number() type top[0].to_number());\
    (--top)[0]=val;

inline void nasal_vm::opr_add(){op_calc(+);}
inline void nasal_vm::opr_sub(){op_calc(-);}
inline void nasal_vm::opr_mul(){op_calc(*);}
inline void nasal_vm::opr_div(){op_calc(/);}
inline void nasal_vm::opr_lnk()
{
    nasal_ref val=gc.alloc(vm_str);
    *val.str()=top[-1].to_string()+top[0].to_string();
    (--top)[0]=val;
}

#define op_calc_const(type)\
    nasal_ref val(vm_num,top[0].to_number() type num_table[imm[pc]]);\
    top[0]=val;

inline void nasal_vm::opr_addc(){op_calc_const(+);}
inline void nasal_vm::opr_subc(){op_calc_const(-);}
inline void nasal_vm::opr_mulc(){op_calc_const(*);}
inline void nasal_vm::opr_divc(){op_calc_const(/);}
inline void nasal_vm::opr_lnkc()
{
    nasal_ref val=gc.alloc(vm_str);
    *val.str()=top[0].to_string()+str_table[imm[pc]];
    top[0]=val;
}

#define op_calc_eq(type)\
    nasal_ref val(vm_num,mem_addr[0].to_number() type top[-1].to_number());\
    (--top)[0]=mem_addr[0]=val;

inline void nasal_vm::opr_addeq(){op_calc_eq(+);}
inline void nasal_vm::opr_subeq(){op_calc_eq(-);}
inline void nasal_vm::opr_muleq(){op_calc_eq(*);}
inline void nasal_vm::opr_diveq(){op_calc_eq(/);}
inline void nasal_vm::opr_lnkeq()
{
    nasal_ref val=gc.alloc(vm_str);
    *val.str()=mem_addr[0].to_string()+top[-1].to_string();
    (--top)[0]=mem_addr[0]=val;
}

#define op_calc_eq_const(type)\
    nasal_ref val(vm_num,mem_addr[0].to_number() type num_table[imm[pc]]);\
    top[0]=mem_addr[0]=val;

inline void nasal_vm::opr_addeqc(){op_calc_eq_const(+);}
inline void nasal_vm::opr_subeqc(){op_calc_eq_const(-);}
inline void nasal_vm::opr_muleqc(){op_calc_eq_const(*);}
inline void nasal_vm::opr_diveqc(){op_calc_eq_const(/);}
inline void nasal_vm::opr_lnkeqc()
{
    nasal_ref val=gc.alloc(vm_str);
    *val.str()=mem_addr[0].to_string()+str_table[imm[pc]];
    top[0]=mem_addr[0]=val;
}

inline void nasal_vm::opr_meq()
{
    mem_addr[0]=(--top)[0];
}
inline void nasal_vm::opr_eq()
{
    nasal_ref val2=top[0];
    nasal_ref val1=(--top)[0];
    uint8_t a_type=val1.type;
    uint8_t b_type=val2.type;
    if(a_type==vm_nil && b_type==vm_nil)
        top[0]=gc.one;
    else if(a_type==vm_str && b_type==vm_str)
        top[0]=(*val1.str()==*val2.str())?gc.one:gc.zero;
    else if(a_type==vm_num || b_type==vm_num)
        top[0]=(val1.to_number()==val2.to_number())?gc.one:gc.zero;
    else
        top[0]=(val1==val2)?gc.one:gc.zero;
}
inline void nasal_vm::opr_neq()
{
    nasal_ref val2=top[0];
    nasal_ref val1=(--top)[0];
    uint8_t a_type=val1.type;
    uint8_t b_type=val2.type;
    if(a_type==vm_nil && b_type==vm_nil)
        top[0]=gc.zero;
    else if(a_type==vm_str && b_type==vm_str)
        top[0]=(*val1.str()!=*val2.str())?gc.one:gc.zero;
    else if(a_type==vm_num || b_type==vm_num)
        top[0]=(val1.to_number()!=val2.to_number())?gc.one:gc.zero;
    else
        top[0]=(val1!=val2)?gc.one:gc.zero;
}

#define op_cmp(type)\
    --top;\
    top[0]=(top[0].to_number() type top[1].to_number())?gc.one:gc.zero;

inline void nasal_vm::opr_less(){op_cmp(<);}
inline void nasal_vm::opr_leq(){op_cmp(<=);}
inline void nasal_vm::opr_grt(){op_cmp(>);}
inline void nasal_vm::opr_geq(){op_cmp(>=);}

#define op_cmp_const(type)\
    top[0]=(top[0].to_number() type num_table[imm[pc]])?gc.one:gc.zero;

inline void nasal_vm::opr_lessc(){op_cmp_const(<);}
inline void nasal_vm::opr_leqc(){op_cmp_const(<=);}
inline void nasal_vm::opr_grtc(){op_cmp_const(>);}
inline void nasal_vm::opr_geqc(){op_cmp_const(>=);}

inline void nasal_vm::opr_pop()
{
    --top;
}
inline void nasal_vm::opr_jmp()
{
    pc=imm[pc]-1;
}
inline void nasal_vm::opr_jt()
{
    if(condition(top[0]))
        pc=imm[pc]-1;
}
inline void nasal_vm::opr_jf()
{
    if(!condition(top[0]))
        pc=imm[pc]-1;
    --top;
}
inline void nasal_vm::opr_counter()
{
    counter.push(-1);
    if(top[0].type!=vm_vec)
        die("cnt: must use vector in forindex/foreach");
}
inline void nasal_vm::opr_cntpop()
{
    counter.pop();
}
inline void nasal_vm::opr_findex()
{
    if(++counter.top()>=top[0].vec()->elems.size())
    {
        pc=imm[pc]-1;
        return;
    }
    (++top)[0]={vm_num,static_cast<double>(counter.top())};
}
inline void nasal_vm::opr_feach()
{
    std::vector<nasal_ref>& ref=top[0].vec()->elems;
    if(++counter.top()>=ref.size())
    {
        pc=imm[pc]-1;
        return;
    }
    (++top)[0]=ref[counter.top()];
}
inline void nasal_vm::opr_callg()
{
    (++top)[0]=gc.stack[imm[pc]];
}
inline void nasal_vm::opr_calll()
{
    (++top)[0]=gc.local.back().vec()->elems[imm[pc]];
}
inline void nasal_vm::opr_upval()
{
    (++top)[0]=func_stk.top()->upvalue[(imm[pc]>>16)&0xffff].vec()->elems[imm[pc]&0xffff];
}
inline void nasal_vm::opr_callv()
{
    nasal_ref val=top[0];
    nasal_ref vec=(--top)[0];
    if(vec.type==vm_vec)
    {
        top[0]=vec.vec()->get_val(val.to_number());
        if(top[0].type==vm_none)
            die("callv: index out of range:"+std::to_string(val.to_number()));
    }
    else if(vec.type==vm_hash)
    {
        if(val.type!=vm_str)
            die("callv: must use string as the key");
        top[0]=vec.hash()->get_val(*val.value.gcobj->ptr.str);
        if(top[0].type==vm_none)
            die("callv: cannot find member \""+*val.str()+"\" of this hash");
        if(top[0].type==vm_func)
            top[0].func()->local[0]=val;// me
    }
    else if(vec.type==vm_str)
    {
        std::string& str=*vec.str();
        int num=val.to_number();
        int str_size=str.length();
        if(num<-str_size || num>=str_size)
            die("callv: index out of range:"+std::to_string(val.to_number()));
        top[0]={vm_num,static_cast<double>(str[num>=0? num:num+str_size])};
    }
    else
        die("callv: must call a vector/hash/string");
}
inline void nasal_vm::opr_callvi()
{
    nasal_ref val=top[0];
    if(val.type!=vm_vec)
        die("callvi: must use a vector");

    // cannot use operator[],because this may cause overflow
    (++top)[0]=val.vec()->get_val(imm[pc]);
    if(top[0].type==vm_none)
        die("callvi: index out of range:"+std::to_string(imm[pc]));
}
inline void nasal_vm::opr_callh()
{
    nasal_ref val=top[0];
    if(val.type!=vm_hash)
        die("callh: must call a hash");

    top[0]=val.hash()->get_val(str_table[imm[pc]]);
    if(top[0].type==vm_none)
        die("callh: member \""+str_table[imm[pc]]+"\" does not exist");

    if(top[0].type==vm_func)
        top[0].func()->local[0]=val;// me
}
inline void nasal_vm::opr_callfv()
{
    size_t args_size=imm[pc];
    nasal_ref* args=top-args_size+1;
    if(args[-1].type!=vm_func)
        die("callfv: must call a function");
    // push new local scope
    func_stk.push(args[-1].func());
    auto& func=*args[-1].func();

    gc.local.push_back(gc.alloc(vm_vec));
    gc.local.back().vec()->elems=func.local;
    // load parameters
    auto& closure=gc.local.back().vec()->elems;

    size_t para_size=func.keys.size();
    // load arguments
    // if the first default value is not vm_none,then values after it are not nullptr
    if(args_size<para_size && func.local[args_size+1/*1 is reserved for 'me'*/].type==vm_none)
        die("callfv: lack argument(s)");
    // if args_size>para_size,for 0 to args_size will cause corruption
    size_t min_size=std::min(para_size,args_size);
    for(uint32_t i=0;i<min_size;++i)
        closure[i+1]=args[i];
    // load dynamic argument if args_size>=para_size
    if(func.dynpara)
    {
        nasal_ref vec=gc.alloc(vm_vec);
        for(uint32_t i=para_size;i<args_size;++i)
            vec.vec()->elems.push_back(args[i]);
        closure.back()=vec;
    }
    
    top-=args_size;// pop arguments
    ret.push(pc);
    pc=func.entry-1;
}
inline void nasal_vm::opr_callfh()
{
    // get parameter list and function value
    auto& hash=top[0].hash()->elems;
    if(top[-1].type!=vm_func)
        die("callfh: must call a function");
    // push new local scope
    func_stk.push(top[-1].func());
    auto& func=*top[-1].func();
    gc.local.push_back(gc.alloc(vm_vec));
    gc.local.back().vec()->elems=func.local;
    // load parameters
    auto& closure=gc.local.back().vec()->elems;
    if(func.dynpara)
        die("callfh: special call cannot use dynamic argument");

    for(auto& i:func.keys)
    {
        if(hash.count(i.first))
            closure[i.second+1]=hash[i.first];
        else if(func.local[i.second+1/*1 is reserved for 'me'*/].type==vm_none)
            die("callfh: lack argument(s): \""+i.first+"\"");
    }

    --top;// pop hash
    ret.push(pc);
    pc=func.entry-1;
}
inline void nasal_vm::opr_callb()
{
    (++top)[0]=(*builtin_func[imm[pc]].func)(gc.local.back().vec()->elems,gc);
    if(top[0].type==vm_none)
        die("native function error.");
}
inline void nasal_vm::opr_slcbegin()
{
    // | slice_vector | <-- top[0]
    // ----------------
    // | resource_vec | <-- top[-1]
    // ----------------
    (++top)[0]=gc.alloc(vm_vec);
    if(top[-1].type!=vm_vec)
        die("slcbegin: must slice a vector");
}
inline void nasal_vm::opr_slcend()
{
    top[-1]=top[0];
    --top;
}
inline void nasal_vm::opr_slc()
{
    nasal_ref val=(top--)[0];
    nasal_ref res=top[-1].vec()->get_val(val.to_number());
    if(res.type==vm_none)
        die("slc: index out of range:"+std::to_string(val.to_number()));
    top[0].vec()->elems.push_back(res);
}
inline void nasal_vm::opr_slc2()
{
    nasal_ref val2=(top--)[0];
    nasal_ref val1=(top--)[0];
    std::vector<nasal_ref>& ref=top[-1].vec()->elems;
    std::vector<nasal_ref>& aim=top[0].vec()->elems;

    uint8_t type1=val1.type,type2=val2.type;
    int num1=val1.to_number();
    int num2=val2.to_number();
    size_t size=ref.size();
    if(type1==vm_nil && type2==vm_nil)
    {
        num1=0;
        num2=size-1;
    }
    else if(type1==vm_nil && type2!=vm_nil)
        num1=num2<0? -size:0;
    else if(type1!=vm_nil && type2==vm_nil)
        num2=num1<0? -1:size-1;

    if(num1>=num2)
        die("slc2: begin index must be less than end index");
    else if(num1<-size || num1>=size)
        die("slc2: begin index out of range: "+std::to_string(num1));
    else if(num2<-size || num2>=size)
        die("slc2: end index out of range: "+std::to_string(num2));
    else
        for(int i=num1;i<=num2;++i)
            aim.push_back(i>=0?ref[i]:ref[i+size]);
}
inline void nasal_vm::opr_mcallg()
{
    mem_addr=gc.stack+imm[pc];
    (++top)[0]=mem_addr[0];
}
inline void nasal_vm::opr_mcalll()
{
    mem_addr=&(gc.local.back().vec()->elems[imm[pc]]);
    (++top)[0]=mem_addr[0];
}
inline void nasal_vm::opr_mupval()
{
    mem_addr=&func_stk.top()->upvalue[(imm[pc]>>16)&0xffff].vec()->elems[imm[pc]&0xffff];
    (++top)[0]=mem_addr[0];
}
inline void nasal_vm::opr_mcallv()
{
    nasal_ref val=top[0];
    nasal_ref vec=(--top)[0];
    if(vec.type==vm_vec)
    {
        mem_addr=vec.vec()->get_mem(val.to_number());
        if(!mem_addr)
            die("mcallv: index out of range:"+std::to_string(val.to_number()));
    }
    else if(vec.type==vm_hash)
    {
        if(val.type!=vm_str)
            die("mcallv: must use string as the key");
        nasal_hash& ref=*vec.hash();
        std::string& str=*val.str();
        mem_addr=ref.get_mem(str);
        if(!mem_addr)
        {
            ref.elems[str]=gc.nil;
            mem_addr=ref.get_mem(str);
        }
    }
    else
        die("mcallv: cannot get memory space in other types");
}
inline void nasal_vm::opr_mcallh()
{
    nasal_ref hash=top[0];
    if(hash.type!=vm_hash)
        die("mcallh: must call a hash");
    nasal_hash& ref=*hash.hash();
    std::string& str=str_table[imm[pc]];
    mem_addr=ref.get_mem(str);
    if(!mem_addr) // create a new key
    {
        ref.elems[str]=gc.nil;
        mem_addr=ref.get_mem(str);
    }
}
inline void nasal_vm::opr_ret()
{
    // get nasal_func and set 'me' to nil
    // and rewrite nasal_func with returned value
    top[-1].func()->local[0]={vm_nil};
    --top;
    top[0]=top[1];

    func_stk.pop();         // pop function stack
    gc.local.pop_back();    // pop local scope
    pc=ret.top();ret.pop(); // fetch pc
}
void nasal_vm::run(
    const nasal_codegen& gen,
    const nasal_import& linker,
    const bool opcnt,
    const bool debug)
{
    detail_info=debug;
    init(gen.get_strs(),gen.get_nums(),linker.get_file());
    uint64_t count[op_exit+1]={0};
    const void* opr_table[]=
    {
        &&nop,     &&intg,     &&intl,   &&loadg,
        &&loadl,   &&loadu,    &&pnum,   &&pone,
        &&pzero,   &&pnil,     &&pstr,   &&newv,
        &&newh,    &&newf,     &&happ,   &&para,
        &&defpara, &&dynpara,  &&unot,   &&usub,
        &&add,     &&sub,      &&mul,    &&div,
        &&lnk,     &&addc,     &&subc,   &&mulc,
        &&divc,    &&lnkc,     &&addeq,  &&subeq,
        &&muleq,   &&diveq,    &&lnkeq,  &&addeqc,
        &&subeqc,  &&muleqc,   &&diveqc, &&lnkeqc,
        &&meq,     &&eq,       &&neq,    &&less,
        &&leq,     &&grt,      &&geq,    &&lessc,
        &&leqc,    &&grtc,     &&geqc,   &&pop,
        &&jmp,     &&jt,       &&jf,     &&counter,
        &&cntpop,  &&findex,   &&feach,  &&callg,
        &&calll,   &&upval,    &&callv,  &&callvi,
        &&callh,   &&callfv,   &&callfh, &&callb,
        &&slcbegin,&&slcend,   &&slc,    &&slc2,
        &&mcallg,  &&mcalll,   &&mupval, &&mcallv,
        &&mcallh,  &&ret,      &&vmexit
    };
    bytecode=gen.get_code();
    std::vector<const void*> code;
    for(auto& i:bytecode)
    {
        code.push_back(opr_table[i.op]);
        imm.push_back(i.num);
    }

    // set canary and program counter
    auto& canary=gc.stack[STACK_MAX_DEPTH-1];
    canary.value.gcobj=nullptr;
    pc=0;
    // goto the first operand
    goto *code[pc];

vmexit:
    if(canary.value.gcobj)
        die("stack overflow");
    if(opcnt)
        opcallsort(count);
    clear();
    return;
// may cause stackoverflow
#define exec_operand(op,num) {op();++count[num];if(!canary.value.gcobj)goto *code[++pc];goto vmexit;}
// do not cause stackoverflow
#define exec_opnodie(op,num) {op();++count[num];goto *code[++pc];}

nop:     exec_opnodie(opr_nop     ,op_nop     ); // 0
intg:    exec_opnodie(opr_intg    ,op_intg    ); // +imm[pc] (detected at codegen)
intl:    exec_opnodie(opr_intl    ,op_intl    ); // -0
loadg:   exec_opnodie(opr_loadg   ,op_loadg   ); // -1
loadl:   exec_opnodie(opr_loadl   ,op_loadl   ); // -1
loadu:   exec_opnodie(opr_loadu   ,op_loadu   ); // -1
pnum:    exec_operand(opr_pnum    ,op_pnum    ); // +1
pone:    exec_operand(opr_pone    ,op_pone    ); // +1
pzero:   exec_operand(opr_pzero   ,op_pzero   ); // +1
pnil:    exec_operand(opr_pnil    ,op_pnil    ); // +1
pstr:    exec_operand(opr_pstr    ,op_pstr    ); // +1
newv:    exec_operand(opr_newv    ,op_newv    ); // +1-imm[pc]
newh:    exec_operand(opr_newh    ,op_newh    ); // +1
newf:    exec_operand(opr_newf    ,op_newf    ); // +1
happ:    exec_opnodie(opr_happ    ,op_happ    ); // -1
para:    exec_opnodie(opr_para    ,op_para    ); // -0
defpara: exec_opnodie(opr_defpara ,op_defpara ); // -1
dynpara: exec_opnodie(opr_dynpara ,op_dynpara ); // -0
unot:    exec_opnodie(opr_unot    ,op_unot    ); // -0
usub:    exec_opnodie(opr_usub    ,op_usub    ); // -0
add:     exec_opnodie(opr_add     ,op_add     ); // -1
sub:     exec_opnodie(opr_sub     ,op_sub     ); // -1
mul:     exec_opnodie(opr_mul     ,op_mul     ); // -1
div:     exec_opnodie(opr_div     ,op_div     ); // -1
lnk:     exec_opnodie(opr_lnk     ,op_lnk     ); // -1
addc:    exec_opnodie(opr_addc    ,op_addc    ); // -0
subc:    exec_opnodie(opr_subc    ,op_subc    ); // -0
mulc:    exec_opnodie(opr_mulc    ,op_mulc    ); // -0
divc:    exec_opnodie(opr_divc    ,op_divc    ); // -0
lnkc:    exec_opnodie(opr_lnkc    ,op_lnkc    ); // -0
addeq:   exec_opnodie(opr_addeq   ,op_addeq   ); // -1
subeq:   exec_opnodie(opr_subeq   ,op_subeq   ); // -1
muleq:   exec_opnodie(opr_muleq   ,op_muleq   ); // -1
diveq:   exec_opnodie(opr_diveq   ,op_diveq   ); // -1
lnkeq:   exec_opnodie(opr_lnkeq   ,op_lnkeq   ); // -1
addeqc:  exec_opnodie(opr_addeqc  ,op_addeqc  ); // -0
subeqc:  exec_opnodie(opr_subeqc  ,op_subeqc  ); // -0
muleqc:  exec_opnodie(opr_muleqc  ,op_muleqc  ); // -0
diveqc:  exec_opnodie(opr_diveqc  ,op_diveqc  ); // -0
lnkeqc:  exec_opnodie(opr_lnkeqc  ,op_lnkeqc  ); // -0
meq:     exec_opnodie(opr_meq     ,op_meq     ); // -1
eq:      exec_opnodie(opr_eq      ,op_eq      ); // -1
neq:     exec_opnodie(opr_neq     ,op_neq     ); // -1
less:    exec_opnodie(opr_less    ,op_less    ); // -1
leq:     exec_opnodie(opr_leq     ,op_leq     ); // -1
grt:     exec_opnodie(opr_grt     ,op_grt     ); // -1
geq:     exec_opnodie(opr_geq     ,op_geq     ); // -1
lessc:   exec_opnodie(opr_lessc   ,op_lessc   ); // -0
leqc:    exec_opnodie(opr_leqc    ,op_leqc    ); // -0
grtc:    exec_opnodie(opr_grtc    ,op_grtc    ); // -0
geqc:    exec_opnodie(opr_geqc    ,op_geqc    ); // -0
pop:     exec_opnodie(opr_pop     ,op_pop     ); // -1
jmp:     exec_opnodie(opr_jmp     ,op_jmp     ); // -0
jt:      exec_opnodie(opr_jt      ,op_jt      ); // -0
jf:      exec_opnodie(opr_jf      ,op_jf      ); // -1
counter: exec_opnodie(opr_counter ,op_cnt     ); // -0
cntpop:  exec_opnodie(opr_cntpop  ,op_cntpop  ); // -0
findex:  exec_operand(opr_findex  ,op_findex  ); // +1
feach:   exec_operand(opr_feach   ,op_feach   ); // +1
callg:   exec_operand(opr_callg   ,op_callg   ); // +1
calll:   exec_operand(opr_calll   ,op_calll   ); // +1
upval:   exec_operand(opr_upval   ,op_upval   ); // +1
callv:   exec_opnodie(opr_callv   ,op_callv   ); // -0
callvi:  exec_opnodie(opr_callvi  ,op_callvi  ); // -0
callh:   exec_opnodie(opr_callh   ,op_callh   ); // -0
callfv:  exec_opnodie(opr_callfv  ,op_callfv  ); // -0
callfh:  exec_opnodie(opr_callfh  ,op_callfh  ); // -0
callb:   exec_opnodie(opr_callb   ,op_callb   ); // -0
slcbegin:exec_operand(opr_slcbegin,op_slcbegin); // +1
slcend:  exec_opnodie(opr_slcend  ,op_slcend  ); // -1
slc:     exec_opnodie(opr_slc     ,op_slc     ); // -1
slc2:    exec_opnodie(opr_slc2    ,op_slc2    ); // -2
mcallg:  exec_operand(opr_mcallg  ,op_mcallg  ); // +1
mcalll:  exec_operand(opr_mcalll  ,op_mcalll  ); // +1
mupval:  exec_operand(opr_mupval  ,op_mupval  ); // +1
mcallv:  exec_opnodie(opr_mcallv  ,op_mcallv  ); // -0
mcallh:  exec_opnodie(opr_mcallh  ,op_mcallh  ); // -0
ret:     exec_opnodie(opr_ret     ,op_ret     ); // -1
}
#endif