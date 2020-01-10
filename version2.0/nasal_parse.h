#ifndef __NASAL_PARSE_H__
#define __NASAL_PARSE_H__

class nasal_parse
{
	private:
		std::stack<token> parse_token_stream;
		std::stack<token> checked_tokens;
		token this_token;
		int error;
		int warning;
		abstract_syntax_tree root;
	public:
		// basic
		void print_detail_token();
		void get_token_list(std::list<token>&);
		void get_token();
		void push_token();
		int get_error();
		abstract_syntax_tree& get_root();
		
		// abstract_syntax_tree generation
		void main_generate();
		abstract_syntax_tree number_expr();
		abstract_syntax_tree string_expr();
		abstract_syntax_tree nil_expr();
		abstract_syntax_tree var_outside_definition();
		abstract_syntax_tree loop_expr();
		abstract_syntax_tree choose_expr();
};

void nasal_parse::print_detail_token()
{
	std::stack<token> tmp=parse_token_stream;
	std::string space="";
	int line=1;
	std::cout<<line<<"\t";
	while(!tmp.empty())
	{
		if(tmp.top().line!=line)
		{
			for(int i=line+1;i<tmp.top().line;++i)
				std::cout<<std::endl<<i<<"\t";
			line=tmp.top().line;
			std::cout<<std::endl<<line<<"\t"<<space;
		}
		print_parse_token(tmp.top().type);
		if(tmp.top().type==__left_brace)
			space+=' ';
		std::cout<<" ";
		tmp.pop();
		if(!tmp.empty() && tmp.top().type==__right_brace)
		{
			std::string str="";
			for(int i=0;i<space.length()-1;++i)
				str+=space[i];
			space=str;
		}
	}
	std::cout<<std::endl;
	return;
}

void nasal_parse::get_token_list(std::list<token>& detail_token_stream)
{
	while(!parse_token_stream.empty())
		parse_token_stream.pop();
	while(!checked_tokens.empty())
		checked_tokens.pop();
	// clear stack
	std::stack<token> backward_tmp;
	for(std::list<token>::iterator i=detail_token_stream.begin();i!=detail_token_stream.end();++i)
		backward_tmp.push(*i);
	while(!backward_tmp.empty())
	{
		parse_token_stream.push(backward_tmp.top());
		backward_tmp.pop();
	}
	return;
}

void nasal_parse::get_token()
{
	if(!parse_token_stream.empty())
	{
		this_token=parse_token_stream.top();
		parse_token_stream.pop();
		checked_tokens.push(this_token);
	}
	else
	{
		this_token.type=__stack_end;
		this_token.str="__stack_end";
		std::cout<<">>[Parse-error] fatal error occurred."<<std::endl;
		std::cout<<">>[Parse-error] empty token stack."<<std::endl;
	}
	return;
}

void nasal_parse::push_token()
{
	if(!checked_tokens.empty())
	{
		parse_token_stream.push(checked_tokens.top());
		this_token=checked_tokens.top();
		checked_tokens.pop();
	}
	else
	{
		std::cout<<">>[Parse-error] fatal error occurred."<<std::endl;
		std::cout<<">>[Parse-error] empty checked-token stack."<<std::endl;
	}
	return;
}

int nasal_parse::get_error()
{
	return error;
}

abstract_syntax_tree& nasal_parse::get_root()
{
	std::cout<<">>[Abstract-syntax-tree] get root address: "<<(&root)<<" ."<<std::endl;
	return root;
}

void nasal_parse::main_generate()
{
	error=0;
	warning=0;
	root.set_clear();
	root.set_line(1);
	root.set_type(__root);
	
	while(!parse_token_stream.empty())
	{
		this->get_token();
		switch(this_token.type)
		{
			case __var:this->push_token();root.get_children().push_back(var_outside_definition());break;
			case __nor_operator:
			case __sub_operator:
			case __number:
			case __string:
			case __nil:
			case __id:break;
			case __left_curve:break;
			case __left_bracket:break;
			case __left_brace:break;
			case __func:break;
			case __if:break;
			case __while:
			case __for:
			case __foreach:
			case __forindex:this->push_token();root.get_children().push_back(loop_expr());break;
			case __semi:break;
			default:++error;print_parse_error(error_token_in_main,this_token.line,this_token.type);
		}
	}
	std::cout<<">>[Parse] complete generation. "<<error<<" error(s), "<<warning<<" warning(s)."<<std::endl;
	return;
}

abstract_syntax_tree nasal_parse::number_expr()
{
	abstract_syntax_tree node;
	this->get_token();
	node.set_line(this_token.line);
	node.set_type(__number);
	node.set_number(this_token.str);
	return node;
}

abstract_syntax_tree nasal_parse::string_expr()
{
	abstract_syntax_tree node;
	this->get_token();
	node.set_line(this_token.line);
	node.set_type(__string);
	node.set_string(this_token.str);
	return node;
}

abstract_syntax_tree nasal_parse::nil_expr()
{
	abstract_syntax_tree node;
	this->get_token();
	node.set_line(this_token.line);
	node.set_type(__nil);
	return node;
}

abstract_syntax_tree nasal_parse::var_outside_definition()
{
	abstract_syntax_tree definition_node;
	definition_node.set_type(__definition);
	this->get_token();
	definition_node.set_line(this_token.line);
	if(this_token.type!=__var)
	{
		++error;
		print_parse_error(definition_lack_var,this_token.line);
	}
	this->get_token();
	if(this_token.type==__id)
	{
	}
	else if(this_token.type==__left_curve)
	{
	}
	else
	{
		++error;
		print_parse_error(definition_lack_id,this_token.line);
	}
	return definition_node;
}

abstract_syntax_tree nasal_parse::loop_expr()
{
	abstract_syntax_tree loop_main_node;
	loop_main_node.set_type(__loop);
	this->get_token();
	loop_main_node.set_line(this_token.line);
	switch(this_token.type)
	{
		case __for:
		case __while:
		case __foreach:
		case __forindex:break;
	}
	return loop_main_node;
}

abstract_syntax_tree nasal_parse::choose_expr()
{
	abstract_syntax_tree choose_main_node;
	choose_main_node.set_type(__ifelse);
	this->get_token();
	choose_main_node.set_line(this_token.line);
	return choose_main_node;
}
#endif