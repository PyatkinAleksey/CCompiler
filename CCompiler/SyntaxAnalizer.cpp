/**
* ���������� ������������ ����� C
*
* �����, ����������� �������������� ����������.
* ���� ������������ ������� ���������� � ����� ��������������� �������.
*
* �����:			������� ������, ��. ��-110
* ���� ��������:	2012-05-11 23:50
*/

#include "SyntaxAnalizer.h"

/**
* ������������� ��������������� �����������.
*
* ���������:
* c - ��������� �� ������ ������ Compiler ��� ������� � ����� ������� � ����� �����������.
* fn - ��� ����� � �������� ����������.
*/
SyntaxAnalizer::SyntaxAnalizer(Compiler *c, string fn) {

	code.open(fn + ".c");	// ��������� ������� ����� � �������� ������� ���������
	
	comp = c;
	
	// ��������� ������� ����������� �������� � �� ���������������
	operation["="]		= 0;
	operation["=="]		= 1;
	operation["!="]		= 1;
	operation["<"]		= 2;
	operation["<="]		= 2;
	operation[">"]		= 2;
	operation[">="]		= 2;
	operation["+"]		= 3;
	operation["-"]		= 3;
	operation["*"]		= 4;
	operation["/"]		= 4;
	operation["%"]		= 4;

	// ��������� ������ ����� ������:
	types[0] = "int";
	
	status	= 1;
	ch		= -1;

	program(); // ������ �������

}

/**
* ����������� ������ �� �� ����.
*
* ���������:
* ec - ��� ������.
* value - ��������, ��� ������� �������� ������.
*/
void SyntaxAnalizer::getError(int ec, string value) {

	switch (ec) {
		case EOF_BEFORE_LE:		comp->printError("End of file before logical end of the program.");			break;
		case UNKNOWN_CHAR:		comp->printError("Unknown character " + value + ".");						break;
		case MUST_BE_OFB:		comp->printError(value + ": Must be \"{\".");								break;
		case MUST_BE_EFB:		comp->printError(value + ": Must be \"}\".");								break;
		case MUST_BE_OPER:		comp->printError(value + ": Must be operator.");							break;
		case MUST_BE_EQV:		comp->printError(value + ": Must be \"=\".");								break;
		case MUST_BE_IDCSOB:	comp->printError(value + ": Must be ID, constant, \"-\" or \"(\".");		break;
		case MUST_BE_MINUS:		comp->printError(value + ": Must be \"-\".");								break;
		case MUST_BE_OB:		comp->printError(value + ": Must be \"(\".");								break;
		case MUST_BE_EB:		comp->printError(value + ": Must be \")\".");								break;
		case UNDEF_TYPE:		comp->printError("Use of undefined type '" + value + "'");					break;
		case MUST_BE_ID:		comp->printError(value + ": Must be an identifier.");						break;
		case MUST_BE_COEB:		comp->printError(value + ": Must be \",\" or \")\".");						break;
		case MUST_BE_EBOT:		comp->printError(value + ": Must be \")\" or a type.");						break;
		case MUST_BE_SEMI:		comp->printError("Expected ';' but found '" + value + "'");					break;
		case MUST_RETURN:		comp->printError("Function " + head->info + " must to return any "
									+ functions[head->info].type + " value.");								break;
		case FUNC_AH_BODY:		comp->printError("Function " + value + " already has a body.");				break;
		case EXP_OBOAS:			comp->printError("Expected '(', '=' or ';' but found '" + value + "'");		break;
		case EXP_CONST:			comp->printError("Expected constant but found '" + value + "'");			break;
		case ID_WAS_DEF:		comp->printError("The identifier was already defined as '" + value + "'");	break;
		default:				comp->printError("Unknown error message.");									break;
	}

	status = -1;

	exit(status);

}

/**
* ��������� ���������� ������� �� �������� ������.
*/
void SyntaxAnalizer::getChar() {
	
	if (!code.eof()) {
		code >> noskipws >> ch;
	} else {
		getError(EOF_BEFORE_LE, "");
	}
	
}

