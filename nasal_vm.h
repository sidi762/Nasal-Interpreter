#ifndef __NASAL_VM_H__
#define __NASAL_VM_H__

class nasal_vm
{
protected:
    /* registers and constants of nasal_vm */
    uint32_t              pc;       // program counter
    nasal_ref*            localr;   // local scope register
    nasal_ref*            memr;     // used for mem_call
    nasal_ref             funcr;    // function register
    nasal_ref             upvalr;   // upvalue register
    nasal_ref*            canary;   // avoid stackoverflow
    nasal_ref*            top;      // stack top
    const double*         num_table;// const numbers, ref from nasal_codegen
    const std::string*    str_table;// const symbols, ref from nasal_codegen
    std::vector<uint32_t> imm;      // immediate number
    
    /* garbage collector */
    nasal_gc              gc;
    /* main stack */
    nasal_ref             stack[STACK_DEPTH];

    /* values used for debugger */
    const std::string*    files;    // ref from nasal_import
    const opcode*         bytecode; // ref from nasal_codegen

    void init(
        const std::vector<std::string>&,
        const std::vector<double>&,
        const std::vector<opcode>&,
        const std::vector<std::string>&,
        const std::vector<std::string>&);
    /* debug functions */
    bool detail_info;
    void valinfo(nasal_ref&);
    void bytecodeinfo(const char*,const uint32_t);
    void traceback();
    void stackinfo(const uint32_t);
    void register_info();
    void global_state();
    void local_state();
    void upval_state();
    void detail();
    void opcallsort(const uint64_t*);
    void die(const std::string&);
    /* vm calculation functions*/
    bool condition(nasal_ref);
    /* vm operands */
    void opr_intg();
    void opr_intl();
    void opr_loadg();
    void opr_loadl();
    void opr_loadu();
    void opr_pnum();
    void opr_pnil();
    void opr_pstr();
    void opr_newv();
    void opr_newh();
    void opr_newf();
    void opr_happ();
    void opr_para();
    void opr_deft();
    void opr_dyn();
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
    void opr_cnt();
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
    void opr_slcbeg();
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
    nasal_vm():pc(0),localr(nullptr),memr(nullptr),funcr(nil),
               upvalr(nil),canary(nullptr),top(stack),
               num_table(nullptr),str_table(nullptr),
               gc(pc,localr,memr,funcr,upvalr,canary,top,stack),
               files(nullptr),bytecode(nullptr),detail_info(false){}
    void run(
        const nasal_codegen&,
        const nasal_import&,
        const std::vector<std::string>&,
        const bool,
        const bool);
};

