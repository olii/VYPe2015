%{
#include <algorithm>
#include <vector>
#include <cstdarg>

#include "frontend/ast.h"
#include "frontend/context.h"
#include "frontend/name_mangler.h"

using namespace frontend;

extern int yylex();
extern void yyerror(const char* e, ...);

extern Program program;
extern Context context;

extern void finalize(int exitCode);
%}

%define parse.error verbose // Better error reporting
%debug

%union
{
	frontend::Function* function;
	frontend::FunctionSymbol::ParameterList* functionParameters;
	frontend::StatementBlock* statements;
	frontend::Statement* statement;
	std::vector<frontend::Expression*>* expressions;
	frontend::Expression* expression;
	std::vector<frontend::Declaration*>* declList;
	frontend::AssignStatement* forInitStmt;
	frontend::ForIterationStatement* forIterStmt;
	std::string* strValue;
	char charValue;
	int intValue;
}

%token COMMA SEMICOLON ASSIGN LEFT_PAREN RIGHT_PAREN LEFT_CBRACE RIGHT_CBRACE PLUS MINUS
	MULTIPLY DIVIDE MODULO NOT LESS LESS_EQUAL GREATER GREATER_EQUAL EQUAL NOT_EQUAL AND OR
	IF ELSE WHILE FOR RETURN VOID BIT_AND BIT_OR BIT_NOT RESERVED
%token <intValue> INT_LIT
%token <charValue> CHAR_LIT
%token <strValue> STRING_LIT ID TYPE BUILTIN

%type <function> func_impl
%type <functionParameters> decl_param_list impl_param_list
%type <statements> stmts stmt_list
%type <statement> stmt assign_stmt decl_stmt if_stmt while_stmt for_stmt return_stmt call_stmt empty_stmt
%type <expressions> exprs expr_list
%type <expression> expr decl_init_expr for_cond
%type <declList> decl_id_list
%type <forInitStmt> for_init
%type <forIterStmt> for_iter

%left OR
%left AND
%left BIT_OR
%left BIT_AND
%left EQUAL NOT_EQUAL
%left LESS LESS_EQUAL GREATER GREATER_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT BIT_NOT UNARY_PLUS UNARY_MINUS
%nonassoc LEFT_PAREN RIGHT_PAREN

%start program

%%

program :   body
		|   %empty
		;

body    :   body func_decl
		|   body func_impl  { program.addFunction($2); }
		|   func_decl
		|   func_impl       { program.addFunction($1); }
		;

func_decl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON                                   {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), {}, false)) == nullptr)
																									{
																										yyerror("Redeclaration of function '%s'.", $2->c_str());
																										delete $1;
																										delete $2;
																										finalize(3);
																										YYERROR;
																									}

																									delete $1;
																									delete $2;
																								}
			|   TYPE ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON                        {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), *$4, false)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										delete $1;
																										delete $2;
																										delete $4;
																										finalize(3);
																										YYERROR;
																									}

																									delete $1;
																									delete $2;
																									delete $4;
																								}
			|   VOID ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON                                   {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::DataType::VOID, {}, false)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										delete $2;
																										finalize(3);
																										YYERROR;
																									}

																									delete $2;
																								}
			|   VOID ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON                        {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::DataType::VOID, *$4, false)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										delete $2;
																										delete $4;
																										finalize(3);
																										YYERROR;
																									}

																									delete $2;
																									delete $4;
																								}
			;

