%{
#include <vector>
#include <cstdarg>

#include "frontend/ast.h"
#include "frontend/context.h"

using namespace frontend;

extern int yylex();
//extern void yyerror(const char* e);

extern Program program;
extern Context context;

void yyerror(const char *s, ...)
{
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
}
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
	std::vector<std::string>* strList;
	std::string* strValue;
	char charValue;
	int intValue;
}

%token COMMA SEMICOLON ASSIGN LEFT_PAREN RIGHT_PAREN LEFT_CBRACE RIGHT_CBRACE PLUS MINUS
	MULTIPLY DIVIDE MODULO NOT LESS LESS_EQUAL GREATER GREATER_EQUAL EQUAL NOT_EQUAL AND OR
	IF ELSE WHILE RETURN VOID
%token <intValue> INT_LIT
%token <charValue> CHAR_LIT
%token <strValue> STRING_LIT ID TYPE BUILTIN

%type <function> func_impl
%type <functionParameters> decl_param_list impl_param_list
%type <statements> stmts stmt_list
%type <statement> stmt assign_stmt decl_stmt if_stmt while_stmt return_stmt call_stmt empty_stmt
%type <expressions> exprs expr_list
%type <expression> expr
%type <strList> decl_id_list

%left OR
%left AND
%left EQUAL NOT_EQUAL
%left LESS LESS_EQUAL GREATER GREATER_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
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

func_decl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON
			|   TYPE ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON
			|   VOID ID LEFT_PAREN VOID RIGHT_PAREN SEMICOLON
			|   VOID ID LEFT_PAREN decl_param_list RIGHT_PAREN SEMICOLON
			;

func_impl   :   TYPE ID LEFT_PAREN VOID RIGHT_PAREN                                             {
																									context.newSymbolTable();
																									context.setExpectedReturnType(Symbol::stringToDataType(*$1));
																								}
													LEFT_CBRACE stmts RIGHT_CBRACE              {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), {}, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										YYERROR;
																									}

																									$$ = new Function(funcSymbol, $8);
																									context.popSymbolTable();
																								}
			|   TYPE ID LEFT_PAREN impl_param_list RIGHT_PAREN                                  {
																									context.newSymbolTable();
																									context.setExpectedReturnType(Symbol::stringToDataType(*$1));

																									// Add parameters as variables to newly created symbol table
																									for (const auto& param : *$4)
																									{
																										if (context.currentSymbolTable()->addVariable(param) == nullptr)
																										{
																											yyerror("Redefinition of function argument '%s'.", param->getName().c_str());
																											YYERROR;
																										}
																									}
																								}
																LEFT_CBRACE stmts RIGHT_CBRACE  {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::stringToDataType(*$1), *$4, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										YYERROR;
																									}

																									$$ = new Function(funcSymbol, $8);
																									context.popSymbolTable();
																								}
			|   VOID ID LEFT_PAREN VOID RIGHT_PAREN                                             {
																									context.newSymbolTable();
																									context.setExpectedReturnType(Symbol::DataType::VOID);
																								}
													LEFT_CBRACE stmts RIGHT_CBRACE              {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::VOID, {}, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										YYERROR;
																									}

																									$$ = new Function(funcSymbol, $8);
																									context.popSymbolTable();
																								}
			|   VOID ID LEFT_PAREN impl_param_list RIGHT_PAREN                                  {
																									context.newSymbolTable();
																									context.setExpectedReturnType(Symbol::DataType::VOID);

																									// Add parameters as variables to newly created symbol table
																									for (const auto& param : *$4)
																									{
																										if (context.currentSymbolTable()->addVariable(param) == nullptr)
																										{
																											yyerror("Redefinition of function argument '%s'.", param->getName().c_str());
																											YYERROR;
																										}
																									}
																								}
															   LEFT_CBRACE stmts RIGHT_CBRACE   {
																									// We can look for functions only in global space
																									SymbolTable* globalSymbolTable = context.globalSymbolTable();
																									FunctionSymbol* funcSymbol;
																									if ((funcSymbol = globalSymbolTable->addFunction(*$2, Symbol::VOID, *$4, true)) == nullptr)
																									{
																										yyerror("Redefinition of function '%s'.", $2->c_str());
																										YYERROR;
																									}

																									$$ = new Function(funcSymbol, $8);

																									context.popSymbolTable();
																								}
			;