void nasal_vm::init(
    const std::vector<std::string>& strs,
    const std::vector<double>&      nums,
    const std::vector<opcode>&      code,
    const std::vector<std::string>& filenames,
    const std::vector<std::string>& argv)
{
    gc.init(strs,argv);
    num_table=nums.data();
    str_table=strs.data();
    bytecode=code.data();
    files=filenames.data();

    /* set canary and program counter */
    pc=0;
    localr=memr=nullptr;
    funcr=upvalr=nil;
    canary=stack+STACK_DEPTH-1; // stack[STACK_DEPTH-1]
    top=stack;
}
void nasal_vm::valinfo(nasal_ref& val)
{
    const nasal_val* p=val.value.gcobj;
    std::cout<<"\t";
    switch(val.type)
    {
        case vm_none: std::cout<<"| null |";break;
        case vm_ret:  std::cout<<"| pc   | 0x"<<std::hex
                               <<val.ret()<<std::dec;break;
        case vm_addr: std::cout<<"| addr | "<<std::hex
                               <<(void*)val.addr()<<std::dec;break;
        case vm_cnt:  std::cout<<"| cnt  | "<<val.cnt();break;
        case vm_nil:  std::cout<<"| nil  |";break;
        case vm_num:  std::cout<<"| num  | "<<val.num();break;
        case vm_str:  std::cout<<"| str  | <"<<std::hex<<p<<"> "
                               <<rawstr(val.str(),16)<<std::dec;break;
        case vm_func: std::cout<<"| func | <"<<std::hex<<p<<"> entry:0x"
                               <<val.func().entry<<std::dec;break;
        case vm_upval:std::cout<<"| upval| <"<<std::hex<<p<<std::dec<<"> ["
                               <<val.upval().size<<" val]";break;
        case vm_vec:  std::cout<<"| vec  | <"<<std::hex<<p<<std::dec<<"> ["
                               <<val.vec().size()<<" val]";break;
        case vm_hash: std::cout<<"| hash | <"<<std::hex<<p<<std::dec<<"> {"
                               <<val.hash().size()<<" val}";break;
        case vm_obj:  std::cout<<"| obj  | <"<<std::hex<<p<<"> obj:0x"
                               <<(uint64_t)val.obj().ptr<<std::dec;break;
        case vm_co:   std::cout<<"| co   | <"<<std::hex<<p<<std::dec
                               <<"> coroutine";break;
        default:      std::cout<<"| err  | <"<<std::hex<<p<<std::dec
                               <<"> unknown object";break;
    }
    std::cout<<"\n";
}
void nasal_vm::bytecodeinfo(const char* header,const uint32_t p)
{
    const opcode& c=bytecode[p];
    c.print(header,num_table,str_table,p,true);
    std::cout<<" ("<<files[c.fidx]<<":"<<c.line<<")\n";
}
void nasal_vm::traceback()
{
    const uint32_t global_size=bytecode[0].num; // bytecode[0] is op_intg
    nasal_ref* t=top;
    nasal_ref* bottom=stack+global_size;
    std::stack<uint32_t> ret;
    for(nasal_ref* i=bottom;i<=t;++i)
        if(i->type==vm_ret)
            ret.push(i->ret());
    // push pc to ret stack to store the position program crashed
    ret.push(pc);
    std::cout<<"trace back:\n";
    uint32_t same=0,last=0xffffffff;
    for(uint32_t point=0;!ret.empty();last=point,ret.pop())
    {
        if((point=ret.top())==last)
        {
            ++same;
            continue;
        }
        if(same)
            std::cout<<"  0x"<<std::hex<<std::setw(8)<<std::setfill('0')
                     <<last<<std::dec<<":       "<<same<<" same call(s)\n";
        same=0;
        bytecodeinfo("  ",point);
    }
    if(same)
        std::cout<<"  0x"<<std::hex<<std::setw(8)<<std::setfill('0')
                 <<last<<std::dec<<":       "<<same<<" same call(s)\n";
}
void nasal_vm::stackinfo(const uint32_t limit=10)
{
    /* bytecode[0] is op_intg, the .num is the global size */
    uint32_t   gsize=gc.stack==stack?bytecode[0].num:0;
    nasal_ref* t=top;
    nasal_ref* bottom=gc.stack+gsize;
    std::cout<<"vm stack(0x"<<std::hex<<(uint64_t)bottom<<std::dec
             <<"<sp+"<<gsize<<">, limit "<<limit<<", total "
             <<(t<bottom? 0:(int64_t)(t-bottom+1))<<")\n";
    if(t<bottom)
        return;
    for(uint32_t i=0;i<limit && t>=bottom;++i,--t)
    {
        std::cout<<"  0x"<<std::hex
                 <<std::setw(8)<<std::setfill('0')
                 <<(uint64_t)(t-gc.stack)<<std::dec;
        valinfo(t[0]);
    }
}
void nasal_vm::register_info()
{
    std::cout<<"registers("<<(gc.coroutine?"coroutine":"main")<<")\n"<<std::hex
             <<"  [ pc     ]    | pc   | 0x"<<pc<<"\n"
             <<"  [ global ]    | addr | "<<stack<<"\n"
             <<"  [ localr ]    | addr | "<<localr<<"\n"
             <<"  [ memr   ]    | addr | "<<memr<<"\n"
             <<"  [ canary ]    | addr | "<<canary<<"\n"
             <<"  [ top    ]    | addr | "<<top<<"\n"
             <<std::dec;
    std::cout<<"  [ funcr  ]";valinfo(funcr);
    std::cout<<"  [ upvalr ]";valinfo(upvalr);
}
void nasal_vm::global_state()
{
    if(!bytecode[0].num || stack[0].type==vm_none) // bytecode[0].op is op_intg
        return;
    std::cout<<"global(0x"<<std::hex<<(uint64_t)stack<<"<sp+0>)\n"<<std::dec;
    for(uint32_t i=0;i<bytecode[0].num;++i)
    {
        std::cout<<"  0x"<<std::hex<<std::setw(8)
                 <<std::setfill('0')<<i<<std::dec;
        valinfo(stack[i]);
    }
}
void nasal_vm::local_state()
{
    if(!localr || !funcr.func().lsize)
        return;
    const uint32_t lsize=funcr.func().lsize;
    std::cout<<"local(0x"<<std::hex<<(uint64_t)localr
             <<"<sp+"<<(uint64_t)(localr-gc.stack)<<">)\n"<<std::dec;
    for(uint32_t i=0;i<lsize;++i)
    {
        std::cout<<"  0x"<<std::hex<<std::setw(8)
                 <<std::setfill('0')<<i<<std::dec;
        valinfo(localr[i]);
    }
}
void nasal_vm::upval_state()
{
    if(funcr.type==vm_nil || funcr.func().upvalue.empty())
        return;
    std::cout<<"upvalue\n";
    auto& upval=funcr.func().upvalue;
    for(uint32_t i=0;i<upval.size();++i)
    {
        std::cout<<"  -> upval["<<i<<"]:\n";
        auto& uv=upval[i].upval();
        for(uint32_t j=0;j<uv.size;++j)
        {
            std::cout<<"     0x"<<std::hex<<std::setw(8)
                     <<std::setfill('0')<<j<<std::dec;
            valinfo(uv[j]);
        }
    }
}
void nasal_vm::detail()
{
    register_info();
    global_state();
    local_state();
    upval_state();
}
void nasal_vm::opcallsort(const uint64_t* arr)
{
    typedef std::pair<uint32_t,uint64_t> op;
    std::vector<op> opcall;
    uint64_t total=0;
    for(uint32_t i=0;i<op_ret+1;++i)
    {
        total+=arr[i];
        if(arr[i])
            opcall.push_back({i,arr[i]});
    }
    std::sort(opcall.begin(),opcall.end(),
        [](const op& a,const op& b){return a.second>b.second;}
    );
    std::cout<<"\noperands call info";
    for(auto& i:opcall)
    {
        uint64_t rate=i.second*100/total;
        if(rate)
            std::cout<<"\n "<<code_table[i.first].name
                     <<" : "<<i.second<<" ("<<rate<<"%)";
        else
        {
            std::cout<<"\n ...";
            break;
        }
    }
    std::cout<<"\n total  : "<<total<<'\n';
}
void nasal_vm::die(const std::string& str)
{
    std::cout<<"[vm] "<<str<<"\n";
    traceback();
    stackinfo();
    if(detail_info)
        detail();
    std::exit(1);
}
inline bool nasal_vm::condition(nasal_ref val)
{
    if(val.type==vm_num)
        return val.num();
    else if(val.type==vm_str)
    {
        const double num=str2num(val.str().c_str());
        if(std::isnan(num))
            return !val.str().empty();
        return num;
    }
    return false;
}
inline void nasal_vm::opr_intg()
{
    // global values store on stack
    for(uint32_t i=0;i<imm[pc];++i)
        (top++)[0].type=vm_nil;
    --top;// point to the top
}
inline void nasal_vm::opr_intl()
{
    top[0].func().local.resize(imm[pc],nil);
    top[0].func().lsize=imm[pc];
}
inline void nasal_vm::opr_loadg()
{
    stack[imm[pc]]=(top--)[0];
}
inline void nasal_vm::opr_loadl()
{
    localr[imm[pc]]=(top--)[0];
}
inline void nasal_vm::opr_loadu()
{
    funcr.func().upvalue[(imm[pc]>>16)&0xffff]
         .upval()[imm[pc]&0xffff]=(top--)[0];
}
inline void nasal_vm::opr_pnum()
{
    (++top)[0]={vm_num,num_table[imm[pc]]};
}
inline void nasal_vm::opr_pnil()
{
    (++top)[0]=nil;
}
inline void nasal_vm::opr_pstr()
{
    (++top)[0]=gc.strs[imm[pc]];
}
inline void nasal_vm::opr_newv()
{
    nasal_ref newv=gc.alloc(vm_vec);
    auto& vec=newv.vec().elems;
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
    (++top)[0]=gc.alloc(vm_func);
    nasal_func& func=top[0].func();
    func.entry=imm[pc];
    func.psize=1;

    /* this means you create a new function in local scope */
    if(localr)
    {
        func.upvalue=funcr.func().upvalue;
        nasal_ref upval=(upvalr.type==vm_nil)?gc.alloc(vm_upval):upvalr;
        upval.upval().size=funcr.func().lsize;
        upval.upval().stk=localr;
        func.upvalue.push_back(upval);
        upvalr=upval;
    }
}
inline void nasal_vm::opr_happ()
{
    top[-1].hash().elems[str_table[imm[pc]]]=top[0];
    --top;
}
inline void nasal_vm::opr_para()
{
    nasal_func& func=top[0].func();
    // func->size has 1 place reserved for "me"
    func.keys[str_table[imm[pc]]]=func.psize;
    func.local[func.psize++]={vm_none};
}
inline void nasal_vm::opr_deft()
{
    nasal_ref val=top[0];
    nasal_func& func=(--top)[0].func();
    // func->size has 1 place reserved for "me"
    func.keys[str_table[imm[pc]]]=func.psize;
    func.local[func.psize++]=val;
}
inline void nasal_vm::opr_dyn()
{
    top[0].func().dynpara=imm[pc];
}
inline void nasal_vm::opr_unot()
{
    nasal_ref val=top[0];
    switch(val.type)
    {
        case vm_nil:top[0]=one;break;
        case vm_num:top[0]=val.num()?zero:one;break;
        case vm_str:
        {
            const double num=str2num(val.str().c_str());
            if(std::isnan(num))
                top[0]={vm_num,(double)val.str().empty()};
            else
                top[0]=num?zero:one;
        }break;
        default:die("unot: incorrect value type");break;
    }
}
inline void nasal_vm::opr_usub()
{
    top[0]={vm_num,-top[0].tonum()};
}