func_impl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN                                             {
																									context.newSymbolTable();

																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), {}, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										delete $1;
																										delete $2;
																										finalize(3);
																										YYERROR;
																									}
																									context.setCurrentFunction(funcSymbol);
																								}
													LEFT_CBRACE stmts RIGHT_CBRACE              {
																									$$ = new Function(context.getCurrentFunction(), $8);
																									context.popSymbolTable();
																									delete $1;
																									delete $2;
																								}
			|   TYPE ID LEFT_PAREN impl_param_list RIGHT_PAREN                                  {
																									context.newSymbolTable();

																									// Add parameters as variables to newly created symbol table
																									for (const auto& param : *$4)
																									{
																										if (context.currentSymbolTable()->addVariable(param) == nullptr)
																										{
																											yyerror("Redefinition of function argument '%s'.", param->getName().c_str());
																											delete $1;
																											delete $2;
																											delete $4;
																											finalize(3);
																											YYERROR;
																										}
																									}

																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), *$4, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										delete $1;
																										delete $2;
																										delete $4;
																										finalize(3);
																										YYERROR;
																									}
																									context.setCurrentFunction(funcSymbol);
																								}
																LEFT_CBRACE stmts RIGHT_CBRACE  {
																									$$ = new Function(context.getCurrentFunction(), $8);
																									context.popSymbolTable();
																									delete $1;
																									delete $2;
																									delete $4;
																								}
			|   VOID ID LEFT_PAREN VOID RIGHT_PAREN                                             {
																									context.newSymbolTable();

																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::DataType::VOID, {}, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										delete $2;
																										finalize(3);
																										YYERROR;
																									}
																									context.setCurrentFunction(funcSymbol);
																								}
													LEFT_CBRACE stmts RIGHT_CBRACE              {
																									$$ = new Function(context.getCurrentFunction(), $8);
																									context.popSymbolTable();
																									delete $2;
																								}
			|   VOID ID LEFT_PAREN impl_param_list RIGHT_PAREN                                  {
																									context.newSymbolTable();

																									// Add parameters as variables to newly created symbol table
																									for (const auto& param : *$4)
																									{
																										if (context.currentSymbolTable()->addVariable(param) == nullptr)
																										{
																											yyerror("Redefinition of function argument '%s'.", param->getName().c_str());
																											delete $2;
																											delete $4;
																											finalize(3);
																											YYERROR;
																										}
																									}

																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::DataType::VOID, *$4, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										delete $2;
																										delete $4;
																										finalize(3);
																										YYERROR;
																									}
																									context.setCurrentFunction(funcSymbol);
																								}
															   LEFT_CBRACE stmts RIGHT_CBRACE   {
																									$$ = new Function(context.getCurrentFunction(), $8);
																									context.popSymbolTable();
																									delete $2;
																									delete $4;
																								}
			;

decl_param_list :   decl_param_list COMMA TYPE      { $$->push_back(new VariableSymbol("", Symbol::stringToDataType({*$3}))); delete $3; }
				|   TYPE                            { $$ = new FunctionSymbol::ParameterList( { new VariableSymbol("", Symbol::stringToDataType({*$1})) } ); delete $1; }
				;

impl_param_list :   impl_param_list COMMA TYPE ID   { $$->push_back(new VariableSymbol(*$4, Symbol::stringToDataType({*$3}))); delete $3; delete $4; }
				|   TYPE ID                         { $$ = new FunctionSymbol::ParameterList( { new VariableSymbol(*$2, Symbol::stringToDataType({*$1})) } ); delete $1; delete $2; }
				;

stmts   :   stmt_list   { $$ = $1; }
		|   %empty      { $$ = new StatementBlock(); }
		;

stmt    :   assign_stmt { $$ = $1; }
		|   decl_stmt   { $$ = $1; }
		|   if_stmt     { $$ = $1; }
		|   while_stmt  { $$ = $1; }
		|   for_stmt    { $$ = $1; }
		|   return_stmt { $$ = $1; }
		|   call_stmt   { $$ = $1; }
		|   empty_stmt  { $$ = $1; }
		;

stmt_list   :   stmt_list stmt              {
												// Do not add empty statements
												if ($2 != nullptr)
													$$->addStatement($2);
											}
			|   stmt                        {
												// Check whether it is an empty statement, if so, create just empty statement list
												$$ = new StatementBlock();
												if ($1 != nullptr)
													$$->addStatement($1);
											}
			;

assign_stmt :   ID ASSIGN expr SEMICOLON    {
												Symbol* symbol = context.findSymbol(*$1);
												if (symbol == nullptr || symbol->getType() != Symbol::Type::VARIABLE)
												{
													yyerror("Assignment to undefined symbol '%s'.", $1->c_str());
													delete $1;
													finalize(3);
													YYERROR;
												}

												VariableSymbol* varSymbol = static_cast<VariableSymbol*>(symbol);
												if (varSymbol->getDataType() != $3->getDataType())
												{
													yyerror("Unable to assign '%s' to variable '%s' of type '%s'.",
																Symbol::dataTypeToString($3->getDataType()).c_str(),
																varSymbol->getName().c_str(),
																Symbol::dataTypeToString(varSymbol->getDataType()).c_str());
													delete $1;
													finalize(3);
													YYERROR;
												}

												$$ = new AssignStatement(varSymbol, $3);
												delete $1;
											}
			;