decl_param_list :   decl_param_list COMMA TYPE      { $$->push_back(new VariableSymbol("", Symbol::stringToDataType({*$3}))); }
				|   TYPE                            { $$ = new FunctionSymbol::ParameterList( { new VariableSymbol("", Symbol::stringToDataType({*$1})) } ); }
				;

impl_param_list :   impl_param_list COMMA TYPE ID   { $$->push_back(new VariableSymbol(*$4, Symbol::stringToDataType({*$3}))); }
				|   TYPE ID                         { $$ = new FunctionSymbol::ParameterList( { new VariableSymbol(*$2, Symbol::stringToDataType({*$1})) } ); }
				;

stmts   :   stmt_list   { $$ = $1; }
		|   %empty      { $$ = new StatementBlock(); }
		;

stmt    :   assign_stmt { $$ = $1; }
		|   decl_stmt   { $$ = $1; }
		|   if_stmt     { $$ = $1; }
		|   while_stmt  { $$ = $1; }
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
												if (symbol == nullptr || symbol->getType() != Symbol::VARIABLE)
												{
													yyerror("Assignment to undefined symbol '%s'.", $1->c_str());
													YYERROR;
												}

												VariableSymbol* varSymbol = static_cast<VariableSymbol*>(symbol);
												if (varSymbol->getDataType() != $3->getDataType())
												{
													yyerror("Unable to assign '%s' to variable '%s' of type '%s'.",
																Symbol::dataTypeToString($3->getDataType()).c_str(),
																varSymbol->getName().c_str(),
																Symbol::dataTypeToString(varSymbol->getDataType()).c_str());
													YYERROR;
												}

												$$ = new AssignStatement(varSymbol, $3);
											}
			;

decl_stmt   :   TYPE decl_id_list SEMICOLON {
												std::vector<VariableSymbol*> variables;
												for (auto& varName : *$2)
												{
													Symbol* symbol = nullptr;
													if ((symbol = context.findSymbol(varName)) != nullptr)
													{
														// We cannot shadow functions
														if (symbol->getType() == Symbol::FUNCTION)
														{
															yyerror("Redefinition of symbol '%s'. Cannot shadow functions.", symbol->getName().c_str());
															YYERROR;
														}
													}

													// Adding can fail if there is symbol in the same block, we cannot shadow in the same block
													if ((symbol = context.currentSymbolTable()->addVariable(varName, Symbol::stringToDataType(*$1))) == nullptr)
													{
														yyerror("Redefinition of symbol '%s'.", varName.c_str());
														YYERROR;
													}

													variables.push_back(static_cast<VariableSymbol*>(symbol));
												}

												$$ = new DeclarationStatement(variables);
											}
			;

decl_id_list    :   decl_id_list COMMA ID   { $$->push_back(*$3); }
				|   ID                      { $$ = new std::vector<std::string>({*$1}); }
				;

if_stmt :   IF LEFT_PAREN expr RIGHT_PAREN          {
														if ($3->getDataType() != Symbol::DataType::INT)
														{
															yyerror("Condition of if statement is of type '%s'. Must be 'int'.",
																Symbol::dataTypeToString($3->getDataType()).c_str());
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
															YYERROR;
														}

														context.newSymbolTable();
													}
					LEFT_CBRACE stmts RIGHT_CBRACE  {
														$$ = new WhileStatement($3, $7);
														context.popSymbolTable();
													}
			;

return_stmt :   RETURN expr SEMICOLON	{
											if (context.getExpectedReturnType() != $2->getDataType())
											{
												yyerror("Return type mismatch. Expected '%s', got '%s'.",
													Symbol::dataTypeToString(context.getExpectedReturnType()).c_str(),
													Symbol::dataTypeToString($2->getDataType()).c_str());
												YYERROR;
											}

											$$ = new ReturnStatement($2);
										}
			|   RETURN SEMICOLON		{
											if (context.getExpectedReturnType() != Symbol::DataType::VOID)
											{
												yyerror("Return type mismatch. Expected '%s', got 'void'.",
													Symbol::dataTypeToString(context.getExpectedReturnType()).c_str());
												YYERROR;
											}

											$$ = new ReturnStatement();
										}
			;

