#ifndef __NASAL_VM_H__
#define __NASAL_VM_H__

class nasal_vm
{
private:
    bool main_loop_break_mark;
    int ptr;
    int me_index; // this is the index of "me" in string_table
    nasal_val* global_scope_addr;
    // garbage collector and memory manager
    nasal_gc vm;
    // byte codes store here
    std::vector<opcode> exec_code;
    // main calculation stack
#define val_stack_MAX_DEPTH 8192
    nasal_val** val_stack;
    nasal_val** val_stack_top;
    // stack for mem_call
    std::stack<nasal_val**> ptr_stack;
    // local scope for function block
    std::stack<nasal_val*> local_scope_stack;
    // slice stack for vec[val,val,val:val]
    std::stack<nasal_val*> slice_stack;
    // ptr stack stores address for function to return
    std::stack<int> call_stack;
    // iterator stack for forindex/foreach
    std::stack<int> counter_stack;
    // string table
    std::vector<std::string> string_table;
    // number table
    std::vector<double> number_table;
    // builtin function address table
    std::map<std::string,nasal_val* (*)(nasal_val* x,nasal_gc& vm)> builtin_func_hashmap;
    void die(std::string);
    bool check_condition(nasal_val*);
    void opr_nop();
    void opr_load();
    void opr_pushnum();
    void opr_pushone();
    void opr_pushzero();
    void opr_pushnil();
    void opr_pushstr();
    void opr_newvec();
    void opr_newhash();
    void opr_newfunc();
    void opr_vecapp();
    void opr_hashapp();
    void opr_para();
    void opr_defpara();
    void opr_dynpara();
    void opr_entry();
    void opr_unot();
    void opr_usub();
    void opr_add();
    void opr_sub();
    void opr_mul();
    void opr_div();
    void opr_lnk();
    void opr_addeq();
    void opr_subeq();
    void opr_muleq();
    void opr_diveq();
    void opr_lnkeq();
    void opr_meq();
    void opr_eq();
    void opr_neq();
    void opr_less();
    void opr_leq();
    void opr_grt();
    void opr_geq();
    void opr_pop();
    void opr_jmp();
    void opr_jmptrue();
    void opr_jmpfalse();
    void opr_counter();
    void opr_cntpop();
    void opr_forindex();
    void opr_foreach();
    void opr_call();
    void opr_callv();
    void opr_callvi();
    void opr_callh();
    void opr_callf();
    void opr_builtincall();
    void opr_slicebegin();
    void opr_sliceend();
    void opr_slice();
    void opr_slice2();
    void opr_mcall();
    void opr_mcallv();
    void opr_mcallh();
    void opr_return();
public:
    nasal_vm();
    ~nasal_vm();
    void clear();
    void run(std::vector<std::string>&,std::vector<double>&,std::vector<opcode>&);
};