decl_stmt   :   TYPE decl_id_list SEMICOLON {
												for (auto& varDecl : *$2)
												{
													Symbol* symbol = nullptr;
													if ((symbol = context.findSymbol(varDecl->getVariableName())) != nullptr)
													{
														// We cannot shadow functions
														if (symbol->getType() != Symbol::Type::VARIABLE)
														{
															yyerror("Redefinition of symbol '%s'. Cannot shadow functions.", symbol->getName().c_str());
															delete $1;
															delete $2;
															finalize(3);
															YYERROR;
														}
													}

													// Adding can fail if there is symbol in the same block, we cannot shadow in the same block
													if ((symbol = context.currentSymbolTable()->addVariable(varDecl->getVariableName(), Symbol::stringToDataType(*$1))) == nullptr)
													{
														yyerror("Redefinition of symbol '%s'.", varDecl->getVariableName().c_str());
														delete $1;
														delete $2;
														finalize(3);
														YYERROR;
													}

													VariableSymbol* varSymbol = static_cast<VariableSymbol*>(symbol);
													if (varDecl->getInitialization() && varDecl->getInitialization()->getDataType() != varSymbol->getDataType())
													{
														yyerror("Unable to initialize variable '%s' of type '%s' with '%s'.",
																	varSymbol->getName().c_str(),
																	Symbol::dataTypeToString(varSymbol->getDataType()).c_str(),
																	Symbol::dataTypeToString(varDecl->getInitialization()->getDataType()).c_str());
														delete $1;
														delete $2;
														finalize(3);
														YYERROR;
													}

													varDecl->setVariableSymbol(varSymbol);
												}

												$$ = new DeclarationStatement(*$2);
												delete $1;
												delete $2;
											}
			;

decl_id_list    :   decl_id_list COMMA ID decl_init_expr   {
														$$->push_back(new Declaration(*$3, $4));
														delete $3;
													}
				|   ID decl_init_expr               {
														$$ = new std::vector<Declaration*>( { new Declaration(*$1, $2) } );
														delete $1;
													}
				;

decl_init_expr  :   ASSIGN expr                     { $$ = $2; }
				|   %empty                          { $$ = nullptr; }
				;

if_stmt :   IF LEFT_PAREN expr RIGHT_PAREN          {
														if ($3->getDataType() != Symbol::DataType::INT)
														{
															yyerror("Condition of if statement is of type '%s'. Must be 'int'.",
																Symbol::dataTypeToString($3->getDataType()).c_str());
															finalize(3);
															YYERROR;
														}

														context.newSymbolTable();
													}
				LEFT_CBRACE stmts RIGHT_CBRACE ELSE {
														context.popSymbolTable();
														context.newSymbolTable();
													}
				LEFT_CBRACE stmts RIGHT_CBRACE      {
														$$ = new IfStatement($3, $7, $12);
														context.popSymbolTable();
													}

		;

while_stmt  :   WHILE LEFT_PAREN expr RIGHT_PAREN   {
														if ($3->getDataType() != Symbol::DataType::INT)
														{
															yyerror("Condition of while statement is of type '%s'. Must be 'int'.",
																Symbol::dataTypeToString($3->getDataType()).c_str());
															finalize(3);
															YYERROR;
														}

														context.newSymbolTable();
													}
					LEFT_CBRACE stmts RIGHT_CBRACE  {
														$$ = new WhileStatement($3, $7);
														context.popSymbolTable();
													}
			;

for_stmt    :   FOR LEFT_PAREN for_init SEMICOLON for_cond SEMICOLON for_iter RIGHT_PAREN
													{
														if ($5 && $5->getDataType() != Symbol::DataType::INT)
														{
															yyerror("Condition of for statement is of type '%s'. Must be 'int'.",
																Symbol::dataTypeToString($5->getDataType()).c_str());
															finalize(3);
															YYERROR;
														}

														context.newSymbolTable();
													}
					LEFT_CBRACE stmts RIGHT_CBRACE  {
														$$ = new ForStatement($3, $5, $7, $11);
														context.popSymbolTable();
													}
			;