call_stmt   :   ID LEFT_PAREN exprs RIGHT_PAREN SEMICOLON   {
																// We can look for functions only in global space
																Symbol* symbol = nullptr;
																if ((symbol = context.globalSymbolTable()->findSymbol(*$1)) == nullptr)
																{
																	yyerror("Undeclared identifier '%s' used.", $1->c_str());
																	YYERROR;
																}

																// Symbol needs to represent function
																if (symbol->getType() != Symbol::FUNCTION)
																{
																	yyerror("Identifier '%s' is not a function.", symbol->getName().c_str());
																	YYERROR;
																}

																// Check whether number of parameters match
																FunctionSymbol* func = static_cast<FunctionSymbol*>(symbol);
																if (func->getParameters().size() != $3->size())
																{
																	yyerror("Unexpected amount of arguments provided when calling '%s'. Expected %u, got %u.",
																		func->getName().c_str(), func->getParameters().size(), $3->size());
																	YYERROR;
																}

																$$ = new CallStatement(func, *$3);
															}
			|   BUILTIN LEFT_PAREN exprs RIGHT_PAREN        {
																if (*$1 == "print")
																{
																	if ($3->size() < 1)
																	{
																		yyerror("Builtin function 'print' requires at least 1 argument.");
																		YYERROR;
																	}

																}
																else
																{
																	yyerror("Builtin function '%s' cannot be in form of statement.", $1->c_str());
																	YYERROR;
																}

																$$ = new BuiltinCallStatement(*$1, *$3);
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
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::NOT_EQUAL, Symbol::DataType::INT, $1, $3);
							}
		|   expr AND expr	{
								if (($1->getDataType() != Symbol::DataType::INT) || ($3->getDataType() != Symbol::DataType::INT))
								{
									yyerror("No match for operation '%s && %s'.",
										Symbol::dataTypeToString($1->getDataType()).c_str(),
										Symbol::dataTypeToString($3->getDataType()).c_str());
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
									YYERROR;
								}

								$$ = new BinaryExpression(Expression::Type::OR, Symbol::DataType::INT, $1, $3);
							}
		|   NOT expr		{
								if ($2->getDataType() != Symbol::DataType::INT)
								{
									yyerror("No match for operation '! %s'.",
										Symbol::dataTypeToString($2->getDataType()).c_str());
									YYERROR;
								}

								$$ = new UnaryExpression(Expression::Type::NOT, Symbol::DataType::INT, $2);
							}
		|   LEFT_PAREN TYPE RIGHT_PAREN expr {
								Symbol::DataType castToType;
								if ((($4->getDataType() == Symbol::DataType::CHAR) && (Symbol::stringToDataType(*$2) == Symbol::DataType::INT)) ||
									(($4->getDataType() == Symbol::DataType::STRING) && (Symbol::stringToDataType(*$2) == Symbol::DataType::CHAR)))
								{
									castToType = Symbol::stringToDataType(*$2);
								}
								else
								{
									yyerror("Unable to cast type '%s' to type '%s'.",
										Symbol::dataTypeToString($4->getDataType()).c_str(),
										$2->c_str());
									YYERROR;
								}

								$$ = new UnaryExpression(Expression::Type::TYPECAST, castToType, $4);
							}
		|   LEFT_PAREN expr RIGHT_PAREN { $$ = $2; }
		|   ID LEFT_PAREN exprs RIGHT_PAREN
				{
					// We can look for functions only in global space
					Symbol* symbol = nullptr;
					if ((symbol = context.globalSymbolTable()->findSymbol(*$1)) == nullptr)
					{
						yyerror("Undeclared identifier '%s' used.", $1->c_str());
						YYERROR;
					}

					// Symbol needs to represent function
					if (symbol->getType() != Symbol::FUNCTION)
					{
						yyerror("Identifier '%s' is not a function.", symbol->getName().c_str());
						YYERROR;
					}

					// Check whether number of parameters match
					FunctionSymbol* func = static_cast<FunctionSymbol*>(symbol);
					if (func->getParameters().size() != $3->size())
					{
						yyerror("Unexpected amount of arguments provided when calling '%s'. Expected %u, got %u.",
							func->getName().c_str(), func->getParameters().size(), $3->size());
						YYERROR;
					}

					$$ = new Call(func, *$3);
				}
		|   BUILTIN LEFT_PAREN exprs RIGHT_PAREN
				{
					Symbol::DataType returnType;
					if (*$1 == "print")
					{
						if ($3->size() < 1)
						{
							yyerror("Builtin function 'print' requires at least 1 argument.");
							YYERROR;
						}

						returnType = Symbol::DataType::VOID;
					}
					else if (*$1 == "read_char")
					{
						if ($3->size() != 0)
						{
							yyerror("Builtin function 'read_char' requires 0 arguments.");
							YYERROR;
						}

						returnType = Symbol::DataType::CHAR;
					}
					else if (*$1 == "read_int")
					{
						if ($3->size() != 0)
						{
							yyerror("Builtin function 'read_int' requires 0 arguments.");
							YYERROR;
						}

						returnType = Symbol::DataType::INT;
					}
					else if (*$1 == "read_string")
					{
						if ($3->size() != 0)
						{
							yyerror("Builtin function 'read_string' requires 0 arguments.");
							YYERROR;
						}

						returnType = Symbol::DataType::STRING;
					}
					else if (*$1 == "get_at")
					{
						if ($3->size() != 2)
						{
							yyerror("Builtin function 'get_at' requires 2 arguments.");
							YYERROR;
						}

						if (($3->at(0)->getDataType() != Symbol::DataType::STRING) || ($3->at(1)->getDataType() != Symbol::DataType::INT))
						{
							yyerror("Function 'get_at' requires its arguments type to be (string, int). Got (%s, %s).",
								Symbol::dataTypeToString($3->at(0)->getDataType()),
								Symbol::dataTypeToString($3->at(1)->getDataType()));
						}

						returnType = Symbol::DataType::CHAR;
					}
					else if (*$1 == "set_at")
					{
						if ($3->size() != 3)
						{
							yyerror("Builtin function 'set_at' requires 3 arguments.");
							YYERROR;
						}

						if (($3->at(0)->getDataType() != Symbol::DataType::STRING) || ($3->at(1)->getDataType() != Symbol::DataType::INT)
							|| ($3->at(2)->getDataType() != Symbol::DataType::CHAR))
						{
							yyerror("Function 'set_at' requires its arguments type to be (string, int, char). Got (%s, %s, %s).",
								Symbol::dataTypeToString($3->at(0)->getDataType()),
								Symbol::dataTypeToString($3->at(1)->getDataType()),
								Symbol::dataTypeToString($3->at(2)->getDataType()));
						}

						returnType = Symbol::DataType::STRING;
					}
					else if (*$1 == "strcat")
					{
						if ($3->size() != 2)
						{
							yyerror("Builtin function 'strcat' requires 2 arguments.");
							YYERROR;
						}

						if (($3->at(0)->getDataType() != Symbol::DataType::STRING) || ($3->at(1)->getDataType() != Symbol::DataType::STRING))
						{
							yyerror("Function 'strcat' requires its arguments type to be (string, string). Got (%s, %s).",
								Symbol::dataTypeToString($3->at(0)->getDataType()),
								Symbol::dataTypeToString($3->at(1)->getDataType()));
						}

						returnType = Symbol::DataType::STRING;
					}

					$$ = new BuiltinCall(*$1, returnType, *$3);
				}
		|   ID  {
					// Find the symbol that represents the variable
					Symbol* symbol = context.findSymbol(*$1);
					if (symbol == nullptr)
					{
						yyerror("Undeclared identifier '%s' used.", $1->c_str());
						YYERROR;
					}

					if (symbol->getType() != Symbol::VARIABLE)
					{
						yyerror("Identifier '%s' is not a variable.", symbol->getName().c_str());
						YYERROR;
					}

					$$ = new Variable(static_cast<VariableSymbol*>(symbol));
				}
		|   INT_LIT { $$ = new IntLiteral($1); }
		|   CHAR_LIT { $$ = new CharLiteral($1); }
		|   STRING_LIT { $$ = new StringLiteral(*$1); }
		;

%%
