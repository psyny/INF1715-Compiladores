INF1715 - Compiladores
Professor: Roberto Ierusalimschy


Rafael Augusto Gaseta França 	- 1421436
Guilherme Simas					- ⁠⁠⁠1311812


OBSERVAÇÃO!!! Este arquivo é referente a todas as etapas do trabalho.
Sendo atualizado em cada nova etapa.
Para a etapa atual ( ETAPA 6 - FINAL ) leia apenas o tópico 8.


0) 	Compilando a etapa atual
	Basta executar "make"
	Output: main_p6.out


1) Requisitos
	bash
	make
	gcc
	flex
	bison
	diff
	llvm


2) Decisões de projeto
	2.1) Warnings
	Warning é notificado mas não aborta o LEX.
	No monga.lex é possível desativar os warnings editando _SHOW_WARNINGS 1 para 0
	
	O unico warning gerado é quando simbolos não esperados são encontrados.
	Estes simbolos geram token com o proprio codigo ASCII.
	O tratamento deve ser feiro em outro nivel.
	
	2.2) Error ( Fatal )
	-Comentários e strings não fechados ao final do arquivo
	-Strings não fechato ao final da linha
	-Escapes invalidos
	-Plics que não contem sequencias conhecidas
	
	2.3) Plics
	Escapes e caracteres entre plics são convertidos para seu inteiro correspondente em ASCII


3) Testes LEX
	3.1) Conceito
	O sucesso ou falha de um teste não está vinculado ao sucesso ou falha do LEX.
	Ou seja, se o erro no LEX for esperado pelo teste, o teste tem sucesso.
	Generalizando: o teste tem sucesso quando o resultado no LEX é o resperado pelo teste.
	Obs.: Para "testar o teste", um dos testes, o "teste_que_falha", falha propositalmente.
	
	O teste consiste em escanear um arquivo de entrada com um programa, o monga.out.
	O monga.out utiliza o yylex do flex e gera um arquivo *.tkn
	Os tokens de multi simbolos são escritos como seu "ENUM"
	Ao final, o arquivo *.tkn é comparado com o *.gab correspondente com o programa "diff"
	
	Os arquivos de teste referentes apenas ao lexico residem na pasta:
	testes_p1_lex
	
	3.2) Teste basico de funcionamento
	Comando: make Lex
	Monta tudo, desde o *.lex e ao final executa um teste geral, o "geral"
	
	3.3) Testes em massa
	Comando: make LexTestes
	Faz tudo que o "make Lex" faz, além disso executa todos os testes da pasta "testes_p1_lex"
	

4) Testes YACC - Gramatica
	4.1) Conceito
	Os teste apenas retornan se a sintaxe é válida ou não.
	Para facilitar quais testes deveriam falhar, os arquivos de teste destinados a falhar
	possuemo sufixo "fail"
	
	Os arquivos de teste referentes a esta etapa residem na pasta:
	testes_p2_gramatica	
	
	4.2) Teste basico de funcionamento
	Comando: make Yacc
	Monta tudo, desde o *.lex e ao final executa um teste geral, o "geral"
	
	4.3) Testes em massa
	Comando: make YaccTestes1
	Faz tudo que o "make Yacc" faz, além disso executa todos os testes da pasta "testes_p2_gramatica"
	
	
	
	
5) Testes AST - Construção da Arvore Sintatica Abstrata
	5.1) Conceito
	Constroi a AST a partir de um arquivo e imprime na tela OU salva em arquivo.
	Em tela: Entradas em um mesmo nivel possuem prefixo da mesma cor.
	Em arquivo: prefixo numero simboliza o nivel da entrada
	
	Os arquivos de teste referentes a esta etapa residem na pasta:
	testes_p3_ast

	Sendo os *.monga a entrada, e *.out a saida
	
	5.2) Teste basico de funcionamento
	Comando: make AST
	Monta tudo, desde o *.lex e ao final executa um teste geral, o "geral"
	Esse teste será exibido em tela
	
	5.3) Testes em massa - Output em Arquivos
	Comando: make ASTTestes1
	Faz tudo que o "make AST" faz, além disso executa todos os testes da pasta "testes_p3_AST"

	5.3) Testes em massa - Output em Tela	
	Comando: make ASTTestes2
	Faz tudo que o "make AST" faz, além disso executa todos os testes da pasta "testes_p3_AST"	
	



6) Testes Tipagem - Costura de AST e cheque de tipagem
	6.1) Conceito
	Baseado em um arquivo de entrada:
	-Executa os topicos anteriores
	-Constroi a AST
	-vincula as variaveis e funcoes ( ids ) as suas declarações
	-verifica tipagens
	
	Os arquivos de teste referentes a esta etapa residem na pasta:
	testes_p4_tipagem

	Sendo os *.monga as entradas
	
	6.2) Teste basico de funcionamento
	Comando: make Types
	Monta tudo, desde o *.lex e ao final executa um teste geral, o "geral"
	Esse teste será exibido em tela
	
	6.3) Testes em massa - Output em Arquivos
	Comando: make TypesTestes
	Faz tudo que o "make Types" faz, além disso executa todos os testes da pasta "testes_p4_tipagem"




7) Testes Codigo - Geração e compilação do codigo LLVM - Parte 1
	Obsoleto, substituido pelo topico seguinte ( 8 )
	
	
8) Testes Codigo - Geração e compilação do codigo LLVM - Parte 2
	8.1) Decisões sobre LLVM
	-Variaveis locais seguem o conceito SSA:
	--Registro para a construção do PHI é feito por Array ( varTracking[] )
	--A AST guarda o INDICE da variavel no VARTRACKING
	--varTracking[i] guarda o ultimo id do LLVM da variavel
	--Indices declarados em um escopo são descartados quando o escopo acaba.
			Ou seja, são liberados para reutilização.		
	-Variaveis seguem o nome [%|@]v_<int>
	-Labels seguem o nome l_<int>
	-Condicionais fazem curto circuito
	-Valores de decisão são inteiros
	-Não gera código para Array
			Motivo: Não conseguimos fazer um SSA para array simples o suficiente
	
	8.2) Conceito do Programa
	Com base em um arquivo de entrada:
	-Executa os topicos anteriores
	-Gera o codigo LLVM no arquivo *.ll a partir do arquivo *.monga
	-Gera o *.o do codigo LLVM gerado
	-Compila o "llvm_print_float.c" ligando com o *.o gerado
	
	Os arquivos de teste referentes a esta etapa residem na pasta:
	testes_p6_llvm
	
	Falta geração de codigo para: IF, WHILE e ARRAY

	8.3) Teste basico de funcionamento
	Comando: make LLVM
	Monta tudo, desde o *.lex e ao final executa um teste geral, o "geral.monga"

	Para melhor analise, o make LLVL imprime:
	-conteudo de "geral.monga"
	-AST de "geral.monga"
	-Codigo LLVM gerado por "geral.monga"
	-Resultado da chamada do codigo LLVM compilado

	Principais etapas: 
	-Gera o LLVM do "geral.monga"
	-Compila o LLVM em Obj
	-Compila "llvm_print_float.c" ligando com o *.o gerado
	-Executa o resultado da compilação e imprime a saida na tela	
	
		
	