#define op_calc(type)\
    top[-1]={vm_num,top[-1].tonum() type top[0].tonum()};\
    --top;

inline void nasal_vm::opr_add(){op_calc(+);}
inline void nasal_vm::opr_sub(){op_calc(-);}
inline void nasal_vm::opr_mul(){op_calc(*);}
inline void nasal_vm::opr_div(){op_calc(/);}
inline void nasal_vm::opr_lnk()
{
    top[-1]=gc.newstr(top[-1].tostr()+top[0].tostr());
    --top;
}

#define op_calc_const(type)\
    top[0]={vm_num,top[0].tonum() type num_table[imm[pc]]};

inline void nasal_vm::opr_addc(){op_calc_const(+);}
inline void nasal_vm::opr_subc(){op_calc_const(-);}
inline void nasal_vm::opr_mulc(){op_calc_const(*);}
inline void nasal_vm::opr_divc(){op_calc_const(/);}
inline void nasal_vm::opr_lnkc()
{
    top[0]=gc.newstr(top[0].tostr()+str_table[imm[pc]]);
}

#define op_calc_eq(type)\
    top[-1]=memr[0]={vm_num,memr[0].tonum() type top[-1].tonum()};\
    memr=nullptr;\
    top-=imm[pc]+1;

inline void nasal_vm::opr_addeq(){op_calc_eq(+);}
inline void nasal_vm::opr_subeq(){op_calc_eq(-);}
inline void nasal_vm::opr_muleq(){op_calc_eq(*);}
inline void nasal_vm::opr_diveq(){op_calc_eq(/);}
inline void nasal_vm::opr_lnkeq()
{
    top[-1]=memr[0]=gc.newstr(memr[0].tostr()+top[-1].tostr());
    memr=nullptr;
    top-=imm[pc]+1;
}