/**
* ����������� �������.
*/
void SyntaxAnalizer::getLex() {
	
	while (ch == -1 || ch == ' ' || ch == '\n' || ch == '\t') {
		getChar();
	}
	
	lex = ch;
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {lexType = ID;}
	else if (ch >= '0' && ch <= '9') {lexType = CNST;}
	else {
		switch (ch) {
			case '{': lexType = OFB; break;
			case '}': lexType = EFB; break;
			case ',': lexType = COMMA; break;
			case ';': lexType = SEMI; break;
			case '=': lexType = ASSIGN; break;
			case '+': lexType = ADD; break;
			case '*': lexType = MUL; break;
			case '-': lexType = SUB; break;
			case '/': lexType = DIV; break;
			case '%': lexType = MOD; break;
			case '<': lexType = LESS; break;
			case '>': lexType = GRT; break;
			case '!': lexType = NOT; break;
			case '(': lexType = OB; break;
			case ')': lexType = EB; break;
			case -1:
				return;
				break; // �������� ���� ����
			default: getError(UNKNOWN_CHAR, lex); break;
		}
	}
	
	if (lexType == LESS || lexType == ASSIGN || lexType == NOT || lexType == GRT) {
		getChar();
		if (ch == '=') {
			lex += ch;
		} else {
			if (lexType == NOT) {
				getError(MUST_BE_EQV, lex);
			}
		}
		if (lex == "<")			{lexType = LESS;}
		else if (lex == "<=")	{lexType = LESS_EQV;}
		else if (lex == "==")	{lexType = EQV;}
		else if (lex == "!=")	{lexType = NOT_EQV;}
		else if (lex == ">")	{lexType = GRT;}
		else if (lex == ">=")	{lexType = GRT_EQV;}
		return;
	}

	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) { // ��������������
		getChar();
		while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
			lex += ch;
			getChar();
		}
		// �������� �����:
		if (lex == "if") {lexType = IF;}
		else if (lex == "else") {lexType = ELSE;}
		else if (lex == "while") {lexType = WHILE;}
		else if (lex == "void") {lexType = TYPE;}
		else if (lex == "int") {lexType = TYPE;}
		else if (lex == "return") {lexType = RET;}
		else { // �������� ������������� � ������ ���������������, ���� ��� ��� ��� ���
			list<string>::iterator res = find(ids.begin(), ids.end(), lex);
			if (res == ids.end()) { // ������������� �� ������ � ������
				ids.push_back(lex);
			}
		}
		return;
	}
	
	if (ch >= '0' && ch <= '9') { // ���������
		getChar();
		while (ch >= '0' && ch <= '9') {
			lex += ch;
			getChar();
		}
	}
	
}

/**
* �����, ����������� ���������� ������ ��� ���������.
*/
void SyntaxAnalizer::program() {

	while (!code.eof()) {
		string	type;
		string	id;
		getChar();
		if (code.eof()) {break;}
		head	= NULL;
		pt		= NULL;
		getLex();
		switch (lexType) {
			case TYPE:
				type = lex;
				// ����� ���� ������ ���� �������������
				getLex();
				if (lexType != ID) {
					getError(MUST_BE_ID, lex);
				}
				if (functions.find(lex) != functions.end()) {
					getError(FUNC_AH_BODY, lex);
				}
				id = lex;
				getLex();
				switch (lexType) {
					case OB: // �������
						// ������������� ������� ���������� ������ ������ �������
						head = new Tree();
						head->info = id;
						head->llink = NULL;
						head->rlink = NULL;
						pt = head;
						// � ������ ������� ������������� ������� � ����� ����� �� ������
						functions[id] = Function();
						functions[id].address	= head;
						functions[id].type		= type;
						function();
						break;
					case ASSIGN: // ���������� ����������
					case SEMI:
						declaration(type, id, 0, NULL, 0);
						break;
					default:
						getError(EXP_OBOAS, lex);
						break;
				}
				break;
			default:
				getError(UNDEF_TYPE, lex);
				break;
		}
		getChar();
	}
	
}

/**
* ����� ���������� ����������.
*
* ���������:
* type - ��� ����������.
* id - ������������� ����������� ����������.
*/
void SyntaxAnalizer::declaration(string type, string id, int lvl, Tree *p, int b) {

	if (type != "void") {
		
			Variable var;
			
			var.type	= type;
			if (lexType == ASSIGN) {
				getLex();
				if (lexType != CNST) {
					getError(EXP_CONST, lex);
				}
				var.initVal = lex;
				getLex();
			} else {
				var.initVal = "?";
			}
			var.level	= lvl;
			if (head != NULL) {
				var.funcName = head->info;
			} else {
				var.funcName = "";
			}
			
			map<string,Variable>::iterator it = variables.find(id);
			if ((it == variables.end()) || ((*it).second.level != lvl) || ((*it).second.level == lvl && (*it).second.funcName != var.funcName)) {
				Tree *q;
				variables[id] = var;
				if (p != NULL) {
					//p = new Tree;
					//p->info = "$";
					if (b) {
						p->rlink = new Tree();
						q = p->rlink;
					} else {
						p->llink = new Tree();
						q = p->llink;
					}
					q->info = "dec";
					q->llink = new Tree();
					q->llink->info = id;
					q->llink->llink = NULL;
					q->llink->rlink = NULL;
					q->rlink = NULL;
				}
			} else {
				getError(ID_WAS_DEF, type);
			}

	} else {
		getError(UNDEF_TYPE, type);
	}

}

