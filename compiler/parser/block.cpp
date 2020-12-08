//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#include "parser.h"

namespace carbon {

ptr<Parser::BlockNode> Parser::_parse_block(const ptr<Node>& p_parent, bool p_single_statement, stdvec<Token> p_termination) {
	ptr<BlockNode> block_node = newptr<BlockNode>();
	block_node->parernt_node = p_parent;

	parser_context.current_block = block_node.get();
	class ScopeDestruct {
	public:
		Parser::ParserContext* context = nullptr;
		ScopeDestruct(Parser::ParserContext* p_context) {
			context = p_context;
		}
		~ScopeDestruct() {
			context->current_block = nullptr;
		}
	};
	ScopeDestruct distruct = ScopeDestruct(&parser_context);

	const TokenData* tk;

	while (true) {
		tk = &tokenizer->peek();
		switch (tk->type) {
			case Token::_EOF: {
				throw PARSER_ERROR(Error::UNEXPECTED_EOF, "Unexpected end of file.", Vect2i());
			} break;

			case Token::KWORD_VAR: {
				tokenizer->next(); // eat "var"
				stdvec<ptr<VarNode>> vars = _parse_var(block_node);
				for (ptr<VarNode>& _var : vars) {
					block_node->local_vars.push_back(_var); // for quick access.
					block_node->statements.push_back(_var);
				}
			} break;

			case Token::KWORD_CONST: {
				tokenizer->next(); // ear "const"
				ptr<ConstNode> _const = _parse_const(block_node);
				block_node->local_const.push_back(_const);
				block_node->statements.push_back(_const);
			} break;

			// Ignore.
			case Token::SYM_SEMI_COLLON:
				tokenizer->next(); // eat ";"
				break;

			case Token::VALUE_NULL:
			case Token::VALUE_BOOL:
			case Token::VALUE_INT:
			case Token::VALUE_FLOAT:
			case Token::VALUE_STRING: {
				tk = &tokenizer->next(); // will be ignored by analyzer
				ptr<ConstValueNode> value = new_node<ConstValueNode>(tk->constant);
				block_node->statements.push_back(value);
			} break;

			case Token::KWORD_IF: {
				tokenizer->next(); // eat "if"
				block_node->statements.push_back(_parse_if_block(block_node));
			} break;

			case Token::KWORD_SWITCH: {
				tk = &tokenizer->next(); // eat "switch"
				ptr<ControlFlowNode> switch_block = new_node<ControlFlowNode>(ControlFlowNode::SWITCH);
				switch_block->parernt_node = p_parent;
				ControlFlowNode* outer_break = parser_context.current_break;
				parser_context.current_break = switch_block.get();

				switch_block->args.push_back(_parse_expression(block_node, false));
				if (tokenizer->next().type != Token::BRACKET_LCUR) throw UNEXP_TOKEN_ERROR("symbol \"{\"");

				while (true) {
					tk = &tokenizer->next();
					if (tk->type == Token::KWORD_CASE) {
						ControlFlowNode::SwitchCase _case;
						_case.pos = tk->get_pos();
						_case.expr = _parse_expression(block_node, false);
						if (tokenizer->next().type != Token::SYM_COLLON) throw UNEXP_TOKEN_ERROR("symbol \":\"");

						// COMMENTED: `case VALUE: { expr; expr; }` <--- curly brackets are not allowed.
						//tk = &tokenizer->peek();
						//if (tk->type == Token::BRACKET_LCUR) {
						//	tokenizer->next(); // eat "{"
						//	_case.body = _parse_block(block_node);
						//	if (tokenizer->next().type != Token::BRACKET_RCUR) throw UNEXP_TOKEN("symbol \"}\"");
						//} else {
						_case.body = _parse_block(block_node, false, { Token::KWORD_CASE, Token::KWORD_DEFAULT, Token::BRACKET_RCUR });
						//}
						switch_block->switch_cases.push_back(_case);

					} else if (tk->type == Token::KWORD_DEFAULT) {
						ControlFlowNode::SwitchCase _case;
						_case.default_case = true;
						if (tokenizer->next().type != Token::SYM_COLLON) throw UNEXP_TOKEN_ERROR("symbol \":\"");
						_case.body = _parse_block(block_node, false);
					} else if (tk->type == Token::BRACKET_RCUR) {
						break;
					} else {
						throw UNEXP_TOKEN_ERROR("keyword \"case\" or symbol \"}\"");
					}
				}
				block_node->statements.push_back(switch_block);
				parser_context.current_break = outer_break;
			} break;

			case Token::KWORD_WHILE: {
				tk = &tokenizer->next(); // eat "while"
				ptr<ControlFlowNode> while_block = new_node<ControlFlowNode>(ControlFlowNode::WHILE);

				ControlFlowNode* outer_break = parser_context.current_break;
				ControlFlowNode* outer_continue = parser_context.current_continue;
				parser_context.current_break = while_block.get();
				parser_context.current_continue = while_block.get();

				while_block->parernt_node = p_parent;
				while_block->args.push_back(_parse_expression(block_node, false));
				tk = &tokenizer->peek();
				if (tk->type == Token::BRACKET_LCUR) {
					tokenizer->next(); // eat "{"
					while_block->body = _parse_block(block_node);
					if (tokenizer->next().type != Token::BRACKET_RCUR) throw UNEXP_TOKEN_ERROR("symbol \"}\"");
				} else {
					while_block->body = _parse_block(block_node, true);
				}
				block_node->statements.push_back(while_block);
				parser_context.current_break = outer_break;
				parser_context.current_continue = outer_continue;
			} break;

			case Token::KWORD_FOR: {
				tk = &tokenizer->next(); // eat "for"
				ptr<ControlFlowNode> for_block = new_node<ControlFlowNode>(ControlFlowNode::FOR);
				ControlFlowNode* outer_break = parser_context.current_break;
				ControlFlowNode* outer_continue = parser_context.current_continue;
				parser_context.current_break = for_block.get();
				parser_context.current_continue = for_block.get();

				for_block->parernt_node = p_parent;
				if (tokenizer->next().type != Token::BRACKET_LPARAN) throw UNEXP_TOKEN_ERROR("symbol \"(\"");

				if (tokenizer->peek().type == Token::SYM_SEMI_COLLON) {
					tokenizer->next(); // eat ";"
					for_block->args.push_back(nullptr);
				} else {
					if (tokenizer->peek().type == Token::KWORD_VAR) {
						tokenizer->next(); // eat "var"

						tk = &tokenizer->next();
						if (tk->type != Token::IDENTIFIER) throw UNEXP_TOKEN_ERROR("an identifier");
						_check_identifier_predefinition(tk->identifier, block_node.get());

						ptr<VarNode> var_node = new_node<VarNode>();
						var_node->parernt_node = p_parent;
						var_node->name = tk->identifier;

						tk = &tokenizer->next();
						if (tk->type == Token::OP_EQ) {
							parser_context.current_var = var_node.get();
							ptr<Node> expr = _parse_expression(p_parent, false);
							parser_context.current_var = nullptr;
							var_node->assignment = expr;
							if (tokenizer->next().type != Token::SYM_SEMI_COLLON) throw UNEXP_TOKEN_ERROR("symbol \";\"");
						} else if (tk->type == Token::SYM_SEMI_COLLON) {
							
						} else if (tk->type == Token::SYM_COLLON) {
							for_block->cf_type = ControlFlowNode::CfType::FOREACH;
						}
						for_block->args.push_back(var_node);
					} else {
						for_block->args.push_back(_parse_expression(block_node, true));
						if (tokenizer->next().type != Token::SYM_SEMI_COLLON) throw UNEXP_TOKEN_ERROR("symbol \";\"");
					}
				}

				if (for_block->cf_type == ControlFlowNode::CfType::FOREACH) {
					for_block->args.push_back(_parse_expression(block_node, false));
					if (tokenizer->next().type != Token::BRACKET_RPARAN) throw UNEXP_TOKEN_ERROR("symbol \")\"");
				} else {
					if (tokenizer->peek().type == Token::SYM_SEMI_COLLON) {
						tokenizer->next(); // eat ";"
						for_block->args.push_back(nullptr);
					} else {
						for_block->args.push_back(_parse_expression(block_node, false));
						if (tokenizer->next().type != Token::SYM_SEMI_COLLON) throw UNEXP_TOKEN_ERROR("symbol \";\"");
					}

					if (tokenizer->peek().type == Token::BRACKET_RPARAN) {
						tokenizer->next(); // eat ")"
						for_block->args.push_back(nullptr);
					} else {
						for_block->args.push_back(_parse_expression(block_node, true));
						if (tokenizer->next().type != Token::BRACKET_RPARAN) throw UNEXP_TOKEN_ERROR("symbol \")\"");
					}
				}

				if (tokenizer->peek().type == Token::BRACKET_LCUR) {
					tokenizer->next(); // eat "{"
					for_block->body = _parse_block(block_node);
					if (tokenizer->next().type != Token::BRACKET_RCUR) throw UNEXP_TOKEN_ERROR("symbol \"}\"");
				} else {
					for_block->body = _parse_block(block_node, true);
				}

				// add loop counter initialization to local vars.
				if (for_block->args[0] != nullptr && for_block->args[0]->type == Node::Type::VAR) {
					for_block->body->local_vars.insert(for_block->body->local_vars.begin(), ptrcast<VarNode>(for_block->args[0]));
				}

				block_node->statements.push_back(for_block);
				parser_context.current_break = outer_break;
				parser_context.current_continue = outer_continue;
			} break;

			case Token::KWORD_BREAK: {
				tk = &tokenizer->next(); // eat "break"
				if (!parser_context.current_break) throw PARSER_ERROR(Error::SYNTAX_ERROR, "can't use break outside a loop/switch.", tk->get_pos());
				ptr<ControlFlowNode> _break = new_node<ControlFlowNode>(ControlFlowNode::BREAK);
				_break->break_continue = parser_context.current_break;
				parser_context.current_break->has_break = true;
				_break->parernt_node = p_parent;
				block_node->statements.push_back(_break);
			} break;

			case Token::KWORD_CONTINUE: {
				tk = &tokenizer->next(); // eat "continue"
				if (!parser_context.current_continue) throw PARSER_ERROR(Error::SYNTAX_ERROR, "can't use continue outside a loop.", tk->get_pos());
				ptr<ControlFlowNode> _continue = new_node<ControlFlowNode>(ControlFlowNode::CONTINUE);
				_continue->break_continue = parser_context.current_continue;
				parser_context.current_continue->has_continue = true;
				_continue->parernt_node = p_parent;
				block_node->statements.push_back(_continue);
			} break;

			case Token::KWORD_RETURN: {
				tk = &tokenizer->next(); // eat "return"
				if (!parser_context.current_func) throw PARSER_ERROR(Error::SYNTAX_ERROR, "can't use return outside a function.", tk->get_pos());
				if (parser_context.current_class && parser_context.current_class->constructor) {
					if (parser_context.current_class->constructor == parser_context.current_func) {
						throw PARSER_ERROR(Error::SYNTAX_ERROR, "constructor can't return anything.", tk->get_pos());
					}
				}
				ptr<ControlFlowNode> _return = new_node<ControlFlowNode>(ControlFlowNode::RETURN);
				if (tokenizer->peek().type != Token::SYM_SEMI_COLLON)  _return->args.push_back(_parse_expression(block_node, false));
				_return->parernt_node = p_parent;
				_return->_return = parser_context.current_func;
				if (tokenizer->next().type != Token::SYM_SEMI_COLLON) throw UNEXP_TOKEN_ERROR("symbol \";\"");
				parser_context.current_func->has_return = true;
				block_node->statements.push_back(_return);
			} break;

			default: {
				for (Token termination : p_termination) {
					if (tk->type == termination) {
						return block_node;
					}
				}
				ptr<Node> expr = _parse_expression(block_node, true);
				if (tokenizer->next().type != Token::SYM_SEMI_COLLON) throw UNEXP_TOKEN_ERROR("symbol \";\"");
				block_node->statements.push_back(expr);
			}
		}

		if (p_single_statement)
			break;
	}

	return block_node;
}


ptr<Parser::ControlFlowNode> Parser::_parse_if_block(const ptr<BlockNode>& p_parent) {
	ASSERT(tokenizer->peek(-1).type == Token::KWORD_IF);

	ptr<ControlFlowNode> if_block = new_node<ControlFlowNode>(ControlFlowNode::IF);
	if_block->parernt_node = p_parent;
	ptr<Node> cond = _parse_expression(p_parent, false);
	if_block->args.push_back(cond);

	const TokenData* tk = &tokenizer->peek();
	if (tk->type == Token::BRACKET_LCUR) {
		tokenizer->next(); // eat "{"
		if_block->body = _parse_block(p_parent);
		if (tokenizer->next().type != Token::BRACKET_RCUR) throw UNEXP_TOKEN_ERROR("symbol \"}\"");
	} else {
		if_block->body = _parse_block(p_parent, true);
	}

	tk = &tokenizer->peek(0);
	while (tk->type == Token::KWORD_ELSE) {
		tokenizer->next(); // eat "else"
		tk = &tokenizer->peek(0);
		switch (tk->type) {
			case Token::KWORD_IF: {
				tokenizer->next(); // eat "if"
				if_block->body_else = new_node<BlockNode>();
				if_block->body_else->parernt_node = p_parent;
				if_block->body_else->statements.push_back(_parse_if_block(p_parent));
			} break;
			case Token::BRACKET_LCUR: {
				tokenizer->next(); // eat "{"
				if_block->body_else = _parse_block(p_parent);
				if (tokenizer->next().type != Token::BRACKET_RCUR) throw UNEXP_TOKEN_ERROR("symbol \"}\"");
			} break;
			default: {
				if_block->body_else = _parse_block(p_parent, true);
			}
		}
		tk = &tokenizer->peek();
	}

	return if_block;
}

}