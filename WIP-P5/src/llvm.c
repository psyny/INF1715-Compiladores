#include "llvm.h"
#include "types.h"
#include "absSyntaxTree.h"
#include "tools.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static int g_id = 0;
static int g_ident = 0;

/*
 * returns the corresponding llvm type (int = i32, for example)
 */
char* code_getType( int type ) {
	switch ( type ) {
	case INT: {
		return "i32";
		break;
	}
		//TODO: other cases

	}
}
/*
 * returns the corresponding llvm size (int = 4, for example)
 */
int code_getSize( int type ) {
	switch ( type ) {
	case INT: {
		return 4;
		break;
	}
		//TODO: other cases

	}
}

// FLUFF: Keep Track of identation
void code_enterBlock(void) {
	g_ident++;
}

// FLUFF: Keep Track of identation
void code_exitBlock(void) {
	g_ident--;
}

// FLUFF: Print Current identation ( with new line )
void code_printIdent(void) {
	int i;
	printf("\n");
	for( i = 0 ; i < g_ident ; i++ ) {
		printf("  ");
	}
}

// Link with AST: Set node last tracked ID on LLVM ( mainly or declarations and atributions )
void link_setLastID( ABS_node* node ) {
	switch( node->tag ) {
		case DEC_VAR:
			node->node.decl.vardecl.lastCodeID = g_id;
			break;
	}
}

// Link with AST: Get node last tracked ID on LLVM ( mainly for variables )
int link_getLastID( ABS_node* node ) {
	switch( node->tag ) {
		case DEC_VAR:
			return node->node.decl.vardecl.lastCodeID;
			
		case VAR_MONO:
			return link_getLastID(node->node.var.id->declNode);
			
		case EXP_VAR:								
			return link_getLastID(node->node.exp.data.varexp);
	}
}

// Print a literal node preceeded of its type code
void code_literalNode( ABS_node* node ) {
	char* type = code_getType( node->node.exp.type );
	
	switch( node->tag ) {
		case LIT_INT:
			printf("%s %d" , type , node->node.exp.data.literal.vInt );
			break;	
			
		case LIT_FLOAT:
			printf("FLOAT");
			break;	
			
		case LIT_STRING:
			printf("STRING");
			break;	
	}	
}

// Print a id on llvm format, simple but grants name prefix
void code_llvmID( int id ) {
	printf("%%v_%d", id);
}

// Create a new var ( updateglobal id ) and print its reference
int code_newVar(void) {
	int id = g_id;
	g_id++;
	code_llvmID( id );
	return id;
}

// Generate code for: alloca and return its ID
int code_alloca( int aType ) {
	int id;
	char* type;
	int size;
	
	type = code_getType( aType );
	size = code_getSize( aType );

	// Alloca
	code_printIdent();
	id = code_newVar();	
	printf(" = alloca %s, align %d" , type , size );
	
	return id;
}


// Generate code for: store of literal onto sId
void code_storeLiteralInt( int sType , int sId , ABS_node* literal ) {
	char* type;
	int size;
	
	type = code_getType( sType );
	size = code_getSize( sType );

	// Store
	code_printIdent();
	printf("store " );
	code_literalNode( literal );
	printf(", %s* ", type );
	code_llvmID( sId );
	printf(", align %d" , size );
}

// Generate code for: load and lId of lType and return its new ID
int code_load( int lType , int lId  ) {
	int id;
	char* type;
	int size;
	
	type = code_getType( lType );
	size = code_getSize( lType );	
	code_printIdent();
	id = code_newVar();	
	printf(" = load %s, %s* " , type , type );
	code_llvmID( lId );	
	printf(", align %d",size);
			
	return id;
}


// --------------------------------------------------------------------
// Main Code
// --------------------------------------------------------------------

void genCode_list( ABS_node* node );
void genCode( ABS_node* node );
void codeDecl( ABS_node* node );
void codeDeclFunc( ABS_node* node );
void codeDeclVar( ABS_node* node );
void codeParam( ABS_node* param );
void codeBlock( ABS_node* block );
void codeVarDecl( ABS_node* decl );
void codeCmd( ABS_node* cmd );
int codeExp( ABS_node* exp );
void codeCmd_ret( ABS_node* cmd );
int codeCmd_atr( ABS_node* cmd );


void genASTCode(void) {
	ABS_node* thisNode = programNode;	
	
	genCode_list( thisNode );	
}


void genCode_list( ABS_node* node ) {

	while( node != NULL ) {
		genCode( node );
		node = node->next;
	}
}


void genCode(ABS_node* node) {
	switch( node->type ) {	
		case TYPE_DECL:
			return codeDecl( node );
					
		case TYPE_ID:
			//return genCode_id( node );
		
		case TYPE_EXP:
			//return genCode_exp( node );
			
		case TYPE_VAR:
			//return genCode_var( node );
			
		case TYPE_CMD:
			return codeCmd( node );
			
		default:
			return;
	}
}


/*
 * generates code for declarations
 */