#define op_calc_eq_const(type)\
    top[0]=memr[0]={vm_num,memr[0].tonum() type num_table[imm[pc]&0x7fffffff]};\
    memr=nullptr;\
    top-=(imm[pc]>>31);

inline void nasal_vm::opr_addeqc(){op_calc_eq_const(+);}
inline void nasal_vm::opr_subeqc(){op_calc_eq_const(-);}
inline void nasal_vm::opr_muleqc(){op_calc_eq_const(*);}
inline void nasal_vm::opr_diveqc(){op_calc_eq_const(/);}
inline void nasal_vm::opr_lnkeqc()
{
    top[0]=memr[0]=gc.newstr(memr[0].tostr()+str_table[imm[pc]&0x7fffffff]);
    memr=nullptr;
    top-=(imm[pc]>>31);
}

inline void nasal_vm::opr_meq()
{
    // pop old memr[0] and replace it
    // the reason why we should get memr and push the old value on stack
    // is that when lnkeq/lnkeqc is called, there will be
    // a new gc object vm_str which is returned by gc::alloc
    // this may cause gc, so we should temporarily put it on stack
    memr[0]=top[-1];
    memr=nullptr;
    top-=imm[pc]+1;
}
inline void nasal_vm::opr_eq()
{
    nasal_ref val2=top[0];
    nasal_ref val1=(--top)[0];
    if(val1.type==vm_nil && val2.type==vm_nil)
        top[0]=one;
    else if(val1.type==vm_str && val2.type==vm_str)
        top[0]=(val1.str()==val2.str())?one:zero;
    else if((val1.type==vm_num || val2.type==vm_num)
        && val1.type!=vm_nil && val2.type!=vm_nil)
        top[0]=(val1.tonum()==val2.tonum())?one:zero;
    else
        top[0]=(val1==val2)?one:zero;
}
inline void nasal_vm::opr_neq()
{
    nasal_ref val2=top[0];
    nasal_ref val1=(--top)[0];
    if(val1.type==vm_nil && val2.type==vm_nil)
        top[0]=zero;
    else if(val1.type==vm_str && val2.type==vm_str)
        top[0]=(val1.str()!=val2.str())?one:zero;
    else if((val1.type==vm_num || val2.type==vm_num)
        && val1.type!=vm_nil && val2.type!=vm_nil)
        top[0]=(val1.tonum()!=val2.tonum())?one:zero;
    else
        top[0]=(val1!=val2)?one:zero;
}

#define op_cmp(type)\
    --top;\
    top[0]=(top[0].tonum() type top[1].tonum())?one:zero;

inline void nasal_vm::opr_less(){op_cmp(<);}
inline void nasal_vm::opr_leq(){op_cmp(<=);}
inline void nasal_vm::opr_grt(){op_cmp(>);}
inline void nasal_vm::opr_geq(){op_cmp(>=);}