/**
* �����, ���������� �� ���������� �������� ��� �������.
*/
void SyntaxAnalizer::function() {
	
	sTRoot	= NULL;
	// ������ ���������� �������
	getChar();
	getLex();
	if (lexType == TYPE) {
		functionArguments();
	}
	if (lexType != EB) {
		getError(MUST_BE_EBOT, lex);
	}
	// ������������������ ��������, ����������� � ��������, �������� � ��������� ���������:
	getChar();
	getLex();
	compound(NULL, 1);
	
}

/**
* �����, ����������� ��������� �������. ������� �� � ������ ����������.
*/
void SyntaxAnalizer::functionArguments() {
	
	string type;
	string id;

	if (lexType == EB) {
		return;
	}
	if (lexType != TYPE) {
		getError(UNDEF_TYPE, lex);
	}

	type = lex;
	getLex();
	if (lexType != ID) {
		getError(MUST_BE_ID, lex);
	}
	id = lex;
	// ��������� � ������ ����������
	declaration(type, id, 1, pt, 0);
	// ��������� ���������� �� ��������� � ������:
	arguments[id] = Argument();
	arguments[id].funcName = head->info;
	arguments[id].argType = type;
	// �������, ��� ������
	getLex();
	switch (lexType) {
		case COMMA:
			getChar();
			getLex();
			functionArguments();
			break;
		case EB: break;
		default:
			getError(MUST_BE_COEB, lex);
			break;
	}
	
}

/**
* �����, ����������� ���������� ������ ��� ���������� ���������.
*
* ���������:
* p - ��������� �� ������� ������, � �������� ������������ ��������� ���������� ���������.
* b - ����, � ����� ����� ������������ ��������� ���������� ��������� (1 - ������, ������ - �����).
*/
void SyntaxAnalizer::compound(Tree *p, int b) {
	
	int isFunc = 0;		// �������� ����� �������
	int isReturned = 0; // ���� �������� �������� ��������
	int isIf = 0;		// ���������� �������� If
	int opCounter = 0;
	Tree *q = NULL;
	
	if (lexType == OFB) {
		if (p == NULL) {
			isFunc = 1;
			if (b) { // ������ �����
				pt->rlink = new Tree();
				pt = pt->rlink;
			} else { // ����� �����
				pt->llink = new Tree();
				pt = pt->llink;
			}
			p = pt;
		} else {
			if (b) { // ������ �����
				p->rlink = new Tree();
				p = p->rlink;
			} else { // ����� �����
				p->llink = new Tree();
				p = p->llink;
			}
			pt = p;
		}
		p->info		= lex;
		p->llink	= NULL;
		p->rlink	= NULL;
		getChar();
		getLex();
		if (lexType == IF) {
			isIf = 1;
		}
		while (lexType != EFB) {
			opCounter++;
			if (opCounter > 1) {
				p->rlink = new Tree();
				q = p->rlink;
				q->info		= "$";
				q->llink	= NULL;
				q->rlink	= NULL;
				p = q;
			} else {
				q = p;
			}
			if (lexType == RET) {
				isReturned = 1;
			}
			oper(q, 0);
			if (!isIf) {
				getChar();
				getLex();
			}
		}
		if (isFunc) { // ��������� �������� - ���� �������
			// ���� ��� ������������� �������� ������, ��� �������� �� void,
			// �� ��������� ������� �������� ���������������� ����
			if (functions[head->info].type != "void") {
				if (!isReturned) {
					getError(MUST_RETURN, "");
				}
			}
		}
	} else {
		getError(MUST_BE_OFB, lex);
	}
	
}