void codeDecl( ABS_node* node ) {
	switch ( node->tag ) {
		case DEC_FUNC: {
			codeDeclFunc( node );
			break;
		}
		case DEC_VAR: {
			codeDeclVar( node );
			break;
		}
	}
}

/*
 * generates code for function definiton
 */

void codeDeclFunc( ABS_node* node ) {
	char* type = code_getType( node->node.decl.funcdecl.type );
	char* func_name = node->node.decl.funcdecl.id->node.id.name; //@func_name
				
	printf( "define %s @%s", type, func_name );

	codeParam( node->node.decl.funcdecl.param );
	codeBlock( node->node.decl.funcdecl.block );
}

/*
 * generates code for variable definition
 */
void codeDeclVar( ABS_node* node ) {
	//%name = alloca <type>, align <size>

	char* name = node->node.decl.vardecl.id->node.id.name;
	char* type = code_getType( node->node.decl.vardecl.type );
	int size = code_getSize( node->node.decl.vardecl.type );

	code_printIdent();	
	code_llvmID(g_id);
	printf( " = alloca %s,align %d", type, size );
	link_setLastID( node );
	g_id++;
}

/*
 * prints the parameters in the llvm format
 */
void codeParam( ABS_node* param ) {
	char* type;
	char* id;
	ABS_node* current;
	int  commaFlag = 0;
	
	printf( "( " );
	
	while( param != NULL ) {
		//id = param->node.decl.vardecl.id->node.id.name;
		type = code_getType( param->node.decl.vardecl.type );
		
		// Annoying Separator Flag
		if( commaFlag ) {
			printf(" , ");
		}
		else {
			commaFlag = 1;
		}
		
		// Var ID and Type
		printf( "%s ", type );
		code_llvmID(g_id);
		link_setLastID( param );
		g_id++;
		
		param = param->next;
	}

	printf( " )" );
}


/*
 * prints a block
 */

void codeBlock( ABS_node* block ) {
	code_printIdent();
	printf( "{" );
	
	code_enterBlock();

	genCode_list( block->node.cmd.blockcmd.decl );
	genCode_list( block->node.cmd.blockcmd.cmd );	

	code_exitBlock();
	
	code_printIdent();
	printf( "}\n" );
}

/*
 * generates code for variable declarations (alloca)
 */

void codeVarDecl( ABS_node* decl ) {
	//TODO
}

/*
 * generates code for commands
 */
void codeCmd( ABS_node* cmd ) {
	switch ( cmd->tag ) {
		case CMD_RET: {
			codeCmd_ret( cmd );
			break;
		}
			
		case CMD_ATTR: {
			codeCmd_atr( cmd );	
			break;
		}
			//TODO: other cases
	}
	//cmd->node.cmd.retcmd : struct(ABS_node* exp)
}

/*
 * generates code for exp
 */

int codeExp( ABS_node* exp ) {
	int id;
	int size;
	char* type;
	
	switch ( exp->tag ) {
		case EXP_VAR: {
			id = link_getLastID(exp);
			break;
		}
	
		case EXP_ARIT: {
			int id1 , id2;
			type = code_getType( exp->node.exp.type );
			
			id1 = codeExp( exp->node.exp.data.operexp.exp1 );
			id2 = codeExp( exp->node.exp.data.operexp.exp2 );

			// Assigment Part
			code_printIdent();			
			id = code_newVar();
			printf(" = ");
			
			// Signal Part
			switch( exp->node.exp.data.operexp.opr ) {
				case '+':
					printf("add");
					break;
				
				case '-':
					printf("sub");
					break;
					
				case '*':
					printf("mul");
					break;				
				
				case '/':
					printf("sdiv");
					break;	
			}
			
			// References Part
			printf(" nsw %s " , type );
			code_llvmID(id1);
			printf(" , ");
			code_llvmID(id2);			
			
			break;
		}
	
		case LIT_INT: {
			int id2;
		
			type = code_getType( INT );
			size = code_getSize( INT );
		
			// Alloca
			id2 = code_alloca( INT );
			
			// Store
			code_storeLiteralInt( INT , id2 , exp );
			
			// Load
			id = code_load( INT , id2  );
			
			break;
		}
			//TODO: other cases
	}
	
	return id;
}


/*
 * generates code for return
 */
void codeCmd_ret( ABS_node* cmd ) {
	char* type = code_getType( cmd->node.cmd.retcmd.type );
	
	switch( cmd->node.cmd.retcmd.exp->tag ) {
		case LIT_INT:
		case LIT_FLOAT:
		case LIT_STRING:
			code_printIdent();
			printf("ret " );		
			code_literalNode( cmd->node.cmd.retcmd.exp );
			break;	
			
		default:
			{
			int id = codeExp( cmd->node.cmd.retcmd.exp );
			code_printIdent();
			printf("ret %s " , type );	
			code_llvmID(id);
			}
	}		

}


/*
 * generates code for attribution
 */
int codeCmd_atr( ABS_node* cmd ) {
	int id;

	// TODO: everything

	return id;
}