for_init    :   ID ASSIGN expr                      {
														Symbol* symbol = context.findSymbol(*$1);
														if (symbol == nullptr || symbol->getType() != Symbol::Type::VARIABLE)
														{
															yyerror("Assignment to undefined symbol '%s'.", $1->c_str());
															delete $1;
															finalize(3);
															YYERROR;
														}

														VariableSymbol* varSymbol = static_cast<VariableSymbol*>(symbol);
														if (varSymbol->getDataType() != $3->getDataType())
														{
															yyerror("Unable to assign '%s' to variable '%s' of type '%s'.",
																		Symbol::dataTypeToString($3->getDataType()).c_str(),
																		varSymbol->getName().c_str(),
																		Symbol::dataTypeToString(varSymbol->getDataType()).c_str());
															delete $1;
															finalize(3);
															YYERROR;
														}

														$$ = new AssignStatement(varSymbol, $3);
														delete $1;
													}
			|   %empty                              { $$ = nullptr; }
			;

for_cond    :   expr                                { $$ = $1; }
			|   %empty                              { $$ = nullptr; }
			;

for_iter    :   ID ASSIGN expr                      {
														Symbol* symbol = context.findSymbol(*$1);
														if (symbol == nullptr || symbol->getType() != Symbol::Type::VARIABLE)
														{
															yyerror("Assignment to undefined symbol '%s'.", $1->c_str());
															delete $1;
															finalize(3);
															YYERROR;
														}

														VariableSymbol* varSymbol = static_cast<VariableSymbol*>(symbol);
														if (varSymbol->getDataType() != $3->getDataType())
														{
															yyerror("Unable to assign '%s' to variable '%s' of type '%s'.",
																		Symbol::dataTypeToString($3->getDataType()).c_str(),
																		varSymbol->getName().c_str(),
																		Symbol::dataTypeToString(varSymbol->getDataType()).c_str());
															delete $1;
															finalize(3);
															YYERROR;
														}

														$$ = new ForIterationStatement(new AssignStatement(varSymbol, $3));
														delete $1;
													}
			|   expr                                { $$ = new ForIterationStatement($1); }
			|   %empty                              { $$ = nullptr; }
			;

return_stmt :   RETURN expr SEMICOLON	{
											if (context.getCurrentFunction()->getReturnType() != $2->getDataType())
											{
												yyerror("Return type mismatch. Expected '%s', got '%s'.",
													Symbol::dataTypeToString(context.getCurrentFunction()->getReturnType()).c_str(),
													Symbol::dataTypeToString($2->getDataType()).c_str());
												finalize(3);
												YYERROR;
											}

											$$ = new ReturnStatement($2);
										}
			|   RETURN SEMICOLON		{
											if (context.getCurrentFunction()->getReturnType() != Symbol::DataType::VOID)
											{
												yyerror("Return type mismatch. Expected '%s', got 'void'.",
													Symbol::dataTypeToString(context.getCurrentFunction()->getReturnType()).c_str());
												finalize(3);
												YYERROR;
											}

											$$ = new ReturnStatement();
										}
			;