/**
* �����, ����������� ���������� ������ ��� ����������.
*
* ���������:
* p - ��������� �� ������� ������, � �������� �������������� ��������� ���������.
* b - ����, � ����� ����� ������������ ��������� ��������� (1 - ������, ������ - �����).
*/
void SyntaxAnalizer::oper(Tree *p, int b) {
	
	Tree *q;
	int lext = lexType;
	string l = lex;
	if (p == NULL) {p = pt;}
	string type;
	string id;

	switch (lexType) {
		case SEMI:	// ������ ��������
			if (b) { // ������ �����
				p->rlink = new Tree();
				q = p->rlink;
			} else { // ����� �����
				p->llink = new Tree();
				q = p->llink;
			}
			q->info = lex;
			q->llink = NULL;
			q->rlink = NULL;
			break;
		case OFB:	// ��������� ��������
			compound(p, b);
			break;
		case IF:	// �������� �������� IF
			if (b) { // ������ �����
				p->rlink = new Tree();
				p = p->rlink;
			} else { // ����� �����
				p->llink = new Tree();
				p = p->llink;
			}
			p->info = lex;
			p->llink = NULL;
			p->rlink = NULL;
			getLex();
			if (lexType == OB) {
				getChar();
				sTRoot = p;
				expression(p, 0, 0);
				getLex();
				if (lexType == EB) {
					p->rlink = new Tree();
					p = p->rlink;
					p->info = "fi";
					p->llink = NULL;
					p->rlink = NULL;
					getChar();
					getLex();
					oper(p, 0);
					getChar();
					getLex();
					if (lexType == ELSE) {
						getLex();
						oper(p, 1);
					}
				} else {
					getError(MUST_BE_EB, lex);
				}
			} else {
				getError(MUST_BE_OB, lex);
			}
			break;
		case WHILE:	// �������� ����� WHILE
			if (b) { // ������ �����
				p->rlink = new Tree();
				p = p->rlink;
			} else { // ����� �����
				p->llink = new Tree();
				p = p->llink;
			}
			p->info = lex;
			p->llink = NULL;
			p->rlink = NULL;
			getLex();
			if (lexType == OB) {
				getChar();
				sTRoot = p;
				expression(p, 0, 0);
				getLex();
				if (lexType == EB) {
					getChar();
					getLex();
					oper(p, 1);
				} else {
					getError(MUST_BE_EB, lex);
				}
			} else {
				getError(MUST_BE_OB, lex);
			}
			break;
		case ID:	// �������� ������������
			getLex();
			if (lexType == ASSIGN) {
				if (b) { // ������ �����
					p->rlink = new Tree();
					q = p->rlink;
				} else { // ����� �����
					p->llink = new Tree();
					q = p->llink;
				}
				q->info = lex;
				q->llink = new Tree();
				q->llink->info = l;
				q->llink->llink = NULL;
				q->llink->rlink = NULL;
				q->rlink = NULL;
				sTRoot = q;
				expression(q, 1, 0);
			} else {
				getError(MUST_BE_EQV, lex);
			}
			break;
		case TYPE: // ���������� ����������
			type = lex;
			getLex();
			if (lexType != ID) {
				getError(MUST_BE_ID, lex);
			}
			id = lex;
			declaration(type, id, 2, p, 0);
			getLex();
			if (lexType != SEMI) {
				getError(MUST_BE_SEMI, lex);
			}
			break;
		case RET:	// �������� �������� �� �������
			if (b) {
				p->rlink = new Tree();
				q = p->rlink;
			} else {
				p->llink = new Tree();
				q = p->llink;
			}
			q->info = lex;
			q->llink = NULL;
			q->rlink = NULL;
			if (functions[head->info].type == "int") { // ������ ���� ���������� �������� ���� int
				expression(q, 1, 0);
			}
			getChar();
			getLex();
			break;
		default: // ������: ������ ���� ��������
			getError(MUST_BE_OPER, lex);
			break;
	}
	
}

