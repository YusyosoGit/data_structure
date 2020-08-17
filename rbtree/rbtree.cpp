/*
	赤黒木（２分探索法）
	生成＆出力プログラム
	[コマンド] [ランダム生成要素数|数値リストのファイル]
	[コマンド] [ランダム生成要素数]　[数値リストのファイル]

	引数は１つか２つかを選ぶことが出来る。

	引数が１つの時は要素数かファイル名を指定。
		N個の要素の自動生成かファイルに記載の数値をもとに赤黒木の生成を行う
	引数が２つの時は引数１に要素数　引数２にファイル名を指定する。
		N個の要素を自動生成した後、ファイル記載の要素を足したり引いたりする
*/




#include "rbtree.h"
#include <string.h>
#include <fstream>
#include <vector>
#include <map>


#define E int
//E elem[] = {0, 2, 9, 10, 12, 5, 3, 29, 20, 16};

// 引数で受け取った赤黒木を標準出力する
template<class T>
class printer
{
	typedef pair<T, T> pr_val;
	typedef map<int, pr_val> bmap;
	class bmap::iterator it;// class はprinter<T>::bemap::iteratorの前に必要
	bmap bough;
	const rbtree<T> &tree;
public:
	printer(const rbtree<T> &rbt);
	void out(const struct rbtree<T>::node *v, int dep, int lr);
	void out() {out(tree.root, 0, 0);}
};

template<class T>
printer<T>::printer(const rbtree<T> &rbt):
	tree(rbt)
{
	cerr << hex << uppercase;
}

template<class T>
void printer<T>::out(const struct rbtree<T>::node *v, int dep, int lr)
{
	if (v == tree.nil) {
		if (lr != 2) return;
		else {
			bough.erase(dep);//長子不在の場合、親の枝をここで消す
			return;
		}
	}
	// いずれかの子がいる場合範囲登録
	if (v->ch[0] != tree.nil && v->ch[1] != tree.nil) {
		pr_val pr;
		if (v->ch[1] == tree.nil) {
			pr = make_pair(v->ch[0]->key, v->key);
		}
		else if (v->ch[0] == tree.nil) {
			pr = make_pair(v->key, v->ch[1]->key);
		}
		else {
			pr = make_pair(v->ch[0]->key, v->ch[1]->key);
		}
		bough.insert(make_pair(dep+1, pr));
	}
	out(v->ch[1], dep+1, 1);
	// 枝部分の表示
	for (int i=0; i< dep; i++) {
		//iterator::bmap it= bough.find(i);
		//bmap::iterator it;
		it = bough.find(i);
		if (it == bough.end() || v->key > it->second.second) {
			cerr << "  ";
		}
		else {
			cerr << "│ ";
		}
	}

	//cerr << (!lr? "--": (lr == 1?  "「": "Ｌ"));
	cerr << (!lr? "─ ": (lr == 1?  "┌ ": "└ "));
	cerr << (v->red? "\x1b[31;1m": "\x1b[30;1m");// "1"はBoldのオプション
	cerr << right << setfill('0') << setw(2) << v->key << endl;
	cerr << "\x1b[0m";
	if (lr == 2) bough.erase(dep);//長子の場合、親の枝は自分を表示したあとで消す
	out(v->ch[0], dep+1, 2);
}


int main(int argc, char * argv[])
{
	// 要素ゼロのリスト生成
    rbtree<E> tree;
    // 出力プログラムに通知
	printer<E> dsp(tree);

    if (argc == 1) {
        cout << "USAGE:" << argv[0] << " <arg1> [arg2]" << endl;
        return 0;
    }
	// 要素のあるファイル名	
	const char * entry;
	int nElems;
	nElems = atoi(argv[1]);
	bool b1stArgNum = (nElems != 0);

	switch ( argc ) 
	{
	case 2:
		// 引数一つだけ
		if (b1stArgNum)
			// 引数が要素数／ファイル名なし
			entry = NULL;
		else
			// 要素数なし／引数がファイル名
			entry = argv[1];
		break;

	case 3:
		// 引数二つ
		if (!b1stArgNum) 
			// 引数１の要素数が０のため異常終了
			return 1;
		else
			// 引数１が要素数／引数２がファイル名
			entry = argv[2];
		break;

	default:
		// エラー
		cout << "too may arguments" << endl;
		return 1;
	}

	if (nElems)
	{
		// 要素数未設定の場合要素を設定
		srand(0);
		vector<int> deck;
		deck.reserve(255);
		for (int i= 0; i< 255; i++) {
			deck.push_back(i);
		}
		for (int i= 0; i< nElems; i++) {
			int index = rand() % deck.size();
			tree.insert(deck[index]);
			deck.erase(deck.begin() + index);
		}
	}

	if (entry) {
		// ファイルから要素を読み込む	
		ifstream in(entry);
/*	for (int i= 0, n= sizeof(elem)/ sizeof(E); i < n; i++)
		tree.insert(elem[i]);
*/
		if (!in) return (1);

        cout << hex << uppercase;
		E e;
		char str[16], *endptr;
		while (!in.eof()) {
			in >> str;
			e = strtol(str, &endptr, 16);
			// eofのみでファイルの終わりを正確に検出できないので、終了の印-1を用いる
			if (e == -1) break;
            cout << right << setfill('0') << setw(2);
            cout << e << ":";
			auto pr = tree.insert(e);
			// 同じ数値を入力するとそれを削除する
			if (!pr.first) {
				tree.erase(pr.second);
                cout << right << setfill('0') << setw(2);
				cout << e << " erased." << std::endl;
			}
			else {
                cout << right << setfill('0') << setw(2);
				cout << e << " inserted." << endl;
			}
		}
	}
	// １６進、大文字使用、右詰め、０埋め、２桁
	dsp.out();
	return 0;
}