call_stmt   :   ID LEFT_PAREN exprs RIGHT_PAREN SEMICOLON   {
																std::vector<Symbol::DataType> paramDataTypes;
																std::for_each($3->begin(), $3->end(), [&paramDataTypes](Expression* expr) { paramDataTypes.push_back(expr->getDataType()); });
																std::string mangledName = NameMangler::mangle(*$1, paramDataTypes);

																// We can look for functions only in global space
																Symbol* symbol = nullptr;
																if ((symbol = context.globalSymbolTable()->findSymbol(mangledName)) == nullptr)
																{
																	yyerror("Undeclared identifier '%s' used.", $1->c_str());
																	delete $1;
																	delete $3;
																	finalize(3);
																	YYERROR;
																}

																// Symbol needs to represent function
																if (symbol->getType() != Symbol::Type::FUNCTION)
																{
																	yyerror("Identifier '%s' is not a function.", $1->c_str());
																	delete $1;
																	delete $3;
																	finalize(3);
																	YYERROR;
																}

																// Check whether number of parameters match
																FunctionSymbol* func = static_cast<FunctionSymbol*>(symbol);
																if (func->getParameters().size() != $3->size())
																{
																	yyerror("Unexpected amount of arguments provided when calling '%s'. Expected %u, got %u.",
																		$1->c_str(), func->getParameters().size(), $3->size());
																	delete $1;
																	delete $3;
																	finalize(3);
																	YYERROR;
																}

																const FunctionSymbol::ParameterList& paramList = func->getParameters();
																for (uint32_t i = 0; i < paramList.size(); ++i)
																{
																	if (paramList[i]->getDataType() != $3->at(i)->getDataType())
																	{
																		yyerror("Type mismatch for parameter on position %u in call of function '%s'. Expected %s, got %s.",
																				i, $1->c_str(),
																				Symbol::dataTypeToString($3->at(i)->getDataType()).c_str(),
																				Symbol::dataTypeToString(paramList[i]->getDataType()).c_str());
																		delete $1;
																		delete $3;
																		finalize(3);
																		YYERROR;
																	}
																}

																$$ = new CallStatement(func, *$3);
																delete $1;
																delete $3;
															}
			|   BUILTIN LEFT_PAREN exprs RIGHT_PAREN        {
																if (*$1 == "print")
																{
																	if ($3->size() < 1)
																	{
																		yyerror("Builtin function 'print' requires at least 1 argument.");
																		delete $1;
																		delete $3;
																		finalize(3);
																		YYERROR;
																	}

																}
																else
																{
																	yyerror("Builtin function '%s' cannot be in form of statement.", $1->c_str());
																	delete $1;
																	finalize(3);
																	YYERROR;
																}

																$$ = new BuiltinCallStatement(*$1, *$3);
																delete $1;
																delete $3;
															}
														;

empty_stmt  :   SEMICOLON { $$ = nullptr; }
			;

exprs       :   expr_list { $$ = $1; }
			|   %empty { $$ = new std::vector<Expression*>(); }
			;

expr_list   :   expr_list COMMA expr    { $$->push_back($3); }
			|   expr                    { $$ = new std::vector<Expression*>({$1}); }
			;