/**
* ����� ��������� ������ ��� ���������.
*
* ���������:
* p - ��������� �� ������� ������������� ������, � �������� �������������� ��������� ���������.
* b - ����, � ����� ����� �������������� ��������� ��������� (1 - ������, ������ - �����).
* be - ����, �������� �� ��������� �������� (1 - ��������, ������ - ���).
*/
void SyntaxAnalizer::expression(Tree *p, int b, int be) {
	
	Tree *s;
	
	getLex();
	string l = lex;
	int lType = lexType;
	
	switch (lexType) {	// � ��������� ����������
		case ID:		// �������������
		case CNST:		// ��� ���������
			getLex();
			switch (lexType) {	// ����� �������������� ��� ��������� �����
				case ADD:		// ���� ��������
				case SUB:		// ��� ���������
				case DIV:		// ��� �������
				case MOD:		// ��� ������� �� ������
				case MUL:		// ��� ���������
				case LESS:		// ��� "������"
				case LESS_EQV:	// ��� "������ ��� �����"
				case EQV:		// ��� "�����"
				case NOT_EQV:	// ��� "�� �����"
				case GRT:		// ��� "������"
				case GRT_EQV:	// ��� "������ ��� �����"
					if (be || !operation.count(p->info) || (operation.find(lex)->second > operation.find(p->info)->second)) {
						// ��������� ������� �������� ������ ���������� ����������
						// ��� ��������� ��������� � ������
						if (b) { // ������ �����
							p->rlink = new Tree();
							s = p->rlink;
						} else { // ����� �����
							p->llink = new Tree();
							s = p->llink;
						}
						s->info = lex;
						s->llink = new Tree();
						s->llink->info = l;
						s->llink->llink = NULL;
						s->llink->rlink = NULL;
						s->rlink = NULL;
						sTRoot = p;
					} else {
						// ��������� ������� �������� ������ ��� ����� ���������� ����������
						if (b) { // ������ �����
							p->rlink = new Tree();
							p->rlink->info = l;
							p->rlink->llink = NULL;
							p->rlink->rlink = NULL;
						} else { // ����� �����
							p->llink = new Tree();
							p->llink->info = l;
							p->llink->llink = NULL;
							p->llink->rlink = NULL;
						}
						if (sTRoot->llink == p) {
							sTRoot->llink = new Tree();
							sTRoot->llink->info = lex;
							sTRoot->llink->llink = p;
							sTRoot->llink->rlink = NULL;
							s = sTRoot->llink;
						} else {
							sTRoot->rlink = new Tree();
							sTRoot->rlink->info = lex;
							sTRoot->rlink->llink = p;
							sTRoot->rlink->rlink = NULL;
							s = sTRoot->rlink;
						}
					}
					getChar();
					expression(s, 1, 0);
					break;
				default:	// ����� �������������� ��� ��������� ����� ����� ������, ����� ��������
							// ��������� ���������
					if (b) { // ������ �����
						p->rlink = new Tree();
						s = p->rlink;
					} else { // ����� �����
						p->llink = new Tree();
						s = p->llink;
					}
					s->info = l;
					s->llink = NULL;
					s->rlink = NULL;
					break;
			}
			break;
		case SUB: // ���� "�����"
			s = new Tree();
			if (b) { // ������ �����
				p->rlink = s;
			} else { // ����� �����
				p->llink = s;
			}
			s->info = lex;
			s->llink = NULL;
			s->rlink = NULL;
			getChar();
			expression(s, 0, 0);
			break;
		case OB: // ����������� ������
			getChar();
			expression(p, b, 1);
			getLex();
			if (lexType != EB) {
				getError(MUST_BE_EB, lex);
			}
			getChar();
			getLex();
			if (lexType == ADD || lexType == SUB || lexType == MUL || lexType == DIV
				|| lexType == LESS || lexType == LESS_EQV || lexType == EQV
				|| lexType == NOT_EQV || lexType == GRT || lexType == GRT_EQV) {
				Tree *q = new Tree();
				q->info = lex;
				if (b) { // ������ �����
					q->llink = p->rlink;
					p->rlink = q;
				} else { // ����� �����
					q->llink = p->llink;
					p->llink = q;
				}
				q->rlink = NULL;
				getChar();
				expression(q, b, 0);
			}
			break;
		default:
			// ������: ������ ���� �������������, ��� ���������, ��� ���� "�����", ��� ����������� ������
			getError(MUST_BE_IDCSOB, lex);
			break;
	}
	
}

/**
* ����� ������ �������������� ������ ���������.
*
* ���������:
* p - ��������� �� ������� ������� ������, ������� ����� �������.
*/
void SyntaxAnalizer::printTree(Tree *p) {
	
	if (head != NULL) {
		
		if (p == NULL) {
			p = head;
			comp->logs << "Semantic Tree:" << endl;
		}
		
		comp->logs << p << "\t" << p->info << "\t" << p->llink << "\t" << p->rlink << endl;
		
		if (p->llink != NULL) {printTree(p->llink);}
		if (p->rlink != NULL) {printTree(p->rlink);}

	}

}

/**
* �������� �� ������ ������, ���������������� �������������� ������������.
*
* ���������:
* p - ��������� �� ������� ������ ��� ��������.
*/
void SyntaxAnalizer::deleteTree(Tree *p) {
	
	if (p->llink != NULL) {deleteTree(p->llink);}
	if (p->rlink != NULL) {deleteTree(p->rlink);}
	
	if (p->llink == NULL && p->rlink == NULL) {
		delete p;
	}
	
}

/**
* ������������ ������, ������� �������������� ������������ � �������� �������.
*/
SyntaxAnalizer::~SyntaxAnalizer() {
	
	for (map<string,Function>::iterator iter = functions.begin(); iter != functions.end(); iter++) {
		deleteTree((*iter).second.address);
	}
	
	code.close();
	
}