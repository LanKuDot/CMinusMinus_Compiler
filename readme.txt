組員：F74002183 李昆憶 ( LanKuDot )
      F74002141 曾冠博 ( brandboat )

Project on Github：https://github.com/LanKuDot/CMinusMinus_Compiler

Lexer
========================================================
+ 實作函式：lexial_analyzer( const char *source_file,
                             vector< Token_info > *token_list );
+ 資料結構：
typedef struct {
    string token;        // 該 token 的內容
    string tranToken;    // ???
    int lineNumber;      // 該 token 在原始碼中的行號
    enum Category category;  // 該 token 的種類
} Token_info;

+ 實作：
1. 從 source code 中一次讀取一行 code
2. 透過 strtok 函式將 token 從 buffer 中分離出來
3. 將所得到的 token 傳送到 start 進行 lexial analysis

   lexial analysis 是由 DFA 組成，有四個主要的處理函式：
   - start, number, character, identifier
   每轉換一個 function 都會繼續判斷下一個 character

   先判斷 token 的第一個 character
   3.1 如果是字母或 _ ：到 identifier state
     3.1.1 先 match 是不是 keyword，如果是，判定此 token 為 KEYWORD
     3.1.2 判斷接下來的字元是皆為字元和數字或_，如果都是，判定此 token 為 IDENTIFIER
	       如果不是，就判定為 ERROR

   3.2 如果是數字：到 number state
     3.2.1 如果後面所帶的字元皆為數字，則判定為 NUMBER，否則為 ERROR

   3.3 如果是單引號：到 character state
     3.3.1 先 match escape character \t 及 \n，如果 \ 後面不是 n 或 t，則為 ERROR
     3.3.2 如果單引號後面的第一個字元不是 \，則直接讀取該字元
     3.3.3 再來判斷下一個字元是否為單引號，（除非是 escape character，不然兩個單引號中間
           只能有一個字元 ），如果不是則為 ERROR
     3.3.4 判斷單引號後面是否還有字元，如果有則為 ERROR，否則為 CHAR

   3.4 [ ] { } ( ) ; , ：如果後面沒有接任何字元，則為 SPECIAL_SYMBOL
   3.5 + - * ：如果後面沒有接任何字元，則為 OPERATOR
   3.6 /：如果下一個字元是 /，則為 COMMENT，並結束該行的 lexial analysis
       否則沒有接其他字元，則為 OPERATOR
   3.7 其他的 operators，也一樣，如果後面沒有接任何字元，則為 OPERATOR，否則是 ERROR

4. 從 DFA 出來後，會回傳此 token 的種類，如果收到 COMMENT，則直接讀取下一行的 code
5. 將這些資訊儲存到 Token_info 並 push 到 token_list 中，以及輸出 token list file


Syntax_Analzyer
========================================================
function 如下 : 

- 我們先來定義資料結構的部分:
*LHS : 我把 grammar lhs 的部分直接定義成一個type。
  typedef string LHS;
*RHS : 這邊我用一個二維vector去儲存grammar rhs部分
  typedef vector< vector<string> > RHS;
*最後再用map分別去對應LHS RHS
  typedef map<LHS, RHS> GRAMMAR;
EX. VarDeclList
		VarDecl VarDeclList
		epsilon
	會產生
	LHS : VarDeclList
	RHS : RHS[0][0]VarDecl [0][1]VarDeclList
	      RHS[1][0]epsilon

前置 : 任何grammar都要有一start symbol，這邊我是預先在readfile中先放進去 S -> Program $。
		  
- 步驟

1. 產生 Set
[nullable] : 先確認一個symbol是否為nonterminal，再往下查找，概念很簡單，就是去grammar查找，有epsilon則稱該symbol為nullable。
[first] : 確認nullable之後即可開始先找它的first set，我是按照講義上的教法，當一個RHS 第一個symbol nullable=true，那麼就往下一個繼續查找，我是利用recursive
不斷查找。為避免重複加入相同symbol，我使用了C++ STL中的 set。
[follow] : 上述都做完之後才可做follow的部分，先看一下follow的規則，
	// A -> BC
	// Rules : 
	// 1. Follow(B) contains First(C)
	// 2. Follow(B) contains Follow(C) if nullable(C) is true
	// 3. Follow(B) contains Follow(A) if nullable(C) is true
	// 4. Follow(C) contains Follow(A) 
依照上述規則，我將"基本的"follow table做好，但是裏頭有包含許多nonterminal，所以我必須再一個個將followtable中的nonterminal消除，
具體消除的辦法，有點瑣碎，首先followset可能包含自己，那我們要先把自己給刪掉，再來將所有的follow(nonterminal)一個個查表填入follow(自己)，
但因為我不知道哪一個nonterminal先將本身follow中的nonterminal消除，我必須用一個for loop用一個最大上限跑到他們全部做完為止。(這不是個好做法，
但我目前也沒有打算再做更動了...)

2. 產生 LLTable
LLTable概念很簡單，吃進一symbol，first(LHS)中有symbol，就去找其對應的rule，有的話就先塞進LLTable，沒有的話就看follow(LHS)，然後塞epsilon。

3. 產生Parse_Tree
基本上我就只是照著講義上提供的演算法實作出來，其他部分沒有特別要講的地方。主要就是利用stack實作



Semantic_Analyzer
===============================================
產生 Symbol_Table.txt

stack<vector<Symbol_Detail>> Symbol_Table

每展開一個scope都會先宣告一vector，先塞到stack top並將其所有變數存儲在vector中。
Symbol_Detail是每個symbol的詳細內容，具體來說有symbol token scope isDeclared isInitialized。

這邊我們只有考慮int 和char，並沒有實際做出semantic rule，概念基本上來說也是利用stack實作，每讀進 "(" scope就加1(有例外情況喇!我是把它可能會爆掉的情況列出來，
像是左邊有 + - * / 就代表他不是有效scope宣告)，吃到id先判斷該變數使否有宣告過，宣告過就僅在stack top中的vector中查找是否有相同名子且有宣告的變數，若有則抱錯。
若吃進的symbol沒有宣告過，就得不斷去stack中查，如果都沒有查到有宣告過的變數，則報錯。
目前來說就只是做到可以查看變數是否合法。


Quadruple generator
==============================================
已經定義的 quadruple：

Op      Arg1    Arg2    Result
-------------------------------
BinOp   Arg1    Arg2    Result   將 Arg1 BinOp Arg2 的結果存到 Result
=       Arg1            Result   將 Arg1 的值 assign 給 Result
VARDECL type            id       產生 type 的 id 變數
ARRAYDE type    num     id       產生擁有 num 個元素的 type 陣列 id
ARRAYAD id      index   Result   將名為 id 的 array 的指定元素的位址存到 Result
ARRAYVA id      index   Result   將名為 id 的 array 的指定元素的值存到 Result
RETURN  Arg1                     將 Arg1 輸出到螢幕上

實作：以 Parse tree 為資料來源，用 grammar 來產生 quadruple