nasal_vm::nasal_vm()
{
    val_stack=new nasal_val*[val_stack_MAX_DEPTH];
    val_stack_top=val_stack;
    local_scope_stack.push(NULL);
    for(int i=0;builtin_func_table[i].func_pointer;++i)
        builtin_func_hashmap[builtin_func_table[i].func_name]=builtin_func_table[i].func_pointer;
    return;
}
nasal_vm::~nasal_vm()
{
    delete []val_stack;
    return;
}
void nasal_vm::clear()
{
    vm.clear();
    global_scope_addr=NULL;
    val_stack_top=val_stack;
    while(!ptr_stack.empty())ptr_stack.pop();
    while(!local_scope_stack.empty())local_scope_stack.pop();
    local_scope_stack.push(NULL);
    while(!slice_stack.empty())slice_stack.pop();
    while(!call_stack.empty())call_stack.pop();
    while(!counter_stack.empty())counter_stack.pop();
    string_table.clear();
    number_table.clear();
    exec_code.clear();
    return;
}
void nasal_vm::die(std::string str)
{
    printf(">> [vm] 0x%.8x: %s\n",ptr,str.data());
    main_loop_break_mark=false;
    return;
}
bool nasal_vm::check_condition(nasal_val* value_addr)
{
    int type=value_addr->get_type();
    if(type==vm_num)
        return (value_addr->get_number()!=0);
    else if(type==vm_str)
    {
        std::string str=value_addr->get_string();
        double number=trans_string_to_number(str);
        if(std::isnan(number))
            return str.length()!=0;
        return (number!=0);
    }
    return false;
}
void nasal_vm::opr_nop()
{
    // nop is the end of this program
    main_loop_break_mark=false;
    return;
}
void nasal_vm::opr_load()
{
    nasal_val* val_addr=*val_stack_top--;
    nasal_val* local_scope_top=local_scope_stack.top();
    (local_scope_top?local_scope_top:global_scope_addr)->get_closure().add_new_value(exec_code[ptr].index,val_addr);
    return;
}
void nasal_vm::opr_pushnum()
{
    nasal_val* val_addr=vm.gc_alloc(vm_num);
    val_addr->set_number(number_table[exec_code[ptr].index]);
    *(++val_stack_top)=val_addr;
    return;
}
void nasal_vm::opr_pushone()
{
    nasal_val* val_addr=vm.gc_alloc(vm_num);
    val_addr->set_number(1);
    *(++val_stack_top)=val_addr;
    return;
}
void nasal_vm::opr_pushzero()
{
    nasal_val* val_addr=vm.gc_alloc(vm_num);
    val_addr->set_number(0);
    *(++val_stack_top)=val_addr;
    return;
}
void nasal_vm::opr_pushnil()
{
    *(++val_stack_top)=vm.gc_alloc(vm_nil);
    return;
}
void nasal_vm::opr_pushstr()
{
    nasal_val* val_addr=vm.gc_alloc(vm_str);
    val_addr->set_string(string_table[exec_code[ptr].index]);
    *(++val_stack_top)=val_addr;
    return;
}
void nasal_vm::opr_newvec()
{
    *(++val_stack_top)=vm.gc_alloc(vm_vec);
    return;
}
void nasal_vm::opr_newhash()
{
    *(++val_stack_top)=vm.gc_alloc(vm_hash);
    return;
}
void nasal_vm::opr_newfunc()
{
    nasal_val* val_addr=vm.gc_alloc(vm_func);
    nasal_val* local_scope_top=local_scope_stack.top();
    if(local_scope_top)
        val_addr->get_func().set_closure_addr(local_scope_top);
    else
        val_addr->get_func().set_new_closure();
    *(++val_stack_top)=val_addr;
    return;
}
void nasal_vm::opr_vecapp()
{
    nasal_val* val_addr=*val_stack_top--;
    (*val_stack_top)->get_vector().add_elem(val_addr);
    return;
}
void nasal_vm::opr_hashapp()
{
    nasal_val* val_addr=*val_stack_top--;
    (*val_stack_top)->get_hash().add_elem(string_table[exec_code[ptr].index],val_addr);
    return;
}
void nasal_vm::opr_para()
{
    (*val_stack_top)->get_func().add_para(exec_code[ptr].index);
    return;
}
void nasal_vm::opr_defpara()
{
    nasal_val* val_addr=*val_stack_top--;
    (*val_stack_top)->get_func().add_para(exec_code[ptr].index,val_addr);
    return;
}
void nasal_vm::opr_dynpara()
{
    (*val_stack_top)->get_func().add_para(exec_code[ptr].index,NULL,true);
    return;
}
void nasal_vm::opr_entry()
{
    (*val_stack_top)->get_func().set_entry(exec_code[ptr].index);
    return;
}
void nasal_vm::opr_unot()
{
    nasal_val* val_addr=*val_stack_top;
    int type=val_addr->get_type();
    nasal_val* new_value_address=NULL;
    if(type==vm_nil)
    {
        new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(1);
    }
    else if(type==vm_num)
    {
        new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(val_addr->get_number()==0);
    }
    else if(type==vm_str)
    {
        double number=trans_string_to_number(val_addr->get_string());
        if(std::isnan(number))
        {
            new_value_address=vm.gc_alloc(vm_num);
            new_value_address->set_number(!val_addr->get_string().length());
        }
        else
        {
            new_value_address=vm.gc_alloc(vm_num);
            new_value_address->set_number(number==0);
        }
    }
    else
        die("unot: incorrect value type");
    *val_stack_top=new_value_address;
    vm.del_reference(val_addr);
    return;
}
void nasal_vm::opr_usub()
{
    nasal_val* val_addr=*val_stack_top;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(-val_addr->to_number());
    *val_stack_top=new_value_address;
    vm.del_reference(val_addr);
    return;
}
void nasal_vm::opr_add()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()+val_addr2->to_number());
    *val_stack_top=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_sub()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()-val_addr2->to_number());
    *val_stack_top=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_mul()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()*val_addr2->to_number());
    *val_stack_top=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_div()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()/val_addr2->to_number());
    *val_stack_top=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_lnk()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top;
    nasal_val* new_value_address=vm.gc_alloc(vm_str);
    new_value_address->set_string(val_addr1->to_string()+val_addr2->to_string());
    *val_stack_top=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_addeq()
{
    nasal_val** mem_addr=ptr_stack.top();
    ptr_stack.pop();
    nasal_val* val_addr2=*val_stack_top;
    nasal_val* val_addr1=*mem_addr;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()+val_addr2->to_number());
    vm.add_reference(new_value_address);
    *val_stack_top=new_value_address;
    vm.del_reference(*mem_addr);
    *mem_addr=new_value_address;
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_subeq()
{
    nasal_val** mem_addr=ptr_stack.top();
    ptr_stack.pop();
    nasal_val* val_addr2=*val_stack_top;
    nasal_val* val_addr1=*mem_addr;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()-val_addr2->to_number());
    vm.add_reference(new_value_address);
    *val_stack_top=new_value_address;
    vm.del_reference(*mem_addr);
    *mem_addr=new_value_address;
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_muleq()
{
    nasal_val** mem_addr=ptr_stack.top();
    ptr_stack.pop();
    nasal_val* val_addr2=*val_stack_top;
    nasal_val* val_addr1=*mem_addr;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()*val_addr2->to_number());
    vm.add_reference(new_value_address);
    *val_stack_top=new_value_address;
    vm.del_reference(*mem_addr);
    *mem_addr=new_value_address;
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_diveq()
{
    nasal_val** mem_addr=ptr_stack.top();
    ptr_stack.pop();
    nasal_val* val_addr2=*val_stack_top;
    nasal_val* val_addr1=*mem_addr;
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()/val_addr2->to_number());
    vm.add_reference(new_value_address);
    *val_stack_top=new_value_address;
    vm.del_reference(*mem_addr);
    *mem_addr=new_value_address;
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_lnkeq()
{
    nasal_val** mem_addr=ptr_stack.top();
    ptr_stack.pop();
    nasal_val* val_addr2=*val_stack_top;
    nasal_val* val_addr1=*mem_addr;
    nasal_val* new_value_address=vm.gc_alloc(vm_str);
    new_value_address->set_string(val_addr1->to_string()+val_addr2->to_string());
    vm.add_reference(new_value_address);
    *val_stack_top=new_value_address;
    vm.del_reference(*mem_addr);
    *mem_addr=new_value_address;
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_meq()
{
    nasal_val** mem_addr=ptr_stack.top();
    ptr_stack.pop();
    nasal_val* val_addr=*val_stack_top;
    vm.add_reference(val_addr);
    vm.del_reference(*mem_addr);
    *mem_addr=val_addr;
    return;
}
void nasal_vm::opr_eq()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top--;
    int a_ref_type=val_addr1->get_type();
    int b_ref_type=val_addr2->get_type();
    if(a_ref_type==vm_nil && b_ref_type==vm_nil)
    {
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(1);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    else if((a_ref_type==vm_num || a_ref_type==vm_str) && (b_ref_type==vm_num || b_ref_type==vm_str))
    {
        if(a_ref_type==vm_str && b_ref_type==vm_str)
        {
            std::string astr=val_addr1->get_string();
            std::string bstr=val_addr2->get_string();
            nasal_val* new_value_address=vm.gc_alloc(vm_num);
            new_value_address->set_number((double)(astr==bstr));
            *(++val_stack_top)=new_value_address;
            vm.del_reference(val_addr1);
            vm.del_reference(val_addr2);
            return;
        }
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number((double)(val_addr1->to_number()==val_addr2->to_number()));
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    else
    {
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(val_addr1==val_addr2);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    return;
}
void nasal_vm::opr_neq()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top--;
    int a_ref_type=val_addr1->get_type();
    int b_ref_type=val_addr2->get_type();
    if(a_ref_type==vm_nil && b_ref_type==vm_nil)
    {
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(0);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    else if((a_ref_type==vm_num || a_ref_type==vm_str) && (b_ref_type==vm_num || b_ref_type==vm_str))
    {
        if(a_ref_type==vm_str && b_ref_type==vm_str)
        {
            std::string astr=val_addr1->get_string();
            std::string bstr=val_addr2->get_string();
            nasal_val* new_value_address=vm.gc_alloc(vm_num);
            new_value_address->set_number((double)(astr!=bstr));
            *(++val_stack_top)=new_value_address;
            vm.del_reference(val_addr1);
            vm.del_reference(val_addr2);
            return;
        }
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number((double)(val_addr1->to_number()!=val_addr2->to_number()));
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    else
    {
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(val_addr1!=val_addr2);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_less()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top--;
    int a_ref_type=val_addr1->get_type();
    int b_ref_type=val_addr2->get_type();
    if(a_ref_type==vm_str && b_ref_type==vm_str)
    {
        std::string a_str=val_addr1->get_string();
        std::string b_str=val_addr2->get_string();
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(a_str<b_str);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()<val_addr2->to_number());
    *(++val_stack_top)=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_leq()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top--;
    int a_ref_type=val_addr1->get_type();
    int b_ref_type=val_addr2->get_type();
    if(a_ref_type==vm_str && b_ref_type==vm_str)
    {
        std::string a_str=val_addr1->get_string();
        std::string b_str=val_addr2->get_string();
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(a_str<=b_str);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()<=val_addr2->to_number());
    *(++val_stack_top)=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_grt()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top--;
    int a_ref_type=val_addr1->get_type();
    int b_ref_type=val_addr2->get_type();
    if(a_ref_type==vm_str && b_ref_type==vm_str)
    {
        std::string a_str=val_addr1->get_string();
        std::string b_str=val_addr2->get_string();
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(a_str>b_str);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()>val_addr2->to_number());
    *(++val_stack_top)=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_geq()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top--;
    int a_ref_type=val_addr1->get_type();
    int b_ref_type=val_addr2->get_type();
    if(a_ref_type==vm_str && b_ref_type==vm_str)
    {
        std::string a_str=val_addr1->get_string();
        std::string b_str=val_addr2->get_string();
        nasal_val* new_value_address=vm.gc_alloc(vm_num);
        new_value_address->set_number(a_str>=b_str);
        *(++val_stack_top)=new_value_address;
        vm.del_reference(val_addr1);
        vm.del_reference(val_addr2);
        return;
    }
    nasal_val* new_value_address=vm.gc_alloc(vm_num);
    new_value_address->set_number(val_addr1->to_number()>=val_addr2->to_number());
    *(++val_stack_top)=new_value_address;
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_pop()
{
    vm.del_reference(*val_stack_top--);
    return;
}
void nasal_vm::opr_jmp()
{
    ptr=exec_code[ptr].index-1;
    return;
}
void nasal_vm::opr_jmptrue()
{
    if(check_condition(*val_stack_top))
        ptr=exec_code[ptr].index-1;
    return;
}
void nasal_vm::opr_jmpfalse()
{
    if(!check_condition(*val_stack_top))
        ptr=exec_code[ptr].index-1;
    vm.del_reference(*val_stack_top--);
    return;
}
void nasal_vm::opr_counter()
{
    if((*val_stack_top)->get_type()!=vm_vec)
    {
        die("cnt: must use vector in forindex/foreach");
        return;
    }
    counter_stack.push(-1);
    return;
}
void nasal_vm::opr_cntpop()
{
    counter_stack.pop();
    return;
}
void nasal_vm::opr_forindex()
{
    nasal_vec& ref=(*val_stack_top)->get_vector();
    ++counter_stack.top();
    if(counter_stack.top()>=ref.size())
    {
        ptr=exec_code[ptr].index-1;
        return;
    }
    nasal_val* res=vm.gc_alloc(vm_num);
    res->set_number((double)counter_stack.top());
    *(++val_stack_top)=res;
    return;
}
void nasal_vm::opr_foreach()
{
    nasal_vec& ref=(*val_stack_top)->get_vector();
    ++counter_stack.top();
    if(counter_stack.top()>=ref.size())
    {
        ptr=exec_code[ptr].index-1;
        return;
    }
    nasal_val* res=ref[counter_stack.top()];
    vm.add_reference(res);
    *(++val_stack_top)=res;
    return;
}
void nasal_vm::opr_call()
{
    nasal_val* val_addr=NULL;
    int name_index=exec_code[ptr].index;
    nasal_val* local_scope_top=local_scope_stack.top();
    if(local_scope_top)
        val_addr=local_scope_top->get_closure().get_value_address(name_index);
    if(val_addr)
    {
        vm.add_reference(val_addr);
        *(++val_stack_top)=val_addr;
        return;
    }
    else
        val_addr=global_scope_addr->get_closure().get_value_address(name_index);
    if(val_addr)
    {
        vm.add_reference(val_addr);
        *(++val_stack_top)=val_addr;
        return;
    }
    die("call: cannot find symbol named \""+string_table[name_index]+"\"");
    return;
}
void nasal_vm::opr_callv()
{
    nasal_val* val_addr=*val_stack_top--;
    nasal_val* vec_addr=*val_stack_top--;
    int type=vec_addr->get_type();
    if(type==vm_vec)
    {
        int num=val_addr->to_number();
        nasal_val* res=vec_addr->get_vector().get_value_address(num);
        if(res)
        {
            vm.add_reference(res);
            *(++val_stack_top)=res;
        }
        else
        {
            die("callv: index out of range");
            return;
        }
    }
    else if(type==vm_hash)
    {
        if(val_addr->get_type()!=vm_str)
        {
            die("callv: must use string as the key");
            return;
        }
        nasal_val* res=vec_addr->get_hash().get_value_address(val_addr->get_string());
        if(!res)
        {
            die("callv: cannot find member \""+val_addr->get_string()+"\" of this hash");
            return;
        }
        if(res->get_type()==vm_func)
        {
            (res->get_func().get_closure_addr())->get_closure().add_new_value(me_index,val_addr);
            vm.add_reference(val_addr);
        }
        vm.add_reference(res);
        *(++val_stack_top)=res;
    }
    else if(type==vm_str)
    {
        std::string str=vec_addr->get_string();
        int num=val_addr->to_number();
        int str_size=str.length();
        if(num<-str_size || num>=str_size)
        {
            die("callv: index out of range");
            return;
        }
        nasal_val* res=vm.gc_alloc(vm_num);
        res->set_number((double)str[num>=0? num:num+str_size]);
        *(++val_stack_top)=res;
    }
    else
        die("callv: must call a vector/hash/string");
    vm.del_reference(val_addr);
    vm.del_reference(vec_addr);
    return;
}
void nasal_vm::opr_callvi()
{
    nasal_val* val_addr=*val_stack_top;
    if(val_addr->get_type()!=vm_vec)
    {
        die("callvi: multi-definition/multi-assignment must use a vector");
        return;
    }
    // cannot use operator[],because this may cause overflow
    nasal_val* res=val_addr->get_vector().get_value_address(exec_code[ptr].index);
    if(!res)
    {
        die("callvi: index out of range");
        return;
    }
    vm.add_reference(res);
    *(++val_stack_top)=res;
    return;
}
void nasal_vm::opr_callh()
{
    nasal_val* val_addr=*val_stack_top--;
    if(val_addr->get_type()!=vm_hash)
    {
        die("callh: must call a hash");
        return;
    }
    nasal_val* res=val_addr->get_hash().get_value_address(string_table[exec_code[ptr].index]);
    if(res)
        vm.add_reference(res);
    else
    {
        die("callh: hash member \""+string_table[exec_code[ptr].index]+"\" does not exist");
        return;
    }
    if(res->get_type()==vm_func)
        (res->get_func().get_closure_addr())->get_closure().add_new_value(me_index,val_addr);
    else
        vm.del_reference(val_addr);
    *(++val_stack_top)=res;
    return;
}
void nasal_vm::opr_callf()
{
    nasal_val* para_addr=*val_stack_top--;
    nasal_val* func_addr=*val_stack_top;
    if(func_addr->get_type()!=vm_func)
    {
        die("callf: called a value that is not a function");
        return;
    }
    nasal_func& ref=func_addr->get_func();
    nasal_val* closure=ref.get_closure_addr();
    nasal_scop& ref_closure=closure->get_closure();
    ref_closure.add_scope();
    local_scope_stack.push(closure);
    vm.add_reference(closure);
    if(para_addr->get_type()==vm_vec)
    {
        nasal_vec& ref_vec=para_addr->get_vector();
        std::vector<int>& ref_para=ref.get_para();
        std::vector<nasal_val*>& ref_default=ref.get_default();
        int i=0;
        for(;i<ref_para.size();++i)
        {
            if(i>=ref_vec.size())
            {
                if(!ref_default[i])
                {
                    die("callf: lack argument(s)");
                    return;
                }
                ref_closure.add_new_value(ref_para[i],ref_default[i]);
                vm.add_reference(ref_default[i]);
            }
            else
            {
                nasal_val* tmp=ref_vec[i];
                ref_closure.add_new_value(ref_para[i],tmp);
                vm.add_reference(tmp);
            }
        }
        if(ref.get_dynamic_para()>=0)
        {
            nasal_val* vec_addr=vm.gc_alloc(vm_vec);
            for(;i<ref_vec.size();++i)
            {
                nasal_val* tmp=ref_vec[i];
                vec_addr->get_vector().add_elem(tmp);
                vm.add_reference(tmp);
            }
            ref_closure.add_new_value(ref.get_dynamic_para(),vec_addr);
        }
    }
    else
    {
        nasal_hash& ref_hash=para_addr->get_hash();
        std::vector<int>& ref_para=ref.get_para();
        std::vector<nasal_val*>& ref_default=ref.get_default();
        if(ref.get_dynamic_para()>=0)
        {
            die("callf: special call cannot use dynamic parameter");
            return;
        }
        for(int i=0;i<ref_para.size();++i)
        {
            nasal_val* tmp=ref_hash.get_special_para(string_table[ref_para[i]]);
            if(!tmp)
                tmp=ref_default[i];
            if(!tmp)
            {
                die("callf: lack argument(s)");
                return;
            }
            ref_closure.add_new_value(ref_para[i],tmp);
            vm.add_reference(tmp);
        }
    }
    vm.del_reference(para_addr);
    call_stack.push(ptr);
    ptr=ref.get_entry()-1;
    return;
}
void nasal_vm::opr_builtincall()
{
    nasal_val* ret_value_addr=NULL;
    std::string val_name=string_table[exec_code[ptr].index];
    ret_value_addr=(*builtin_func_hashmap[val_name])(local_scope_stack.top(),vm);
    main_loop_break_mark=!builtin_die_state;
    *(++val_stack_top)=ret_value_addr;
    return;
}
void nasal_vm::opr_slicebegin()
{
    slice_stack.push(vm.gc_alloc(vm_vec));
    if((*val_stack_top)->get_type()==vm_vec)
        return;
    die("slcbegin: must slice a vector");
    return;
}
void nasal_vm::opr_sliceend()
{
    vm.del_reference(*val_stack_top);
    *val_stack_top=slice_stack.top();
    slice_stack.pop();
    return;
}
void nasal_vm::opr_slice()
{
    nasal_val* val_addr=*val_stack_top--;
    int type=val_addr->get_type();
    double num;
    switch(type)
    {
        case vm_num:num=val_addr->get_number();break;
        case vm_str:num=trans_string_to_number(val_addr->get_string());break;
        default:die("slc: error value type");break;
    }
    nasal_val* res=(*val_stack_top)->get_vector().get_value_address((int)num);
    if(res)
    {
        vm.add_reference(res);
        (slice_stack.top())->get_vector().add_elem(res);
        vm.del_reference(val_addr);
        return;
    }
    die("slc: index out of range");
    return;
}
void nasal_vm::opr_slice2()
{
    nasal_val* val_addr2=*val_stack_top--;
    nasal_val* val_addr1=*val_stack_top--;
    nasal_vec& ref=(*val_stack_top)->get_vector();
    nasal_vec& aim=(slice_stack.top())->get_vector();

    int type1=val_addr1->get_type();
    int num1;
    switch(type1)
    {
        case vm_nil:break;
        case vm_num:num1=(int)val_addr1->get_number();break;
        case vm_str:num1=(int)trans_string_to_number(val_addr1->get_string());break;
        default:die("slc2: error value type");break;
    }
    int type2=val_addr2->get_type();
    int num2;
    switch(type2)
    {
        case vm_nil:break;
        case vm_num:num2=(int)val_addr2->get_number();break;
        case vm_str:num2=(int)trans_string_to_number(val_addr2->get_string());break;
        default:die("slc2: error value type");break;
    }
    int ref_size=ref.size();
    if(type1==vm_nil && type2==vm_nil)
    {
        num1=0;
        num2=ref_size-1;
    }
    else if(type1==vm_nil && type2!=vm_nil)
        num1=num2<0? -ref_size:0;
    else if(type1!=vm_nil && type2==vm_nil)
        num2=num1<0? -1:ref_size-1;
    if(num1>=num2)
    {
        die("slc2: begin index must be less than end index");
        return;
    }
    if(num1<-ref_size || num1>=ref_size || num2<-ref_size || num2>=ref_size)
    {
        die("slc2: begin or end index out of range");
        return;
    }
    for(int i=num1;i<=num2;++i)
    {
        nasal_val* tmp=ref.get_value_address(i);
        vm.add_reference(tmp);
        aim.add_elem(tmp);
    }
    vm.del_reference(val_addr1);
    vm.del_reference(val_addr2);
    return;
}
void nasal_vm::opr_mcall()
{
    nasal_val** mem_addr=NULL;
    int name_index=exec_code[ptr].index;
    nasal_val* local_scope_top=local_scope_stack.top();
    if(local_scope_top)
        mem_addr=local_scope_top->get_closure().get_mem_address(name_index);
    if(mem_addr)
    {
        ptr_stack.push(mem_addr);
        return;
    }
    else
        mem_addr=global_scope_addr->get_closure().get_mem_address(name_index);
    if(mem_addr)
    {
        ptr_stack.push(mem_addr);
        return;
    }
    die("mcall: cannot find symbol named \""+string_table[name_index]+"\"");
    return;
}
void nasal_vm::opr_mcallv()
{
    nasal_val* val_addr=*val_stack_top--;
    nasal_val** vec_addr=ptr_stack.top();
    ptr_stack.pop();
    int type=(*vec_addr)->get_type();
    if(type==vm_vec)
    {
        int num;
        switch(val_addr->get_type())
        {
            case vm_num:num=(int)val_addr->get_number();break;
            case vm_str:num=(int)trans_string_to_number(val_addr->get_string());break;
            default:die("mcallv: error value type");break;
        }
        nasal_val** res=(*vec_addr)->get_vector().get_mem_address(num);
        if(!res)
        {
            die("mcallv: index out of range");
            return;
        }
        ptr_stack.push(res);
    }
    else if(type==vm_hash)
    {
        if(val_addr->get_type()!=vm_str)
        {
            die("mcallv: must use string as the key");
            return;
        }
        nasal_hash& ref=(*vec_addr)->get_hash();
        std::string str=val_addr->get_string();
        nasal_val** res=ref.get_mem_address(str);
        if(!res)
        {
            ref.add_elem(str,vm.gc_alloc(vm_nil));
            res=ref.get_mem_address(str);
        }
        ptr_stack.push(res);
    }
    else
    {
        die("mcallv: cannot get memory space in a string");
        return;
    }
    vm.del_reference(val_addr);
    return;
}
void nasal_vm::opr_mcallh()
{
    nasal_val** mem_addr=NULL;
    nasal_val** hash_addr=ptr_stack.top();
    ptr_stack.pop();
    if((*hash_addr)->get_type()!=vm_hash)
    {
        die("mcallh: must call a hash");
        return;
    }
    nasal_hash& ref=(*hash_addr)->get_hash();
    std::string str=string_table[exec_code[ptr].index];
    mem_addr=ref.get_mem_address(str);
    if(!mem_addr)
    {
        ref.add_elem(str,vm.gc_alloc(vm_nil));
        mem_addr=ref.get_mem_address(str);
    }
    ptr_stack.push(mem_addr);
    return;
}
void nasal_vm::opr_return()
{
    nasal_val* closure_addr=local_scope_stack.top();
    local_scope_stack.pop();
    closure_addr->get_closure().del_scope();
    vm.del_reference(closure_addr);
    ptr=call_stack.top();
    call_stack.pop();
    nasal_val* tmp=*val_stack_top--;
    // delete function
    vm.del_reference(*val_stack_top);
    *val_stack_top=tmp;
    return;
}
void nasal_vm::run(std::vector<std::string>& strs,std::vector<double>& nums,std::vector<opcode>& exec)
{
    me_index=-1;
    string_table=strs;
    builtin_use_string_table.clear();
    for(int i=0;i<string_table.size();++i)
    {
        if(string_table[i]=="me")
            me_index=i;
        builtin_use_string_table[string_table[i]]=i;
    }
    number_table=nums;
    exec_code=exec;

    static void (nasal_vm::*opr_table[])()=
    {
        &nasal_vm::opr_nop,
        &nasal_vm::opr_load,
        &nasal_vm::opr_pushnum,
        &nasal_vm::opr_pushone,
        &nasal_vm::opr_pushzero,
        &nasal_vm::opr_pushnil,
        &nasal_vm::opr_pushstr,
        &nasal_vm::opr_newvec,
        &nasal_vm::opr_newhash,
        &nasal_vm::opr_newfunc,
        &nasal_vm::opr_vecapp,
        &nasal_vm::opr_hashapp,
        &nasal_vm::opr_para,
        &nasal_vm::opr_defpara,
        &nasal_vm::opr_dynpara,
        &nasal_vm::opr_entry,
        &nasal_vm::opr_unot,
        &nasal_vm::opr_usub,
        &nasal_vm::opr_add,
        &nasal_vm::opr_sub,
        &nasal_vm::opr_mul,
        &nasal_vm::opr_div,
        &nasal_vm::opr_lnk,
        &nasal_vm::opr_addeq,
        &nasal_vm::opr_subeq,
        &nasal_vm::opr_muleq,
        &nasal_vm::opr_diveq,
        &nasal_vm::opr_lnkeq,
        &nasal_vm::opr_meq,
        &nasal_vm::opr_eq,
        &nasal_vm::opr_neq,
        &nasal_vm::opr_less,
        &nasal_vm::opr_leq,
        &nasal_vm::opr_grt,
        &nasal_vm::opr_geq,
        &nasal_vm::opr_pop,
        &nasal_vm::opr_jmp,
        &nasal_vm::opr_jmptrue,
        &nasal_vm::opr_jmpfalse,
        &nasal_vm::opr_counter,
        &nasal_vm::opr_cntpop,
        &nasal_vm::opr_forindex,
        &nasal_vm::opr_foreach,
        &nasal_vm::opr_call,
        &nasal_vm::opr_callv,
        &nasal_vm::opr_callvi,
        &nasal_vm::opr_callh,
        &nasal_vm::opr_callf,
        &nasal_vm::opr_builtincall,
        &nasal_vm::opr_slicebegin,
        &nasal_vm::opr_sliceend,
        &nasal_vm::opr_slice,
        &nasal_vm::opr_slice2,
        &nasal_vm::opr_mcall,
        &nasal_vm::opr_mcallv,
        &nasal_vm::opr_mcallh,
        &nasal_vm::opr_return
    };

    main_loop_break_mark=true;
    builtin_die_state=false;

    global_scope_addr=vm.gc_alloc(vm_scop);
    clock_t begin_time=clock();

    // main loop
    for(ptr=0;main_loop_break_mark;++ptr)
        (this->*opr_table[exec_code[ptr].op])();
    
    float total_run_time=((double)(clock()-begin_time))/CLOCKS_PER_SEC;
    std::cout<<">> [vm] process exited after "<<total_run_time<<"s.\n";
    vm.del_reference(global_scope_addr);

    clear();
    return;
}
#endif