expr    :   expr PLUS expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s + %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::ADD, Symbol::DataType::INT, $1, $3);
							}
		|   expr MINUS expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s - %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::SUBTRACT, Symbol::DataType::INT, $1, $3);
							}

		|   expr MULTIPLY expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s * %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::MULTIPLY, Symbol::DataType::INT, $1, $3);
							}
		|   expr DIVIDE expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s / %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::DIVIDE, Symbol::DataType::INT, $1, $3);
							}
		|   expr MODULO expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s % %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::MODULO, Symbol::DataType::INT, $1, $3);
							}
		|   expr LESS expr	{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s < %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::LESS, Symbol::DataType::INT, $1, $3);
							}
		|   expr LESS_EQUAL expr	{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s <= %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::LESS_EQUAL, Symbol::DataType::INT, $1, $3);
							}
		|   expr GREATER expr	{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s > %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::GREATER, Symbol::DataType::INT, $1, $3);
							}
		|   expr GREATER_EQUAL expr	{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s >= %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::GREATER_EQUAL, Symbol::DataType::INT, $1, $3);
							}
		|   expr EQUAL expr		{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s == %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::EQUAL, Symbol::DataType::INT, $1, $3);
							}
		|   expr NOT_EQUAL expr	{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s != %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::NOT_EQUAL, Symbol::DataType::INT, $1, $3);
							}
		|   expr BIT_AND expr	{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s & %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::BIT_AND, Symbol::DataType::INT, $1, $3);
							}
		|   expr BIT_OR expr	{
								if ($1->getDataType() != $3->getDataType())
								{
									yyerror("No match for operation '%s | %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::BIT_OR, Symbol::DataType::INT, $1, $3);
							}
		|   expr AND expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s && %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::AND, Symbol::DataType::INT, $1, $3);
							}
		|   expr OR expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s || %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::OR, Symbol::DataType::INT, $1, $3);
							}
		|   NOT expr		{
								if ($2->getDataType() != Symbol::DataType::INT)
								{
									yyerror("No match for operation '! %s'.",
										Symbol::dataTypeToString($2->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new UnaryExpression(Expression::Type::NOT, Symbol::DataType::INT, $2);
							}
		|   BIT_NOT expr	{
								if ($2->getDataType() != Symbol::DataType::INT)
								{
									yyerror("No match for operation '~ %s'.",
										Symbol::dataTypeToString($2->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new UnaryExpression(Expression::Type::BIT_NOT, Symbol::DataType::INT, $2);
							}
		|   PLUS expr %prec UNARY_PLUS {
								if ($2->getDataType() != Symbol::DataType::INT)
								{
									yyerror("No match for operation '+ %s'.",
										Symbol::dataTypeToString($2->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = $2; // Unary plus doesn't change anything
							}
		|   MINUS expr %prec UNARY_MINUS {
								if ($2->getDataType() != Symbol::DataType::INT)
								{
									yyerror("No match for operation '- %s'.",
										Symbol::dataTypeToString($2->getDataType()).c_str());
									finalize(3);
									YYERROR;
								}

								$$ = new UnaryExpression(Expression::Type::NEG, Symbol::DataType::INT, $2);
							}
		|   LEFT_PAREN TYPE RIGHT_PAREN expr {
								// In case of typecast to same type, do nothing
								if ($4->getDataType() == Symbol::stringToDataType(*$2))
								{
									$$ = $4;
									delete $2;
								}
								else if ((($4->getDataType() == Symbol::DataType::CHAR) && (Symbol::stringToDataType(*$2) == Symbol::DataType::INT)) ||
									(($4->getDataType() == Symbol::DataType::CHAR) && (Symbol::stringToDataType(*$2) == Symbol::DataType::STRING)) ||
									(($4->getDataType() == Symbol::DataType::INT) && (Symbol::stringToDataType(*$2) == Symbol::DataType::CHAR)))
								{
									$$ = new UnaryExpression(Expression::Type::TYPECAST, Symbol::stringToDataType(*$2), $4);
									delete $2;
								}
								else
								{
									yyerror("Unable to cast type '%s' to type '%s'.",
										Symbol::dataTypeToString($4->getDataType()).c_str(),
										$2->c_str());
									delete $2;
									finalize(3);
									YYERROR;
								}
							}
		|   LEFT_PAREN expr RIGHT_PAREN { $$ = $2; }
		|   ID LEFT_PAREN exprs RIGHT_PAREN
				{
					std::vector<Symbol::DataType> paramDataTypes;
					std::for_each($3->begin(), $3->end(), [&paramDataTypes](Expression* expr) { paramDataTypes.push_back(expr->getDataType()); });
					std::string mangledName = NameMangler::mangle(*$1, paramDataTypes);

					// We can look for functions only in global space
					Symbol* symbol = nullptr;
					if ((symbol = context.globalSymbolTable()->findSymbol(mangledName)) == nullptr)
					{
						yyerror("Undeclared identifier '%s' used.", $1->c_str());
						delete $1;
						delete $3;
						finalize(3);
						YYERROR;
					}

					// Symbol needs to represent function
					if (symbol->getType() != Symbol::Type::FUNCTION)
					{
						yyerror("Identifier '%s' is not a function.", $1->c_str());
						delete $1;
						delete $3;
						finalize(3);
						YYERROR;
					}

					// Check whether number of parameters match
					FunctionSymbol* func = static_cast<FunctionSymbol*>(symbol);
					if (func->getParameters().size() != $3->size())
					{
						yyerror("Unexpected amount of arguments provided when calling '%s'. Expected %u, got %u.",
							$1->c_str(), func->getParameters().size(), $3->size());
						delete $1;
						delete $3;
						finalize(3);
						YYERROR;
					}

					const FunctionSymbol::ParameterList& paramList = func->getParameters();
					for (uint32_t i = 0; i < paramList.size(); ++i)
					{
						if (paramList[i]->getDataType() != $3->at(i)->getDataType())
						{
							yyerror("Type mismatch for parameter on position %u in call of function '%s'. Expected %s, got %s.",
									i, $1->c_str(),
									Symbol::dataTypeToString($3->at(i)->getDataType()).c_str(),
									Symbol::dataTypeToString(paramList[i]->getDataType()).c_str());
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}
					}

					$$ = new Call(func, *$3);
					delete $1;
					delete $3;
				}
		|   BUILTIN LEFT_PAREN exprs RIGHT_PAREN
				{
					Symbol::DataType returnType;
					if (*$1 == "print")
					{
						if ($3->size() < 1)
						{
							yyerror("Builtin function 'print' requires at least 1 argument.");
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						returnType = Symbol::DataType::VOID;
					}
					else if (*$1 == "read_char")
					{
						if ($3->size() != 0)
						{
							yyerror("Builtin function 'read_char' requires 0 arguments.");
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						returnType = Symbol::DataType::CHAR;
					}
					else if (*$1 == "read_int")
					{
						if ($3->size() != 0)
						{
							yyerror("Builtin function 'read_int' requires 0 arguments.");
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						returnType = Symbol::DataType::INT;
					}
					else if (*$1 == "read_string")
					{
						if ($3->size() != 0)
						{
							yyerror("Builtin function 'read_string' requires 0 arguments.");
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						returnType = Symbol::DataType::STRING;
					}
					else if (*$1 == "get_at")
					{
						if ($3->size() != 2)
						{
							yyerror("Builtin function 'get_at' requires 2 arguments.");
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						if (($3->at(0)->getDataType() != Symbol::DataType::STRING) || ($3->at(1)->getDataType() != Symbol::DataType::INT))
						{
							yyerror("Function 'get_at' requires its arguments type to be (string, int). Got (%s, %s).",
								Symbol::dataTypeToString($3->at(0)->getDataType()).c_str(),
								Symbol::dataTypeToString($3->at(1)->getDataType()).c_str());
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						returnType = Symbol::DataType::CHAR;
					}
					else if (*$1 == "set_at")
					{
						if ($3->size() != 3)
						{
							yyerror("Builtin function 'set_at' requires 3 arguments.");
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						if (($3->at(0)->getDataType() != Symbol::DataType::STRING) || ($3->at(1)->getDataType() != Symbol::DataType::INT)
							|| ($3->at(2)->getDataType() != Symbol::DataType::CHAR))
						{
							yyerror("Function 'set_at' requires its arguments type to be (string, int, char). Got (%s, %s, %s).",
								Symbol::dataTypeToString($3->at(0)->getDataType()).c_str(),
								Symbol::dataTypeToString($3->at(1)->getDataType()).c_str(),
								Symbol::dataTypeToString($3->at(2)->getDataType()).c_str());
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						returnType = Symbol::DataType::STRING;
					}
					else if (*$1 == "strcat")
					{
						if ($3->size() != 2)
						{
							yyerror("Builtin function 'strcat' requires 2 arguments.");
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						if (($3->at(0)->getDataType() != Symbol::DataType::STRING) || ($3->at(1)->getDataType() != Symbol::DataType::STRING))
						{
							yyerror("Function 'strcat' requires its arguments type to be (string, string). Got (%s, %s).",
								Symbol::dataTypeToString($3->at(0)->getDataType()).c_str(),
								Symbol::dataTypeToString($3->at(1)->getDataType()).c_str());
							delete $1;
							delete $3;
							finalize(3);
							YYERROR;
						}

						returnType = Symbol::DataType::STRING;
					}

					$$ = new BuiltinCall(*$1, returnType, *$3);
					delete $1;
					delete $3;
				}
		|   ID  {
					// Find the symbol that represents the variable
					Symbol* symbol = context.findSymbol(*$1);
					if (symbol == nullptr)
					{
						yyerror("Undeclared identifier '%s' used.", $1->c_str());
						delete $1;
						finalize(3);
						YYERROR;
					}

					if (symbol->getType() != Symbol::Type::VARIABLE)
					{
						yyerror("Identifier '%s' is not a variable.", symbol->getName().c_str());
						delete $1;
						finalize(3);
						YYERROR;
					}

					$$ = new Variable(static_cast<VariableSymbol*>(symbol));
					delete $1;
				}
		|   INT_LIT { $$ = new IntLiteral($1); }
		|   CHAR_LIT { $$ = new CharLiteral($1); }
		|   STRING_LIT { $$ = new StringLiteral(*$1); delete $1; }
		;

%%