#define op_cmp_const(type)\
    top[0]=(top[0].tonum() type num_table[imm[pc]])?one:zero;

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
inline void nasal_vm::opr_cnt()
{
    (++top)[0]={vm_cnt,(int64_t)-1};
    if(top[-1].type!=vm_vec)
        die("cnt: must use vector in forindex/foreach");
}
inline void nasal_vm::opr_findex()
{
    if((size_t)(++top[0].cnt())>=top[-1].vec().size())
    {
        pc=imm[pc]-1;
        return;
    }
    top[1]={vm_num,(double)top[0].cnt()};
    ++top;
}
inline void nasal_vm::opr_feach()
{
    std::vector<nasal_ref>& ref=top[-1].vec().elems;
    if((size_t)(++top[0].cnt())>=ref.size())
    {
        pc=imm[pc]-1;
        return;
    }
    top[1]=ref[top[0].cnt()];
    ++top;
}
inline void nasal_vm::opr_callg()
{
    (++top)[0]=stack[imm[pc]];
}
inline void nasal_vm::opr_calll()
{
    (++top)[0]=localr[imm[pc]];
}
inline void nasal_vm::opr_upval()
{
    (++top)[0]=funcr.func().upvalue[(imm[pc]>>16)&0xffff]
                    .upval()[imm[pc]&0xffff];
}
inline void nasal_vm::opr_callv()
{
    nasal_ref val=top[0];
    nasal_ref vec=(--top)[0];
    if(vec.type==vm_vec)
    {
        top[0]=vec.vec().get_val(val.tonum());
        if(top[0].type==vm_none)
            die("callv: index out of range:"+std::to_string(val.tonum()));
    }
    else if(vec.type==vm_hash)
    {
        if(val.type!=vm_str)
            die("callv: must use string as the key");
        top[0]=vec.hash().get_val(val.str());
        if(top[0].type==vm_none)
            die("callv: cannot find member \""+val.str()+"\" of this hash");
        if(top[0].type==vm_func)
            top[0].func().local[0]=val;// 'me'
    }
    else if(vec.type==vm_str)
    {
        std::string& str=vec.str();
        int num=val.tonum();
        int str_size=str.length();
        if(num<-str_size || num>=str_size)
            die("callv: index out of range:"+std::to_string(val.tonum()));
        top[0]={vm_num,double((uint8_t)str[num>=0? num:num+str_size])};
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
    (++top)[0]=val.vec().get_val(imm[pc]);
    if(top[0].type==vm_none)
        die("callvi: index out of range:"+std::to_string(imm[pc]));
}
inline void nasal_vm::opr_callh()
{
    nasal_ref val=top[0];
    if(val.type!=vm_hash)
        die("callh: must call a hash");

    top[0]=val.hash().get_val(str_table[imm[pc]]);
    if(top[0].type==vm_none)
        die("callh: member \""+str_table[imm[pc]]+"\" does not exist");

    if(top[0].type==vm_func)
        top[0].func().local[0]=val;// 'me'
}
inline void nasal_vm::opr_callfv()
{
    uint32_t   argc=imm[pc];    // arguments counter
    nasal_ref* local=top-argc+1;// arguments begin address
    if(local[-1].type!=vm_func)
        die("callfv: must call a function");
    
    auto& func=local[-1].func();
    nasal_ref tmp=local[-1];
    local[-1]=funcr;
    funcr=tmp;
    // top-argc+lsize(local) +1(old pc) +1(old localr) +1(old upvalr)
    if(top-argc+func.lsize+3>=canary)
        die("stack overflow");
    // parameter size is func->psize-1, 1 is reserved for "me"
    uint32_t psize=func.psize-1;
    if(argc<psize && func.local[argc+1].type==vm_none)
        die("callfv: lack argument(s)");

    nasal_ref dynamic=nil;
    top=local+func.lsize;
    if(func.dynpara>=0)// load dynamic arguments
    {
        dynamic=gc.alloc(vm_vec);
        for(uint32_t i=psize;i<argc;++i)
            dynamic.vec().elems.push_back(local[i]);
    }
    uint32_t min_size=std::min(psize,argc);
    for(uint32_t i=min_size;i>=1;--i)// load arguments
        local[i]=local[i-1];
    local[0]=func.local[0];// load "me"
    for(uint32_t i=min_size+1;i<func.lsize;++i)// load local scope & default arguments
        local[i]=func.local[i];
    if(func.dynpara>=0)
        local[psize+1]=dynamic;

    top[0]=upvalr;
    (++top)[0]={vm_addr,localr};
    (++top)[0]={vm_ret,pc};
    pc=func.entry-1;
    localr=local;
    upvalr=nil;
}
inline void nasal_vm::opr_callfh()
{
    auto& hash=top[0].hash().elems;
    if(top[-1].type!=vm_func)
        die("callfh: must call a function");

    auto& func=top[-1].func();
    nasal_ref tmp=top[-1];
    top[-1]=funcr;
    funcr=tmp;
    // top -1(hash) +lsize(local) +1(old pc) +1(old localr) +1(old upvalr)
    if(top+func.lsize+2>=canary)
        die("stack overflow");
    if(func.dynpara>=0)
        die("callfh: special call cannot use dynamic argument");

    nasal_ref* local=top;
    top+=func.lsize;
    for(uint32_t i=0;i<func.lsize;++i)
        local[i]=func.local[i];
    
    for(auto& i:func.keys)
    {
        if(hash.count(i.first))
            local[i.second]=hash[i.first];
        else if(local[i.second].type==vm_none)
            die("callfh: lack argument(s): \""+i.first+"\"");
    }

    top[0]=upvalr;
    (++top)[0]={vm_addr,localr};
    (++top)[0]={vm_ret,pc}; // rewrite top with vm_ret
    pc=func.entry-1;
    localr=local;
    upvalr=nil;
}
inline void nasal_vm::opr_callb()
{
    // reserve place for builtin function return,
    // in fact this code is changed because of coroutine
    (++top)[0]=nil;
    // this ++top should not be used like: (++top)[0] here
    // because if running a builtin function about coroutine
    // (top) will be set to another context.top, instead of main_context.top
    top[0]=(*builtin[imm[pc]].func)(localr,gc);
    if(top[0].type==vm_none)
        die("native function error.");
}
inline void nasal_vm::opr_slcbeg()
{
    // +--------------+
    // | slice_vector | <-- top[0]
    // +--------------+
    // | resource_vec | <-- top[-1]
    // +--------------+
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
    nasal_ref res=top[-1].vec().get_val(val.tonum());
    if(res.type==vm_none)
        die("slc: index out of range:"+std::to_string(val.tonum()));
    top[0].vec().elems.push_back(res);
}
inline void nasal_vm::opr_slc2()
{
    nasal_ref val2=(top--)[0];
    nasal_ref val1=(top--)[0];
    std::vector<nasal_ref>& ref=top[-1].vec().elems;
    std::vector<nasal_ref>& aim=top[0].vec().elems;

    uint8_t type1=val1.type,type2=val2.type;
    int num1=val1.tonum();
    int num2=val2.tonum();
    int size=ref.size();
    if(type1==vm_nil && type2==vm_nil)
    {
        num1=0;
        num2=size-1;
    }
    else if(type1==vm_nil && type2!=vm_nil)
        num1=num2<0? -size:0;
    else if(type1!=vm_nil && type2==vm_nil)
        num2=num1<0? -1:size-1;

    if(num1>num2)
        die("slc2: begin index must be less than or equal to end index");
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
    memr=stack+imm[pc];
    (++top)[0]=memr[0];
    // push value in this memory space on stack
    // to avoid being garbage collected
}
inline void nasal_vm::opr_mcalll()
{
    memr=localr+imm[pc];
    (++top)[0]=memr[0];
    // push value in this memory space on stack
    // to avoid being garbage collected
}
inline void nasal_vm::opr_mupval()
{
    memr=&(funcr.func().upvalue[(imm[pc]>>16)&0xffff].upval()[imm[pc]&0xffff]);
    (++top)[0]=memr[0];
    // push value in this memory space on stack
    // to avoid being garbage collected
}
inline void nasal_vm::opr_mcallv()
{
    nasal_ref val=top[0];     // index
    nasal_ref vec=(--top)[0]; // mcall vector, reserved on stack to avoid gc
    if(vec.type==vm_vec)
    {
        memr=vec.vec().get_mem(val.tonum());
        if(!memr)
            die("mcallv: index out of range:"+std::to_string(val.tonum()));
    }
    else if(vec.type==vm_hash) // do mcallh but use the mcallv way
    {
        if(val.type!=vm_str)
            die("mcallv: must use string as the key");
        nasal_hash& ref=vec.hash();
        std::string& str=val.str();
        memr=ref.get_mem(str);
        if(!memr)
        {
            ref.elems[str]=nil;
            memr=ref.get_mem(str);
        }
    }
    else
        die("mcallv: cannot get memory space in other types");
}
inline void nasal_vm::opr_mcallh()
{
    nasal_ref hash=top[0]; // mcall hash, reserved on stack to avoid gc
    if(hash.type!=vm_hash)
        die("mcallh: must call a hash");
    nasal_hash& ref=hash.hash();
    const std::string& str=str_table[imm[pc]];
    memr=ref.get_mem(str);
    if(!memr) // create a new key
    {
        ref.elems[str]=nil;
        memr=ref.get_mem(str);
    }
}
inline void nasal_vm::opr_ret()
{
/*  +-----------------+
*   | return value    | <- top[0]
*   +-----------------+
*   | old pc          | <- top[-1]
*   +-----------------+
*   | old localr      | <- top[-2]
*   +-----------------+
*   | old upvalr      | <- top[-3]
*   +-----------------+
*   | local scope     |
*   | ...             |
*   +-----------------+ <- local pointer stored in localr
*   | old funcr       | <- old function stored in funcr
*   +-----------------+
*/
    nasal_ref  ret  =top[0];
    nasal_ref* local=localr;
    nasal_ref  func =funcr;
    nasal_ref  up   =upvalr;

    pc    =top[-1].ret();
    localr=top[-2].addr();
    upvalr=top[-3];

    top=local-1;
    funcr=top[0];

    top[0]=ret; // rewrite func with returned value

    if(up.type==vm_upval) // synchronize upvalue
    {
        auto& upval=up.upval();
        auto size=func.func().lsize;
        upval.onstk=false;
        for(uint32_t i=0;i<size;++i)
            upval.elems.push_back(local[i]);
    }
    // cannot use gc.coroutine to judge,
    // because there maybe another function call inside
    if(!pc)
        gc.ctxreserve();
}
void nasal_vm::run(
    const nasal_codegen& gen,
    const nasal_import& linker,
    const std::vector<std::string>& argv,
    const bool opcnt,
    const bool detail)
{
    detail_info=detail;
    init(gen.get_strs(),gen.get_nums(),gen.get_code(),linker.get_file(),argv);
    const void* oprs[]=
    {
        &&vmexit, &&intg,   &&intl,   &&loadg,
        &&loadl,  &&loadu,  &&pnum,   &&pnil,
        &&pstr,   &&newv,   &&newh,   &&newf,
        &&happ,   &&para,   &&deft,   &&dyn,
        &&unot,   &&usub,   &&add,    &&sub,
        &&mul,    &&div,    &&lnk,    &&addc,
        &&subc,   &&mulc,   &&divc,   &&lnkc,
        &&addeq,  &&subeq,  &&muleq,  &&diveq,
        &&lnkeq,  &&addeqc, &&subeqc, &&muleqc,
        &&diveqc, &&lnkeqc, &&meq,    &&eq,
        &&neq,    &&less,   &&leq,    &&grt,
        &&geq,    &&lessc,  &&leqc,   &&grtc,
        &&geqc,   &&pop,    &&jmp,    &&jt,
        &&jf,     &&cnt,    &&findex, &&feach,
        &&callg,  &&calll,  &&upval,  &&callv,
        &&callvi, &&callh,  &&callfv, &&callfh,
        &&callb,  &&slcbeg, &&slcend, &&slc,
        &&slc2,   &&mcallg, &&mcalll, &&mupval,
        &&mcallv, &&mcallh, &&ret
    };
    uint64_t count[op_ret+1]={0};
    std::vector<const void*> code;
    for(auto& i:gen.get_code())
    {
        code.push_back(oprs[i.op]);
        imm.push_back(i.num);
    }
    // goto the first operand
    goto *code[pc];

vmexit:
    if(top>=canary)
        die("stack overflow");
    if(opcnt)
        opcallsort(count);
    if(detail)
        gc.info();
    gc.clear();
    imm.clear();
    return;
// may cause stackoverflow
#define exec_operand(op,num) {\
        op();\
        ++count[num];\
        if(top<canary)\
            goto *code[++pc];\
        goto vmexit;\
    }
// do not cause stackoverflow
#define exec_opnodie(op,num) {op();++count[num];goto *code[++pc];}

intg:   exec_opnodie(opr_intg  ,op_intg  ); // +imm[pc] (detected at codegen)
intl:   exec_opnodie(opr_intl  ,op_intl  ); // -0
loadg:  exec_opnodie(opr_loadg ,op_loadg ); // -1
loadl:  exec_opnodie(opr_loadl ,op_loadl ); // -1
loadu:  exec_opnodie(opr_loadu ,op_loadu ); // -1
pnum:   exec_operand(opr_pnum  ,op_pnum  ); // +1
pnil:   exec_operand(opr_pnil  ,op_pnil  ); // +1
pstr:   exec_operand(opr_pstr  ,op_pstr  ); // +1
newv:   exec_operand(opr_newv  ,op_newv  ); // +1-imm[pc]
newh:   exec_operand(opr_newh  ,op_newh  ); // +1
newf:   exec_operand(opr_newf  ,op_newf  ); // +1
happ:   exec_opnodie(opr_happ  ,op_happ  ); // -1
para:   exec_opnodie(opr_para  ,op_para  ); // -0
deft:   exec_opnodie(opr_deft  ,op_deft  ); // -1
dyn:    exec_opnodie(opr_dyn   ,op_dyn   ); // -0
unot:   exec_opnodie(opr_unot  ,op_unot  ); // -0
usub:   exec_opnodie(opr_usub  ,op_usub  ); // -0
add:    exec_opnodie(opr_add   ,op_add   ); // -1
sub:    exec_opnodie(opr_sub   ,op_sub   ); // -1
mul:    exec_opnodie(opr_mul   ,op_mul   ); // -1
div:    exec_opnodie(opr_div   ,op_div   ); // -1
lnk:    exec_opnodie(opr_lnk   ,op_lnk   ); // -1
addc:   exec_opnodie(opr_addc  ,op_addc  ); // -0
subc:   exec_opnodie(opr_subc  ,op_subc  ); // -0
mulc:   exec_opnodie(opr_mulc  ,op_mulc  ); // -0
divc:   exec_opnodie(opr_divc  ,op_divc  ); // -0
lnkc:   exec_opnodie(opr_lnkc  ,op_lnkc  ); // -0
addeq:  exec_opnodie(opr_addeq ,op_addeq ); // -1
subeq:  exec_opnodie(opr_subeq ,op_subeq ); // -1
muleq:  exec_opnodie(opr_muleq ,op_muleq ); // -1
diveq:  exec_opnodie(opr_diveq ,op_diveq ); // -1
lnkeq:  exec_opnodie(opr_lnkeq ,op_lnkeq ); // -1
addeqc: exec_opnodie(opr_addeqc,op_addeqc); // -0
subeqc: exec_opnodie(opr_subeqc,op_subeqc); // -0
muleqc: exec_opnodie(opr_muleqc,op_muleqc); // -0
diveqc: exec_opnodie(opr_diveqc,op_diveqc); // -0
lnkeqc: exec_opnodie(opr_lnkeqc,op_lnkeqc); // -0
meq:    exec_opnodie(opr_meq   ,op_meq   ); // -1
eq:     exec_opnodie(opr_eq    ,op_eq    ); // -1
neq:    exec_opnodie(opr_neq   ,op_neq   ); // -1
less:   exec_opnodie(opr_less  ,op_less  ); // -1
leq:    exec_opnodie(opr_leq   ,op_leq   ); // -1
grt:    exec_opnodie(opr_grt   ,op_grt   ); // -1
geq:    exec_opnodie(opr_geq   ,op_geq   ); // -1
lessc:  exec_opnodie(opr_lessc ,op_lessc ); // -0
leqc:   exec_opnodie(opr_leqc  ,op_leqc  ); // -0
grtc:   exec_opnodie(opr_grtc  ,op_grtc  ); // -0
geqc:   exec_opnodie(opr_geqc  ,op_geqc  ); // -0
pop:    exec_opnodie(opr_pop   ,op_pop   ); // -1
jmp:    exec_opnodie(opr_jmp   ,op_jmp   ); // -0
jt:     exec_opnodie(opr_jt    ,op_jt    ); // -0
jf:     exec_opnodie(opr_jf    ,op_jf    ); // -1
cnt:    exec_opnodie(opr_cnt   ,op_cnt   ); // -0
findex: exec_operand(opr_findex,op_findex); // +1
feach:  exec_operand(opr_feach ,op_feach ); // +1
callg:  exec_operand(opr_callg ,op_callg ); // +1
calll:  exec_operand(opr_calll ,op_calll ); // +1
upval:  exec_operand(opr_upval ,op_upval ); // +1
callv:  exec_opnodie(opr_callv ,op_callv ); // -0
callvi: exec_opnodie(opr_callvi,op_callvi); // -0
callh:  exec_opnodie(opr_callh ,op_callh ); // -0
callfv: exec_opnodie(opr_callfv,op_callfv); // check in the function
callfh: exec_opnodie(opr_callfh,op_callfh); // check in the function
callb:  exec_opnodie(opr_callb ,op_callb ); // -0
slcbeg: exec_operand(opr_slcbeg,op_slcbeg); // +1
slcend: exec_opnodie(opr_slcend,op_slcend); // -1
slc:    exec_opnodie(opr_slc   ,op_slc   ); // -1
slc2:   exec_opnodie(opr_slc2  ,op_slc2  ); // -2
mcallg: exec_operand(opr_mcallg,op_mcallg); // +1
mcalll: exec_operand(opr_mcalll,op_mcalll); // +1
mupval: exec_operand(opr_mupval,op_mupval); // +1
mcallv: exec_opnodie(opr_mcallv,op_mcallv); // -0
mcallh: exec_opnodie(opr_mcallh,op_mcallh); // -0
ret:    exec_opnodie(opr_ret   ,op_ret   ); // -2
}
#endif