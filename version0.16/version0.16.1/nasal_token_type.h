#ifndef __NASAL_TOKEN_TYPE_H__
#define __NASAL_TOKEN_TYPE_H__

#include <cstring>

enum token_type
{
	__stack_end=1,
	__equal,                                       // =
	__cmp_equal,__cmp_not_equal,                   // == !=
	__cmp_less,__cmp_less_or_equal,                // < <=
	__cmp_more,__cmp_more_or_equal,                // > >=
	__and_operator,__or_operator,__nor_operator,   // and or !
	__add_operator,__sub_operator,                 // + -
	__mul_operator,__div_operator,__link_operator, // * / ~
	__add_equal,__sub_equal,                       // += -=
	__mul_equal,__div_equal,__link_equal,          // *= /= ~=
	__left_brace,__right_brace,                    // {}
	__left_bracket,__right_bracket,                // []
	__left_curve,__right_curve,                    // ()
	__semi,__comma,__colon,__dot,                  // ; , : .
	__var,__func,__return,
	__if,__elsif,__else,
	__continue,__break,
	__for,__forindex,__foreach,__while,
	//operators & reserve words
	
	__number,__string,__id,__dynamic_id,
	//basic elements
	
	__root,__operator,
};

void print_token(int type)
{
	std::string context="";
	switch(type)
	{
		case __stack_end:         context="#";break;
		case __equal:             context="=";break;
		case __cmp_equal:         context="==";break;
		case __cmp_not_equal:     context="!=";break;
		case __cmp_less:          context="<";break;
		case __cmp_less_or_equal: context="<=";break;
		case __cmp_more:          context=">";break;
		case __cmp_more_or_equal: context=">=";break;
		case __and_operator:      context="and";break;
		case __or_operator:       context="or";break;
		case __nor_operator:      context="!";break;
		case __add_operator:      context="+";break;
		case __sub_operator:      context="-";break;
		case __mul_operator:      context="*";break;
		case __div_operator:      context="/";break;
		case __link_operator:     context="~";break;
		case __add_equal:         context="+=";break;
		case __sub_equal:         context="-=";break;
		case __mul_equal:         context="*=";break;
		case __div_equal:         context="/=";break;
		case __link_equal:        context="~=";break;
		case __left_brace:        context="{";break;
		case __right_brace:       context="}";break;
		case __left_bracket:      context="[";break;
		case __right_bracket:     context="]";break;
		case __left_curve:        context="(";break;
		case __right_curve:       context=")";break;
		case __semi:              context=";";break;
		case __comma:             context=",";break;
		case __colon:             context=":";break;
		case __dot:               context=".";break;
		case __var:               context="var";break;
		case __func:              context="func";break;
		case __continue:          context="ctn";break;
		case __break:             context="brk";break;
		case __for:               context="for";break;
		case __forindex:          context="foridx";break;
		case __foreach:           context="foreh";break;
		case __while:             context="while";break;
		case __if:                context="if";break;
		case __elsif:             context="elsif";break;
		case __else:              context="else";break;
		case __return:            context="rtrn";break;
		
		case __id:                context="id";break;
		case __dynamic_id:        context="id...";break;
		case __number:            context="num";break;
		case __string:            context="str";break;
		
		case __root:              context="root";break;
		case __operator:          context="optr";break;
		default:                  context="ukn_tkn";break;
	}
	std::cout<<context;
	return;
}


#endif
