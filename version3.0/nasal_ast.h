#ifndef __NASAL_AST_H__
#define __NASAL_AST_H__

class nasal_ast
{
private:
    int line;
    int type;
    std::string str;
    std::vector<nasal_ast> children;
public:
    nasal_ast();
    nasal_ast(const nasal_ast&);
    ~nasal_ast();
    nasal_ast& operator=(const nasal_ast&);
    void clear();
    void set_line(int);
    void set_type(int);
    void set_str(std::string&);
    void add_child(nasal_ast);
    int  get_line();
    int  get_type();
    std::string get_str();
    std::vector<nasal_ast>& get_children();
    void print_ast(int);
};

nasal_ast::nasal_ast()
{
    this->line=0;
    this->type=ast_null;
    this->str="";
    return;
}

nasal_ast::nasal_ast(const nasal_ast& tmp)
{
    this->line=tmp.line;
    this->type=tmp.type;
    this->str=tmp.str;
    this->children=tmp.children;
    return;
}

nasal_ast::~nasal_ast()
{
    this->str.clear();
    this->children.clear();
    return;
}

nasal_ast& nasal_ast::operator=(const nasal_ast& tmp)
{
    this->line=tmp.line;
    this->type=tmp.type;
    this->str=tmp.str;
    this->children=tmp.children;
    return *this;
}

void nasal_ast::clear()
{
    this->line=0;
    this->type=ast_null;
    this->str="";
    this->children.clear();
    return;
}

void nasal_ast::set_line(int l)
{
    this->line=l;
    return;
}

void nasal_ast::set_type(int t)
{
    this->type=t;
    return;
}

void nasal_ast::set_str(std::string& s)
{
    this->str=s;
    return;
}

void nasal_ast::add_child(nasal_ast ast)
{
    children.push_back(ast);
    return;
}

int nasal_ast::get_line()
{
    return this->line;
}

int nasal_ast::get_type()
{
    return this->type;
}

std::string nasal_ast::get_str()
{
    return this->str;
}

std::vector<nasal_ast>& nasal_ast::get_children()
{
    return this->children;
}

void nasal_ast::print_ast(int depth)
{
    std::string indentation="";
    for(int i=0;i<depth;++i) indentation+="|  ";
    indentation+=ast_str(this->type);
    std::cout<<indentation;
    switch(this->type)
    {
        case ast_number:    std::cout<<":"<<this->str;break;
        case ast_string:    std::cout<<":"<<this->str;break;
        case ast_identifier:std::cout<<":"<<this->str;break;
        case ast_call_hash: std::cout<<":"<<this->str;break;
    }
    std::cout<<std::endl;
    int child_size=this->children.size();
    for(int i=0;i<child_size;++i)
        this->children[i].print_ast(depth+1);
    return;
}

#endif